#ifndef HEAP_H
#define HEAP_H

#include <stdint.h>

// Structure definition of a generic heap
typedef struct s_OS_heap_t {
	void * * heapStore;
	uint32_t size;
	// case-specific data comparison function
	int_fast8_t (*heapComparator) (void *, void *);
} OS_heap_t;

#define OS_HEAP_INITIALISER(heapStore, heapComparator) { .heapStore = (heapStore), .size = 0, .heapComparator = (heapComparator) }

void OS_heap_insert(OS_heap_t *heap, void *value);

void *OS_heap_extract(OS_heap_t *heap);

uint_fast8_t OS_heap_isEmpty(OS_heap_t *heap);

#endif /* HEAP_H */
