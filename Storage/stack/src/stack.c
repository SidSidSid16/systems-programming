#include "stack.h"

void push(int32_t **sp, int32_t value) {
	**sp = value;
	(*sp)++;
}

int32_t pop(int **sp) {
	(*sp)--;
	return **sp;
}
