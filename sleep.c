#include "sleep.h"
#include "os_internal.h"

/*
 * Takes a uint32_t as the time to wake the task, returns nothing
 * Gets the pointer to the current task control block (TCB) & the current program time
 * Puts the program time into the data field of the TCB as its time that it wants to be woken
 * Sets the state field to 1 to indicate that it is sleeping
 * Yields to start a task switch.
*/
void OS_sleep(uint32_t time) {
	OS_TCB_t * currTCB = OS_currentTCB();
	currTCB->data = OS_elapsedTicks() + time;
	currTCB->state = TASK_STATE_SLEEP;
	OS_yield();
}
