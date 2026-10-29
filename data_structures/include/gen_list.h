#ifndef GEN_LIST_H
#define GEN_LIST_H
	
#include <stdbool.h>

typedef struct gen_list_ GEN_LIST;

// o liberador pode ser nulo quando a estrutura nao for dona dos objetos
GEN_LIST* gen_list_init(void (*destroy)(void*));
// o liberador recebe cada objeto ao destruir a estrutura
void gen_list_destroy(GEN_LIST **list);

// indica se o objeto e um item ou uma sublista
bool gen_list_push_back(GEN_LIST* list, void *object, bool atom);
// informa se o objeto removido era item ou sublista
void* gen_list_pop_back(GEN_LIST *list, bool *isAtom);
// informa se o objeto e item ou sublista
void* gen_list_back(GEN_LIST *list, bool *isAtom);

// indica se o objeto e um item ou uma sublista
bool gen_list_push_front(GEN_LIST* list, void *object, bool atom);
// informa se o objeto removido era item ou sublista
void* gen_list_pop_front(GEN_LIST* list, bool *isAtom);
// informa se o objeto e item ou sublista
void* gen_list_front(GEN_LIST *list, bool *isAtom);

// informa se o objeto e item ou sublista do que foi achado
void* gen_list_find(GEN_LIST *list, void *object, 
		bool atom, int (*cmp)(void*, void*), bool *isAtom);

// informa se o objeto removido era item ou sublista
void* gen_list_erase(GEN_LIST *list, void *object, 
		bool atom, int (*cmp)(void*, void*), bool *isAtom);

// informa se o objeto e item ou sublista do que foi trocado
void* gen_list_set(GEN_LIST *list, void *object, 
		bool obj_atom, void *target, bool targ_atom, int (*cmp)(void*, void*), bool *isAtom);

// coloca o novo objeto antes do alvo encontrado
bool gen_list_insert(GEN_LIST *list, void *object, 
		bool obj_atom, void *target, bool targ_atom, int (*cmp)(void*, void*));

int gen_list_size(GEN_LIST *list);
bool gen_list_empty(GEN_LIST *list);

// interrompe o percurso quando a funcao devolve falso
bool gen_list_visit(GEN_LIST *list, 
		bool (*visitor)(void *object, void *context), void *context);

#endif
