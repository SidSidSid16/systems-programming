#include "Utils/utils.h"
#include <stdio.h>

static void count(void) {
	static unsigned int counter = 0;
	counter++;
	printf("count() called, value = %d\r\n", counter);
}

int main(void) {
	configClock();
	configUSART2(38400);
	
	while(1) {
		count();
	}
}
