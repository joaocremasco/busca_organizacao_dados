/* Implementa uma lista generalizada com nós atômicos ou sublistas. */
#include <stdlib.h>
#include "../include/gen_list.h"

typedef struct node_ NODE;

struct node_ {
	NODE *next;
	NODE *prev;
	bool atom; // true indica um átomo; false indica uma sublista.
	void *object;
};

struct gen_list_ {
	int size;
	NODE *front;
	NODE *back;
	void (*destroy)(void*);
};

/* Atualiza o marcador de átomo quando o chamador forneceu esse endereço. */
static void set_atom(bool *ptr, bool v) {
	if(ptr != NULL)
		*ptr = v;
}

GEN_LIST* gen_list_init(void (*destroy)(void*)) {

	GEN_LIST *list = malloc(sizeof(*list));
	
	if(list == NULL)
		return NULL;

	list->size = 0;
	list->front = NULL;
	list->back = NULL;
	list->destroy = destroy;

	return list;
}

void gen_list_destroy(GEN_LIST **list) {
	if(list == NULL || *list == NULL)
		return;

	while((*list)->size > 0) {
		bool atom;
		void *obj = gen_list_pop_back(*list, &atom);

		if(atom) {
			if((*list)->destroy != NULL) {
				(*list)->destroy(obj);
			}
		}
		else {
			GEN_LIST *sub = obj;
			gen_list_destroy(&sub);
		}
	}

	free(*list);
	*list = NULL;
}

bool gen_list_push_back(GEN_LIST *list, void *object, bool atom) {
	if(list == NULL || object == NULL)
		return false;
	
	NODE *node = malloc(sizeof(*node));

	if(node == NULL)
		return false;

	node->object = object;
	node->atom = atom;
	node->next = NULL;
	
	if(list->size == 0) {
		node->prev = NULL;
		list->front = node;
	}
	else {
		node->prev = list->back;
		list->back->next = node;
	}

	list->back = node;
	
	list->size++;

	return true;
}

void* gen_list_pop_back(GEN_LIST *list, bool *isAtom) {
	
	set_atom(isAtom, false);
	
	if(list == NULL || list->size == 0) {
		return NULL;
	}
	
	void *object = list->back->object;
	set_atom(isAtom, list->back->atom);
	
	NODE *tmp = list->back;
	
	if(tmp->prev != NULL)
		tmp->prev->next = NULL;
	
	list->back = tmp->prev;

	if(list->back == NULL)
		list->front = NULL;
	
	free(tmp);

	list->size--;
	
	return object;
}

void* gen_list_back(GEN_LIST *list, bool *isAtom) {
	
	set_atom(isAtom, false);

	if(list == NULL || list->size == 0) {
		return NULL;
	}
	
	set_atom(isAtom, list->back->atom);
	return list->back->object;
}

bool gen_list_push_front(GEN_LIST *list, void *object, bool atom) {
	if(list == NULL || object == NULL)
		return false;
	
	NODE *node = malloc(sizeof(*node));
	
	if(node == NULL)
		return false;

	node->object = object;
	node->atom = atom;
	node->prev = NULL;
	
	if(list->size == 0) {
		node->next = NULL;
		list->back = node;
	}
	else {
		node->next = list->front;
		list->front->prev = node;
	}

	list->front = node;
	
	list->size++;
	
	return true;
}

void* gen_list_pop_front(GEN_LIST* list, bool *isAtom) {
	
	set_atom(isAtom, false);

	if(list == NULL || list->size == 0) {
		return NULL;
	}

	void *object = list->front->object;
	set_atom(isAtom, list->front->atom);

	NODE *tmp = list->front;

	if(tmp->next != NULL)
		tmp->next->prev = NULL;

	list->front = list->front->next;

	if(list->front == NULL)
		list->back = NULL;

	free(tmp);

	list->size--;

	return object;
}

void* gen_list_front(GEN_LIST* list, bool *isAtom) {
	
	set_atom(isAtom, false);
	
	if(list == NULL || list->size == 0) {
		return NULL;
	}
	
	set_atom(isAtom, list->front->atom);
	return list->front->object;
}

/* Busca recursivamente um nó que corresponda ao objeto e ao tipo de entrada. */
static NODE* find_helper(GEN_LIST *list, void *object, int (*cmp)(void*, void*), bool atom) {
	if(list == NULL || object == NULL || cmp == NULL) {
		return NULL;
	}

	int list_size = list->size;
	NODE *idx = list->front;
	
	for(int i = 0; i < list_size; i++) {
		if(idx->atom == atom && cmp(idx->object, object) == 0) {
			return idx;
		}

		idx = idx->next;
	}
	
	return NULL;
}

void* gen_list_find(GEN_LIST *list, void *object, bool atom, int (*cmp)(void*, void*), bool *isAtom) {
	
	set_atom(isAtom, false);

	if(list == NULL || object == NULL || cmp == NULL) {
		return NULL;
	}

	NODE *res = find_helper(list, object, cmp, atom);
	
	if(res == NULL)
		return NULL;
	
	set_atom(isAtom, res->atom);

	return res->object;
}

void* gen_list_erase(GEN_LIST *list, void *object, bool atom, int (*cmp)(void*, void*), bool *isAtom) {
	
	set_atom(isAtom, false);

	if(list == NULL || object == NULL || cmp == NULL) {
		return NULL;
	}
	
	NODE *idx = find_helper(list, object, cmp, atom);

	if(idx == NULL)
		return NULL;

	void *obj = idx->object;
	
	if(idx->prev != NULL) {
		idx->prev->next = idx->next;
	}
	else {
		list->front = idx->next;
	}
	if(idx->next != NULL) {
		idx->next->prev = idx->prev;
	}
	else {
		list->back = idx->prev;
	}
		
	list->size--;
	
	set_atom(isAtom, idx->atom);

	free(idx);

	return obj;
}

bool gen_list_insert(GEN_LIST *list, void *object, bool obj_atom, void *target, bool targ_atom, int (*cmp)(void*, void*)) {
	if(list == NULL || object == NULL || target == NULL || cmp == NULL)
		return false;
		
	NODE *idx = find_helper(list, target, cmp, targ_atom);
	
	if(idx == NULL) {
		return false;
	}

	NODE *node = malloc(sizeof(*node));

	if(node == NULL) {
		return false;
	}

	node->object = object;
	node->atom = obj_atom;
	
	node->next = idx;
	node->prev = idx->prev;

	if(idx->prev != NULL)
		idx->prev->next = node;
	else
		list->front = node;

	idx->prev = node;
	
	list->size++;

	return true;
}

void* gen_list_set(GEN_LIST *list, void *object, bool obj_atom, void *target, bool targ_atom, int (*cmp)(void*, void*), bool *isAtom) {
	
	set_atom(isAtom, false);

	if(list == NULL || object == NULL || target == NULL || cmp == NULL) {
		return NULL;
	}
		
	NODE *idx = find_helper(list, target, cmp, targ_atom);
	
	if(idx == NULL) {
		return NULL;
	}
	
	void *obj = idx->object;
	set_atom(isAtom, idx->atom);

	idx->object = object;
	idx->atom = obj_atom;

	return obj;
}

int gen_list_size(GEN_LIST *list) {
	return list == NULL ? 0 : list->size;
}

bool gen_list_empty(GEN_LIST *list) {
	return list == NULL ? true : list->size == 0;
}

bool gen_list_visit(GEN_LIST *list, bool (*visitor)(void *object, void *context), void *context) {
	if(list == NULL || visitor == NULL)
		return false;

	for(NODE *node = list->front; node != NULL; node = node->next) {
		if(node->atom) {
			if(!visitor(node->object, context))
				return false;
		}
		else if(node->object != NULL && !gen_list_visit(node->object, visitor, context)) {
			return false;
		}
	}

	return true;
}
