#include "Utils/utils.h"
#include <stdio.h>
#include <inttypes.h>
#include "heap.h"
#include "string.h"

typedef struct {
	char *name;
	uint32_t priority;
} example_item_t;

static int_fast8_t comparator (void * item1, void * item2) {
	uint32_t example_item1 = ((example_item_t*)item1)->priority;
	uint32_t example_item2 = ((example_item_t*)item2)->priority;
	return (int_fast8_t)(example_item1 - example_item2);
}

int main(void) {
	
	configClock();
	configUSART2(38400);
	
	void *store[20];
	heap_t heap = HEAP_INITIALISER(store, comparator);

	// Remember not to insert so many things that the heap overflows!
	heap_insert(&heap, &(example_item_t){.name="first", .priority=2});
	heap_insert(&heap, &(example_item_t){.name="second", .priority=4});
	heap_insert(&heap, &(example_item_t){.name="third", .priority=3});
	heap_insert(&heap, &(example_item_t){.name="fourth", .priority=5});
	heap_insert(&heap, &(example_item_t){.name="fifth", .priority=6});
	heap_insert(&heap, &(example_item_t){.name="sixth", .priority=1});
	
	while (!heap_isEmpty(&heap)) {
		example_item_t *item = heap_extract(&heap);
		printf("Extracted %s (%" PRId32 ")\r\n", item->name, item->priority);
	}

	while(1);
}
