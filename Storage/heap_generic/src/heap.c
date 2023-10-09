#include "heap.h"

static void heap_up(heap_t *heap) {
	// Write me!
}

static void heap_down(heap_t *heap) {
	// Write me!
}

void heap_insert(heap_t *heap, void * item) {
	// The new element is always added to the end of a heap
	heap->store[(heap->size)++] = item;
	heap_up(heap);
}

void * heap_extract(heap_t *heap) {
	// The root value is extracted, and the space filled by the value from the end
	// If the heap is empty, this will fail horribly...
	void * item = heap->store[0];
	heap->store[0] = heap->store[--(heap->size)];
	heap_down(heap);
	return item;
}

uint_fast8_t heap_isEmpty(heap_t *heap) {
	return !(heap->size);
}
