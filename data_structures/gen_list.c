#include <stdlib.h>
#include "gen_list.h"

typedef struct node_ NODE;

struct node_ {
	NODE *next;
	NODE *prev;
	bool atom; // 1 eh atomo | 0 nao eh
	void *object;
};

struct gen_list_ {
	int size;
	NODE *front;
	NODE *back;
};

GEN_LIST* gen_list_init() {
	
	GEN_LIST *list = malloc(sizeof(GEN_LIST));
	
	if(list == NULL)
		return NULL;

	list->size = 0;
	list->front = NULL;
	list->back = NULL;

	return list;
}

void gen_list_clear(void *l) {
	
	GEN_LIST **list = l;

	if(list == NULL || *list == NULL)
		return;

	int list_size = (*list)->size;

	for(int i = 0; i < list_size; i++) {
		if(!(*list)->back->atom) {
			gen_list_clear(&(*list)->back->object);
		}

		gen_list_pop_back(*list);
	}

	free(*list);
	*list = NULL;
}

bool gen_list_push_back(GEN_LIST *list, void *object, bool atom) {
	if(list == NULL || object == NULL)
		return false;
	
	NODE *node = malloc(sizeof(NODE));

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

void* gen_list_pop_back(GEN_LIST *list) {
	if(list == NULL || list->size == 0)
		return NULL;
	
	void *object = list->back->object;
	
	NODE *tmp = list->back;
	
	if(tmp->prev != NULL)
		tmp->prev->next = NULL;
	
	list->back = tmp->prev;
	
	free(tmp);

	list->size--;

	return object;
}

void* gen_list_back(GEN_LIST *list) {
	return list == NULL ? NULL : list->back;
}

bool gen_list_push_front(GEN_LIST *list, void *object, bool atom) {
	if(list == NULL)
		return false;
	
	NODE *node = malloc(sizeof(NODE));
	
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

void* gen_list_pop_front(GEN_LIST* list) {
	if(list == NULL || list->size == 0)
		return NULL;

	void *object = list->front->object;
	
	NODE *tmp = list->front;

	if(tmp->next != NULL)
		tmp->next->prev = NULL;

	list->front = list->front->next;

	free(tmp);

	list->size--;

	return object;
}

void* gen_list_front(GEN_LIST* list) {
	return list == NULL ? NULL : list->front;
}

NODE* find_helper(GEN_LIST *list, void *object, int (*cmp)(void*, void*)) {
	if(list == NULL || object == NULL)
		return NULL;
	
	int list_size = list->size;
	NODE *idx = list->front;
	
	for(int i = 0; i < list_size; i++) {
		if(cmp(idx->object, object))
			return idx;

		idx = idx->next;
	}
	
	return NULL;
}

void* gen_list_find(GEN_LIST *list, void *object, int (*cmp)(void*, void*)) {
	if(list == NULL || object == NULL)
		return NULL;
	
	NODE *res = find_helper(list, object, cmp);
	
	if(res == NULL)
		return NULL;
		
	return res->object;
}

void* gen_list_erase(GEN_LIST *list, void *object, int (*cmp)(void*, void*)) {
	if(list == NULL || object == NULL)
		return NULL;
	
	NODE *idx = find_helper(list, object, cmp);

	if(idx == NULL)
		return NULL;

	void *obj = idx->object;
	
	if(idx->prev != NULL) {
		idx->prev->next = idx->next;
	}
	if(idx->next != NULL) {
		idx->next->prev = idx->prev;
	}
		
	list->size--;

	free(idx);

	return obj;
}

bool gen_list_insert(GEN_LIST *list, void *object, bool atom, void *target, int (*cmp)(void*, void*)) {
	if(list == NULL || object == NULL || target == NULL)
		return false;
		
	NODE *node = malloc(sizeof(NODE));

	if(node == NULL) {
		return false;
	}

	NODE *idx = find_helper(list, target, cmp);
	
	if(idx == NULL) {
		free(node);
		return false;
	}

	node->object = object;
	node->atom = atom;
	
	node->next = idx;
	node->prev = idx->prev;
	idx->prev = node;

	return true;
}

int gen_list_size(GEN_LIST *list) {
	return list == NULL ? 0 : list->size;
}

bool gen_list_empty(GEN_LIST *list) {
	return list == NULL ? true : list->size == 0;
}

bool gen_list_visit(const GEN_LIST *list,
                    bool (*visitor)(void *data, void *context),
                    void *context) {
	if(list == NULL || visitor == NULL)
		return false;

	for(NODE *node = list->front; node != NULL; node = node->next) {
		if(!visitor(node->object, context))
			return false;
	}

	return true;
}
