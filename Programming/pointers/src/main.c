#include "Utils/utils.h"
#include <stdio.h>


static void square(long long *i) {
	printf("after: %lld\n", (*i)*(*i));
}

int main(void) {
	
	configClock();
	configUSART2(38400);
	
	long long i = 3;
	
	printf("i = %p, i+1 = %p\n", (void *) &i, (void *) ((&i)+1));
	
	printf("before: %lld\n", i);
	
	square(&i);
	
	while(1);

}
