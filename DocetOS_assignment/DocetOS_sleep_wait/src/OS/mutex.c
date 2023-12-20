#include "OS/mutex.h"

/* A generic heap is implemented to hold the list of tasks waiting for this mutex. 

	 Since this is a generic heap, a use-case-specialised comparator function must be present. In
	 this case, the function compares the priority value in the TCB's data field. Higher priority
	 tasks (denoted with a smaller numeric value) will be ordered first.
*/
static int_fast8_t heapComparator (void * task1, void * task2) {
	uint32_t taskPriority1 = ((OS_TCB_t*)task1)->priority;
	uint32_t taskPriority2 = ((OS_TCB_t*)task2)->priority;
	return (int_fast8_t)(taskPriority1 - taskPriority2);
}

OS_mutex_t OS_createMutex() {
	static void *heapStore[_OS_MUTEX_WAITINGHEAP_SIZE];
	static OS_heap_t waiting_heap = OS_HEAP_INITIALISER(heapStore, heapComparator);
	OS_mutex_t mutex = {.counter = 0, .task = 0, .waiting_heap = waiting_heap};
	return mutex;
}

void OS_mutex_acquire(OS_mutex_t * mutex) {
	// get the current OS task and store it
	OS_TCB_t *currentTCB = OS_currentTCB();
	while (1) {
		// get and store the current OS notification count
		uint32_t currentNotificationCount = OS_notificationCount_get();
		// load in the mutex's TCB field
		OS_TCB_t * mutexTask = (OS_TCB_t *) __LDREXW ((uint32_t volatile *)&(mutex->task));
		// if the mutex's TCB field is unset, we can acquire the mutex
		if (!mutexTask) {
			// try to use exclusive store for TCB to get ownership of the mutex
			if (!(__STREXW ((uint32_t)currentTCB, (uint32_t *)&(mutex->task)))) {
				// if STREXW succeeds, then current TCB has acquired the mutex, break out of while loop
				break;
			}
			// if STREXW fails, then mutex is already aquired, keep iterating the while loop
		} else if (mutexTask != currentTCB) {
			// if the mutex is already acquired by another task, we can send it to the wait list
			OS_wait(currentNotificationCount);
		} else if (mutexTask == currentTCB) {
			// if the mutex is acquired by the same task, we can just increment the counter
			break;
		}
	}
	// Once everything above is finished, we can increment the counter in the mutex
	mutex->counter++;
}

void OS_mutex_release(OS_mutex_t * mutex) {
	// check if the mutex is owned by the task that is calling this function
	if (mutex->task == OS_currentTCB()) {
		// we decrement the counter in the mutex
		mutex->counter--;
		// if the counter is now at 0..
		if (!mutex->counter) {
			// we reset the task field
			mutex->task = 0;
			// notify the OS
			OS_notifyAll();
		}
		/* prevents a spinlock as a task may immediately re-acquire the mutex after
		   releasing in a tight loop. */
		OS_yield();
	}
}
