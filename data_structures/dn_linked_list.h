#ifndef DN_DN_LINKED_LIST_H
#define DN_DN_LINKED_LIST_H
	
#include <stdbool.h>

typedef struct dn_linked_list_ DN_LINKED_LIST;

DN_LINKED_LIST* dn_linked_list_init();
void dn_linked_list_clear(DN_LINKED_LIST **list);

bool dn_linked_list_push_back(DN_LINKED_LIST* list, void *object);
void* dn_linked_list_pop_back(DN_LINKED_LIST *list);
void* dn_linked_list_back(DN_LINKED_LIST *list);

bool dn_linked_list_push_front(DN_LINKED_LIST* list, void *object);
void* dn_linked_list_pop_front(DN_LINKED_LIST* list);
void* dn_linked_list_front(DN_LINKED_LIST *list);

void* dn_linked_list_find(DN_LINKED_LIST *list, void *target, int (*cmp)(void*, void*));
void* dn_linked_list_erase(DN_LINKED_LIST *list, void *target, int (*cmp)(void*, void*));
bool dn_linked_list_insert(DN_LINKED_LIST *list, void *object, void *target, int (*cmp)(void*, void*));

int dn_linked_list_size(DN_LINKED_LIST *list);
bool dn_linked_list_empty(DN_LINKED_LIST *list);

#endif
