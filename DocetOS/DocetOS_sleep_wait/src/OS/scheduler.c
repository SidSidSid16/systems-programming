#define OS_INTERNAL

#include "OS/scheduler.h"
#include "OS/os.h"

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

static _OS_tasklist_t task_list = {.head = 0};

// A singly-linked list to contain waiting tasks
static _OS_tasklist_t wait_list = {.head = 0};

static void list_add(_OS_tasklist_t *list, OS_TCB_t *task) {
	if (!(list->head)) {
		task->next = task;
		task->prev = task;
		list->head = task;
	} else {
		task->next = list->head;
		task->prev = list->head->prev;
		task->prev->next = task;
		list->head->prev = task;
	}
}

static void list_remove(_OS_tasklist_t *list, OS_TCB_t *task) {
	if (task->next == task) {
		list->head = 0;
		return;
	} else if (list->head == task) {
		list->head = task->next;
	}
	task->next->prev = task->prev;
	task->prev->next = task->next;
}

/* Function to push an item into a singly-linked (sl) list */
static void list_push_sl(_OS_tasklist_t *list, OS_TCB_t *task) {
	/* We want to load the head atomically so we use LDREX/STREX: 
		 LDREX atomically loads the value from a location and tags
     the location simulatenously.

		 STREX then stores the value to a memory location, only if
	   the tag is still valid. It will fail to store to a location
	   if the location has been modified between the LDR and STR.
	
		 When STREX fails, it will return '1', the do-while loop will
	   iterate the do-logic where the LDREX instruction occurs until
	   the STREX is successful.
	
			The 'W' at the end of STREX and LDREX signifies a word of data.*/
	do {
		// First LDREX the pointer to the head of the list into a pointer variable
		OS_TCB_t *head = (OS_TCB_t *) __LDREXW ((uint32_t *)&(list->head));
		// Set the pointer of the old head as the next field of the task to add
		task->next = head;
	}
	// Repeat do-logic until STREX returns a '0' signifying a successful store.
	while (__STREXW ((uint32_t) task, (uint32_t *)&(list->head)));
}

static OS_TCB_t * list_pop_sl(_OS_tasklist_t *list) {
	// cache the oldHead (current head of the list that needs popping) and newHead
	// (the item that needs to become the head after the old head is popped)
	OS_TCB_t *oldHead = NULL;
	OS_TCB_t *newHead = NULL;
	do {
		// get the current head of the list
		oldHead = (OS_TCB_t *) __LDREXW ((uint32_t *)&(list->head));
		// if the list is empty, we want to break out of the do-while to return NULL
		if (!oldHead) {
			break;
		}
		// if the list does have a head, we can set it to point to the next field of the
		// head.
		newHead = oldHead->next;
		// clear the next field of the old head so that there aren't any dangling pointers
		oldHead->next = NULL;
	}
	// do-logic is iterated until the new head is successfully stored as the list head.
	while (__STREXW ((uint32_t) newHead, (uint32_t *)&(list->head)));
	// we can return the popped task that was once the head of the list
	return oldHead;
}

/* Round-robin scheduler */
OS_TCB_t const * _OS_schedule(void) {
	// check if there are any scheduled tasks, return idle task if there are none
	if (task_list.head) {
		// cache the head task item on entry to run the check to see if we loop over the round robin
		OS_TCB_t * headOnEntry = task_list.head;
		do {
			// move the head over by one in the scheduler
			task_list.head = task_list.head->next;
			// check if the task is not sleeping with the wake time in the future
			if (!((task_list.head->state & TASK_STATE_SLEEP) && *(uint32_t *)task_list.head->data > OS_elapsedTicks())) {
				// this task can be returned, reset sleep flag if set to 1, and reset yield flag
				task_list.head->state &= ~TASK_STATE_SLEEP;
				task_list.head->state &= ~TASK_STATE_YIELD;
				return task_list.head;
			}
		}
		// do-while runs until the round robin has been looped over completely
		while (headOnEntry != task_list.head);
		// once looped over completely, return the idle task
		return _OS_idleTCB_p;
	}
	// return idle task if there are not tasks scheduled
	return _OS_idleTCB_p;
}

/* Initialises a task control block (TCB) and its associated stack.  See os.h for details. */
void OS_initialiseTCB(OS_TCB_t * TCB, uint32_t * const stack, void (* const func)(void const * const), void const * const data) {
	TCB->sp = stack - (sizeof(_OS_StackFrame_t) / sizeof(uint32_t));
	TCB->state = 0;
	TCB->prev = TCB->next = 0;
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
	list_add(&task_list, tcb);
}

/* SVC handler that's called by _OS_task_end when a task finishes.  Removes the
   task from the scheduler and then queues PendSV to reschedule. */
void _OS_taskExit_delegate(void) {
	// Remove the given TCB from the list of tasks so it won't be run again
	OS_TCB_t * tcb = OS_currentTCB();
	list_remove(&task_list, tcb);
	SCB->ICSR = SCB_ICSR_PENDSVSET_Msk;
}

/* SVC handler that calls list_remove() to remove the current task from the round robin
   and calls list_push_sl() to add the current task to the wait list. PendSV bit is set
   to invoke a context switch */
void _OS_wait_delegate(void) {
	// get the current task and cache it
	OS_TCB_t * currentTask = task_list.head;
	// remove this task from the round robin
	list_remove(&task_list, currentTask);
	// add the current task to the wait list
	list_push_sl(&wait_list, currentTask);
	// set PendSV bit to invoke context switch
	SCB->ICSR = SCB_ICSR_PENDSVSET_Msk;
}
