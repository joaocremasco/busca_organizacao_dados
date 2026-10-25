#ifndef GEN_LIST_H
#define GEN_LIST_H
	
#include <stdbool.h>

typedef struct gen_list_ GEN_LIST;

GEN_LIST* gen_list_init();
void gen_list_clear(void *list); // PASSAR PONTEIRO DUPLO!!!!!!

bool gen_list_push_back(GEN_LIST* list, void *object, bool atom);
void* gen_list_pop_back(GEN_LIST *list);
void* gen_list_back(GEN_LIST *list);

bool gen_list_push_front(GEN_LIST* list, void *object, bool atom);
void* gen_list_pop_front(GEN_LIST* list);
void* gen_list_front(GEN_LIST *list);

void* gen_list_find(GEN_LIST *list, void *target, int (*cmp)(void*, void*));
void* gen_list_erase(GEN_LIST *list, void *target, int (*cmp)(void*, void*));
bool gen_list_insert(GEN_LIST *list, void *object, bool atom, void *target, int (*cmp)(void*, void*));

int gen_list_size(GEN_LIST *list);
bool gen_list_empty(GEN_LIST *list);
bool gen_list_visit(const GEN_LIST *list,
                    bool (*visitor)(void *data, void *context),
                    void *context);

#endif
