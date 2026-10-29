#ifndef LINKED_LIST_H
#define LINKED_LIST_H
	
#include <stdbool.h>

typedef struct linked_list_ LINKED_LIST;

// o liberador pode ser nulo quando a estrutura nao for dona dos objetos
LINKED_LIST* linked_list_init(void (*destroy)(void*));
// o liberador recebe cada objeto ao destruir a estrutura
void linked_list_destroy(LINKED_LIST **list);

bool linked_list_push_back(LINKED_LIST* list, void *object);
// remove da estrutura sem liberar o objeto
void* linked_list_pop_back(LINKED_LIST *list);
void* linked_list_back(LINKED_LIST *list);

bool linked_list_push_front(LINKED_LIST* list, void *object);
// remove da estrutura sem liberar o objeto
void* linked_list_pop_front(LINKED_LIST* list);
void* linked_list_front(LINKED_LIST *list);

// a comparacao devolve zero quando encontra o objeto
void* linked_list_find(LINKED_LIST *list, 
		void *target, int (*cmp)(void*, void*));

// remove da estrutura sem liberar o objeto
void* linked_list_erase(LINKED_LIST *list, 
		void *target, int (*cmp)(void*, void*));

// troca o objeto e devolve a referencia anterior
void* linked_list_set(LINKED_LIST *list, void *object, 
		void *target, int (*cmp)(void*, void*));

// coloca o novo objeto antes do alvo encontrado
bool linked_list_insert(LINKED_LIST *list, void *object, 
		void *target, int (*cmp)(void*, void*));

int linked_list_size(LINKED_LIST *list);
bool linked_list_empty(LINKED_LIST *list);

// interrompe o percurso quando a funcao devolve falso
bool linked_list_visit(LINKED_LIST *list, 
		bool (*visitor)(void *object, void *context), void *context);

#endif
