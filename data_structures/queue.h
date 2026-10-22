#ifndef QUEUE_H
#define QUEUE_H

#include <stdbool.h>
#include <stddef.h>

typedef struct queue QUEUE;

QUEUE *queue_create(void);
bool   queue_enqueue(QUEUE *queue, void *data);
void  *queue_dequeue(QUEUE *queue);
void   queue_destroy(QUEUE **queue);
void  *queue_front(const QUEUE *queue);
size_t queue_size(const QUEUE *queue);
bool   queue_is_empty(const QUEUE *queue);
bool   queue_visit(const QUEUE *queue,
                   bool (*visitor)(void *data, void *context),
                   void *context);

#endif
