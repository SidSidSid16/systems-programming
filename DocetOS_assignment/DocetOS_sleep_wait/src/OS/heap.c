#include "OS/heap.h"

static void heap_up(OS_heap_t *heap) {
	uint32_t childNode = heap->size;
	while (childNode > 1) {
		uint32_t parentNode = childNode / 2;
		if (heap->heapComparator(heap->heapStore[parentNode - 1], heap->heapStore[childNode - 1]) > 0) {
			void * parent_temp = heap->heapStore[parentNode - 1];
			heap->heapStore[parentNode - 1] = heap->heapStore[childNode - 1];
			heap->heapStore[childNode - 1] = parent_temp;
		}
		childNode = parentNode;
	}
}

static void heap_down(OS_heap_t *heap) {
	uint32_t parentNode = 1;
	uint32_t leftChildNode = parentNode * 2;
	uint32_t smallestChildNode;
	while (leftChildNode <= heap->size) {
		uint32_t rightChildNode = leftChildNode + 1;
		if (rightChildNode <= heap->size) {
			if (heap->heapComparator(heap->heapStore[rightChildNode - 1], heap->heapStore[leftChildNode - 1]) > 0) {
				smallestChildNode = leftChildNode;
			} else {
				smallestChildNode = rightChildNode;
			}
		} else {
			smallestChildNode = leftChildNode;
		}
		if (heap->heapComparator(heap->heapStore[parentNode - 1], heap->heapStore[smallestChildNode - 1]) > 0) {
			void * parent_temp = heap->heapStore[parentNode - 1];
			heap->heapStore[parentNode - 1] = heap->heapStore[smallestChildNode - 1];
			heap->heapStore[smallestChildNode - 1] = parent_temp;
		}
		parentNode = smallestChildNode;
		leftChildNode = parentNode * 2;
	}
}

void OS_heap_insert(OS_heap_t *heap, void * item) {
	// The new element is always added to the end of a heap
	heap->heapStore[(heap->size)++] = item;
	heap_up(heap);
}

void * OS_heap_extract(OS_heap_t *heap) {
	// The root value is extracted, and the space filled by the value from the end
	// If the heap is empty, this will fail horribly...
	void * item = heap->heapStore[0];
	heap->heapStore[0] = heap->heapStore[--(heap->size)];
	heap_down(heap);
	return item;
}

uint_fast8_t OS_heap_isEmpty(OS_heap_t *heap) {
	return !(heap->size);
}
