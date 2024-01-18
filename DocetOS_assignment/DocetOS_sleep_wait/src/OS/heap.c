#include "OS/heap.h"

#include <stdio.h>

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

/* A function that checks if the heap is empty, returning a 1 if empty, and a
	 non-zero value if heap contains items. */
uint_fast8_t OS_heap_isEmpty(OS_heap_t *heap) {
	return !(heap->size);
}

/* A function to insert an item into the heap. */
void OS_heap_insert(OS_heap_t *heap, void * item) {
	// The new element is always added to the end of a heap
	heap->heapStore[(heap->size)++] = item;
	heap_up(heap);
}

/* A function to extract the head item from the heap. */
void * OS_heap_extract(OS_heap_t *heap) {
	// only proceed with extraction if heap is not empty
	if (!OS_heap_isEmpty(heap)) {
		// cache the head item
		void * item = heap->heapStore[0];
		// update the index and size counter
		heap->heapStore[0] = heap->heapStore[--(heap->size)];
		// fill space from the end
		heap_down(heap);
		// return the head item
		return item;
	} else {
		// return null if heap is empty
		return NULL;
	}
}

/* A function to peek the head of the heap. */
void * OS_heap_peek(OS_heap_t *heap) {
	// only proceed if heap is not empty
	if (!OS_heap_isEmpty(heap)) {
		// cache the item at the head
		void * item = heap->heapStore[0];
		// return the head item
		return item;
	} else {
		// return null if heap is empty
		return NULL;
	}
}
