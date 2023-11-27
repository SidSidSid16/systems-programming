#include "OS/os.h"
#include "Utils/utils.h"
#include <stdio.h>

#include "OS/sleep.h"

__attribute__((noreturn))
static void task1(void const *const args) {
	(void) args;
	uint32_t counter = 0;
	while (1) {
		printf("AAAAAAAAA\n");
		counter++;
		if (counter == 5) {
			OS_wait();
		}
	}
}

__attribute__((noreturn))
static void task2(void const *const args) {
	(void) args;
	while (1) {
		printf("BBBBBBBB\n");
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
	OS_addTask(&TCB2);
	
	/* Start the OS */
	OS_start();
	
}
