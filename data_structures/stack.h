#ifndef STACK_H
#define STACK_H

#include <stdbool.h>
#include "node.h"

typedef struct stack_ STACK;

STACK* stack_init();
void stack_erase(STACK **st);
bool stack_push(STACK *st, OBJECT object);
OBJECT stack_pop(STACK *st);
OBJECT stack_top(STACK *st);
int stack_size(STACK *st);

#endif
