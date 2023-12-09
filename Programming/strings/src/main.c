#include "Utils/utils.h"
#include <stdio.h>

int main(void) {
	
	configClock();
	configUSART2(38400);
	
	char array[] = "Hello!";
	char * literal = "Hello!";
	
	char * literal2 = "Hello!";

	printf("array: %s\n", array);
	printf("literal: %s\n", literal);
	printf("literal2: %s\n", literal2);

	printf("array is stored at %p\n", (void *)array);
	printf("literal is stored at %p\n", (void *)literal);
	printf("literal2 is stored at %p\n", (void *)literal2);

	while(1);
}
