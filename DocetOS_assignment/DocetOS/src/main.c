#include "OS/mutex.h"
#include "OS/semaphore.h"
#include "OS/os.h"
#include "Utils/utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

// initialise the mutexes and semaphores
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
		printf("sense_temperature: Measured a temperature reading of %" PRId8 "*C \n\n\n", currentTemp);
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
			printf("control_boiler: Heating has been turned on \n\n\n");
			OS_mutex_release(&consoleOutMutex);
		} else {
			// if the desired is equal to or less than current temp, heating off
			OS_mutex_acquire(&heatingStatusMutex);
			heatingStatus = 0;
			OS_mutex_release(&heatingStatusMutex);
			// log this event to console via serial
			OS_mutex_acquire(&consoleOutMutex);
			printf("control_heating: Heating has been turned off \n\n\n");
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
		printf("display_LCD: Displayed to LCD \n\n\n");
		OS_mutex_release(&consoleOutMutex);
}

/* This task is used to broadcast the data to various outputs such as LCD
	 displays and other peripherals. */
__attribute__((noreturn))
static void broadcast_data() {
	while (1) {
		// access and read temps using semaphore
		OS_semaphore_acquire(&readTempSemaphore);
		uint8_t currentTempToDisplay = currentTemp;
		uint8_t desiredTempToDisplay = desiredTemp;
		OS_semaphore_release(&readTempSemaphore);
		
		// access the heating status bus using mutex
		OS_mutex_acquire(&heatingStatusMutex);
		uint8_t heatingStatusToDisplay = heatingStatus;
		OS_mutex_release(&heatingStatusMutex);
		
		// output to console via mutex
		OS_mutex_acquire(&consoleOutMutex);
		printf("broadcast_data: Curr.: %" PRId8 "*C, Desi.: %" PRId8 "*C, Heat.: %" PRId8 " \n\n\n",
						currentTempToDisplay, desiredTempToDisplay, heatingStatusToDisplay);
		display_LCD(currentTempToDisplay, desiredTempToDisplay, heatingStatusToDisplay);
		// Other peripherals...
		OS_mutex_release(&consoleOutMutex);
		
		// run every 3 seconds
		OS_sleep(3000);
	}
}

/* This task emulates a thread for a remote device controlling the heating
	 by setting the desired temperature. */
__attribute__((noreturn))
static void control_thread_dev1() {
	// loop counter to generate a random temp
	uint8_t loopCounter = 0;
	
	// starts working 5 seconds into the emulation
	OS_sleep(5000);
	while (1) {
		// retrieve the desired and current temperature
		OS_semaphore_acquire(&readTempSemaphore);
		uint8_t currentTempToDisplay = currentTemp;
		uint8_t desiredTempToDisplay = desiredTemp;
		OS_semaphore_release(&readTempSemaphore);
		
		// retrieve the heating status
		OS_mutex_acquire(&heatingStatusMutex);
		uint8_t heatingStatusToDisplay = heatingStatus;
		OS_mutex_release(&heatingStatusMutex);
		
		// set the desired temperature
		OS_mutex_acquire(&tempSensorMutex);
		desiredTemp = loopCounter++;
		OS_mutex_release(&tempSensorMutex);
		
		// retrieve newly set desired temp
		OS_semaphore_acquire(&readTempSemaphore);
		uint8_t newDesiredTempToDisplay = desiredTemp;
		OS_semaphore_release(&readTempSemaphore);
		
		// log to the console
		OS_mutex_acquire(&consoleOutMutex);
		printf("control_thread_dev1: Curr.: %" PRId8 "*C, Desi.: %" PRId8 "*C, new Desi.: %" PRId8 "*C, Heat.: %" PRId8 " \n\n\n",
						currentTempToDisplay, desiredTempToDisplay, newDesiredTempToDisplay, heatingStatusToDisplay);
		OS_mutex_release(&consoleOutMutex);
		
		// change temp another time after 10 seconds
		OS_sleep(10000);
	}
}

/* This task emulates a thread for a remote device controlling the heating
	 by setting the desired temperature. */
static void control_thread_dev2() {	
	// starts working 10 seconds into the emulation
	OS_sleep(10000);
	for (uint8_t i = 0; i < 4; ++i) {
		// retrieve the desired and current temperature
		OS_semaphore_acquire(&readTempSemaphore);
		uint8_t currentTempToDisplay = currentTemp;
		uint8_t desiredTempToDisplay = desiredTemp;
		OS_semaphore_release(&readTempSemaphore);
		
		// retrieve the heating status
		OS_mutex_acquire(&heatingStatusMutex);
		uint8_t heatingStatusToDisplay = heatingStatus;
		OS_mutex_release(&heatingStatusMutex);
		
		// set the desired temperature
		OS_mutex_acquire(&tempSensorMutex);
		desiredTemp = i;
		OS_mutex_release(&tempSensorMutex);
		
		// retrieve newly set desired temp
		OS_semaphore_acquire(&readTempSemaphore);
		uint8_t newDesiredTempToDisplay = desiredTemp;
		OS_semaphore_release(&readTempSemaphore);
		
		// log to the console
		OS_mutex_acquire(&consoleOutMutex);
		printf("control_thread_dev2: Curr.: %" PRId8 "*C, Desi.: %" PRId8 "*C, new Desi.: %" PRId8 "*C, Heat.: %" PRId8 " \n\n\n",
						currentTempToDisplay, desiredTempToDisplay, heatingStatusToDisplay, newDesiredTempToDisplay);
		OS_mutex_release(&consoleOutMutex);
		
		// change temp another time after 10 seconds
		OS_sleep(15000);
	}
}

/* This task emulates a faulty thread for a fauly remote device controlling
	 the heating by setting the desired temperature. */
static void control_thread_dev3() {
	// starts working 15 seconds into the emulation
	OS_sleep(15000);
	
	// hog the mutex with a for-loop
	OS_mutex_acquire(&consoleOutMutex);
	for (uint8_t i = 0; i < 100; ++i) {
		printf("control_thread_dev3: Connection failed, retrying... \n\n\n");
	}
	OS_mutex_release(&consoleOutMutex);
}

/* MAIN FUNCTION */

int main(void) {
	
	configClock();
	configUSART2(38400);
	
	printf("\r\nDocetOS\r\n\n\n");

	/* Reserve memory for two stacks and two TCBs.
	   Remember that stacks must be 8-byte aligned. */
	static uint32_t stack1[128] __attribute__ (( aligned(8) ));
	static uint32_t stack2[128] __attribute__ (( aligned(8) ));
	static uint32_t stack3[128] __attribute__ (( aligned(8) ));
	static uint32_t stack4[128] __attribute__ (( aligned(8) ));
	static uint32_t stack5[128] __attribute__ (( aligned(8) ));
	static uint32_t stack6[128] __attribute__ (( aligned(8) ));
	static OS_TCB_t TCB1, TCB2, TCB3, TCB4, TCB5, TCB6;

	/* sense_temperature TCB must be of highest priority since the
		 main task of a thermostat is to measure the temperature. */
	OS_initialiseTCB(&TCB1, stack1+128, sense_temperature, NULL, 1);
	
	/* control_heating TCB must also be of highest priority since the
		 other main task of a thermostat is to toggle the heating. */
	OS_initialiseTCB(&TCB2, stack2+128, control_heating, NULL, 1);
	
	/* broadcast_data TCB must be the same priority as the sense and
		 control TCBs, since it's important to output data for users to
		 observe system. */
	OS_initialiseTCB(&TCB3, stack3+128, broadcast_data, NULL, 1);
	
	/* control_thread_dev1 TCB is of lower priority than the three prior
		 defined tasks, this task emulates a remote device that changes the
		 desired temps every 10 seconds. */
	OS_initialiseTCB(&TCB4, stack4+128, control_thread_dev1, NULL, 2);
	
	/* control_thread_dev2 TCB is of lower priority than device 1 defined
		 tasks, this task emulates a remote device that changes the desired
		 temps every 15 seconds. */
	OS_initialiseTCB(&TCB5, stack5+128, control_thread_dev2, NULL, 3);
	
	/* control_thread_dev3 TCB is of the lowest priority and emulates a
		 badly implemented device thread, which is hogging the serial mutex
		 due to a connection issue. */
	OS_initialiseTCB(&TCB6, stack6+128, control_thread_dev3, NULL, 4);
	
	/* Add the tasks to the scheduler */
	OS_addTask(&TCB1);
	OS_addTask(&TCB2);
	OS_addTask(&TCB3);
	OS_addTask(&TCB4);
	OS_addTask(&TCB5);
	OS_addTask(&TCB6);
	
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
