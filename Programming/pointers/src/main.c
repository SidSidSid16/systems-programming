#include "Utils/utils.h"
#include <stdio.h>


static void square(long long *i) {
	
}

int main(void) {
	
	configClock();
	configUSART2(38400);
	
	int i = 3;
	
	printf("i = %p, i+1 = %p\n", (void *) &i, (void *) ((&i)+1));
	
	while(1);

}
