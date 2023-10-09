#include "Utils/utils.h"
#include <stdio.h>

// #pragma pack(push)
// #pragma pack(1)
typedef struct {
	int8_t x;
	uint8_t _padding[3];
	int32_t y;
} test_t;
// #pragma pack(pop)

int main(void) {
	
	configClock();
	configUSART2(38400);
		
	test_t test;
	test_t *test_p = &test;
	
	printf("size test: %d\nsize test_t: %d\n\n", sizeof(test), sizeof(test_t));
	
	printf("test.x: %d\ntest.y: %d\n", test.x, test.y);
	
	test_t test2 = {
		.x = 100,
		.y = -1
	};
	
	printf("test2.x: %d\ntest2.y: %d\n\n", test2.x, test2.y);
	
	test = (test_t) { .x = 123, .y = -1 };
	
	printf("test.x: %d\ntest.y: %d\n\n", test.x, test.y);
	
	
	test_p->x = 5;
	test_p->y = 20;
	printf("test.x: %d\ntest.y: %d\n\n", test.x, test.y);
	

	while(1);
}
