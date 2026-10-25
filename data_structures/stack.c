#include <stdlib.h>
#include "stack.h"

typedef struct node_ NODE;

struct node_ {
	NODE *prev;
	void* object;
};

struct stack_ {
	int size;
	NODE *top;
};

STACK* stack_init() {
	
	STACK *st = malloc(sizeof(STACK));
	
	if(st == NULL)
		return NULL;
	
	st->size = 0;
	st->top = NULL;

	return st;
}

void stack_clear(STACK **st) {
	if(st == NULL || *st == NULL)
		return;
	
	int stack_size = (*st)->size;
	
	for(int i = 0; i < stack_size; i++) {
		stack_pop(*st);	
	}

	free(*st);
	*st = NULL;
}

bool stack_push(STACK *st, void *object) {
	if(st == NULL || object == NULL)
		return false;
	
	NODE *node = malloc(sizeof(NODE));

	if(node == NULL)
		return false;
	
	node->object = object;
	
	node->prev = st->top;
	
	st->top = node;

	st->size++;
	
	return true;
}

void* stack_pop(STACK *st) {
	if(st == NULL || st->size == 0) {
		return NULL;
	}

	void *object = st->top->object;
	
	NODE *tmp = st->top;

	st->top = tmp->prev;

	free(tmp);

	st->size--;

	return object;
}

void* stack_top(STACK *st) {
	if(st == NULL || st->size == 0) {
		return NULL;
	}

	return st->top->object;
}

int stack_size(STACK *st) {
	return st == NULL ? -1 : st->size;
}

bool stack_empty(STACK *st) {
	return st == NULL ? true : st->size == 0;
}

bool stack_visit(const STACK *st,
                 bool (*visitor)(void *data, void *context),
                 void *context) {
	if(st == NULL || visitor == NULL)
		return false;

	for(NODE *node = st->top; node != NULL; node = node->prev) {
		if(!visitor(node->object, context))
			return false;
	}

	return true;
}
