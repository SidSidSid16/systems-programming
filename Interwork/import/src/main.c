#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include "Utils/utils.h"

void fib(uint32_t n);

void report(uint32_t value) {
	printf("%" PRIu32 "\r\n", value);
}

int main(void) {
	configClock();
	configUSART2(38400);

	fib(20);

	while(1);
}
