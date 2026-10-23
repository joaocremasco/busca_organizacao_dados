#include <stdlib.h>
#include "linked_list.h"

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
};

LINKED_LIST* linked_list_init() {
	
	LINKED_LIST *list = malloc(sizeof(LINKED_LIST));
	
	if(list == NULL)
		return NULL;

	list->size = 0;
	list->front = NULL;
	list->back = NULL;

	return list;
}

void linked_list_clear(LINKED_LIST **list) {
	if(list == NULL || *list == NULL)
		return;

	int list_size = (*list)->size;

	for(int i = 0; i < list_size; i++) {
		linked_list_pop_back(*list);
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
	
	free(tmp);

	list->size--;

	return object;
}

void* linked_list_back(LINKED_LIST *list) {
	return list == NULL ? NULL : list->back;
}

bool linked_list_push_front(LINKED_LIST *list, void *object) {
	if(list == NULL)
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

	free(tmp);

	list->size--;

	return object;
}

void* linked_list_front(LINKED_LIST* list) {
	return list == NULL ? NULL : list->front;
}

NODE* find_helper(LINKED_LIST *list, void *object, int (*cmp)(void*, void*)) {
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

void* linked_list_find(LINKED_LIST *list, void *object, int (*cmp)(void*, void*)) {
	if(list == NULL || object == NULL)
		return NULL;
	
	NODE *res = find_helper(list, object, cmp);
	
	if(res == NULL)
		return NULL;
		
	return res->object;
}

void* linked_list_erase(LINKED_LIST *list, void *object, int (*cmp)(void*, void*)) {
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

bool linked_list_insert(LINKED_LIST *list, void *object, void *target, int (*cmp)(void*, void*)) {
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
	
	node->next = idx;
	node->prev = idx->prev;
	idx->prev = node;

	return true;
}

int linked_list_size(LINKED_LIST *list) {
	return list == NULL ? 0 : list->size;
}

bool linked_list_empty(LINKED_LIST *list) {
	return list == NULL ? true : list->size == 0;
}
