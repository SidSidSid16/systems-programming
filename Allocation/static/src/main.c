#include "Utils/utils.h"
#include <stdio.h>
#include <inttypes.h>

#include "static_alloc.h"

int main(void) {
	
	configClock();
	configUSART2(38400);

	// Array lengths
	uint_fast8_t len_a = 10;
	uint_fast8_t len_b = 12;
	
	// Arrays
	uint32_t * a = static_alloc(len_a * sizeof(uint32_t));
	uint32_t * b = static_alloc(len_b * sizeof(uint32_t));
	
	// Fill arrays
	for (uint_fast8_t i = 0; i < len_a; ++i) {
		a[i] = 2*i;
	}
	
	for (uint_fast8_t i = 0; i < len_b; ++i) {
		b[i] = 3*i;
	}
	
	// Print arrays
	for (uint_fast8_t i = 0; i < len_a; ++i) {
		printf("a[%" PRIuFAST16 "] = %" PRIu32 "\r\n", i, a[i]);
	}

	for (uint_fast8_t i = 0; i < len_b; ++i) {
		printf("b[%" PRIuFAST16 "] = %" PRIu32 "\r\n", i, b[i]);
	}

	while(1);
}
