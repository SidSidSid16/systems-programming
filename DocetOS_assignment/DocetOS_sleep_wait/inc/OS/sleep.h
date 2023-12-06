#ifndef SLEEP_H
#define SLEEP_H

#define OS_INTERNAL

#include "OS/os.h"

#define TASK_HEAP_SIZE 20

typedef struct s_OS_taskHeap_t {
	OS_TCB_t * * const store;
	uint32_t size;
} _OS_taskHeap_t;

#define TASK_HEAP_INITIALISER(heapStore) { .store = heapStore, .size = 0 }

extern _OS_taskHeap_t sleeping_list;

void taskHeap_insert(_OS_taskHeap_t *heap, OS_TCB_t *task);
OS_TCB_t * taskHeap_extract(_OS_taskHeap_t *heap);
uint_fast8_t taskHeap_isEmpty(_OS_taskHeap_t *heap);

#endif /* SLEEP_H */
