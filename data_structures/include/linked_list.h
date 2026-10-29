#ifndef LINKED_LIST_H
#define LINKED_LIST_H
	
#include <stdbool.h>

typedef struct linked_list_ LINKED_LIST;

// destroy e opcional, passa NULL se nao precisar
LINKED_LIST* linked_list_init(void (*destroy)(void*));
// se tiver destroy chama em cada objeto, mas aqui quem libera os itens e o ITEM_STORE
void linked_list_destroy(LINKED_LIST **list);

bool linked_list_push_back(LINKED_LIST* list, void *object);
// so tira da estrutura, o objeto continua vivo
void* linked_list_pop_back(LINKED_LIST *list);
void* linked_list_back(LINKED_LIST *list);

bool linked_list_push_front(LINKED_LIST* list, void *object);
// so tira da estrutura, o objeto continua vivo
void* linked_list_pop_front(LINKED_LIST* list);
void* linked_list_front(LINKED_LIST *list);

// cmp devolve 0 quando bate
void* linked_list_find(LINKED_LIST *list, 
		void *target, int (*cmp)(void*, void*));

// so tira da estrutura, o objeto continua vivo
void* linked_list_erase(LINKED_LIST *list, 
		void *target, int (*cmp)(void*, void*));

// devolve o objeto que estava no lugar
void* linked_list_set(LINKED_LIST *list, void *object, 
		void *target, int (*cmp)(void*, void*));

// insere antes do alvo
bool linked_list_insert(LINKED_LIST *list, void *object, 
		void *target, int (*cmp)(void*, void*));

int linked_list_size(LINKED_LIST *list);
bool linked_list_empty(LINKED_LIST *list);

// se o visitor devolver false ja para por ali
bool linked_list_visit(LINKED_LIST *list, 
		bool (*visitor)(void *object, void *context), void *context);

#endif
