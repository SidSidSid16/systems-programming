#include "heap.h"

static void heap_up(heap_t *heap) {
	uint32_t childNode = heap->size;
	while (childNode > 1) {
		uint32_t parentNode = childNode / 2;
		if (heap->comparator(heap->store[parentNode - 1], heap->store[childNode - 1]) > 0) {
			void * parent_temp = heap->store[parentNode - 1];
			heap->store[parentNode - 1] = heap->store[childNode - 1];
			heap->store[childNode - 1] = parent_temp;
		}
		childNode = parentNode;
	}
}

static void heap_down(heap_t *heap) {
	uint32_t parentNode = 1;
	uint32_t leftChildNode = parentNode * 2;
	uint32_t smallestChildNode;
	while (leftChildNode <= heap->size) {
		uint32_t rightChildNode = leftChildNode + 1;
		if (rightChildNode <= heap->size) {
			if (heap->comparator(heap->store[rightChildNode - 1], heap->store[leftChildNode - 1]) > 0) {
				smallestChildNode = leftChildNode;
			} else {
				smallestChildNode = rightChildNode;
			}
		} else {
			smallestChildNode = leftChildNode;
		}
		if (heap->comparator(heap->store[parentNode - 1], heap->store[smallestChildNode - 1]) > 0) {
			void * parent_temp = heap->store[parentNode - 1];
			heap->store[parentNode - 1] = heap->store[smallestChildNode - 1];
			heap->store[smallestChildNode - 1] = parent_temp;
		}
		parentNode = smallestChildNode;
		leftChildNode = parentNode * 2;
	}
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
