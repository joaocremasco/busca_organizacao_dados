#include <stdlib.h>
#include "../include/linked_list.h"

typedef struct node_ NODE;

struct node_ {
	NODE *next;
	NODE *prev;
	void *object;
};

struct linked_list_ {
	int size;
	NODE *front;
	NODE *back;
	void (*destroy)(void*);
};

LINKED_LIST* linked_list_init(void (*destroy)(void*)) {

	LINKED_LIST *list = malloc(sizeof(LINKED_LIST));
	
	if(list == NULL)
		return NULL;

	list->size = 0;
	list->front = NULL;
	list->back = NULL;
	list->destroy = destroy;

	return list;
}

void linked_list_destroy(LINKED_LIST **list) {
	if(list == NULL || *list == NULL)
		return;

	while((*list)->size > 0) {
		void *obj = linked_list_pop_back(*list);
		if((*list)->destroy != NULL)
			(*list)->destroy(obj);
	}

	free(*list);
	*list = NULL;
}

bool linked_list_push_back(LINKED_LIST *list, void *object) {
	if(list == NULL || object == NULL)
		return false;
	
	NODE *node = malloc(sizeof(NODE));

	if(node == NULL)
		return false;

	node->object = object;
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

void* linked_list_pop_back(LINKED_LIST *list) {
	if(list == NULL || list->size == 0)
		return NULL;
	
	void *object = list->back->object;
	
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

void* linked_list_back(LINKED_LIST *list) {
	if(list == NULL)
		return NULL;

	return list->size == 0 ? NULL : list->back->object;
}

bool linked_list_push_front(LINKED_LIST *list, void *object) {
	if(list == NULL || object == NULL)
		return false;
	
	NODE *node = malloc(sizeof(NODE));
	
	if(node == NULL)
		return false;

	node->object = object;
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

void* linked_list_pop_front(LINKED_LIST* list) {
	if(list == NULL || list->size == 0)
		return NULL;

	void *object = list->front->object;
	
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

void* linked_list_front(LINKED_LIST* list) {
	if(list == NULL)
		return NULL;

	return list->size == 0 ? NULL : list->front->object;
}

static NODE* find_helper(LINKED_LIST *list, void *object, int (*cmp)(void*, void*)) {
	if(list == NULL || object == NULL || cmp == NULL)
		return NULL;
	
	int list_size = list->size;
	NODE *idx = list->front;
	
	for(int i = 0; i < list_size; i++) {
		if(cmp(idx->object, object) == 0)
			return idx;

		idx = idx->next;
	}
	
	return NULL;
}

void* linked_list_find(LINKED_LIST *list, void *object, int (*cmp)(void*, void*)) {
	if(list == NULL || object == NULL || cmp == NULL)
		return NULL;
	
	NODE *res = find_helper(list, object, cmp);
	
	if(res == NULL)
		return NULL;
		
	return res->object;
}

void* linked_list_erase(LINKED_LIST *list, void *object, int (*cmp)(void*, void*)) {
	if(list == NULL || object == NULL || cmp == NULL)
		return NULL;
	
	NODE *idx = find_helper(list, object, cmp);

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

	free(idx);

	return obj;
}

void* linked_list_set(LINKED_LIST *list, void *object, void *target, int (*cmp)(void*, void*)) {
	if(list == NULL || object == NULL || target == NULL || cmp == NULL)
		return NULL;
		
	NODE *idx = find_helper(list, target, cmp);
	
	if(idx == NULL) {
		return NULL;
	}

	void* old = idx->object;

	idx->object = object;

	return old;
}

bool linked_list_insert(LINKED_LIST *list, void *object, void *target, int (*cmp)(void*, void*)) {
	if(list == NULL || object == NULL || target == NULL || cmp == NULL)
		return false;
		
	NODE *idx = find_helper(list, target, cmp);
	
	if(idx == NULL) {
		return false;
	}

	NODE *node = malloc(sizeof(NODE));

	if(node == NULL) {
		return false;
	}

	node->object = object;
	
	node->next = idx;
	node->prev = idx->prev;
	
	if(idx->prev != NULL)
		idx->prev->next = node;
	else {
		list->front = node;
	}

	idx->prev = node;
	
	list->size++;

	return true;
}

int linked_list_size(LINKED_LIST *list) {
	return list == NULL ? 0 : list->size;
}

bool linked_list_empty(LINKED_LIST *list) {
	return list == NULL ? true : list->size == 0;
}

bool linked_list_visit(LINKED_LIST *list, bool (*visitor)(void *object, void *context), void *context) {
	if(list == NULL || visitor == NULL)
		return false;

	for(NODE *node = list->front; node != NULL; node = node->next) {
		if(!visitor(node->object, context))
			return false;
	}

	return true;
}
