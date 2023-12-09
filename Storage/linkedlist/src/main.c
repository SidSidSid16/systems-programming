#include "Utils/utils.h"
#include <stdio.h>
#include <inttypes.h>
#include "list.h"
#include "string.h"



int main(void) {
	
	configClock();
	configUSART2(38400);
	
	list_t list = LIST_INITIALISER;

	list_item_t examples[] = {
		{.data = "first"},
		{.data = "second"},
		{.data = "third"},
		{.data = "fourth"},
		{.data = "fifth"},
		{.data = "sixth"}
	};

	puts("Inserting 4 elements\r\n\r\n");
	list_insert(&list, &examples[0]);
	list_insert(&list, &examples[1]);
	list_insert(&list, &examples[2]);
	list_insert(&list, &examples[3]);
	
	// Show 10 elements
	for (uint_fast8_t i = 0; i < 10; ++i) {
		printf("Item: %s\r\n", list_next(&list)->data);
	}
	
	puts("\r\nInserting 2 elements\r\n\r\n");
	list_insert(&list, &examples[4]);
	list_insert(&list, &examples[5]);

	// Show 10 elements
	for (uint_fast8_t i = 0; i < 10; ++i) {
		printf("Item: %s\r\n", list_next(&list)->data);
	}
	
	puts("\r\nRemoving 3 elements\r\n\r\n");
	list_remove(&list, &examples[0]);
	list_remove(&list, &examples[2]);
	list_remove(&list, &examples[4]);
	
	// Show 10 elements
	for (uint_fast8_t i = 0; i < 10; ++i) {
		printf("Item: %s\r\n", list_next(&list)->data);
	}

	puts("\r\nRemoving 3 elements\r\n\r\n");
	list_remove(&list, &examples[1]);
	list_remove(&list, &examples[3]);
	list_remove(&list, &examples[5]);
	
	printf("list_next() returns %p\r\n", (void *)list_next(&list));
	while(1);
}
