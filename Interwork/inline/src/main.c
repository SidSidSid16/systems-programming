#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include "Utils/utils.h"

int main(void) {
	configClock();
	configUSART2(38400);

	uint32_t x=3, y=5, z=0;
  __asm volatile (
    "ADD %[intermediate], %[input], %[input], LSL #3\n\t"							/* z = x*9 */
    "ADD %[intermediate], %[intermediate], %[input], LSL #2\n\t" 			/* z = z +(x*4) = 13x */
		"ADD %[output], %[output], %[intermediate]\n\t" 									/* z = z +(x*4) = 13x */
    : [output] "+&r" (y)
    : [input] "r" (x), [intermediate] "r" (z)
	);

	
	printf("z = %" PRIu32 "\n", z);
	printf("y = %" PRIu32 "\n", y);

	while(1);
}
