#ifndef STACK_H
#define STACK_H

#include <stdbool.h>

typedef struct stack_ STACK;

// destroy e opcional, passa NULL se nao precisar
STACK* stack_init(void (*destroy)(void*));
// se tiver destroy chama em cada objeto, mas aqui quem libera os itens e o ITEM_STORE
void stack_destroy(STACK **st);

bool stack_push(STACK *st, void *object);
// so tira da estrutura, o objeto continua vivo
void* stack_pop(STACK *st);
void* stack_top(STACK *st);

int stack_size(STACK *st);
bool stack_empty(STACK *st);

// se o visitor devolver false ja para por ali
bool stack_visit(STACK *st, bool (*visitor)(void *object, void *context), void *context);

#endif
