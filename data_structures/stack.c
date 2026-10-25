#include <stdlib.h>
#include "stack.h"

#define STARTING_CAPACITY 8

struct stack_ {
	int size;
	int cap;
	void **array;
	void (*destroy)(void*);
};

STACK* stack_init(void (*destroy)(void*)) {

	STACK *st = malloc(sizeof(STACK));
	
	if(st == NULL)
		return NULL;
	
	st->size = 0;
	st->cap = STARTING_CAPACITY;
	st->array = malloc(STARTING_CAPACITY * sizeof(void*));
	st->destroy = destroy;
	
	if(st->array == NULL) {
		free(st);
		return NULL;
	}

	return st;
}

void stack_destroy(STACK **st) {
	if(st == NULL || *st == NULL)
		return;
		
	int st_size = (*st)->size;
	
	if((*st)->destroy != NULL) {
		for(int i = 0; i < st_size; i++)
			(*st)->destroy(stack_pop(*st));
	}

	free((*st)->array);
	free(*st);
	*st = NULL;
}

bool stack_resize(STACK *st, int capacity) {
	if(st == NULL || capacity <= 0)
		return false;
	
	if(capacity < st->size) {
		if(st->destroy != NULL) {
			for(int i = capacity; i < st->size; i++) {
				st->destroy(st->array[i]);
			}
		}
		st->size = capacity;
	}

	void **tmp = realloc(st->array, capacity * sizeof(void*));

	if(tmp == NULL)
		return false;

	st->array = tmp;
	st->cap = capacity;

	return true;
}

bool stack_push(STACK *st, void *object) {
	if(st == NULL || object == NULL)
		return false;
	
	if(st->size == st->cap) {
		if(!stack_resize(st, st->cap * 2))
			return false;
	}

	st->array[st->size] = object;

	st->size++;
	
	return true;
}

void* stack_pop(STACK *st) {
	if(st == NULL || st->size == 0) {
		return NULL;
	}

	st->size--;

	return st->array[st->size];
}

void* stack_top(STACK *st) {
	if(st == NULL || st->size == 0) {
		return NULL;
	}

	return st->array[st->size - 1];
}

int stack_size(STACK *st) {
	return st == NULL ? 0 : st->size;
}

bool stack_empty(STACK *st) {
	return st == NULL ? true : st->size == 0;
}

bool stack_visit(const STACK *st, bool (*visitor)(void *object, void *context), void *context) {
	if(st == NULL || visitor == NULL)
		return false;

	for(int i = st->size - 1; i >= 0; i--) {
		if(!visitor(st->array[i], context))
			return false;
	}

	return true;
}
