#include "Utils/utils.h"
#include <stdio.h>

static void processArray(int *array) {
	*(array + 2) = 10;
}


int main(void) {
	
	configClock();
	configUSART2(38400);

	int numbers[] = {1,3,5,7,9};

	numbers[0] = 11;
	*(numbers + 3) = 42;
	
	int *pointer = &(numbers[0]);

	for (int i = 0; i < 5; ++i) {
		printf("numbers[%d] = %d\n", i, numbers[i]);
	}
	
	for (int i = 0; i < 5; ++i) {
		printf("pointer[%d] = %d\n", i, pointer[i]);
	}
	
	printf("sizeof array: %d\n", sizeof(numbers));
	printf("sizeof pointer: %d\n", sizeof(pointer));
	
	processArray(numbers);
	
	for (int i = 0; i < 5; ++i) {
		printf("numbers[%d] = %d\n", i, numbers[i]);
	}
	
	while(1);

}
