#include "os.h"
#include <stdio.h>
#include "utils/config.h"
#include "mutex.h"
#include "fixedPriority.h"
#include "queue.h"

static OS_mutex_t mutex_p;
static OS_TCB_t TCB1, TCB2;

/*
 * This task demonstrates the sending part
 * of the task communication queue, where
 * it sends a message to TCB2.
 * Also demonstrates the higher priority
 * of this task by being called over the 
 * lower priority but more frequency TCB2.
 */
void messageSender(void const *const args) {
	while(1) {
		// Sleep used so as to not flood the console
		OS_sleep(1000);
		printf("Task 1\r\n");
		
		// Instantiating a message
		OS_message message;
		OS_initMessage(&message);
		uint32_t dataSend = 42;
		message.data = &dataSend;
		message.recipient = &TCB2;
		
		// Sending the message
		OS_sendMessage(&message);
	}
}

/*
 * This task demonstrates the receiving
 * part of the task communciation queue,
 * where it receives the task from TCB1.
 */
void messageReciever(void const *const args) {
	while(1) {
		OS_sleep(500);
		// Recieving the message
		uint32_t * message = OS_readMessage();
		if (message != 0) {
			// Casting to int for demonstration
			printf("Task 2, message recieved: %d\r\n", (int) *message);
		} else {
			printf("Task 2, no message recieved\r\n");
		}
	}
}

/* MAIN FUNCTION */
int main(void) {
	/* Set up core clock and initialise serial port */
	config_init();

	printf("\r\n Systems Programming for ARM Assignment \r\n");

	/* Reserve memory for two stacks and two TCBs.
	   Remember that stacks must be 8-byte aligned. */
	__align(8)
	static uint32_t stack1[128], stack2[128];

	/* Initialise the TCBs using the two functions above */
	OS_initialiseTCB(&TCB1, stack1+64, messageSender, 0);
	OS_initialiseTCB(&TCB2, stack2+64, messageReciever, 0);
	
	TCB1.priority = 3;
	TCB2.priority = 1;
	
	/* Setup the mutex */
	OS_mutex_init(&mutex_p);
	
	/* Initialise and start the OS */
	OS_init(&fixedPriorityScheduler);
	OS_mutex_init(&mutex_p);
	OS_addTask(&TCB1);
	OS_addTask(&TCB2);
	
	OS_start();
}
