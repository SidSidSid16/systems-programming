#include "OS/mutex.h"
#include "OS/os.h"
#include "Utils/utils.h"

#include <stdio.h>

// initialise the mutex
static OS_mutex_t mutex = OS_MUTEX_STATIC_INITIALISER;

static void task1(void const *const args) {
	(void) args;
	for (uint_fast16_t i = 0; i < 9999999; ++i) {
		if (i == 50) {
			OS_sleep(5000);
		}
		OS_mutex_acquire(&mutex);
		printf("AAAAAAAA\n");
		OS_mutex_release(&mutex);
	}
}

__attribute__((noreturn))
static void task2(void const *const args) {
	(void) args;
	while (1) {
		OS_mutex_acquire(&mutex);
		printf("BBBBBBBB\n");
		OS_mutex_release(&mutex);
	}
}

static void task3(void const *const args) {
	(void) args;
	for (uint_fast16_t i = 0; i < 9999999; ++i) {
		if (i == 50) {
			OS_sleep(8000);
		}
		OS_mutex_acquire(&mutex);
		printf("CCCCCCCC\n");
		OS_mutex_release(&mutex);
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
	static uint32_t stack3[128] __attribute__ (( aligned(8) ));
	static OS_TCB_t TCB1, TCB2, TCB3;

	/* Initialise the TCBs using the two functions above */
	OS_initialiseTCB(&TCB1, stack1+128, task1, NULL);
	OS_initialiseTCB(&TCB2, stack2+128, task2, NULL);
	OS_initialiseTCB(&TCB3, stack3+128, task3, NULL);
	
	/* Add the tasks to the scheduler */
	OS_addTask(&TCB1);
	OS_addTask(&TCB2);
	OS_addTask(&TCB3);
	
	/* Start the OS */
	OS_start();
}
