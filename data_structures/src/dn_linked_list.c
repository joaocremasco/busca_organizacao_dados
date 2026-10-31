// Lista com sentinela: front nunca guarda um objeto; o primeiro fica em front->next.
#include <stdlib.h>
#include "../include/dn_linked_list.h"

typedef struct node_ NODE;

struct node_ {
	NODE *next;
	void *object;
};

struct dn_linked_list_ {
	int size;
	NODE *front;
	NODE *back;
	void (*destroy)(void*);
};

// Cria a sentinela e deixa front e back apontando para ela na lista vazia.
// Devolve NULL se não houver memória.
DN_LINKED_LIST* dn_linked_list_init(void (*destroy)(void*)) {
	
	DN_LINKED_LIST *list = malloc(sizeof(*list));
	
	if(list == NULL)
		return NULL;

	NODE *dummy = malloc(sizeof(*dummy));

	if(dummy == NULL) {
		free(list);
		return NULL;
	}

	dummy->next = NULL;
	dummy->object = NULL;

	list->size = 0;
	list->front = dummy;
	list->back = dummy;
	list->destroy = destroy;

	return list;
}

// Libera os nós e a sentinela; o callback opcional recebe cada objeto removido.
// No projeto, os ITEMs pertencem ao ITEM_STORE.
void dn_linked_list_destroy(DN_LINKED_LIST **list) {
	if(list == NULL || *list == NULL)
		return;
	
	while((*list)->size > 0) {
		void *obj = dn_linked_list_pop_front(*list);
		if((*list)->destroy != NULL)
			(*list)->destroy(obj);
	}

	free((*list)->front);
	free(*list);
	*list = NULL;
}

// Acrescenta depois de back e avança esse ponteiro; a sentinela também é back
// quando a lista está vazia. Devolve false se a alocação falhar.
bool dn_linked_list_push_back(DN_LINKED_LIST *list, void *object) {
	if(list == NULL || object == NULL)
		return false;
	
	NODE *node = malloc(sizeof(*node));

	if(node == NULL)
		return false;

	node->object = object;
	node->next = NULL;
	
	list->back->next = node;

	list->back = node;
	
	list->size++;

	return true;
}

// Remove o último objeto; percorre a partir da sentinela para achar o anterior.
// Devolve o objeto sem destruí-lo ou NULL se a lista estiver vazia.
void* dn_linked_list_pop_back(DN_LINKED_LIST *list) {
	if(list == NULL || list->size == 0)
		return NULL;
	
	void *object = list->back->object;
	
	NODE *tmp = list->back;
	NODE *idx = list->front;

	int list_size = list->size;
	
	for(int i = 0; i < list_size - 1; i++) {
		idx = idx->next;
	}

	idx->next = NULL;

	list->back = idx;

	free(tmp);

	list->size--;

	return object;
}

void* dn_linked_list_back(DN_LINKED_LIST *list) {
	if(list == NULL || list->size == 0)
		return NULL;

	return list->back->object;
}

// Insere logo após a sentinela; na lista vazia, o novo nó também vira back.
// Devolve false se a alocação falhar.
bool dn_linked_list_push_front(DN_LINKED_LIST *list, void *object) {
	if(list == NULL || object == NULL)
		return false;
	
	NODE *node = malloc(sizeof(*node));
	
	if(node == NULL)
		return false;

	node->object = object;
	node->next = list->front->next;
	
	list->front->next = node;

	if(list->size == 0)
		list->back = node;
	
	list->size++;
	
	return true;
}

// Remove front->next e restaura back para a sentinela quando era o único nó.
// Devolve o objeto sem destruí-lo ou NULL se a lista estiver vazia.
void* dn_linked_list_pop_front(DN_LINKED_LIST* list) {
	if(list == NULL || list->size == 0)
		return NULL;

	void *object = list->front->next->object;
	
	NODE *tmp = list->front->next;

	list->front->next = tmp->next;

	if(tmp == list->back)
		list->back = list->front;

	free(tmp);

	list->size--;

	return object;
}

void* dn_linked_list_front(DN_LINKED_LIST* list) {
	if(list == NULL || list->size == 0)
		return NULL;

	return list->front->next->object;
}

// Compara cada objeto com object usando cmp; devolve o encontrado ou NULL.
void* dn_linked_list_find(DN_LINKED_LIST *list, void *object, int (*cmp)(void*, void*)) {
	if(list == NULL || object == NULL || cmp == NULL)
		return NULL;
	
	int list_size = list->size;
	NODE *idx = list->front->next;
	
	for(int i = 0; i < list_size; i++) {
		if(cmp(idx->object, object) == 0)
			return idx->object;

		idx = idx->next;
	}

	return NULL;
}

// Desliga o primeiro objeto igual a object, ajustando back se era o último.
// Devolve o ponteiro removido sem destruí-lo, ou NULL se não houver igualdade.
void* dn_linked_list_erase(DN_LINKED_LIST *list, void *object, int (*cmp)(void*, void*)) {
	if(list == NULL || object == NULL || cmp == NULL)
		return NULL;
	
	int list_size = list->size;
	
	NODE *idx = list->front->next;
	NODE *prev = list->front;

	for(int i = 0; i < list_size; i++) {
		if(cmp(idx->object, object) == 0) {
			void* obj = idx->object;
			
			prev->next = idx->next;


			if(idx == list->back)
				list->back = prev;
			
			free(idx);
			list->size--;

			return obj;
		}
		
		prev = idx;
		idx = idx->next;
	}

	return NULL;
}

// Insere object antes do primeiro target igual segundo cmp; a sentinela cobre
// o caso em que target é o primeiro nó. Devolve false se não encontrar target.
bool dn_linked_list_insert(DN_LINKED_LIST *list, void *object, void *target, int (*cmp)(void*, void*)) {
	if(list == NULL || object == NULL || target == NULL || cmp == NULL)
		return false;
		
	NODE *node = malloc(sizeof(*node));

	if(node == NULL) {
		return false;
	}

	node->object = object;

	int list_size = list->size;
	
	NODE *idx = list->front->next;
	NODE *prev = list->front;

	for(int i = 0; i < list_size; i++) {
		if(cmp(idx->object, target) == 0) {
			prev->next = node;
			node->next = idx;

			list->size++;
			
			return true;
		}
		
		prev = idx;
		idx = idx->next;
	}

	free(node);
	
	return false;
}

// Substitui o primeiro target igual segundo cmp e devolve o objeto anterior.
// Devolve NULL se os argumentos forem inválidos ou target não existir.
void* dn_linked_list_set(DN_LINKED_LIST *list, void *object, void *target, int (*cmp)(void*, void*)) {
	if(list == NULL || object == NULL || target == NULL || cmp == NULL)
		return NULL;
		
	int list_size = list->size;
	
	NODE *idx = list->front->next;

	for(int i = 0; i < list_size; i++) {
		if(cmp(idx->object, target) == 0) {
			void *obj = idx->object;
			idx->object = object;
			
			return obj;
		}
		
		idx = idx->next;
	}

	return NULL;
}

int dn_linked_list_size(DN_LINKED_LIST *list) {
	return list == NULL ? 0 : list->size;
}

bool dn_linked_list_empty(DN_LINKED_LIST *list) {
	return list == NULL ? true : list->size == 0;
}

// Percorre apenas os nós após a sentinela; false indica argumento inválido
// ou que visitor pediu para encerrar antes do fim.
bool dn_linked_list_visit(DN_LINKED_LIST *list, bool (*visitor)(void *object, void *context), void *context) {
	if(list == NULL || visitor == NULL)
		return false;

	for(NODE *node = list->front->next; node != NULL; node = node->next) {
		if(!visitor(node->object, context))
			return false;
	}

	return true;
}
