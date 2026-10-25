#ifndef ORD_LIST_H
#define ORD_LIST_H

#include <stdbool.h>

typedef struct ord_list_ ORD_LIST;

ORD_LIST* ord_list_init(void (*destroy)(void*), int (*cmp)(void*, void*));
void ord_list_destroy(ORD_LIST **sq);

bool ord_list_insert(ORD_LIST *sq, void *object);
void* ord_list_erase(ORD_LIST *sq, void *target);

bool ord_list_resize(ORD_LIST *sq, int capacity);
void* ord_list_get(ORD_LIST *sq, int idx);

/// isso aqui tudo retorna indice na array!!!!!!!
int ord_list_find(ORD_LIST *sq, void *target);
int ord_list_upper_bound(ORD_LIST *sq, void *target);
int ord_list_lower_bound(ORD_LIST *sq, void *target);

int ord_list_size(ORD_LIST *sq);
bool ord_list_empty(ORD_LIST *sq);

bool ord_list_visit(const ORD_LIST *sq, 
		bool (*visitor)(void *object, void *context), void *context);

#endif
