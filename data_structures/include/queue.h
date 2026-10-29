#ifndef QUEUE_H
#define QUEUE_H

#include <stdbool.h>

typedef struct queue_ QUEUE;

/* Cria e inicializa a estrutura; retorna NULL se a alocação falhar. A função opcional destroy é
   chamada para os objetos na destruição. */
QUEUE* queue_init(void (*destroy)(void*));
/* Libera a estrutura e seus nós ou vetores; chama destroy para os objetos se configurado. No
   projeto, ITEM_STORE é dono dos ITEMs. */
void queue_destroy(QUEUE **q);

/* Insere o ponteiro do objeto no fim; retorna false para argumento inválido ou falha de
   alocação. No projeto, ITEM_STORE continua dono do ITEM. */
bool queue_push(QUEUE *q, void *object);
/* Remove e devolve o objeto do início; retorna NULL se a estrutura estiver vazia ou inválida. Não
   libera o objeto armazenado. */
void* queue_pop(QUEUE *q);
/* Devolve sem remover o objeto do início; retorna NULL se a estrutura estiver vazia ou inválida.
   Não transfere sua propriedade. */
void* queue_front(QUEUE *q);
/* Devolve sem remover o objeto do fim; retorna NULL se a estrutura estiver vazia ou inválida.
   Não transfere sua propriedade. */
void* queue_back(QUEUE *q);

/* Devolve a quantidade de elementos; retorna 0 se a estrutura for NULL. */
int queue_size(QUEUE *q);
/* Retorna true se a estrutura estiver vazia ou for NULL; false se contiver elementos. */
bool queue_empty(QUEUE *q);

/* Percorre os elementos; retorna true se visitar tudo e false para argumentos inválidos ou
   parada da função de visita. */
bool queue_visit(QUEUE *q, bool (*visitor)(void *object, void *context), void *context);

#endif
