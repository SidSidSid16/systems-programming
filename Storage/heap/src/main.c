#include "Utils/utils.h"
#include <stdio.h>
#include <inttypes.h>
#include "heap.h"

int main(void) {
	
	configClock();
	configUSART2(38400);
	
	int32_t store[20];
	heap_t heap = HEAP_INITIALISER(store);

	// Remember not to insert so many things that the heap overflows!
	heap_insert(&heap, 2);
	heap_insert(&heap, 4);
	heap_insert(&heap, 8);
	heap_insert(&heap, 9);
	heap_insert(&heap, 7);
	heap_insert(&heap, 1);
	heap_insert(&heap, 4);
	heap_insert(&heap, 1);
	heap_insert(&heap, 3);
	heap_insert(&heap, 2);
	heap_insert(&heap, 8);
	heap_insert(&heap, 3);
	heap_insert(&heap, 2);
	heap_insert(&heap, 1);
	heap_insert(&heap, 2);
	heap_insert(&heap, 3);
	heap_insert(&heap, 7);
	heap_insert(&heap, 6);
	heap_insert(&heap, 6);
	heap_insert(&heap, 1);
	
	while (!heap_isEmpty(&heap)) {
		printf("Extracted %" PRId32 "\r\n", heap_extract(&heap));
	}

	while(1);
}
