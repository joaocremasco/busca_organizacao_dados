#include <stdlib.h>
#include <stdbool.h>
#include "node.h"

struct node_ {
	NODE *next;
	NODE *prev;
	OBJECT object;
};
