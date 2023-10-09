#ifndef HEAP_H
#define HEAP_H

#include <stdint.h>

typedef struct {
	void * * const store;
	uint32_t size;
	int_fast8_t (*comparator) (void *, void *);
} heap_t;

#define HEAP_INITIALISER(store, comparator) { .store = (store), .size = 0, .comparator = (comparator) }

void heap_insert(heap_t *heap, void *value);
void *heap_extract(heap_t *heap);
uint_fast8_t heap_isEmpty(heap_t *heap);

#endif /* HEAP_H */
