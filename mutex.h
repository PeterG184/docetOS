#ifndef MUTEX_H
#define MUTEX_H

#include "os.h"
#include "cmsis_armcc.h"

/* Struct for a mutex
 * tcb_p is a pointer to the TCB that owns the mutex
 * counter is the number of times the owner has acquired 
 * 	the mutex minus how many times it has released.
 * When the mutex is released and the counter reaches 0,
 *  the tcp_p field is set to 0 too, in order to allow
 *  a new TCB to acquire the mutex.
 */
typedef struct {
	OS_TCB_t * tcb_p;
	uint32_t counter;
} OS_mutex_t;

void OS_mutex_init(OS_mutex_t * mutex_p);

void OS_mutex_acquire(OS_mutex_t * mutex_p);

void OS_mutex_release(OS_mutex_t * mutex_p);

#endif /* MUTEX_H */
