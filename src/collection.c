#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

typedef enum {
    COLLECTION_STACK, COLLECTION_QUEUE, COLLECTION_ARRAY_LIST, COLLECTION_LINKED_LIST,
    COLLECTION_SENTINEL_LIST, COLLECTION_SORTED_LIST, COLLECTION_GROUPED_LIST,
    COLLECTION_CROSS_LIST
} COLLECTION_TYPE;

struct collection{

};

bool (*COLLECTION_VISITOR)(ITEM *item, void *context){

}

COLLECTION *collection_create(COLLECTION_TYPE type, const COLLECTION_OPTIONS *options){
  COLLECTION *collection = malloc(sizeof(*collection));
  if(collection == NULL)
    return NULL;

  collection->
  collection->
  collection->
}

void collection_destroy(COLLECTION **collection){

}
bool collection_insert(COLLECTION *collection, ITEM *item);
bool collection_remove(COLLECTION *collection, ITEM *item);
size_t collection_size(const COLLECTION *collection);
bool collection_visit(const COLLECTION *collection, COLLECTION_VISITOR visitor, void *context);
bool collection_visit_cross_index(const COLLECTION *collection, bool second_index,
                                  COLLECTION_VISITOR visitor, void *context);
bool collection_type_parse(const char *name, COLLECTION_TYPE *type);
const char *collection_type_name(COLLECTION_TYPE type);

#endif
