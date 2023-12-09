#include "Utils/utils.h"
#include <stdio.h>
#include <inttypes.h>
#include <string.h>

#include "mempool.h"

/* Example data packet structure, just for demonstration purposes */
typedef struct {
	uint32_t id;
	char data[12];
} packet_t;

int main(void) {
	
	configClock();
	configUSART2(38400);

	/* Declare and intialise a memory pool */
	static mempool_t pool = MEMPOOL_INITIALISER;
//	static packet_t poolElements[10];

//	for (int i = 0; i < 10; ++i) {
//		pool_add(&pool, &poolElements[i]);
//	}
	 
	// pool_init(&pool, sizeof(packet_t), 10);
	pool_init(&pool, 33, 10);
	
	/* Example use of the pool */
	
	/* Allocate two blocks for data packets and fill them in */
	packet_t *p1 = pool_allocate(&pool);
	p1->id = 0;
	strncpy(p1->data, "first", 10);

	packet_t *p2 = pool_allocate(&pool);
	p2->id = 1;
	strncpy(p2->data, "second", 10);

	printf("First allocated packet (id %" PRIu32 ", data '%s') at address %p\r\n", p1->id, p1->data, (void *)p1);
	printf("Second allocated packet (id %" PRIu32 ", data '%s') at address %p\r\n", p2->id, p2->data, (void *)p2);

	/* Return the first packet to the pool */
	pool_deallocate(&pool, p1);
	
	/* Allocating a third should now re-use the block from the deallocated first */
	packet_t *p3 = pool_allocate(&pool);
	p3->id = 2;
	strncpy(p3->data, "third", 10);

	packet_t *p4 = pool_allocate(&pool);
	p4->id = 3;
	strncpy(p4->data, "fourth", 10);

	printf("Third allocated packet (id %" PRIu32 ", data '%s') at address %p\r\n", p3->id, p3->data, (void *)p3);
	printf("Fourth allocated packet (id %" PRIu32 ", data '%s') at address %p\r\n", p4->id, p4->data, (void *)p4);

	while(1);
}
