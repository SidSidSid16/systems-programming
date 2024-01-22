#include "OS/semaphore.h"

#include "stm32f4xx.h"

/* A function that initialises a semaphore, addressed by a pointer, in preparation for use,
	 allowing for the use of a semaphore directly from a semaphore pointer type. */
void OS_semaphore_initialise(OS_semaphore_t * semaphore, uint32_t totalTokens) {
	semaphore->tokenCounter = totalTokens;
	semaphore->notificationCounter = 0;
	semaphore->waiting_list.head = 0;
}

/* A function that a task can use to acquire a semaphore, addressed by a pointer. Exclusively
	 loads the semaphore token counter to ensure thread safety. If the semaphore has tokens
	 available, the token count is decremented and stored exclusively. If the semaphore has no
	 tokens left, the semaphore-based wait delegate function is called to send the requesting
	 task to the waiting list. */
void OS_semaphore_acquire(OS_semaphore_t * semaphore) {
	while (1) {
		// get and store the current semaphore notification count
		uint32_t checkCode = semaphore->notificationCounter;
		// load in the semaphores's tockenCounter field
		uint32_t tokens = __LDREXW ((uint32_t volatile *)&(semaphore->tokenCounter));
		// check if the token count is non-zero (whether there are any available)
		if (tokens) {
			// try to exclusively store the decremented token counter field
			if (!(__STREXW ((uint32_t)(--tokens), (uint32_t *)&(semaphore->tokenCounter)))) {
				// if STREXW succeeds, then current TCB has acquired the mutex, break out of while loop
				break;
			}
			// if STREX fails, the semaphore was obtained during this logic. Keep iterating while loop
		} else {
			// if the number of available tokens is zero, the requesting task must wait
			OS_semaphore_wait((uint32_t)semaphore, checkCode);
		}
	}
}

/* A function that can be called by any task or ISR to release a semaphore, addressed by a
	 pointer. Exclusively loads the semaphore token counter to then decrement and exclusively
	 store ensuring thread safety. On successful semaphore release, a waiting task is notified. */
void OS_semaphore_release(OS_semaphore_t * semaphore) {
	while (1) {
		// exclusively load the token counter field of semaphore
		uint32_t tokens = __LDREXW ((uint32_t volatile *)&(semaphore->tokenCounter));
		// try to exclusively store the incremented token counter field
		if (!(__STREXW ((uint32_t)(++tokens), (uint32_t *)&(semaphore->tokenCounter)))) {
			// if stored successfully, break out of while loop, otherwise keep trying
			break;
		}
	}
	/* breaking out of while loop signifying successful token increase, thus semaphore release,
		 therefore we can notify a waiting task. */
	_OS_semaphore_notify(semaphore);
	/* After notifying waiting task, we invoke a context switch to prevent a spinlock as a task
		 may immediately re-acquire the semaphore after releasing in a tight loop. In order to ensure
		 successful yielding in both standard functions and ISRs, logic must detect whether the CPU
		 is in handler-mode (ISR execution) or thread-mode (most standard code). If the CPU is in
		 handler-mode, a manual PendSV bit set must occur, otherwise, the yield delegate can be
		 called. The IPSR register the exception number of the exception being processed, with the
		 field set to 0 if there is no active interrupt. */
	uint32_t handlerMode = __get_IPSR();
	if (handlerMode) {
		// set PendSV bit to invoke context switch
		SCB->ICSR = SCB_ICSR_PENDSVSET_Msk;
	} else {
		// call yield delegate to invoke context switch
		OS_yield();
	}
}

/* A function that notifies a waiting task of semaphore release. Function takes in a pointer
	 to the semaphore. */
void _OS_semaphore_notify(OS_semaphore_t * semaphore) {
	while (1) {
		// exclusively load the notification counter field of semaphore
		uint32_t notifications = __LDREXW ((uint32_t volatile *)&(semaphore->notificationCounter));
		// try to exclusively store the incremented notification counter field
		if (!(__STREXW ((uint32_t)(++notifications), (uint32_t *)&(semaphore->notificationCounter)))) {
			// if stored successfully, break out of while loop, otherwise keep trying
			break;
		}
	}
	// once counter has incremented, pop tail
	OS_TCB_t * waitingTask = list_pop_tail_sl(&(semaphore->waiting_list));
	// check if there is a waiting task present
	if (waitingTask) {
		// move this task to the pending list
		list_push_sl(&pending_list, waitingTask);
	}
}
