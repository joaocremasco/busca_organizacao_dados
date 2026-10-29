#ifndef DN_DN_LINKED_LIST_H
#define DN_DN_LINKED_LIST_H
	
#include <stdbool.h>

typedef struct dn_linked_list_ DN_LINKED_LIST;

// o liberador pode ser nulo quando a estrutura nao for dona dos objetos
DN_LINKED_LIST* dn_linked_list_init(void (*destroy)(void*));
// o liberador recebe cada objeto ao destruir a estrutura
void dn_linked_list_destroy(DN_LINKED_LIST **list);

bool dn_linked_list_push_back(DN_LINKED_LIST* list, void *object);
// remove da estrutura sem liberar o objeto
void* dn_linked_list_pop_back(DN_LINKED_LIST *list);
void* dn_linked_list_back(DN_LINKED_LIST *list);

bool dn_linked_list_push_front(DN_LINKED_LIST* list, void *object);
// remove da estrutura sem liberar o objeto
void* dn_linked_list_pop_front(DN_LINKED_LIST* list);
void* dn_linked_list_front(DN_LINKED_LIST *list);

// a comparacao devolve zero quando encontra o objeto
void* dn_linked_list_find(DN_LINKED_LIST *list, 
		void *target, int (*cmp)(void*, void*));

// remove da estrutura sem liberar o objeto
void* dn_linked_list_erase(DN_LINKED_LIST *list, 
		void *target, int (*cmp)(void*, void*));

// troca o objeto e devolve a referencia anterior
void* dn_linked_list_set(DN_LINKED_LIST *list, 
		void *object, void *target, int (*cmp)(void*, void*));

// coloca o novo objeto antes do alvo encontrado
bool dn_linked_list_insert(DN_LINKED_LIST *list, 
		void *object, void *target, int (*cmp)(void*, void*));

int dn_linked_list_size(DN_LINKED_LIST *list);
bool dn_linked_list_empty(DN_LINKED_LIST *list);

// interrompe o percurso quando a funcao devolve falso
bool dn_linked_list_visit(DN_LINKED_LIST *list, 
		bool (*visitor)(void *object, void *context), void *context);

#endif
