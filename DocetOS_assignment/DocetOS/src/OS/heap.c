#include "OS/heap.h"

#include <stdio.h>

/* Internal heap function to move nodes up to maintain heap ordering. */
static void _heap_up(OS_heap_t * heap) {
	// start with the last node in the heap
	uint32_t childNode = heap->size;
	while (childNode > 1) {
		// calculate the parent node
		uint32_t parentNode = childNode / 2;
		// compare parent and child node field
		if (heap->heapComparator(heap->heapStore[parentNode - 1], heap->heapStore[childNode - 1]) > 0) {
			// swap the two nodes to maintain priority sorting
			void * parent_temp = heap->heapStore[parentNode - 1];
			heap->heapStore[parentNode - 1] = heap->heapStore[childNode - 1];
			heap->heapStore[childNode - 1] = parent_temp;
		}
		// move up to the parent node for the next iteration
		childNode = parentNode;
	}
}

/* Internal heap function to move nodes down to maintain heap ordering. */
static void _heap_down(OS_heap_t * heap) {
	// start with the root node
	uint32_t parentNode = 1;
	// calculate the left child node position first
	uint32_t leftChildNode = parentNode * 2;
	// initialise the var to store the smallest child node
	uint32_t smallestChildNode;
	// find the smallest child node
	while (leftChildNode <= heap->size) {
		// calculate the right child position
		uint32_t rightChildNode = leftChildNode + 1;
		// compare the sizes between the two children
		if (rightChildNode <= heap->size) {
			if (heap->heapComparator(heap->heapStore[rightChildNode - 1], heap->heapStore[leftChildNode - 1]) > 0) {
				smallestChildNode = leftChildNode;
			} else {
				smallestChildNode = rightChildNode;
			}
		} else {
			smallestChildNode = leftChildNode;
		}
		// swap the children around to maintain heap sorting
		if (heap->heapComparator(heap->heapStore[parentNode - 1], heap->heapStore[smallestChildNode - 1]) > 0) {
			void * parent_temp = heap->heapStore[parentNode - 1];
			heap->heapStore[parentNode - 1] = heap->heapStore[smallestChildNode - 1];
			heap->heapStore[smallestChildNode - 1] = parent_temp;
		}
		// move down the tree to the smallest child
		parentNode = smallestChildNode;
		// calculate the next child's position
		leftChildNode = parentNode * 2;
	}
}

/* A function that checks if the heap is empty, returning a 1 if empty, and a
	 non-zero value if heap contains items. Function takes in a pointer to the
	 heap. */
uint_fast8_t OS_heap_isEmpty(OS_heap_t * heap) {
	return !(heap->size);
}

/* A function to insert an item into the heap. Function takes in a pointer to
	 the heap to insert into and a pointer to the item to insert. */
void OS_heap_insert(OS_heap_t * heap, void * item) {
	// The new element is always added to the end of a heap
	heap->heapStore[(heap->size)++] = item;
	_heap_up(heap);
}

/* A function to extract the head item from the heap. Function takes in a	
	 pointer to the heap to extract from. Returns NULL if the heap is empty
	 and an object couldn't be extracted, otherwise returns a void pointer of
	 the generic item. */
void * OS_heap_extract(OS_heap_t * heap) {
	// only proceed with extraction if heap is not empty
	if (!OS_heap_isEmpty(heap)) {
		// cache the head item
		void * item = heap->heapStore[0];
		// update the index and size counter
		heap->heapStore[0] = heap->heapStore[--(heap->size)];
		// fill space from the end
		_heap_down(heap);
		// return the head item
		return item;
	} else {
		// return null if heap is empty
		return NULL;
	}
}

/* A function to peek the head of the heap. Function takes in a pointer
	 to the heap to peek, returns a NULL if the heap is empty, otherwise
	 returns the void pointer to the generic item without extracting it
	 from the heap. */
void * OS_heap_peek(OS_heap_t * heap) {
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
