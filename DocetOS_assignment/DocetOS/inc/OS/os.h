#ifndef _OS_H_
#define _OS_H_

#include "scheduler.h"
#include "cmsis_compiler.h"

/*========================*/
/*      EXTERNAL API      */
/*========================*/

/********************/
/* Type definitions */
/********************/

/* A set of numeric constants giving the appropriate SVC numbers for various callbacks. 
   If this list doesn't match the SVC dispatch table in os_asm.s, BIG TROUBLE will ensue. */
enum OS_SVC_e {
	OS_SVC_ENABLE_SYSTICK=0x00,
	OS_SVC_EXIT,
	OS_SVC_YIELD,
	OS_SVC_SCHEDULE,
	OS_SVC_SLEEP,
	OS_SVC_MUTEX_WAIT,
	OS_SVC_MUTEX_NOTIFY,
	OS_SVC_PRIORITY_RESTORE,
	OS_SVC_SEMAPHORE_WAIT,
};

/***************************/
/* OS management functions */
/***************************/

/* Starts the OS kernel.  Never returns. */
void OS_start(void);

/* Returns a pointer to the TCB of the currently running task. */
OS_TCB_t * OS_currentTCB(void);

/* Returns the number of elapsed systicks since the last reboot (modulo 2^32). */
uint32_t OS_elapsedTicks(void);


/************************/
/* Scheduling functions */
/************************/

/* SVC delegate to yield the current task:
		The current task will be stopped so that the CPU thread can be freed up.
		The yield flag will be set, and the task will stay in the scheduler so that
		it can be run later.
		
		So the yield flag is set, then the PendSV is scheduled by triggering a software
		interrupt.
		
		PendSV_Handler is run in the os_asm.s file as an interrupt. Since PendSV has the
		lowest priority, it'll run when other more important interrupts are completed.
		
		PendSV_Handler is a context switch, hence, there is a push to stack at the start
		and a pop from stack at the end for the r4 and lr. In between this, the _OS_schedule
		function is branched to.
		
		_OS_schedule will the schedule the next task.*/
#define OS_yield() _svc_0(OS_SVC_YIELD)

/* SVC delegate to sleep the current task:
		When called from within a task, the delegate will first calculate the wake up time
	  to wake the task up, this time is stored in the data field of the TCB with the sleep
		field in the status flags TCB field toggled set high. The delegate removes the task
		from the scheduler's DL task list, and adds the task to the sleeping head, sorted by
		order of wake time, soonest wake time at the head. PendSV bit is set to invoke a
		context switch. */
#define OS_sleep(x) _svc_1(x, OS_SVC_SLEEP)

/* SVC delegates for mutual exclusion features:
		The wait delegate function for both re-entrant mutexes (denoted as mutex) and counting
		semaphores will send tasks requesting for acquired mutexes or unavailable semaphores to
		the waiting list within either the mutex structure (generic heap) or the semaphore
		structure (FIFO SL task list). Check code is confirmed prior to invoking a task wait to
		ensure deadlocks are prevented, finally, the PendSV bit is set to invoke a context
		switch.

		The mutex notify function notifies the head of the waiting list of the mutex that the
		mutex has been released and is ready to acquire. The delegate function can be found in
		the mutex souce code.
		
		The priority restore delegate solves priority inversion by granting the mutex-holder the
		priority level of the highest priority waiting task to ensure prompt mutex release. */
#define OS_mutex_wait(x,y) _svc_2(x, y, OS_SVC_MUTEX_WAIT)
#define OS_semaphore_wait(x,y) _svc_2(x, y, OS_SVC_SEMAPHORE_WAIT)
#define OS_mutex_notify(x) _svc_1(x, OS_SVC_MUTEX_NOTIFY)
#define OS_priorityRestore(x) _svc_1(x, OS_SVC_PRIORITY_RESTORE)


/*========================*/
/*      INTERNAL API      */
/*========================*/

/* The following are INTERNAL but are unconditionally included because they're inlined into external API functions. */
/* Operands are forced into named registers according to
 * https://developer.arm.com/documentation/101754/0621/armclang-Reference/armclang-inline-assembler/Forcing-inline-assembly-operands-into-specific-registers */

static inline uint32_t _svc_0(uint32_t const svc) {
	register uint32_t r0 __asm("r0");
	register uint32_t const r7 __asm("r7") = svc;
	__asm (
		"svc #0"
	: "=r" (r0) /* r0 is output only */
	: "r" (r7)  /* r7 is input only */
	: "cc", "memory", "r1", "r2", "r3", "r12", "lr"	/* Clobber list reflects calling convention */
	);
	return r0;
}

static inline uint32_t _svc_1(uint32_t const arg0, uint32_t const svc) {
	register uint32_t r0 __asm("r0") = arg0;
	register uint32_t const r7 __asm("r7") = svc;
	__asm (
		"svc #0"
	: "+&r" (r0) /* r0 is I/O */
	: "r" (r7)   /* r7 is input only */
	: "cc", "memory", "r1", "r2", "r3", "r12", "lr"	/* Clobber list reflects calling convention */
	);
	return r0;
}

static inline uint32_t _svc_2(uint32_t const arg0, uint32_t const arg1, uint32_t const svc) {
	register uint32_t r0 __asm("r0") = arg0;
	register uint32_t const r1 __asm("r1") = arg1;
	register uint32_t const r7 __asm("r7") = svc;
	__asm (
		"svc #0"
	: "+&r" (r0) 					/* r0 is I/O */
	: "r" (r1), "r" (r7)	/* r1 and r7 are input only */
	: "cc", "memory", "r2", "r3", "r12", "lr"	/* Clobber list reflects calling convention */
	);
	return r0;
}

#ifdef OS_INTERNAL

/****************/
/* Declarations */
/****************/

/* Describes a single stack frame, as found at the top of the stack of a task
   that is not currently running.  Registers r0-r3, r12, lr, pc and psr are stacked
	 automatically by the CPU on entry to handler mode.  Registers r4-r11 are subsequently
	 stacked by the task switcher.  That's why the order is a bit weird. */
typedef struct {
	volatile uint32_t r4;
	volatile uint32_t r5;
	volatile uint32_t r6;
	volatile uint32_t r7;
	volatile uint32_t r8;
	volatile uint32_t r9;
	volatile uint32_t r10;
	volatile uint32_t r11;
	volatile uint32_t r0;
	volatile uint32_t r1;
	volatile uint32_t r2;
	volatile uint32_t r3;
	volatile uint32_t r12;
	volatile uint32_t lr;
	volatile uint32_t pc;
	volatile uint32_t psr;
} _OS_StackFrame_t;

/* Describes a single stack frame, as created automatically by the hardware on exception
   entry. */
typedef struct {
	volatile uint32_t r0;
	volatile uint32_t r1;
	volatile uint32_t r2;
	volatile uint32_t r3;
	const volatile uint32_t r12;
	const volatile uint32_t lr;
	const volatile uint32_t pc;
	const volatile uint32_t psr;
} _OS_SVC_StackFrame_t;

/* Idle task TCB */
extern OS_TCB_t const * const _OS_idleTCB_p;

#define ASSERT(x) do{if(!(x))__BKPT(0);}while(0)


/* Globals */
extern OS_TCB_t * volatile _currentTCB;

/* svc */
#define _OS_task_exit() _svc_0(OS_SVC_EXIT)

/* C */
void _OS_task_end(void);

/* asm */
void _task_switch(void);
void _task_init_switch(OS_TCB_t const * const idleTask);

/* ISRs */
void SysTick_Handler(void);

/* SVC delegates */
void _OS_yield_delegate(void);
void _OS_schedule_delegate(void);
void _OS_enable_systick_delegate(void);
void _OS_taskExit_delegate(void);

#endif /* OS_INTERNAL */

#endif /* _OS_H_ */
