#include "mempool.h"
#include "static_alloc.h"
#include <stdint.h>

// boundary for double-word alignment
#define STATIC_ALLOC_ALIGNMENT 8U

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
	// size_t blocksizeAligned = blocksize | (STATIC_ALLOC_ALIGNMENT - 1);
	size_t blocksizeAligned = (blocksize + STATIC_ALLOC_ALIGNMENT - 1) & ~(STATIC_ALLOC_ALIGNMENT - 1);
	// Allocate enough storage to hold requested blocks
	uint8_t * storage = static_alloc(blocksizeAligned*blocks);
	// Proceed only if static_alloc succeeds
	if (storage) {
		// compute the starting address of each block,
		// then pass it to pool_add
		for (size_t i = 0; i < blocks; i++) {
			pool_add(pool, (storage + (i*blocksizeAligned)));
		}
	} else {
		// set the head pointer to 0 if static_alloc fails
		pool->head = 0;
	}
}
