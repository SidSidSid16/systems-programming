#include "OS/sleep.h"

#include "OS/os.h"
#include "stm32f4xx.h"

void OS_sleep_delegate(_OS_SVC_StackFrame_t * stack);
/* Function to put a task to sleep for a number of ticks */
void OS_sleep_delegate(_OS_SVC_StackFrame_t * stack) {
	// Get the sleep duration that's been passed in
	uint32_t sleepDuration = stack->r0;
	// The running task's TCB is retrieved and stored
	OS_TCB_t * currentTCB = OS_currentTCB();
	// wakeTime time is calculated by adding sleepDuration to the elapsed OS ticks
	uint32_t wakeTime = OS_elapsedTicks() + sleepDuration;
	// wakeTime can be stored in TCB in the data field
	currentTCB->data = wakeTime;
	// Set the TCB state to sleeping
	currentTCB->state |= TASK_STATE_SLEEP;
	// Call PendSV to invoke _OS_scheduler to start the next task
	SCB->ICSR = SCB_ICSR_PENDSVSET_Msk;
}
