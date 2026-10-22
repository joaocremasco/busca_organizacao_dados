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
    QUEUE *queue = malloc(sizeof(QUEUE));

    if (queue != NULL) {
        queue->head = NULL;
        queue->tail = NULL;
        queue->size = 0;
    }

    return queue;
}

bool queue_enqueue(QUEUE *queue, ITEM *item) {
    NODE *new_node;

    if (queue == NULL || item == NULL)
        return false;

    new_node = malloc(sizeof(NODE));
    if (new_node == NULL)
        return false;

    new_node->item = item;
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

ITEM *queue_dequeue(QUEUE *queue) {
    NODE *removed_node;
    ITEM *item;

    if (queue == NULL || queue_is_empty(queue))
        return NULL;

    removed_node = queue->head;
    item = removed_node->item;
    queue->head = removed_node->next;

    free(removed_node);
    queue->size--;

    if (queue->size == 0)
        queue->tail = NULL;

    return item;
}

void queue_destroy(QUEUE **queue) {
    ITEM *item;

    if (queue == NULL || *queue == NULL)
        return;

    while (!queue_is_empty(*queue)) {
        item = queue_dequeue(*queue);
        item_destroy(&item);
    }

    free(*queue);
    *queue = NULL;
}

ITEM *queue_front(QUEUE *queue) {
    if (queue == NULL || queue_is_empty(queue))
        return NULL;

    return queue->head->item;
}

int queue_size(QUEUE *queue) {
    return queue != NULL ? (int)queue->size : 0;
}

bool queue_is_empty(QUEUE *queue) {
    return queue == NULL || queue->size == 0;
}

bool queue_is_full(QUEUE *queue) {
    /*
     * A dynamic queue has no fixed capacity.
     * Allocation failure is detected when attempting
     * to allocate a new node in queue_enqueue.
     */
    return queue == NULL;
}

void queue_print(QUEUE *queue) {
    NODE *current;

    if (queue == NULL) {
        printf("Queue does not exist.\n");
        return;
    }

    printf("head -> ");

    current = queue->head;

    while (current != NULL) {
        printf("[%d] ", item_get_key(current->item));
        current = current->next;
    }

    printf("<- tail\n");
}
