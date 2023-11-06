#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include "Utils/utils.h"

int main(void) {
	configClock();
	configUSART2(38400);

	uint32_t x=3, y=5, z=0;
  __asm volatile (
    "ADD %[output], %[input], %[input], LSL #3\n\t" /* x*9 */
    "ADD %[output], %[output], %[input], LSL #2" /* +(x*4) */
    : [output] "=&r" (y)
    : [input] "r" (x)
	);

	printf("z = %" PRIu32 "\n", z);

	while(1);
}
