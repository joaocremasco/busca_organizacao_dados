#include <stdlib.h>
#include "dn_linked_list.h"

typedef struct node_ NODE;

struct node_ {
	NODE *next;
	void *object;
};

struct dn_linked_list_ {
	int size;
	NODE *front;
	NODE *back;
};

DN_LINKED_LIST* dn_linked_list_init() {
	
	DN_LINKED_LIST *list = malloc(sizeof(DN_LINKED_LIST));
	
	if(list == NULL)
		return NULL;

	NODE *dummy = malloc(sizeof(NODE));

	if(dummy == NULL) {
		free(list);
		return NULL;
	}

	dummy->next = NULL;
	dummy->object = NULL;

	list->size = 0;
	list->front = dummy;
	list->back = dummy;

	return list;
}

void dn_linked_list_clear(DN_LINKED_LIST **list) {
	if(list == NULL || *list == NULL)
		return;

	int list_size = (*list)->size;

	for(int i = 0; i < list_size; i++) {
		dn_linked_list_pop_back(*list);
	}

	free(*list);
	*list = NULL;
}

bool dn_linked_list_push_back(DN_LINKED_LIST *list, void *object) {
	if(list == NULL || object == NULL)
		return false;
	
	NODE *node = malloc(sizeof(NODE));

	if(node == NULL)
		return false;

	node->object = object;
	node->next = NULL;
	
	list->back->next = node;

	list->back = node;
	
	list->size++;

	return true;
}

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
	return list == NULL || list->size == 0 ? NULL : list->back;
}

bool dn_linked_list_push_front(DN_LINKED_LIST *list, void *object) {
	if(list == NULL)
		return false;
	
	NODE *node = malloc(sizeof(NODE));
	
	if(node == NULL)
		return false;

	node->object = object;
	node->next = list->front->next;
	
	list->front->next = node;
	
	list->size++;
	
	return true;
}

void* dn_linked_list_pop_front(DN_LINKED_LIST* list) {
	if(list == NULL || list->size == 0)
		return NULL;

	void *object = list->front->object;
	
	NODE *tmp = list->front->next;

	list->front->next = tmp->next;

	free(tmp);

	list->size--;

	return object;
}

void* dn_linked_list_front(DN_LINKED_LIST* list) {
	return list == NULL || list->size == 0 ? NULL : list->front->next;
}

void* dn_linked_list_find(DN_LINKED_LIST *list, void *object, int (*cmp)(void*, void*)) {
	if(list == NULL || object == NULL)
		return NULL;
	
	int list_size = list->size;
	NODE *idx = list->front->next;
	
	for(int i = 0; i < list_size; i++) {
		if(cmp(idx->object, object))
			return idx->object;

		idx = idx->next;
	}

	return NULL;
}

void* dn_linked_list_erase(DN_LINKED_LIST *list, void *object, int (*cmp)(void*, void*)) {
	if(list == NULL || object == NULL)
		return NULL;
	
	int list_size = list->size;
	
	NODE *idx = list->front->next;
	NODE *prev = list->front;

	for(int i = 0; i < list_size; i++) {
		if(cmp(idx->object, object)) {
			void* obj = idx->object;
			
			prev->next = idx->next;
			free(idx);
			
			return obj;
		}
		
		prev = idx;
		idx = idx->next;
	}

	return NULL;
}

bool dn_linked_list_insert(DN_LINKED_LIST *list, void *object, void *target, int (*cmp)(void*, void*)) {
	if(list == NULL || object == NULL || target == NULL)
		return false;
		
	NODE *node = malloc(sizeof(NODE));

	if(node == NULL) {
		return false;
	}

	int list_size = list->size;
	
	NODE *idx = list->front->next;
	NODE *prev = list->front;

	for(int i = 0; i < list_size; i++) {
		if(cmp(idx->object, target)) {
			prev->next = node;
			node->next = idx;
			
			return true;
		}
		
		prev = idx;
		idx = idx->next;
	}

	free(node);
	
	return false;
}

int dn_linked_list_size(DN_LINKED_LIST *list) {
	return list == NULL ? 0 : list->size;
}

bool dn_linked_list_empty(DN_LINKED_LIST *list) {
	return list == NULL ? true : list->size == 0;
}

bool dn_linked_list_visit(const DN_LINKED_LIST *list,
                          bool (*visitor)(void *data, void *context),
                          void *context) {
	if(list == NULL || visitor == NULL)
		return false;

	for(NODE *node = list->front->next; node != NULL; node = node->next) {
		if(!visitor(node->object, context))
			return false;
	}

	return true;
}
