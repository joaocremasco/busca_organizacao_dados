#ifndef QUEUE_H
#define QUEUE_H

#include <stdbool.h>

typedef struct queue_ QUEUE;

// destroy pode ser nulo quando os objetos forem administrados por outro módulo.
QUEUE* queue_init(void (*destroy)(void*));
// Chama destroy para cada objeto, se o callback tiver sido informado.
void queue_destroy(QUEUE **q);

bool queue_push(QUEUE *q, void *object);
// Remove e devolve o objeto sem chamar destroy.
void* queue_pop(QUEUE *q);
void* queue_front(QUEUE *q);
void* queue_back(QUEUE *q);

int queue_size(QUEUE *q);
bool queue_empty(QUEUE *q);

// Para e devolve false se o visitor devolver false.
bool queue_visit(QUEUE *q, bool (*visitor)(void *object, void *context), void *context);

#endif
