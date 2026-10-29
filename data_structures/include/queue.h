#ifndef QUEUE_H
#define QUEUE_H

#include <stdbool.h>

typedef struct queue_ QUEUE;

// o liberador pode ser nulo quando a estrutura nao for dona dos objetos
QUEUE* queue_init(void (*destroy)(void*));
// o liberador recebe cada objeto ao destruir a estrutura
void queue_destroy(QUEUE **q);

bool queue_push(QUEUE *q, void *object);
// remove da estrutura sem liberar o objeto
void* queue_pop(QUEUE *q);
void* queue_front(QUEUE *q);
void* queue_back(QUEUE *q);

int queue_size(QUEUE *q);
bool queue_empty(QUEUE *q);

// interrompe o percurso quando a funcao devolve falso
bool queue_visit(QUEUE *q, bool (*visitor)(void *object, void *context), void *context);

#endif
