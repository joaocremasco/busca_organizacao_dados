#include <stdlib.h>
#include "../include/seq_list.h"

#define STARTING_CAPACITY 8

struct seq_list_ {
	int size;
	int capacity;
	void **array;
	void (*destroy)(void*);
};

SEQ_LIST* seq_list_init(void (*destroy)(void*)) {

	SEQ_LIST* sq = malloc(sizeof(*sq));
	
	if(sq == NULL)
		return NULL;

	sq->size = 0;
	sq->capacity = STARTING_CAPACITY;
	sq->array = malloc(sizeof(*sq->array) * STARTING_CAPACITY);
	sq->destroy = destroy;

	if(sq->array == NULL) {
		free(sq);
		return NULL;
	}

	return sq;
}

void seq_list_destroy(SEQ_LIST **sq) {
	if(sq == NULL || *sq == NULL)
		return;
	
	int sq_size = (*sq)->size;
	
	if((*sq)->destroy != NULL) {
		for(int i = 0; i < sq_size; i++)
			(*sq)->destroy((*sq)->array[i]);
	}

	free((*sq)->array);

	free(*sq);
	*sq = NULL;
}

bool seq_list_resize(SEQ_LIST *sq, int capacity) {
	if(sq == NULL || capacity <= 0)
		return false;
	
	if(capacity < sq->size) {
		if(sq->destroy != NULL) {
			for(int i = capacity; i < sq->size; i++)
				sq->destroy(sq->array[i]);
		}

		sq->size = capacity;
	}

	void **tmp_array = realloc(sq->array, capacity * sizeof(*tmp_array));
		
	if(tmp_array == NULL)
		return false;
	
	sq->array = tmp_array;
	
	sq->capacity = capacity;
	
	return true;
}

bool seq_list_push_back(SEQ_LIST* sq, void *object) {
	if(sq == NULL || object == NULL)
		return false;
	
	if(sq->size == sq->capacity) {
		if(!seq_list_resize(sq, sq->capacity * 2))
			return false;
	}

	sq->array[sq->size++] = object;

	return true;
}

bool seq_list_insert(SEQ_LIST* sq, void *object, int idx) {
	if(sq == NULL || object == NULL || idx < 0 || idx > sq->size)
		return false;

	if(sq->size == sq->capacity) {
		if(!seq_list_resize(sq, sq->capacity * 2))
			return false;
	}

	int seq_size = sq->size;

	for(int i = seq_size; i > idx; i--) {
		sq->array[i] = sq->array[i - 1];
	}

	sq->array[idx] = object;
	
	sq->size++;
	
	return true;
}

void* seq_list_erase(SEQ_LIST* sq, int idx) {
	if(sq == NULL || idx < 0 || idx >= sq->size)
		return NULL;
	
	int seq_size = sq->size;
	
	void *object = sq->array[idx];

	for(int i = idx + 1; i < seq_size; i++) {
		sq->array[i - 1] = sq->array[i];
	}

	sq->size--;

	return object;
}

void* seq_list_pop_back(SEQ_LIST* sq) {
	if(sq == NULL || sq->size == 0)
		return NULL;
	
	void *object = sq->array[--sq->size];

	return object;
}

int seq_list_find(SEQ_LIST *sq, void *object, int (*cmp)(void*, void*)) {
	if(sq == NULL || object == NULL || cmp == NULL)
		return -1;
	
	int sq_size = sq->size;

	for(int i = 0; i < sq_size; i++) {
		if(cmp(sq->array[i], object) == 0)
			return i;
	}

	return -1;
}

void* seq_list_get(SEQ_LIST *sq, int idx) {
	if(sq == NULL || idx < 0 || idx >= sq->size)
		return NULL;

	return sq->array[idx];
}

void* seq_list_set(SEQ_LIST *sq, void *object, int idx) {
	if(sq == NULL || object == NULL ||  idx < 0 || idx >= sq->size)
		return NULL;
	
	void *obj = sq->array[idx];
	
	sq->array[idx] = object;

	return obj;
}

int seq_list_size(SEQ_LIST *sq) {
	return sq == NULL ? 0 : sq->size;
}

bool seq_list_empty(SEQ_LIST *sq) {
	return sq == NULL ? true : sq->size == 0;
}

bool seq_list_visit(SEQ_LIST *sq, bool (*visitor)(void *object, void *context), void *context) {
	if(sq == NULL || visitor == NULL)
		return false;

	for(int i = 0; i < sq->size; i++) {
		if(!visitor(sq->array[i], context))
			return false;
	}

	return true;
}
