#include "OS/os.h"
#include "Utils/utils.h"
#include <stdio.h>

#include "Mode_Utils/mode_utils.h"
#include "OS/sleep.h"

__attribute__((noreturn))
static void task1(void const *const args) {
	(void) args;
	// reportState_yield();
	while (1) {
		OS_sleep(500);
		printf("AAAAAAAAA");
	}
}

__attribute__((noreturn))
static void task2(void const *const args) {
	(void) args;
	// reportState_yield();
	while (1) {
		//OS_sleep(20);
		printf("BBBBBBBB");
	}
}

/* MAIN FUNCTION */

int main(void) {
	
	configClock();
	configUSART2(38400);
	
	printf("\r\nDocetOS\r\n");

	/* Reserve memory for two stacks and two TCBs.
	   Remember that stacks must be 8-byte aligned. */
	static uint32_t stack1[128] __attribute__ (( aligned(8) ));
	static uint32_t stack2[128] __attribute__ (( aligned(8) ));
	static OS_TCB_t TCB1, TCB2;

	/* Initialise the TCBs using the two functions above */
	OS_initialiseTCB(&TCB1, stack1+128, task1, NULL);
	OS_initialiseTCB(&TCB2, stack2+128, task2, NULL);
	
	/* Add the tasks to the scheduler */
	OS_addTask(&TCB1);
	//OS_addTask(&TCB2);

	//reportState();
	
	/* Start the OS */
	OS_start();
	
}