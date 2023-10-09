#include "Utils/utils.h"
#include "queue.h"
#include <inttypes.h>
#include <stdio.h>

queue_t queue = QUEUE_INITIALISER;

int main(void) {
	
	configClock();
	configUSART2(38400);
	
	int32_t output = 0;
	
	printf("Put status: %" PRIdFAST8 " insert: %" PRIdFAST16 " remove: %" PRIdFAST16 "\n", queue_put(&queue, 1), queue.insert, queue.remove);
	printf("Put status: %" PRIdFAST8 " insert: %" PRIdFAST16 " remove: %" PRIdFAST16 "\n", queue_put(&queue, 2), queue.insert, queue.remove);
	printf("Put status: %" PRIdFAST8 " insert: %" PRIdFAST16 " remove: %" PRIdFAST16 "\n", queue_put(&queue, 3), queue.insert, queue.remove);
	printf("Put status: %" PRIdFAST8 " insert: %" PRIdFAST16 " remove: %" PRIdFAST16 "\n", queue_put(&queue, 4), queue.insert, queue.remove);
	printf("Put status: %" PRIdFAST8 " insert: %" PRIdFAST16 " remove: %" PRIdFAST16 "\n", queue_put(&queue, 5), queue.insert, queue.remove);
	printf("Put status: %" PRIdFAST8 " insert: %" PRIdFAST16 " remove: %" PRIdFAST16 "\n", queue_put(&queue, 6), queue.insert, queue.remove);
	printf("Put status: %" PRIdFAST8 " insert: %" PRIdFAST16 " remove: %" PRIdFAST16 "\n", queue_put(&queue, 7), queue.insert, queue.remove);
	printf("Put status: %" PRIdFAST8 " insert: %" PRIdFAST16 " remove: %" PRIdFAST16 "\n", queue_put(&queue, 8), queue.insert, queue.remove);
	printf("Put status: %" PRIdFAST8 " insert: %" PRIdFAST16 " remove: %" PRIdFAST16 "\n", queue_put(&queue, 9), queue.insert, queue.remove);
	printf("Put status: %" PRIdFAST8 " insert: %" PRIdFAST16 " remove: %" PRIdFAST16 "\n", queue_put(&queue, 10), queue.insert, queue.remove);
	//queue_put(&queue, 10);

	printf("Status: %" PRIdFAST8 ", Removed %" PRId32 " from queue\r\n",queue_get(&queue, &output), output);
	printf("Status: %" PRIdFAST8 ", Removed %" PRId32 " from queue\r\n",queue_get(&queue, &output), output);
	printf("Status: %" PRIdFAST8 ", Removed %" PRId32 " from queue\r\n",queue_get(&queue, &output), output);
	printf("Status: %" PRIdFAST8 ", Removed %" PRId32 " from queue\r\n",queue_get(&queue, &output), output);
	printf("Status: %" PRIdFAST8 ", Removed %" PRId32 " from queue\r\n",queue_get(&queue, &output), output);
	printf("Status: %" PRIdFAST8 ", Removed %" PRId32 " from queue\r\n",queue_get(&queue, &output), output);
	printf("Status: %" PRIdFAST8 ", Removed %" PRId32 " from queue\r\n",queue_get(&queue, &output), output);
	printf("Status: %" PRIdFAST8 ", Removed %" PRId32 " from queue\r\n",queue_get(&queue, &output), output);
	printf("Status: %" PRIdFAST8 ", Removed %" PRId32 " from queue\r\n",queue_get(&queue, &output), output);
	printf("Status: %" PRIdFAST8 ", Removed %" PRId32 " from queue\r\n",queue_get(&queue, &output), output);
	//printf("Status: %" PRIdFAST8 ", Removed %" PRId32 " from queue\r\n",queue_get(&queue, &output), output);
}
