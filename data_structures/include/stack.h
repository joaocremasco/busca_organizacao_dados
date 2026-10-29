#ifndef STACK_H
#define STACK_H

#include <stdbool.h>

typedef struct stack_ STACK;

// o liberador pode ser nulo quando a estrutura nao for dona dos objetos
STACK* stack_init(void (*destroy)(void*));
// o liberador recebe cada objeto ao destruir a estrutura
void stack_destroy(STACK **st);

bool stack_push(STACK *st, void *object);
// remove da estrutura sem liberar o objeto
void* stack_pop(STACK *st);
void* stack_top(STACK *st);

int stack_size(STACK *st);
bool stack_empty(STACK *st);

// interrompe o percurso quando a funcao devolve falso
bool stack_visit(STACK *st, bool (*visitor)(void *object, void *context), void *context);

#endif
