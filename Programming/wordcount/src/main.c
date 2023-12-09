#include "Utils/utils.h"
#include <stdio.h>
#include <string.h>

static int isAlpha(char *string) {
	return (*string >= '0' && *string <= '9') || (*string >= 'a' && *string <= 'z') || (*string >= 'A' && *string <= 'Z');
}

static int wordcountNext(char * string) {
	int words = 0;
	while (*string) {
		if (isAlpha(string)) {
			if (!isAlpha(string+1)) {
				words++;
			}
		}
		string++;
	}
	return words;	
}

static int wordcountNextInv(char * string) {
	int words = 0;
	while (*string) {
		if (!isAlpha(string)) {
			if (isAlpha(string+1)) {
				words++;
			}
		}
		string++;
	}
	return words;	
}

static int wordcountFlag(char * string) {
	int words = 0;
	int inWord = 0;
	while (*string) {
		if (isAlpha(string)) {
			if (!inWord) {
				inWord = 1;
				words++;
			}
		} else {
			inWord = 0;
		}
		string++;
	}
	return words;
}

int main(void) {
	
	configClock();
	configUSART2(38400);
	
	// This text contains 11 words.
	char * text = ", This is a  word count test.\nHere is the second line\nAndy Andy Andrew Pomfret!";
	printf("The text is:\n%s\n", text);
	printf("It contains %d words.\n", wordcountNext(text));
	printf("It contains %d words.\n", wordcountNextInv(text));
	printf("It contains %d words.\n", wordcountFlag(text));

	while(1);
}

