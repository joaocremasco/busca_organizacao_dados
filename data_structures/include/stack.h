#ifndef STACK_H
#define STACK_H

#include <stdbool.h>

typedef struct stack_ STACK;

STACK* stack_init(void (*destroy)(void*));
void stack_destroy(STACK **st);

bool stack_push(STACK *st, void *object);
void* stack_pop(STACK *st);
void* stack_top(STACK *st);

int stack_size(STACK *st);
bool stack_empty(STACK *st);

bool stack_visit(STACK *st, bool (*visitor)(void *object, void *context), void *context);

#endif
