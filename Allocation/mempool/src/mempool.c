#include "mempool.h"
#include "static_alloc.h"
#include <stdint.h>

void *pool_allocate(mempool_t *pool) {
	// Return the head of the list of blocks
	// Update the head pointer
	mempool_item_t * headItem = pool->head;
	if (headItem) {
		pool->head = headItem->next;
		return headItem;
	}
	return 0;
}

void pool_deallocate(mempool_t *pool, void *block) {
	// Add the new item to the head of the list
	// Point the 'next' parameter of the block to point to the current head of the pool
	mempool_item_t *item = block;
	item->next = pool->head;
	// Overwrite the head of the pool to point to the new block being stored.
	pool->head = item;
}

void pool_init(mempool_t *pool, size_t blocksize, size_t blocks) {
	uint8_t *item = static_alloc(blocksize*blocks);
	
}