#ifndef QUEUE_H
#define QUEUE_H

#include "os.h"
#include "stm32f3xx.h"

/* Maximum number of messages to be stored in the communication queue */
#define COMM_QUEUE_MAX_MESSAGES 4

/* Structure for a message
 * Contains a pointer to the data its sending
 * Contains a pointer-to-OS_TCB_t as a pointer to the recipient
 */
typedef struct {
	uint32_t * data;
	OS_TCB_t * recipient;
} OS_message;

void OS_initMessage(OS_message * message);

void OS_sendMessage(OS_message * message);

uint32_t * OS_readMessage(void);

#endif /* QUEUE_H */
