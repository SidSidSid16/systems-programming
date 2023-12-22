#ifndef MUTEX_H
#define MUTEX_H

#define OS_INTERNAL

#include "OS/os.h"
#include "OS/scheduler.h"
#include "OS/heap.h"

/* Defines the maximum number of waiting tasks for a mutex: 
		The heap must be initialised by specifying a memory size.
		10 seems to be a reasonable size since this is an embedded OS and there shouldn't be too many tasks
		running and requiring a mutex simultaneously. However, this can be easily increased or decreased.*/
#define _OS_MUTEX_WAITINGHEAP_SIZE 10

typedef struct s_OS_mutex_t {
	// the task that owns this mutex
	OS_TCB_t * task;
	// counter to track the number of acquisitions
	uint32_t acquireCounter;
	// counter to track the number of task notify calls
	uint32_t notificationCounter;
	// heap to store waiting task in order of priority (highest = first)
	OS_heap_t waiting_heap;
	// a memory store for the heap
	void * waiting_heapStore[_OS_MUTEX_WAITINGHEAP_SIZE];
} OS_mutex_t;

void OS_mutex_initialise(OS_mutex_t * mutex);

void OS_mutex_acquire(OS_mutex_t * mutex);
void OS_mutex_release(OS_mutex_t * mutex);

void OS_notifyMutex(OS_mutex_t * mutex);

#endif /* MUTEX_H */
