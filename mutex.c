#include "mutex.h"

/* Initialises a mutex structure.
 * Takes a pointer to a mutex structure as an argument, returns nothing
 * Sets the TCB pointer field and counter field to 0.
 */
void OS_mutex_init(OS_mutex_t * mutex_p) {
	mutex_p->tcb_p = 0;
	mutex_p->counter = 0;
}

/* Attempts to acquire a mutex for the current task
 * Takes a pointer to a mutex structure as an argument, returns nothing
 * Attempts to load the pointer, and if there is no stored TCB pointer in the mutex 
 * 	then the current TCB pointer is stored in the mutex structure
 * If this fails, then it retries the entire function straight away.
 * If the TCB pointer in the mutex points to a different TCB then the wait function
 * 	is called, and once notified attempts to reacquire the mutex again.
 * If it passes these checks, then the mutex has been acquired by the current task
 * 	and the counter is incremented by 1.
 */
void OS_mutex_acquire(OS_mutex_t * mutex_p) {
	uint32_t load = __LDREXW((uint32_t *) mutex_p->tcb_p);
	if (load == 0) {
		uint32_t storeReturn = __STREXW((uint32_t) OS_currentTCB(), (uint32_t *) mutex_p->tcb_p);
		if (storeReturn == 1) {
			OS_mutex_acquire(mutex_p);
		}
	} else if (load != (uint32_t) OS_currentTCB()) {
		OS_wait((uint32_t *) mutex_p, OS_checkCode());
		OS_mutex_acquire(mutex_p);
	}
	mutex_p->counter += 1;
}

/* Releases the mutex after a task has finished its operation.
 * Takes a pointer to a mutex structure as an argument.
 * Decrements the counter of the mutex structure by 1.
 * If the counter is 0, then the TCB pointer field is set to 0, 
 * 	and the notify callback is run, giving the mutex as a reason.
 */
void OS_mutex_release(OS_mutex_t * mutex_p) {
	mutex_p->counter -= 1;
	if (mutex_p->counter == 0) {
		mutex_p->tcb_p = 0;
		OS_notify(mutex_p);
	}
}
