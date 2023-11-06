#include "Utils/utils.h"
#include <stdio.h>
#include <string.h>

static void secret(void) {
	printf("Access granted\r\n");
}

int main(void) {
	configClock();
	configUSART2(38400);

	char const *pwd_store = "password";
	char attempt[10];
	
	// Try to overwrite the link register here so that secret() is run
	// even if the password is incorrect
	strcpy(attempt, "attempt");
	if (0 == strcmp(attempt, pwd_store)) {
		secret();
	}
}
