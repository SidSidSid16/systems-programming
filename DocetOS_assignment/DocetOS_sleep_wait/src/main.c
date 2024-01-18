#include "OS/mutex.h"
#include "OS/semaphore.h"
#include "OS/os.h"
#include "Utils/utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

// initialise the mutex
static OS_mutex_t consoleOutMutex;
static OS_mutex_t tempSensorMutex;
static OS_mutex_t heatingStatusMutex;

static OS_semaphore_t readTempSemaphore;

/* these variables store the temperature measurements and the thermostat only
	 measures in positives, hence the unsigned type. */
static uint8_t currentTemp = 21;			// temp measured by sensor (initialised to 21*C)
static uint8_t desiredTemp = 27;			// temp set by user (initialised to 27*C)
static uint8_t heatingStatus = 0;			// 1=heating on, 0=off (initialised to off)

/* This task emulates reading from a thermometer output. It generates a random
	 number acting as the measured temperature, stores the temperature in a global
	 variable, and finally logs the measurement to the serial output console. */
__attribute__((noreturn))
static void sense_temperature() {
	/* temperature must be sensed for an infinite number of times while the CPU
		 is running. */
	while (1) {
		/* Generate a random 'temperature' value, emulating a thermometer. */
		// get the temperature and exclusively store in global variable
		OS_mutex_acquire(&tempSensorMutex);
		currentTemp = (uint8_t)(rand() % 28 + 15);
		OS_mutex_release(&tempSensorMutex);
		/* Log to the console that a temperature reading has been recorded. */
		// exclusive access to the console with mutex
		OS_mutex_acquire(&consoleOutMutex);
		// semaphore access the current temp variable to read
		OS_semaphore_acquire(&readTempSemaphore);
		// print the current temp reading to console
		printf("sense_temperature: Measured a temperature reading of %" PRId8 "*C \n", currentTemp);
		// release both the temp variable semaphore and console mutex
		OS_semaphore_release(&readTempSemaphore);
		OS_mutex_release(&consoleOutMutex);
		/* Wait for 10 seconds until taking the next temperature reading. */
		OS_sleep(10000);
	}
}

/* This task controls the heating system, which is communicated with via
	 a bus controlled by sending binary to the heatingStatus variable. Ensures
	 heating is switched on or off correctly based on current temperatures. */
__attribute__((noreturn))
static void control_heating() {
	while (1) {
		// reading the temp variables exclusively with the semaphore
		OS_semaphore_acquire(&readTempSemaphore);
		if (desiredTemp > currentTemp) {
			// turn heating on if the desired is higher than the current temp
			OS_mutex_acquire(&heatingStatusMutex);
			heatingStatus = 1;
			OS_mutex_release(&heatingStatusMutex);
			// log this event to console via serial
			OS_mutex_acquire(&consoleOutMutex);
			printf("control_boiler: Heating has been turned on \n");
			OS_mutex_release(&consoleOutMutex);
		} else {
			// if the desired is equal to or less than current temp, heating off
			OS_mutex_acquire(&heatingStatusMutex);
			heatingStatus = 0;
			OS_mutex_release(&heatingStatusMutex);
			// log this event to console via serial
			OS_mutex_acquire(&consoleOutMutex);
			printf("control_heating: Heating has been turned off \n");
			OS_mutex_release(&consoleOutMutex);
		}
		// release the read temp semaphore
		OS_semaphore_release(&readTempSemaphore);
		/* this logic which checks whether the heater needs to be on or off
			 must try to run every 15 seconds. */
		OS_sleep(15000);
	}
}

/* This task displays the three data objects to an LCD screen for the user
	 to view. */
static void display_LCD(uint8_t data1, uint8_t data2, uint8_t data3) {
		(void) data1;
		(void) data2;
		(void) data3;
		OS_mutex_acquire(&consoleOutMutex);
		printf("display_LCD: Displayed to LCD \n");
		OS_mutex_release(&consoleOutMutex);
}

/* This task is used to broadcast the data to various outputs such as LCD
	 displays and other peripherals. */
__attribute__((noreturn))
static void broadcast_data() {
	while (1) {
		OS_semaphore_acquire(&readTempSemaphore);
		uint8_t currentTempToDisplay = currentTemp;
		uint8_t desiredTempToDisplay = desiredTemp;
		OS_semaphore_release(&readTempSemaphore);
		
		OS_mutex_acquire(&heatingStatusMutex);
		uint8_t heatingStatusToDisplay = heatingStatus;
		OS_mutex_release(&heatingStatusMutex);
		
		OS_mutex_acquire(&consoleOutMutex);
		printf("broadcast_data: Curr.: %" PRId8 "*C, Desi.: %" PRId8 "*C, Heat.: %" PRId8 " \n",
						currentTempToDisplay, desiredTempToDisplay, heatingStatusToDisplay);
		display_LCD(currentTempToDisplay, desiredTempToDisplay, heatingStatusToDisplay);
		OS_mutex_release(&consoleOutMutex);
		
		OS_sleep(3000);
	}
}

//__attribute__((noreturn))
//static void task2(void const *const args) {
//	(void) args;
//	//OS_mutex_acquire(&mutex);
//	while (1) {
//		OS_mutex_acquire(&mutex);
//		printf("BBBBBBBB\n");
//		OS_sleep(2000);
//		OS_mutex_release(&mutex);
//	}
//	//OS_mutex_release(&mutex);
//}

//static void task3(void const *const args) {
//	(void) args;
//	for (uint_fast16_t i = 0; i < 20; ++i) {
//		if (i == 5) {
//			OS_sleep(8000);
//		}
//		OS_mutex_acquire(&mutex);
//		printf("CCCCCCCC\n");
//		OS_sleep(2000);
//		OS_mutex_release(&mutex);
//	}
//}

//static void task4(void const *const args) {
//	(void) args;
//	for (uint_fast16_t i = 0; i < 10; ++i) {
//		OS_semaphore_acquire(&semaphore);
//		// OS_mutex_acquire(&mutex);
//		printf("XXXXXXXX\n");
//		OS_sleep(4000);
//		OS_semaphore_release(&semaphore);
//		OS_yield();
//		// OS_mutex_release(&mutex);
//	}
//}

//static void task5(void const *const args) {
//	(void) args;
//	for (uint_fast16_t i = 0; i < 20; ++i) {
//		OS_semaphore_acquire(&semaphore);
//		// OS_mutex_acquire(&mutex);
//		printf("YYYYYYYY\n");
//		OS_sleep(2000);
//		OS_semaphore_release(&semaphore);
//		OS_yield();
//		// OS_mutex_release(&mutex);
//	}
//}

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
	static uint32_t stack4[128] __attribute__ (( aligned(8) ));
	static uint32_t stack5[128] __attribute__ (( aligned(8) ));
	static OS_TCB_t TCB1, TCB2, TCB3, TCB4, TCB5;

	/* sense_temperature TCB must be of highest priority since the
		 main task of a thermostat is to measure the temperature. */
	OS_initialiseTCB(&TCB1, stack1+128, sense_temperature, NULL, 1);
	
	/* control_heating TCB must also be of highest priority since the
		 other main task of a thermostat is to toggle the heating. */
	OS_initialiseTCB(&TCB2, stack2+128, control_heating, NULL, 1);
	
	/* display_LCD TCB must be lower priority than the sense and control
		 TCBs, however still higher than the rest since it's important to
		 output data for users to observe system. */
	OS_initialiseTCB(&TCB3, stack3+128, broadcast_data, NULL, 2);
	
//	OS_initialiseTCB(&TCB3, stack3+128, task3, NULL, 2);
//	OS_initialiseTCB(&TCB4, stack4+128, task4, NULL, 5);
//	OS_initialiseTCB(&TCB5, stack5+128, task5, NULL, 5);
	
	/* Add the tasks to the scheduler */
	OS_addTask(&TCB1);
	OS_addTask(&TCB2);
	OS_addTask(&TCB3);
	// OS_addTask(&TCB4);
	// OS_addTask(&TCB5);
	
	/* only one thread can access the console output at a given
		 time to ensure individual use of the serial port, eliminating
		 scrambled outputs. */
	OS_mutex_initialise(&consoleOutMutex);
	/* only one thread can write to the global temperature reading
		 storage variable to prevent data corruption. */
	OS_mutex_initialise(&tempSensorMutex);
	/* only one thread can send data to the heater at a given time
		 due to the nature of the data bus used for communication with
		 the heater. */
	OS_mutex_initialise(&heatingStatusMutex);
	
	/* initialise the semaphore that's used to read the temperature
		 variables - a max of 3 concurrent reads should be permitted. */
	OS_semaphore_initialise(&readTempSemaphore, 3);
	
	/* Start the OS */
	OS_start();
}
