#include "OS/mutex.h"

void OS_mutex_acquire(OS_mutex_t * mutex) {
	// get the current OS task and store it
	OS_TCB_t *currentTCB = OS_currentTCB();
	do {
		// get and store the notification count
		uint32_t notificationCount = OS_notificationCount_get();
		// load in the mutex's TCB field
		OS_TCB_t * mutexTask = (OS_TCB_t *) __LDREXW ((uint32_t volatile *)&(mutex->task));
		// check if mutex task field is unset or if set, it's the same as the current OS task
		if (!mutexTask || (mutexTask == currentTCB)) {
			// if there is no task in the mutex or if the mutex task is the same as the current
			// OS task, we can loop around back to the LDREX with the continue. By using continue,
			// we jump to the do-while condition which is the STREX function
			continue;
		}
		// If there is a task in the mutex, and it is not the current OS task (the previoud if)
		// statement would've caught it if this condition wasn't met), we induce an OS wait, and
		// we can loop around back to the start (LDREX instruction) by clearing the EX flag.
		OS_wait(notificationCount);
		// By clearing the EX flag, we can induce the STREX to fail, this will cause the do-while
		// to loop around back to the 'do' logic.
		__CLREX();
	} while (__STREXW ((uint32_t)currentTCB, (uint32_t *)&(mutex->task)));
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
		OS_yield();
	}
}
