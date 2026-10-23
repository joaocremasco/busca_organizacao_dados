#ifndef LINKED_LIST_H
#define LINKED_LIST_H
	
#include <stdbool.h>

typedef struct linked_list_ LINKED_LIST;

LINKED_LIST* linked_list_init();
void linked_list_clear(LINKED_LIST **list);
bool linked_list_push_back(LINKED_LIST* list, void *object);
void* linked_list_pop_back(LINKED_LIST *list);
void* linked_list_back(LINKED_LIST *list);
bool linked_list_push_front(LINKED_LIST* list, void *object);
void* linked_list_pop_front(LINKED_LIST* list);
void* linked_list_front(LINKED_LIST *list);
bool linked_list_find(LINKED_LIST *list, void *object);
bool linked_list_erase(LINKED_LIST *list, void *object);
bool linked_list_insert(LINKED_LIST *list, void *object, void *target);
int linked_list_size(LINKED_LIST *list);
bool linked_list_empty(LINKED_LIST *list);

#endif
