#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include <stdbool.h>
#include <stddef.h>

typedef struct linked_list LINKED_LIST;

LINKED_LIST *linked_list_create(void);
void   linked_list_destroy(LINKED_LIST **list);
bool   linked_list_insert(LINKED_LIST *list, void *data);
void  *linked_list_remove(LINKED_LIST *list, const void *data);
size_t linked_list_size(const LINKED_LIST *list);
bool   linked_list_is_empty(const LINKED_LIST *list);
bool   linked_list_visit(const LINKED_LIST *list,
                         bool (*visitor)(void *data, void *context),
                         void *context);

#endif
