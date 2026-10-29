#ifndef GEN_LIST_H
#define GEN_LIST_H
	
#include <stdbool.h>

typedef struct gen_list_ GEN_LIST;

// destroy e opcional, passa NULL se nao precisar
GEN_LIST* gen_list_init(void (*destroy)(void*));
// se tiver destroy chama em cada objeto, mas aqui quem libera os itens e o ITEM_STORE
void gen_list_destroy(GEN_LIST **list);

// atom diz se e item ou sublista
bool gen_list_push_back(GEN_LIST* list, void *object, bool atom);
// isAtom recebe o tipo do que saiu
void* gen_list_pop_back(GEN_LIST *list, bool *isAtom);
// isAtom recebe o tipo
void* gen_list_back(GEN_LIST *list, bool *isAtom);

// atom diz se e item ou sublista
bool gen_list_push_front(GEN_LIST* list, void *object, bool atom);
// isAtom recebe o tipo do que saiu
void* gen_list_pop_front(GEN_LIST* list, bool *isAtom);
// isAtom recebe o tipo
void* gen_list_front(GEN_LIST *list, bool *isAtom);

// isAtom recebe o tipo do que foi achado
void* gen_list_find(GEN_LIST *list, void *object, 
		bool atom, int (*cmp)(void*, void*), bool *isAtom);

// isAtom recebe o tipo do que saiu
void* gen_list_erase(GEN_LIST *list, void *object, 
		bool atom, int (*cmp)(void*, void*), bool *isAtom);

// isAtom recebe o tipo do que foi trocado
void* gen_list_set(GEN_LIST *list, void *object, 
		bool obj_atom, void *target, bool targ_atom, int (*cmp)(void*, void*), bool *isAtom);

// insere antes do alvo
bool gen_list_insert(GEN_LIST *list, void *object, 
		bool obj_atom, void *target, bool targ_atom, int (*cmp)(void*, void*));

int gen_list_size(GEN_LIST *list);
bool gen_list_empty(GEN_LIST *list);

// se o visitor devolver false ja para por ali
bool gen_list_visit(GEN_LIST *list, 
		bool (*visitor)(void *object, void *context), void *context);

#endif
