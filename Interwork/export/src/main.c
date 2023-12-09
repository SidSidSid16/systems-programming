#include <stdio.h>
#include <stdint.h>
#include "Utils/utils.h"

int32_t calculate(int32_t x, int32_t y);

int main(void) {
	
	configClock();
	configUSART2(38400);

	printf("Returned value: %d\r\n", calculate(5, 2));

	while(1);

}
