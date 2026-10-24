#ifndef GEN_LIST_H
#define GEN_LIST_H
	
#include <stdbool.h>

typedef struct gen_list_ GEN_LIST;

GEN_LIST* gen_list_init(void (*destroy)(void*));
void gen_list_destroy(GEN_LIST **list);

bool gen_list_push_back(GEN_LIST* list, void *object, bool atom);
void* gen_list_pop_back(GEN_LIST *list, bool *isAtom);
void* gen_list_back(GEN_LIST *list, bool *isAtom);

bool gen_list_push_front(GEN_LIST* list, void *object, bool atom);
void* gen_list_pop_front(GEN_LIST* list, bool *isAtom);
void* gen_list_front(GEN_LIST *list, bool *isAtom);

void* gen_list_find(GEN_LIST *list, void *target, int (*cmp)(void*, void*), bool *isAtom);
void* gen_list_erase(GEN_LIST *list, void *target, int (*cmp)(void*, void*), bool *isAtom);
bool gen_list_insert(GEN_LIST *list, void *object, bool atom, void *target, int (*cmp)(void*, void*));
void* gen_list_set(GEN_LIST *list, void *object, bool atom, void *target, int (*cmp)(void*, void*), bool *isAtom);

int gen_list_size(GEN_LIST *list);
bool gen_list_empty(GEN_LIST *list);

#endif
