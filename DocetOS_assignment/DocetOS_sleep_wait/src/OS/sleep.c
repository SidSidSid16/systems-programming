#include "OS/sleep.h"
#include "OS/scheduler.h"

#include "stm32f4xx.h"

static OS_TCB_t *heapStore[TASK_HEAP_SIZE];
_OS_taskHeap_t sleeping_list = TASK_HEAP_INITIALISER(heapStore);

static int_fast8_t heap_dataComparator (OS_TCB_t * task1, OS_TCB_t * task2) {
	uint32_t task1Priority = task1->data;
	uint32_t task2Priority = task2->data;
	return (int_fast8_t)(task1Priority - task2Priority);
}

static void heap_up(_OS_taskHeap_t *heap) {
	uint32_t childNode = heap->size;
	while (childNode > 1) {
		uint32_t parentNode = childNode / 2;
		if (heap_dataComparator(heap->store[parentNode - 1], heap->store[childNode - 1]) > 0) {
			OS_TCB_t * parent_temp = heap->store[parentNode - 1];
			heap->store[parentNode - 1] = heap->store[childNode - 1];
			heap->store[childNode - 1] = parent_temp;
		}
		childNode = parentNode;
	}
}

static void heap_down(_OS_taskHeap_t *heap) {
	uint32_t parentNode = 1;
	uint32_t leftChildNode = parentNode * 2;
	uint32_t smallestChildNode;
	while (leftChildNode <= heap->size) {
		uint32_t rightChildNode = leftChildNode + 1;
		if (rightChildNode <= heap->size) {
			if (heap_dataComparator(heap->store[rightChildNode - 1], heap->store[leftChildNode - 1]) > 0) {
				smallestChildNode = leftChildNode;
			} else {
				smallestChildNode = rightChildNode;
			}
		} else {
			smallestChildNode = leftChildNode;
		}
		if (heap_dataComparator(heap->store[parentNode - 1], heap->store[smallestChildNode - 1]) > 0) {
			OS_TCB_t * parent_temp = heap->store[parentNode - 1];
			heap->store[parentNode - 1] = heap->store[smallestChildNode - 1];
			heap->store[smallestChildNode - 1] = parent_temp;
		}
		parentNode = smallestChildNode;
		leftChildNode = parentNode * 2;
	}
}

void taskHeap_insert(_OS_taskHeap_t *heap, OS_TCB_t *task) {
	// The new element is always added to the end of a heap
	heap->store[(heap->size)++] = task;
	heap_up(heap);
}

OS_TCB_t * taskHeap_extract(_OS_taskHeap_t *heap) {
	// The root value is extracted, and the space filled by the value from the end
	// If the heap is empty, this will fail horribly...
	void * item = heap->store[0];
	heap->store[0] = heap->store[--(heap->size)];
	heap_down(heap);
	return item;
}

uint_fast8_t taskHeap_isEmpty(_OS_taskHeap_t *heap) {
	return !(heap->size);
}

void OS_sleep_delegate(_OS_SVC_StackFrame_t * stack);
/* Function to put a task to sleep for a number of ticks */
void OS_sleep_delegate(_OS_SVC_StackFrame_t * stack) {
	// Get the sleep duration that's been passed in
	uint32_t sleepDuration = stack->r0;
	// The running task's TCB is retrieved and stored
	OS_TCB_t * currentTCB = OS_currentTCB();
	// wakeTime time is calculated by adding sleepDuration to the elapsed OS ticks
	uint32_t wakeTime = OS_elapsedTicks() + sleepDuration;
	// wakeTime can be stored in TCB in the data field
	currentTCB->data = wakeTime;
	// Set the TCB state to sleeping
	currentTCB->state |= TASK_STATE_SLEEP;
	// Remove the sleeping task from the scheduler's task list
	list_remove(&task_list, currentTCB);
	// Place the just removed task into the heap
	taskHeap_insert(&sleeping_list, currentTCB);
	// Call PendSV to invoke _OS_scheduler to start the next task
	SCB->ICSR = SCB_ICSR_PENDSVSET_Msk;
}
