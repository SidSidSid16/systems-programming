#include "Utils/utils.h"
#include <stdio.h>

int main(void) {
	
	configClock();
	configUSART2(38400);
	
	printf("System Test\r\n");
	
	int x = 0;
	// Infinite loop: embedded software must never end
	while(1) {
		printf("%d\r\n", x++);
		for (int i = 0; i < 1000000; ++i) {
			// You will learn why this is a bad way to make a delay!
		}
	}	
}
