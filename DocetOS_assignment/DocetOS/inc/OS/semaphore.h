#ifndef SEMAPHORE_H
#define SEMAPHORE_H

#define OS_INTERNAL

#include "OS/os.h"
#include "OS/scheduler.h"

/* Mask to extract the last 9 bits (IPSR) of xPSR */
#define PSR_IPSR_HANDLER_MASK 0x1FF

typedef struct s_OS_semaphore_t {
	// counter to track the number of acquisitions
	uint32_t tokenCounter;
	// counter to track the number of task notify calls
	uint32_t notificationCounter;
	// singly-linked list to hold waiting tasks
	_OS_tasklist_t waiting_list;
} OS_semaphore_t;

/* A function that initialises a semaphore, addressed by a pointer, in preparation for use,
	 allowing for the use of a semaphore directly from a semaphore pointer type. */
void OS_semaphore_initialise(OS_semaphore_t * semaphore, uint32_t totalTokens);
/* A function that can be called by a task to acquire a semaphore. */
void OS_semaphore_acquire(OS_semaphore_t * semaphore);
/* A function that can be called by a task to release a semaphore. */
void OS_semaphore_release(OS_semaphore_t * semaphore);
/* A function that notifies a task on semaphore release. */
void _OS_semaphore_notify(OS_semaphore_t * semaphore);

#endif /* SEMAPHORE_H */
