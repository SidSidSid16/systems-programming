#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include "Utils/utils.h"

#pragma pack(push)
#pragma pack(1)

typedef struct {
	int32_t x;
	int32_t y;
	void *ptr;
	void (*callback) (void *);
} structure_t;

#pragma pack(pop)

void process(structure_t * s);

static void printInteger(void * arg) {
	printf("print integer: %" PRId32 "\r\n", *(int32_t *)arg);
}

static void printDouble(void * arg) {
	printf("print double: %f \r\n", *(double *)arg);
}

int main(void) {
	configClock();
	configUSART2(38400);

	int32_t test_ptr = 111;
	
	structure_t test_struct = {
		.x = 123,
		.y = 321,
		.ptr = &test_ptr,
		.callback = printInteger
	};
	
	process(&test_struct);
	
	printf("x = %" PRId32 "\r\n", test_struct.x);
	printf("y = %" PRId32 "\r\n", test_struct.y);
	
	
	
	double another_test_ptr = 11;
	
	structure_t another_test_struct = {
		.x = 12,
		.y = 21,
		.ptr = &another_test_ptr,
		.callback = printDouble
	};
	
	process(&another_test_struct);
	
	printf("x = %" PRId32 "\r\n", another_test_struct.x);
	printf("y = %" PRId32 "\r\n\n\n\n", another_test_struct.y);

	while(1);
}

