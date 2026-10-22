#ifndef QUEUE_H
#define QUEUE_H

#include <stdbool.h>
#include "item.h"

typedef struct queue QUEUE;
typedef struct queue_node NODE;

QUEUE *queue_create(void);
bool queue_enqueue(QUEUE *queue, ITEM *item);
ITEM *queue_dequeue(QUEUE *queue);
void queue_destroy(QUEUE **queue);
ITEM *queue_front(QUEUE *queue);
int queue_size(QUEUE *queue);
bool queue_is_empty(QUEUE *queue);
bool queue_is_full(QUEUE *queue);
void queue_print(QUEUE *queue);

#endif
