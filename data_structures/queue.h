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
bool queue_visit(const QUEUE *q,
                 bool (*visitor)(void *data, void *context),
                 void *context);

#endif
