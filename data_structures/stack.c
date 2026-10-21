#include <stdlib.h>
#include "stack.h"

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

void stack_erase(STACK **st) {
	if(st == NULL || *st == NULL)
		return;
	
	int stack_size = (*st)->size;
	
	NODE *stack_top = (*st)->top;
	NODE *tmp = NULL;
	
	for(int i = 0; i < stack_size; i++) {
		tmp = node_get_prev(stack_top);
		
		node_erase(stack_top);

		stack_top = tmp;
	}

	free(*st);
	*st = NULL;
}

bool stack_push(STACK *st, OBJECT object) {
	if(st == NULL || object.content_type == NULL_TYPE)
		return false;
	
	NODE *node = node_init();
	node_set_object(node, object);
	
	node_set_prev(node, st->top);
	st->top = node;

	st->size++;
	
	return true;
}

OBJECT stack_pop(STACK *st) {
	if(st == NULL) {
		return (OBJECT) { .content = NULL, .content_type = NULL_TYPE};
	}

	OBJECT object = node_get_object(st->top);
	
	NODE *tmp = st->top;

	st->top = node_get_prev(tmp);

	node_erase(tmp);

	st->size--;

	return object;
}

OBJECT stack_top(STACK *st) {
	if(st == NULL) {
		return (OBJECT) { .content = NULL, .content_type = NULL_TYPE};
	}

	return node_get_object(st->top);
}

int stack_size(STACK *st) {
	return st == NULL ? -1 : st->size;
}
