#ifndef QUEUE_H
#define QUEUE_H

#include <stdbool.h>

typedef struct queue_ QUEUE;

// destroy e opcional, passa NULL se nao precisar
QUEUE* queue_init(void (*destroy)(void*));
// se tiver destroy chama em cada objeto, mas aqui quem libera os itens e o ITEM_STORE
void queue_destroy(QUEUE **q);

bool queue_push(QUEUE *q, void *object);
// so tira da estrutura, o objeto continua vivo
void* queue_pop(QUEUE *q);
void* queue_front(QUEUE *q);
void* queue_back(QUEUE *q);

int queue_size(QUEUE *q);
bool queue_empty(QUEUE *q);

// se o visitor devolver false ja para por ali
bool queue_visit(QUEUE *q, bool (*visitor)(void *object, void *context), void *context);

#endif
