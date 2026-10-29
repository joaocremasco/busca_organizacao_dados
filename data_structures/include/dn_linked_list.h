#ifndef DN_DN_LINKED_LIST_H
#define DN_DN_LINKED_LIST_H
	
#include <stdbool.h>

typedef struct dn_linked_list_ DN_LINKED_LIST;

// destroy e opcional, passa NULL se nao precisar
DN_LINKED_LIST* dn_linked_list_init(void (*destroy)(void*));
// se tiver destroy chama em cada objeto, mas aqui quem libera os itens e o ITEM_STORE
void dn_linked_list_destroy(DN_LINKED_LIST **list);

bool dn_linked_list_push_back(DN_LINKED_LIST* list, void *object);
// so tira da estrutura, o objeto continua vivo
void* dn_linked_list_pop_back(DN_LINKED_LIST *list);
void* dn_linked_list_back(DN_LINKED_LIST *list);

bool dn_linked_list_push_front(DN_LINKED_LIST* list, void *object);
// so tira da estrutura, o objeto continua vivo
void* dn_linked_list_pop_front(DN_LINKED_LIST* list);
void* dn_linked_list_front(DN_LINKED_LIST *list);

// cmp devolve 0 quando bate
void* dn_linked_list_find(DN_LINKED_LIST *list, 
		void *target, int (*cmp)(void*, void*));

// so tira da estrutura, o objeto continua vivo
void* dn_linked_list_erase(DN_LINKED_LIST *list, 
		void *target, int (*cmp)(void*, void*));

// devolve o objeto que estava no lugar
void* dn_linked_list_set(DN_LINKED_LIST *list, 
		void *object, void *target, int (*cmp)(void*, void*));

// insere antes do alvo
bool dn_linked_list_insert(DN_LINKED_LIST *list, 
		void *object, void *target, int (*cmp)(void*, void*));

int dn_linked_list_size(DN_LINKED_LIST *list);
bool dn_linked_list_empty(DN_LINKED_LIST *list);

// se o visitor devolver false ja para por ali
bool dn_linked_list_visit(DN_LINKED_LIST *list, 
		bool (*visitor)(void *object, void *context), void *context);

#endif
