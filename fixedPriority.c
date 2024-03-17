#include "fixedPriority.h"
#include "stm32f3xx.h"

/* Function prototypes */
static void fixedPriority_enqueue(OS_TCB_t * tcb_p);
static void fixedPriority_dequeue(OS_TCB_t * tcb_p);
static void sortQueue(uint32_t start);
static OS_TCB_t const * fixedPriority_getTask(void);
static void fixedPriority_wait(void * const reason, uint32_t code);
static void fixedPriority_notify(void * const reason);

/* Task Array to store the Heap */
static OS_TCB_t * tasks[PRIORITY_QUEUE_MAX_TASKS] = {0};

/* Index for where the final item in the queue is 
 * 0 to initialise - if its 0 then its an empty queue
 */
static uint32_t index = 0;

/* Scheduler block for the fixed priority scheduler */
OS_Scheduler_t const fixedPriorityScheduler = {
	.preemptive = 1,
	.scheduler_callback = fixedPriority_getTask,
	.addtask_callback = fixedPriority_enqueue,
	.taskexit_callback = fixedPriority_dequeue,
	.wait_callback = fixedPriority_wait,
	.notify_callback = fixedPriority_notify
};

/* Adds a task to the bucket queue and sorts it based on priority
 * Takes a pointer to a TCB, returns nothing
 * If the bucket queue is empty, then the task is set as the zeroth
 *	task.
 * If it is not empty, then the element goes to the end of the queue,
 * 	and swaps with other tasks if they have a lesser priority, until
 *  it reaches a task with the same or higher priority. 
 * If two tasks have the same priority, the task that was enqueued
 * 	first will stay in front of the second task.
 */
static void fixedPriority_enqueue(OS_TCB_t * newTask) {
	// Check if full, if not then it won't be added
	if(index < PRIORITY_QUEUE_MAX_TASKS) { 
		// First element to be added, no need to sort
		if (index == 0) {
			tasks[index] = newTask;	
			index += 1;			
		} else { // If not the first element
			for (int i = index - 1; i >= 0; i--) {
				if (tasks[i]->priority > newTask->priority) {
					tasks[i + 1] = tasks[i];
					tasks[i] = newTask;
					index += 1;
				} else {
					tasks[i + 1] = newTask;
					index += 1;
					break;
				}
			}
		}
	}
}

/* Removes the specified task from the queue
 * Takes a pointer to a tcb and returns nothing.
 * Checks to see if the queue is empty (index = -1) before
 * 	dequeueing, then goes down the queue (from index) 
 *  checking whether the tcb matches the current element.
 * If it does, then that element is set to 0 and the queue
 *  is sorted, and the loop breaks.
 */
static void fixedPriority_dequeue(OS_TCB_t * task) {
	if (index != 0) { // If 0 then the queue is already empty
		for (int i = index; i != 0; i--) {
			if (tasks[i] == task) {
				tasks[i] = 0;
				index -= 1;
				sortQueue(i);
				break;
			}
		}
	}
}

/* Sorts the queue from a given point
 * Takes an integer start point, and returns nothing
 * Goes from the start point to the end of the queue.
 * If the current task is 0 and the next task is not,
 *  then it moves the next task into the current element.
 */ 
static void sortQueue(uint32_t start) {
	for (int i = start; i != index; i++) {
		if (tasks[i] == 0 && tasks[i+1] != 0) {
			tasks[i] = tasks[i + 1];
			tasks[i + 1] = 0;
		}
	}
}

/* Returns the next highest priority task (that isn't asleep/waiting)
 * Takes no arguments, returns a pointer to a TCB.
 * Loops backwards through the tasks queue from the highest priority task.
 * If a task is not waiting or asleep then it returns the task to be run.
 * If it is asleep, then it checks whether it should still be asleep
 *	and removes the sleep state bit if it should be awake, and returns the task.
 */
static OS_TCB_t const * fixedPriority_getTask(void) {
	for (int i = index; i >= 0; i--) {
		if ((tasks[i] != 0) && ((tasks[i]->state & TASK_STATE_WAIT) != TASK_STATE_WAIT)) { // Checks if tasks[i] is there and not waiting
			if ((tasks[i]->state & TASK_STATE_SLEEP) != TASK_STATE_SLEEP) {
				return tasks[i];
			} else if (tasks[i]->data < OS_elapsedTicks()) {
				tasks[i]->state &= ~TASK_STATE_SLEEP;
				return tasks[i];
			}
		}
	}
	// If no other tasks to run then idle.
	return OS_idleTCB_p;
}

/* Wait callback
 * Takes a constant pointer to void called 'reason' and a uint32 to check against the check code.
 * Sets the current TCB's data field to 'reason' after casting it to uint32_t.
 * Sets the wait flag in the TCB's state field.
 * Invokes the scheduler by setting the PendSV Bit
 */
static void fixedPriority_wait(void * const reason, uint32_t code) {
	if (code == OS_checkCode()) {
		OS_currentTCB()->data = (uint32_t) reason;
		OS_currentTCB()->state = TASK_STATE_WAIT;
		SCB->ICSR = SCB_ICSR_PENDSVSET_Msk;
	}
	// Else do nothing
}

/* Notify callback 
 * Takes a constant pointer to void called 'reason'.
 * Loops back through all tasks from the current index and 
 *  checks if the wait flag is set, and if the data field 
 *  matches the given reason argument.
 * If the check succeeds, then that is the task to notify.
 * This is done by removing the wait flag with through a bitwise and 
 * 	with a mask of the wait flag, and setting the data field to 0.
 * The task is then able to be run again in the getTask routine.
 */
static void fixedPriority_notify(void * const reason) {
	for (int i = index; i >= 0; i--) {
		if (((tasks[i]->state & TASK_STATE_WAIT) == TASK_STATE_WAIT) && (tasks[i]->data == (uint32_t) reason)) {
			tasks[i]->state &= ~TASK_STATE_WAIT;
			tasks[i]->data = 0;
		}
	}
}
