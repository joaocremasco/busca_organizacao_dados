#ifndef QUEUE_H
#define QUEUE_H

#include <stdbool.h>

typedef struct queue_ QUEUE;

QUEUE* queue_init(void (*destroy)(void*));
void queue_destroy(QUEUE **q);

bool queue_push(QUEUE *q, void *object);
void* queue_pop(QUEUE *q);
void* queue_front(QUEUE *q);
void* queue_back(QUEUE *q);

int queue_size(QUEUE *q);
bool queue_empty(QUEUE *q);
/* Percorre na ordem natural. true = percorreu tudo; false = o visitor
   pediu para parar ou argumento invalido. Nao aloca nem libera nada. */
bool queue_visit(const QUEUE *q,
                 bool (*visitor)(void *data, void *context),
                 void *context);

#endif
