#ifndef STACK_H
#define STACK_H

#include <stdint.h>

void push(int **sp, int32_t value);
int32_t pop(int32_t **sp);

#endif /* STACK_H */
