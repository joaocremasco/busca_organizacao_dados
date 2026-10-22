#ifndef SEQUENTIAL_LIST_H
#define SEQUENTIAL_LIST_H

#include <stdbool.h>
#include <stddef.h>

typedef struct sequential_list SEQ_LIST;

SEQ_LIST *list_create(void);
void list_delete(SEQ_LIST **list);
bool list_insert(SEQ_LIST *list, void *data);
size_t list_size(const SEQ_LIST *list);
bool list_is_empty(const SEQ_LIST *list);

#endif
