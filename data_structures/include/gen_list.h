#ifndef GEN_LIST_H
#define GEN_LIST_H
	
#include <stdbool.h>

typedef struct gen_list_ GEN_LIST;

// destroy pode ser nulo quando os átomos forem administrados por outro módulo.
GEN_LIST* gen_list_init(void (*destroy)(void*));
// Destrói as sublistas e chama destroy para cada átomo, se informado.
void gen_list_destroy(GEN_LIST **list);

// atom=true marca um item; false marca uma sublista.
bool gen_list_push_back(GEN_LIST* list, void *object, bool atom);
// isAtom recebe o tipo do conteúdo removido, quando não nulo.
void* gen_list_pop_back(GEN_LIST *list, bool *isAtom);
// isAtom recebe o tipo do conteúdo consultado, quando não nulo.
void* gen_list_back(GEN_LIST *list, bool *isAtom);

// atom=true marca um item; false marca uma sublista.
bool gen_list_push_front(GEN_LIST* list, void *object, bool atom);
// isAtom recebe o tipo do conteúdo removido, quando não nulo.
void* gen_list_pop_front(GEN_LIST* list, bool *isAtom);
// isAtom recebe o tipo do conteúdo consultado, quando não nulo.
void* gen_list_front(GEN_LIST *list, bool *isAtom);

// atom seleciona o tipo procurado; isAtom recebe o tipo encontrado, quando não nulo.
void* gen_list_find(GEN_LIST *list, void *object, 
		bool atom, int (*cmp)(void*, void*), bool *isAtom);

// atom seleciona o tipo procurado; isAtom recebe o tipo removido, quando não nulo.
void* gen_list_erase(GEN_LIST *list, void *object, 
		bool atom, int (*cmp)(void*, void*), bool *isAtom);

// Os flags indicam os tipos novo e antigo; isAtom recebe o tipo substituído.
void* gen_list_set(GEN_LIST *list, void *object, 
		bool obj_atom, void *target, bool targ_atom, int (*cmp)(void*, void*), bool *isAtom);

// Insere antes do primeiro alvo igual, respeitando targ_atom.
bool gen_list_insert(GEN_LIST *list, void *object, 
		bool obj_atom, void *target, bool targ_atom, int (*cmp)(void*, void*));

int gen_list_size(GEN_LIST *list);
bool gen_list_empty(GEN_LIST *list);

// Percorre os átomos dentro das sublistas e para se visitor devolver false.
bool gen_list_visit(GEN_LIST *list, 
		bool (*visitor)(void *object, void *context), void *context);

#endif
