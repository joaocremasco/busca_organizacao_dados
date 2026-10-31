#ifndef STACK_H
#define STACK_H

#include <stdbool.h>

typedef struct stack_ STACK;

// destroy pode ser nulo quando os objetos forem administrados por outro módulo.
STACK* stack_init(void (*destroy)(void*));
// Chama destroy para cada objeto, se o callback tiver sido informado.
void stack_destroy(STACK **st);

bool stack_push(STACK *st, void *object);
// Remove e devolve o objeto sem chamar destroy.
void* stack_pop(STACK *st);
void* stack_top(STACK *st);

int stack_size(STACK *st);
bool stack_empty(STACK *st);

// Percorre do topo para a base e para se visitor devolver false.
bool stack_visit(STACK *st, bool (*visitor)(void *object, void *context), void *context);

#endif
