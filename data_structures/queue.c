#include <stdlib.h>
#include "queue.h"

typedef struct node_ NODE;

struct node_ {
	NODE *next;
	void *object;
};

struct queue_ {
	int size;
	NODE *front;
	NODE *back;
	void (*destroy)(void*);
};

QUEUE* queue_init(void (*destroy)(void*)) {
	if(destroy == NULL)
		return NULL;

	QUEUE *q = malloc(sizeof(QUEUE));
	
	if(q == NULL)
		return NULL;

	q->size = 0;
	q->front = NULL;
	q->back = NULL;
	q->destroy = destroy;
	
	return q;
}

void queue_destroy(QUEUE **q) {
	if(q == NULL || *q == NULL)
		return;
	
	int q_size = (*q)->size;

	for(int i = 0; i < q_size; i++) {
		(*q)->destroy(queue_pop(*q));
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
	if(q == NULL || q->size == 0)
		return NULL;

	void *object = q->front->object;
	
	NODE *tmp = q->front;
	
	q->front = q->front->next;

	if(q->front == NULL)
		q->back = NULL;

	free(tmp);

	q->size--;

	return object;
}

void* queue_front(QUEUE *q) {
	if(q == NULL)
		return NULL;

	return q->size == 0 ? NULL : q->front->object;
}

void* queue_back(QUEUE *q) {
	if(q == NULL)
		return NULL;

	return q->size == 0 ? NULL : q->back->object;
}

int queue_size(QUEUE *q) {
	return q == NULL ? 0 : q->size;
}

bool queue_empty(QUEUE *q) {
	return q == NULL ? true : q->size == 0;
}
