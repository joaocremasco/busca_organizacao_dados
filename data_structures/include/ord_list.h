#ifndef ORD_LIST_H
#define ORD_LIST_H

#include <stdbool.h>

typedef struct ord_list_ ORD_LIST;
typedef int (*ORD_LIST_COMPARE)(void *left, void *right, void *context);

ORD_LIST* ord_list_init(void (*destroy)(void*), int (*cmp)(void*, void*));
ORD_LIST* ord_list_init_with_context(void (*destroy)(void*),
                                     ORD_LIST_COMPARE cmp, void *context);
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

bool ord_list_visit(ORD_LIST *sq, 
		bool (*visitor)(void *object, void *context), void *context);

#endif
