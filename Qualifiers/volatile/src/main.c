#include "Utils/utils.h"
#include "Utils/gpio.h"
#include <stdio.h>

#define LEDs		(*(unsigned char *)(0x40020C15))
#define buttons	(*(unsigned char volatile *)(0x40021011))

int main(void) {
	
	configClock();
	configUSART2(38400);
	configGPIO();

	printf("Button Test: Push the buttons to see the LEDs change.\r\n");

	while(1) {
		LEDs = buttons;
	}
}
