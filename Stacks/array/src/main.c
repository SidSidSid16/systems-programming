#include "Utils/utils.h"
#include <stdio.h>

static void test(void) {
	printf("Hello!\r\n");
}

int main(void) {
	configClock();
	configUSART2(38400);

	uint32_t array[10];
	
	// First try overflowing the stack with garbage and see what happens
	// Then try to overwrite the link register here so that test() is run
	
	uint32_t * ptr = array;
	*(ptr + 11) = (uint32_t) &test;
}
