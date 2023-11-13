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
	strcpy(attempt, "\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xC1\x23\x00");
	if (0 == strcmp(attempt, pwd_store)) {
		secret();
	}
}

// Address of secret(): 0x080023C0 (ending in C1 setting Thumb bit)
// LR: 0x080002D3
