#include <stdlib.h>
#include "queue.h"

typedef struct node_ NODE;

struct node_ {
	NODE *next;
	NODE *prev;
	void *object;
};

struct queue_ {
	int size;
	NODE *front;
	NODE *back;
};

QUEUE* queue_init() {
	
	QUEUE *q = malloc(sizeof(QUEUE));
	
	if(q == NULL)
		return NULL;

	q->size = 0;
	q->front = NULL;
	q->back = NULL;
	
	return q;
};

void queue_clear(QUEUE **q) {
	if(q == NULL || *q == NULL)
		return;
	
	int queue_size = (*q)->size;

	for(int i = 0; i < queue_size; i++) {
		queue_pop(*q);
	}

	free(*q);
	*q = NULL;
}

bool queue_push(QUEUE *q, void *object) {
	if(q == NULL || object == NULL) {
		return false;
	}

	NODE *node = malloc(sizeof(NODE));

	if(node == NULL)
		return false;
	
	node->object = object;
	node->prev = q->back;
	node->next = NULL;

	if(q->size == 0) {
		q->front = node;
	}
	else { 
		q->back->next = node;
	}

	q->back = node;
	
	q->size++;

	return true;
}

void* queue_pop(QUEUE *q) {
	if(q == NULL)
		return NULL;

	void *object = q->front->object;
	
	NODE *tmp = q->front;
	
	if(q->front->next != NULL)
		q->front->next->prev = NULL;
	
	q->front = q->front->next;

	free(tmp);

	q->size--;

	return object;
}

void* queue_front(QUEUE *q) {
	return q == NULL ? NULL : q->front;
}

void* queue_back(QUEUE *q) {
	return q == NULL ? NULL : q->back;
}

int queue_size(QUEUE *q) {
	return q == NULL ? -1 : q->size;
}

bool queue_empty(QUEUE *q) {
	return q == NULL ? true : q->size == 0;
}
