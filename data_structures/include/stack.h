#ifndef STACK_H
#define STACK_H

#include <stdbool.h>

typedef struct stack_ STACK;

/* Cria e inicializa a estrutura; retorna NULL se a alocação falhar. A função opcional destroy é
   chamada para os objetos na destruição. */
STACK* stack_init(void (*destroy)(void*));
/* Libera a estrutura e seus nós ou vetores; chama destroy para os objetos se configurado. No
   projeto, ITEM_STORE é dono dos ITEMs. */
void stack_destroy(STACK **st);

/* Insere o ponteiro do objeto no fim; retorna false para argumento inválido ou falha de
   alocação. No projeto, ITEM_STORE continua dono do ITEM. */
bool stack_push(STACK *st, void *object);
/* Remove e devolve o objeto do fim; retorna NULL se a estrutura estiver vazia ou inválida. Não
   libera o objeto armazenado. */
void* stack_pop(STACK *st);
/* Devolve sem remover o objeto do topo; retorna NULL se a estrutura estiver vazia ou inválida.
   Não transfere sua propriedade. */
void* stack_top(STACK *st);

/* Devolve a quantidade de elementos; retorna 0 se a estrutura for NULL. */
int stack_size(STACK *st);
/* Retorna true se a estrutura estiver vazia ou for NULL; false se contiver elementos. */
bool stack_empty(STACK *st);

/* Percorre os elementos; retorna true se visitar tudo e false para argumentos inválidos ou
   parada da função de visita. */
bool stack_visit(STACK *st, bool (*visitor)(void *object, void *context), void *context);

#endif
