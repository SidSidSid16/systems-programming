#include "Utils/utils.h"
#include <stdio.h>
#include "count.h"

int main(void) {
	configClock();
	configUSART2(38400);
	
	counter = 0;
	printf("Counter value: %d\r\n", counter);
	count();
	printf("Counter value: %d\r\n", counter);
	count();
	printf("Counter value: %d\r\n", counter);
	
	while(1);
}
