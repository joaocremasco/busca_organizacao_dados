#ifndef ORD_LIST_H
#define ORD_LIST_H

#include <stdbool.h>

typedef struct ord_list_ ORD_LIST;

ORD_LIST* ord_list_init();
void ord_list_clear(ORD_LIST **sq);
bool ord_list_insert(ORD_LIST *sq, void *object, int (*cmp)(void*, void*));
bool ord_list_resize(ORD_LIST *sq, int capacity);
void* ord_list_erase(ORD_LIST *sq, void *object, int (*cmp)(void*, void*));
void* ord_list_get(ORD_LIST *sq, int idx);
int ord_list_find(ORD_LIST *sq, void *object, int (*cmp)(void*, void*));
int ord_list_upper_bound(ORD_LIST *sq, void *object, int (*cmp)(void*, void*));
int ord_list_lower_bound(ORD_LIST *sq, void *object, int (*cmp)(void*, void*));
int ord_list_size(ORD_LIST *sq);
bool ord_list_empty(ORD_LIST *sq);

#endif
