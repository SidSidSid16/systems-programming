#include "heap.h"

static void heap_up(heap_t *heap) {
	uint32_t childNode = heap->size;
	while (childNode > 1) {
		uint32_t parentNode = childNode / 2;
		if (heap->store[parentNode - 1] > heap->store[childNode - 1]) {
			int32_t parent_temp = heap->store[parentNode - 1];
			heap->store[parentNode - 1] = heap->store[childNode - 1];
			heap->store[childNode - 1] = parent_temp;
		}
		childNode = parentNode;
	}
}

static void heap_down(heap_t *heap) {
	uint32_t parentNode = 1;
	uint32_t childNode = parentNode * 2;
	while (parentNode < heap->size) {
		if (heap->store[parentNode - 1] > heap->store[childNode - 1]) {
			int32_t parent_temp = heap->store[parentNode - 1];
			heap->store[parentNode - 1] = heap->store[childNode - 1];
			heap->store[childNode - 1] = parent_temp;
			parentNode = childNode;
			childNode = parentNode * 2;
		}
		childNode++;
	}
}

void heap_insert(heap_t *heap, int32_t value) {
	// The new element is always added to the end of a heap
	heap->store[(heap->size)++] = value;
	heap_up(heap);
}

int32_t heap_extract(heap_t *heap) {
	// The root value is extracted, and the space filled by the value from the end
	// If the heap is empty, this will fail horribly...
	int value = heap->store[0];
	heap->store[0] = heap->store[--(heap->size)];
	heap_down(heap);
	return value;
}

uint_fast8_t heap_isEmpty(heap_t *heap) {
	return !(heap->size);
}
