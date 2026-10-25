#ifndef CROSSED_LIST_H
#define CROSSED_LIST_H

#include <stdbool.h>

typedef struct crossed_list_ CROSSED_LIST;

CROSSED_LIST* crossed_list_init(void (*destroy)(void*));
void crossed_list_destroy(CROSSED_LIST **list);

void* crossed_list_get(CROSSED_LIST *list, int i, int j);
void* crossed_list_set(CROSSED_LIST *list, int i, int j, void *object);
void* crossed_list_find(CROSSED_LIST *list, void *target, int (*cmp)(void*, void*), int *i, int *j);
void* crossed_list_remove(CROSSED_LIST *list, int i, int j);
bool crossed_list_remove_line(CROSSED_LIST *list, int index, bool isRow);

bool crossed_list_resize(CROSSED_LIST *list, int capacity, bool isRow);
int crossed_list_size(CROSSED_LIST *list);
int crossed_list_capacity(CROSSED_LIST *list, bool isRow);
bool crossed_list_empty(CROSSED_LIST *list);
/* Percorre na ordem natural. true = percorreu tudo; false = o visitor
   pediu para parar ou argumento invalido. Nao aloca nem libera nada. */
bool crossed_list_visit(const CROSSED_LIST *list,
                        bool (*visitor)(void *data, void *context),
                        void *context);

#endif
