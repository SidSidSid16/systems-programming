#define OS_INTERNAL

#include "OS/sleep.h"
#include "OS/os.h"

void OS_sleep(uint32_t sleepDuration) {
	// The running task's TCB is retrieved and stored
	OS_TCB_t * currentTCB = OS_currentTCB();
	// wakeTime time is calculated by adding sleepDuration to the elapsed OS ticks
	uint32_t wakeTime = OS_elapsedTicks() + sleepDuration;
	// wakeTime can be stored in TCB in the data field
	currentTCB->data = &wakeTime;
	// Set the TCB state to sleeping
	currentTCB->state |= TASK_STATE_SLEEP;
	// Initiate task switch by calling yield
	OS_yield();
}
