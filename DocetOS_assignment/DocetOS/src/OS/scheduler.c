#define OS_INTERNAL

#include "OS/scheduler.h"
#include "OS/os.h"
#include "OS/heap.h"
#include "OS/mutex.h"
#include "OS/semaphore.h"

#include "stm32f4xx.h"
#include <string.h>


/* This is an implementation of an extremely simple round-robin scheduler.

   A task list structure is declared.  Tasks are added to the list to create a circular buffer.
	 When tasks finish, they are removed from the list.  When the scheduler is invoked, it simply
	 advances the head pointer, and returns it.  If the head pointer is null, a pointer to the
	 idle task is returned instead.
	 
	 The scheduler is reasonably efficient but not very flexible.  The "yield" flag is not
	 checked, but merely cleared before a task is returned, so OS_yield() is equivalent to
	 OS_schedule() in this implementation.
*/

/* An array of doubly-linked lists to contain active tasks in each priority levels for scheduler */
static _OS_tasklist_t task_list[_OS_PRIORITY_LEVELS];

/* singly-linked lists to contain pending tasks */
_OS_tasklist_t pending_list = {.head = 0};

/* A generic heap is implemented to hold the list of sleeping tasks. 

	 Since this is a generic heap, a use-case-specialised comparator function must be present. In
	 this case, the function compares the wake time in the TCB's data field. */
static int_fast8_t heapComparator (void * task1, void * task2) {
	uint32_t wakeTime1 = ((OS_TCB_t*)task1)->data;
	uint32_t wakeTime2 = ((OS_TCB_t*)task2)->data;
	return (int_fast8_t)(wakeTime1 - wakeTime2);
}
/* A memory store is initialised, with a size predefined in the scheduler header file, and the
	 heap itself is initialised using the store and comparator function. */
static void *heapStore[_OS_SLEEPINGHEAP_SIZE];
static OS_heap_t sleeping_heap = OS_HEAP_INITIALISER(heapStore, heapComparator);

/* A function to add a task to the start of a doubly linked list whilst preserving the head,
	 used in the scheduler's round-robin task list. */
static void _list_add(_OS_tasklist_t *list, OS_TCB_t *task) {
	if (!(list->head)) {
		// if list is empty, the new task becomes the head of the list
		// link the new task to itself
		task->next = task;
		task->prev = task;
		// new task becomes head of the list
		list->head = task;
	} else {
		// if list is not empty, preserve the head and add new item at the start
		// link the next element of new task to old head
		task->next = list->head;
		// link the prev element of new task to list tail
		task->prev = list->head->prev;
		// break old tail-to-head link
		task->prev->next = task;
		// break old head-to-tail link
		list->head->prev = task;
	}
}

/* A function to remove a task from a doubly linked list,
	 used in the sheduler's round-robin task list. */
static void _list_remove(_OS_tasklist_t *list, OS_TCB_t *task) {
	// check if task being removed is the only task in the list
	if (task->next == task) {
		// if it is the only task, we make the list empty
		list->head = 0;
		// and removal has finished, thus we return
		return;
	} else if (list->head == task) {
		// check if the task being deleted is at the head
		// if it is then set the next task as the head
		list->head = task->next;
	}
	// the prev of the next task is linked to the prev of the deletion task
	task->next->prev = task->prev;
	// the next of the prev task is linked to the next of the deletion task
	task->prev->next = task->next;
}

/* Function to push an item into the head of a singly-linked (sl) list */
void list_push_sl(_OS_tasklist_t *list, OS_TCB_t *task) {
	/* We want to load the head atomically so we use LDREX/STREX: 
		 LDREX atomically loads the value from a location and sets
		 an exclusive flag simultaneously.

		 STREX then stores the value to a memory location, only if
	   the flag is still valid. It will fail to store to a location
	   if the location has been modified between the LDR and STR.
	
		 When STREX fails, it will return '1', the do-while loop will
	   iterate the do-logic where the LDREX instruction occurs until
	   the STREX is successful.
	
			The 'W' at the end of STREX and LDREX signifies a word of data.*/
	do {
		// First LDREX the pointer to the head of the list into a pointer variable
		OS_TCB_t *head = (OS_TCB_t *) __LDREXW ((uint32_t volatile *)&(list->head));
		// Set the pointer of the old head as the next field of the task to add
		task->next = head;
	}
	// Repeat do-logic until STREX returns a '0' signifying a successful store.
	while (__STREXW ((uint32_t) task, (uint32_t volatile *)&(list->head)));
}

/* Function to pop an item from the head of a singly-linked (sl) list */
OS_TCB_t * list_pop_head_sl(_OS_tasklist_t * list) {
	/* cache the oldHead (current head of the list that needs popping) and newHead
		 (the item that needs to become the head after the old head is popped). */
	OS_TCB_t * oldHead = NULL;
	do {
		// get the current head of the list
		oldHead = (OS_TCB_t *) __LDREXW ((uint32_t volatile *)&(list->head));
		// if the list is empty, we want to break out of the do-while to return NULL
		if (!oldHead) {
			// if we break early, we need to clear the flag since the STREX will not run
			__CLREX();
			return NULL;
		}
	}
	// do-logic is iterated until the new head is successfully stored as the list head.
	while (__STREXW ((uint32_t) oldHead->next, (uint32_t volatile *)&(list->head)));
	// we can return the popped task that was once the head of the list
	return oldHead;
}

/* Function to pop an item from the tail of a singly-linked (sl) list */
OS_TCB_t * list_pop_tail_sl(_OS_tasklist_t * list) {
	/* initialise the current and next items of the list to help with traversing the list
		 with exclusive store and load intrinsics. */
	OS_TCB_t * prev = NULL;
	OS_TCB_t * current = NULL;
	do {
		// ensure that previous item is still NULL
		prev = NULL;
		// get the head of the list
		current = (OS_TCB_t *) __LDREXW ((uint32_t volatile *)&(list->head));
		// if there is no items in the list, we need to exit the do-while
		if (!current) {
			// since we're breaking early, the exclusive flags must be cleared
			__CLREX();
			return NULL;
		}
		// traverse the list to find the last item, ensure current and next items exist
		while (current->next) {
			// cache the current before overwriting it
			prev = current;
			// load in the next value as the current
			current = current->next;
		}
		/* if the while loop above exits with prev still set to null, it means there's
			 only one element in the list. */
		if (!prev && current) {
			// try to exclusively store the reset list
			if (!(__STREXW (0, (uint32_t volatile *)&(list->head)))) {
				// if STREXW succeeds, then return the only item in the list to break out
				return current;
			} else {
				/* if the STREX fails, then the list has been modified, therefore, the
					 do-while logic must be repeated to parse the new and updated list. */
				continue;
			}
		}
		/* reaching this point in the code signifies that there is no next element,
			 therefore, we are at the end of the list. We can try to reset and exclusively
			 store the prev item's next field to remove the tail. */
	} while ((__STREXW (0, (uint32_t volatile *)&(prev->next))));
	// return the tail of the list
	return current;
}

/* Round-robin scheduler */
OS_TCB_t const * _OS_schedule(void) {
	// check if there are any sleeping tasks and check if any needs to be awakened
	while (!OS_heap_isEmpty(&sleeping_heap) && ((OS_TCB_t *)sleeping_heap.heapStore[0])->data <= OS_elapsedTicks()) {
		OS_TCB_t *taskToWake = OS_heap_extract(&sleeping_heap);
		_list_add(&task_list[taskToWake->priority], taskToWake);
	}
	// remove all pending tasks until that list is empty and place them into the round-robin
	while (pending_list.head) {
		// since task_list is doubly-linked, we use list add, pending_list is popped with the
		// singly-linked (sl) pop function
		OS_TCB_t *taskToRun = list_pop_head_sl(&pending_list);
		_list_add(&task_list[taskToRun->priority], taskToRun);
	}
	// iterate for each priority level
	for (uint_fast8_t i = 0; i < _OS_PRIORITY_LEVELS; i++) {
		// check if there are any scheduled tasks for this priority level
		if (task_list[i].head) {
			// move the head over by one in the scheduler
			task_list[i].head = task_list[i].head->next;
			// task can be returned, reset sleep flag if set to 1, and reset yield flag
			task_list[i].head->state &= ~(TASK_STATE_SLEEP | TASK_STATE_YIELD);
			// return the task
			return task_list[i].head;
		}
	}
	/* if all priority levels have been iterated through and no task is scheduled, then we
	   return the idle task */
	return _OS_idleTCB_p;
}

/* Initialises a task control block (TCB) and its associated stack.  See os.h for details. */
void OS_initialiseTCB(OS_TCB_t * TCB, uint32_t * const stack, void (* const func)(void const * const), void const * const data, uint_fast8_t const priority) {
	TCB->sp = stack - (sizeof(_OS_StackFrame_t) / sizeof(uint32_t));
	TCB->state = 0;
	TCB->prev = TCB->next = 0;
	// check if priority has been passed and if it's a valid number
	if (!priority || (priority > _OS_PRIORITY_LEVELS)) {
		// if it's invalid, assign the lowest priority (highest number)
		TCB->priority = _OS_PRIORITY_LEVELS - 1;
	} else {
		// user entered priority is 1-indexed, however, arrays are 0-indexed.
		TCB->priority = priority - 1;
	}
	// initialise to ensure priority level is restored after inheritance promotion
	TCB->originalPriority = TCB->priority;
	_OS_StackFrame_t *sf = (_OS_StackFrame_t *)(TCB->sp);
	/* By placing the address of the task function in pc, and the address of _OS_task_end() in lr, the task
	   function will be executed on the first context switch, and if it ever exits, _OS_task_end() will be
	   called automatically */
	*sf = (_OS_StackFrame_t) {
		.r0 = (uint32_t)(data),
		.r1 = 0,
		.r2 = 0,
		.r3 = 0,
		.r4 = 0,
		.r5 = 0,
		.r6 = 0,
		.r7 = 0,
		.r8 = 0,
		.r9 = 0,
		.r10 = 0,
		.r11 = 0,
		.r12 = 0,
		.lr = (uint32_t)_OS_task_end,
		.pc = (uint32_t)(func),
		.psr = xPSR_T_Msk  /* Sets the thumb bit to avoid a big steaming fault */
	};
}

/* 'Add task' */
void OS_addTask(OS_TCB_t * const tcb) {
	_list_add(&task_list[tcb->priority], tcb);
}

/* SVC handler that's called by _OS_task_end when a task finishes.  Removes the
   task from the scheduler and then queues PendSV to reschedule. */
void _OS_taskExit_delegate(void) {
	// Remove the given TCB from the list of tasks so it won't be run again
	OS_TCB_t * tcb = OS_currentTCB();
	_list_remove(&task_list[tcb->priority], tcb);
	SCB->ICSR = SCB_ICSR_PENDSVSET_Msk;
}

/* Since delegate functions are branched to and not directly accessed via C
   function calls, the prototype does not need to be in the header file, they
   can be placed right above the function for readability. */
void _OS_mutex_wait_delegate(_OS_SVC_StackFrame_t * stack);
/* SVC handler that removes the current task from the round robin and inserts it
	 into the priority level sorted mutex-specific heap. If the mutex-holding task
	 has a lower priority than the task entering the waiting list, the mutex-holder
	 gains a priority level promotion to ensure speedy release. */
void _OS_mutex_wait_delegate(_OS_SVC_StackFrame_t * stack) {
	// get the mutex that the task needs to wait for
	OS_mutex_t * mutex = (OS_mutex_t *) stack->r0;
	/* the notifcation counter check code is passed in via the stacked r0
	   we can extract it by type casting to _OS_SVC_StackFrame_t first. */
	uint32_t checkCode = stack->r1;
	/* Only continue if the check code matches the mutex notification counter
	   if the check code differs from the global notification counter, then it
	   means that the notify function was called, and thus the wait cannot happen
	   since the lists differ. */
	if (mutex->notificationCounter == checkCode) {
		// get the current task and cache it
		OS_TCB_t * currentTask = OS_currentTCB();
		// get the mutex-holding task and cache it
		OS_TCB_t * mutexTask = mutex->task;
		// remove this task from the round robin
		_list_remove(&task_list[currentTask->priority], currentTask);
		// add the current task to the mutex wait heap
		OS_heap_insert(&mutex->waiting_heap, currentTask);
		/* Priority inheritance logic: promote mutex-holder if requesting task is
			 of higher priority. Remembering that higher priority = smaller priority
			 numbers. */
		if (mutexTask->priority > currentTask->priority) {
			// remove mutex-holder from task list
			_list_remove(&task_list[mutexTask->priority], mutexTask);
			// promote the priority of the mutex-holder
			mutexTask->priority = currentTask->priority;
			// add the mutex-holder to the pending list for scheduler to sweep and schedule
			list_push_sl(&pending_list, mutexTask);
		}
		// set PendSV bit to invoke context switch
		SCB->ICSR = SCB_ICSR_PENDSVSET_Msk;
	}
}

/* Since delegate functions are branched to and not directly accessed via C
   function calls, the prototype does not need to be in the header file, they
   can be placed right above the function for readability. */
void _OS_semaphore_wait_delegate(_OS_SVC_StackFrame_t * stack);
/* SVC handler that removes the current task from the round robin and adds to the
	 semaphore-specific singly-linked waiting list. */
void _OS_semaphore_wait_delegate(_OS_SVC_StackFrame_t * stack) {
	// get the semaphore that the task needs to wait for
	OS_semaphore_t * semaphore = (OS_semaphore_t *) stack->r0;
	/* the notifcation counter check code is passed in via the stacked r0
	   we can extract it by type casting to _OS_SVC_StackFrame_t first. */
	uint32_t checkCode = stack->r1;
	/* Only continue if the check code matches the semaphore notification counter
	   if the check code differs from the global notification counter, then it
	   means that the notify function was called, and thus the wait cannot happen
	   since the lists differ. */
	if (semaphore->notificationCounter == checkCode) {
		// get the current task and cache it
		OS_TCB_t * currentTask = OS_currentTCB();
		// remove this task from the round robin
		_list_remove(&task_list[currentTask->priority], currentTask);
		// add the current task to the semaphore wait heap
		list_push_sl(&semaphore->waiting_list, currentTask);
		// set PendSV bit to invoke context switch
		SCB->ICSR = SCB_ICSR_PENDSVSET_Msk;
	}
}

/* Since delegate functions are branched to and not directly accessed via C
   function calls, the prototype does not need to be in the header file, they
   can be placed right above the function for readability. */
void OS_sleep_delegate(_OS_SVC_StackFrame_t * stack);
/* Function to put a task to sleep for a number of ticks */
void OS_sleep_delegate(_OS_SVC_StackFrame_t * stack) {
	// Get the sleep duration that's been passed in
	uint32_t sleepDuration = stack->r0;
	// The running task's TCB is retrieved and stored
	OS_TCB_t * currentTask = OS_currentTCB();
	// wakeTime time is calculated by adding sleepDuration to the elapsed OS ticks
	uint32_t wakeTime = OS_elapsedTicks() + sleepDuration;
	// wakeTime can be stored in TCB in the data field
	currentTask->data = wakeTime;
	// Set the TCB state to sleeping
	currentTask->state |= TASK_STATE_SLEEP;
	// Remove the sleeping task from the scheduler's task list
	_list_remove(&task_list[currentTask->priority], currentTask);
	// Place the just removed task into the heap
	OS_heap_insert(&sleeping_heap, currentTask);
	// Call PendSV to invoke _OS_scheduler to start the next task
	SCB->ICSR = SCB_ICSR_PENDSVSET_Msk;
}

/* Since delegate functions are branched to and not directly accessed via C
   function calls, the prototype does not need to be in the header file, they
   can be placed right above the function for readability. */
void _OS_priorityRestore_delegate(_OS_SVC_StackFrame_t * stack);
/* Function to restore a task's original priority level after a temporary
	 priority promotion from the mutex inheritance implementation. */
void _OS_priorityRestore_delegate(_OS_SVC_StackFrame_t * stack) {
	// get the TCB that needs it's priority restored from the stack
	OS_TCB_t * task = (OS_TCB_t *) stack->r0;
	// check if task needs priority restoration
	if (task->priority != task->originalPriority){
		// remove the task from scheduler task list
		_list_remove(&task_list[task->priority], task);
		// restore the task's original priority
		task->priority = task->originalPriority;
		// add the task to the pending list for scheduler to sweep and schedule
		list_push_sl(&pending_list, task);
	}
}
