#ifndef OBJECT_H
#define OBJECT_H

typedef enum {NULL_TYPE, TYPE_ONE, TYPE_TWO} object_type;

typedef struct {
	void *content;
	object_type content_type;
} OBJECT;

#endif

