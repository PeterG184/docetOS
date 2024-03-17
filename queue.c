#include "queue.h"

/* Message Queue - an array of OS_message structures */
static OS_message * messageQueue[COMM_QUEUE_MAX_MESSAGES] = {0};

/* Index for the queue - the position of the next available spot  */
static uint32_t index = 0;

/* Static Function */
static void dequeue(uint32_t messageIndex);

/* Initialises a message structure
 * Takes a pointer to a message, returns nothing
 * Sets the data and recipient fields of that structure to 0
 */
void OS_initMessage(OS_message * message) {
	message->data = 0;
	message->recipient = 0;
}

/* Adds a message to the queue - allows a task to 'send' messages.
 * Takes a pointer to a message, returns nothing 
 * Checks that the queue is not full, and if not then adds the pointer
 *  to the queue at the next available point, and increments index.
 */
void OS_sendMessage(OS_message * message) {
	if(index < COMM_QUEUE_MAX_MESSAGES) { 
		messageQueue[index] = message;
		index += 1;
	}
}

/* Returns a message to the current TCB - allows a task to 'read' messages.
 * Takse nothing, returns a uint32_t.
 * Creates a temporary variable initialised to 0 in case there is no data to return
 * Loops through the queue checking if the current TCB is the recipient of 
 *  any of the messages. If it is, then the message is dequeued and the data returned.
 */
uint32_t * OS_readMessage(void) {
	uint32_t * returnData = 0;
	for (int i = 0; i <= index; i++) {
		if ((uint32_t) messageQueue[i]->recipient == (uint32_t) OS_currentTCB()) {
			returnData = messageQueue[i]->data;
			dequeue(i);
			return returnData;
		}
	}
	// If it gets to here than there is no messages in the queue for the task
	return returnData;
}

/* Deqeues a given message from the queue
 * Takes an integer as the element to dequeue, returns nothing
 * Sets the index of the message to 0, and then shuffles every other
 *  element down the queue.
 */
static void dequeue(uint32_t messageIndex) {
	messageQueue[messageIndex] = 0;
	for (int i = messageIndex; i < COMM_QUEUE_MAX_MESSAGES - 1; ++i) {
		if (messageQueue[i] == 0) {
			index = i;
			break;
		} else {
			messageQueue[i] = messageQueue[i + 1];
			index = i;
		}
	}
}
