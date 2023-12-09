#ifndef LIST_H
#define LIST_H

typedef struct list_item_s {
	char *data;
	struct list_item_s *prev;
	struct list_item_s *next;
} list_item_t;

typedef struct {
	list_item_t *head;
} list_t;

#define LIST_INITIALISER { .head = 0 }

void list_insert(list_t *list, list_item_t *item);

void list_remove(list_t *list, list_item_t *item);

list_item_t * list_next(list_t *list);

#endif /* LIST_H */
