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
	OS_TCB_t * task;
	uint32_t counter;
	OS_heap_t waiting_heap;
} OS_mutex_t;


OS_mutex_t OS_createMutex(void);

void OS_mutex_acquire(OS_mutex_t * mutex);
void OS_mutex_release(OS_mutex_t * mutex);

#endif /* MUTEX_H */
