
#include <stdio.h>
#include <stdlib.h>
#include "queue.h"

typedef struct queue_node {
    void *data;
    struct queue_node *next;
} QUEUE_NODE;

struct queue {
    QUEUE_NODE *head;
    QUEUE_NODE *tail;
    size_t size;
};

QUEUE *queue_create(void) {
    QUEUE *queue = malloc(sizeof(*queue));

    if (queue == NULL)
        return NULL;

    queue->head = NULL;
    queue->tail = NULL;
    queue->size = 0;

    return queue;
}

bool queue_enqueue(QUEUE *queue, void *data) {
    QUEUE_NODE *new_node;

    if (queue == NULL || data == NULL)
        return false;

    new_node = malloc(sizeof(*new_node));

    if (new_node == NULL)
        return false;

    new_node->data = data;
    new_node->next = NULL;

    if (queue_is_empty(queue)) {
        queue->head = new_node;
        queue->tail = new_node;
    } else {
        queue->tail->next = new_node;
        queue->tail = new_node;
    }

    queue->size++;

    return true;
}

void *queue_dequeue(QUEUE *queue) {
    QUEUE_NODE *removed_node;
    void *data;

    if (queue == NULL || queue_is_empty(queue))
        return NULL;

    removed_node = queue->head;
    data = removed_node->data;

    queue->head = removed_node->next;

    free(removed_node);
    queue->size--;

    if (queue->size == 0)
        queue->tail = NULL;

    return data;
}

void queue_destroy(QUEUE **queue) {
    if (queue == NULL || *queue == NULL)
        return;

    while (!queue_is_empty(*queue)) {
        void *data = queue_dequeue(*queue);
        item_destroy(&data);
    }

    free(*queue);
    *queue = NULL;
}

void *queue_front(const QUEUE *queue) {
    if (queue == NULL || queue_is_empty(queue))
        return NULL;

    return queue->head->data;
}

size_t queue_size(const QUEUE *queue) {
    if (queue == NULL)
        return 0;

    return queue->size;
}

bool queue_is_empty(const QUEUE *queue) {
    return queue == NULL || queue->size == 0;
}

bool queue_visit(
    const QUEUE *queue,
    bool (*visitor)(void *data, void *context),
    void *context
) {
    QUEUE_NODE *atual;

    if (queue == NULL || visitor == NULL)
        return false;

    atual = queue->head;

    while (atual != NULL) {
        if (!visitor(atual->data, context))
            return false;

        atual = atual->next;
    }

    return true;
}
