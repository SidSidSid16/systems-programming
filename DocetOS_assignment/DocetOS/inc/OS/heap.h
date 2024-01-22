#ifndef HEAP_H
#define HEAP_H

#include <stdint.h>

// Structure definition of a generic heap
typedef struct s_OS_heap_t {
	// store for the heap
	void * * heapStore;
	// size of the heap
	uint32_t size;
	// case-specific data comparison function
	int_fast8_t (*heapComparator) (void *, void *);
} OS_heap_t;

#define OS_HEAP_INITIALISER(heapStore, heapComparator) { .heapStore = (heapStore), .size = 0, .heapComparator = (heapComparator) }

/* Utility function to check if a heap is empty. */
uint_fast8_t OS_heap_isEmpty(OS_heap_t *heap);
/* Function to insert an item into the heap. */
void OS_heap_insert(OS_heap_t *heap, void *value);
/* Function to extract an item from the heap. */
void * OS_heap_extract(OS_heap_t *heap);
/* Utility function to peek the head of the heap without extraction. */
void * OS_heap_peek(OS_heap_t *heap);

#endif /* HEAP_H */
