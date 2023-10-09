#include "queue.h"

int_fast8_t queue_put(queue_t * queue, int32_t data) {
	if (((queue->insert + 1) % QUEUE_SIZE ) == queue->remove) {
		return 1;
	}
	queue->data[queue->insert] = data;
	queue->insert = (queue->insert + 1) % QUEUE_SIZE;
	return 0;
}

int_fast8_t queue_get(queue_t * queue, int32_t * data) {
	if (queue->remove == queue->insert) {
		return 1;
	}
	*data = queue->data[queue->remove];
	queue->remove = (queue->remove + 1) % QUEUE_SIZE;
	return 0;
}
