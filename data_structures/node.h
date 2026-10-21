#ifndef NODE_H
#define NODE_H

#include "object.h"

typedef struct node_ NODE;

NODE* node_init();
bool node_erase(NODE *node);

NODE* node_get_next(NODE *node);
NODE* node_get_prev(NODE *node);
OBJECT node_get_object(NODE *node);

void node_set_next(NODE *node, NODE *next_node);
void node_set_prev(NODE *node, NODE *prev_node);
void node_set_object(NODE *node, OBJECT content);

#endif
