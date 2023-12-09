#include "list.h"

void list_insert(list_t *list, list_item_t *item) {
	if (!list->head) {
		item->prev = item;
		item->next = item;
	} else {
		item->next = list->head;
		item->prev = list->head->prev;
		list->head->prev->next = item;
		list->head->prev = item;
	}
	list->head = item;
}

void list_remove(list_t *list, list_item_t *item) {
	if (item->next == item) {
		list->head = 0;
		return;
	} else if (list->head == item) {
		list->head = item->next;
	}
	item->next->prev = item->prev;
	item->prev->next = item->next;
}

list_item_t * list_next(list_t *list) {
	list_item_t * item = list->head;
	list->head = item->next;
	return item;
}
