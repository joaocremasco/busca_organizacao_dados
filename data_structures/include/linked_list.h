#ifndef LINKED_LIST_H
#define LINKED_LIST_H
	
#include <stdbool.h>

typedef struct linked_list_ LINKED_LIST;

// destroy pode ser nulo quando os objetos forem administrados por outro módulo.
LINKED_LIST* linked_list_init(void (*destroy)(void*));
// Chama destroy para cada objeto, se o callback tiver sido informado.
void linked_list_destroy(LINKED_LIST **list);

bool linked_list_push_back(LINKED_LIST* list, void *object);
// Remove e devolve o objeto sem chamar destroy.
void* linked_list_pop_back(LINKED_LIST *list);
void* linked_list_back(LINKED_LIST *list);

bool linked_list_push_front(LINKED_LIST* list, void *object);
// Remove e devolve o objeto sem chamar destroy.
void* linked_list_pop_front(LINKED_LIST* list);
void* linked_list_front(LINKED_LIST *list);

// cmp devolve zero quando considera os dois objetos iguais.
void* linked_list_find(LINKED_LIST *list, 
		void *target, int (*cmp)(void*, void*));

// Desvincula e devolve o objeto sem chamar destroy.
void* linked_list_erase(LINKED_LIST *list, 
		void *target, int (*cmp)(void*, void*));

// Substitui o objeto encontrado e devolve a referência anterior.
void* linked_list_set(LINKED_LIST *list, void *object, 
		void *target, int (*cmp)(void*, void*));

// Insere antes do primeiro alvo que cmp considerar igual.
bool linked_list_insert(LINKED_LIST *list, void *object, 
		void *target, int (*cmp)(void*, void*));

int linked_list_size(LINKED_LIST *list);
bool linked_list_empty(LINKED_LIST *list);

// Para e devolve false se o visitor devolver false.
bool linked_list_visit(LINKED_LIST *list, 
		bool (*visitor)(void *object, void *context), void *context);

#endif
