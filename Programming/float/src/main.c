#include "Utils/utils.h"
#include <stdio.h>

int main(void) {
	
	configClock();
	configUSART2(38400);
	
	float t = 0.0;
	while(1) {
		t += 0.3f;
		printf("t = %f\r\n", (double) t);
	}

}
