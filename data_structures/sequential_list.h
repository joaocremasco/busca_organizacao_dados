#ifndef SEQUENTIAL_LIST_H
#define SEQUENTIAL_LIST_H

#include <stdbool.h>
#include <stddef.h>

typedef struct sequential_list SEQ_LIST;

SEQ_LIST *seq_list_create(void);
void seq_list_delete(SEQ_LIST **list);
bool seq_list_insert(SEQ_LIST *list, void *data);
size_t seq_list_size(const SEQ_LIST *list);
bool seq_list_is_empty(const SEQ_LIST *list);
bool   seq_list_visit(const SEQ_LIST *list,
                   bool (*visitor)(void *data, void *context),
                   void *context);

#endif
