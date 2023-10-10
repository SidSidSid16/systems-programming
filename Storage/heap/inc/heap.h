#ifndef HEAP_H
#define HEAP_H

#include <stdint.h>

typedef struct {
	 * const store;
	uint32_t size;
} heap_t;

#define HEAP_INITIALISER(store) { .store = store, .size = 0 }

void heap_insert(heap_t *heap, int32_t value);
int32_t heap_extract(heap_t *heap);
uint_fast8_t heap_isEmpty (heap_t *heap);

#endif /* HEAP_H */
