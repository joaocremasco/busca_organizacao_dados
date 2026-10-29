// interface unica pras estruturas, o resto do programa nao precisa saber qual esta usando

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "../include/collection.h"
#include "../data_structures/include/stack.h"
#include "../data_structures/include/seq_list.h"
#include "../data_structures/include/queue.h"
#include "../data_structures/include/ord_list.h"
#include "../data_structures/include/linked_list.h"
#include "../data_structures/include/gen_list.h"
#include "../data_structures/include/dn_linked_list.h"
#include "../data_structures/include/crossed_list.h"

struct collection {
  COLLECTION_TYPE type;
  void *impl; // a estrutura de verdade, o tipo depende de type
  const char *order_field;
  bool descending;
  const char *group_field;
  const char *subgroup_field;
  const char *cross_field;
  const char *cross_second_field;
  // valores distintos de cross_field e cross_second_field, a posicao no vetor e o indice na lista cruzada
  char **cross_rows;
  size_t cross_row_count;
  char **cross_columns;
  size_t cross_column_count;
};

typedef struct {
    const char *name;
    COLLECTION_TYPE type;
} TYPE_ENTRY;

static const TYPE_ENTRY type_table[] = {
    {"stack", COLLECTION_STACK},
    {"queue", COLLECTION_QUEUE},
    {"array_list", COLLECTION_ARRAY_LIST},
    {"linked_list", COLLECTION_LINKED_LIST},
    {"sentinel_list", COLLECTION_SENTINEL_LIST},
    {"ordered_list", COLLECTION_SORTED_LIST},
    {"generalized_list", COLLECTION_GROUPED_LIST},
    {"crossed_list", COLLECTION_CROSS_LIST},
    // os nomes que o enunciado pede
    {"pilha", COLLECTION_STACK},
    {"fila", COLLECTION_QUEUE},
    {"lista_seq", COLLECTION_ARRAY_LIST},
    {"lista_enc", COLLECTION_LINKED_LIST},
    {"lista_cabeca", COLLECTION_SENTINEL_LIST},
    {"lista_ord", COLLECTION_SORTED_LIST},
    {"lista_gen", COLLECTION_GROUPED_LIST},
    {"lista_cruz", COLLECTION_CROSS_LIST}
};

#define TYPE_COUNT (sizeof(type_table) / sizeof(type_table[0]))

// compara pelo endereco, serve pra achar um item exato na hora de remover
static int compare_item_pointers(void *left, void *right) {
  uintptr_t a = (uintptr_t)left;
  uintptr_t b = (uintptr_t)right;
  return (a > b) - (a < b);
}

/* numero se os dois campos forem numericos, senao texto
   sem ORDER cai na comparacao de ponteiro */
static int compare_item_fields(void *left, void *right, void *context) {
  const COLLECTION *collection = context;
  if (collection->order_field == NULL)
    return compare_item_pointers(left, right);
  const char *left_value = item_get(left, collection->order_field);
  const char *right_value = item_get(right, collection->order_field);
  double left_number, right_number;
  int result;

  if (left_value == NULL)
    left_value = "";
  if (right_value == NULL)
    right_value = "";

  if (item_get_number(left, collection->order_field, &left_number) &&
      item_get_number(right, collection->order_field, &right_number)) {
    result = left_number < right_number ? -1 : left_number > right_number;
  } else {
    result = strcmp(left_value, right_value);
  }

  return collection->descending ? -result : result;
}

static char *copy_text(const char *text) {
  size_t length = strlen(text) + 1;
  char *copy = malloc(length);
  if (copy != NULL)
    memcpy(copy, text, length);
  return copy;
}

// lista generalizada em 3 niveis, grupo -> subgrupo -> itens
typedef struct {
  char *key;
  GEN_LIST *subgroups;
} GroupEntry;

typedef struct {
  char *key;
  GEN_LIST *items;
} SubgroupEntry;

// os itens nao sao liberados aqui
static void destroy_subgroup(void *object) {
  SubgroupEntry *subgroup = object;
  if (subgroup == NULL)
    return;
  gen_list_destroy(&subgroup->items);
  free(subgroup->key);
  free(subgroup);
}

static void destroy_group(void *object) {
  GroupEntry *group = object;
  if (group == NULL)
    return;
  gen_list_destroy(&group->subgroups);
  free(group->key);
  free(group);
}

typedef struct {
  const char *key;
  void *found;
  bool find_group;
} FindEntryContext;

// find_group diz se o objeto e um GroupEntry ou um SubgroupEntry
static bool find_group_entry(void *object, void *context) {
  FindEntryContext *find = context;
  if (find->find_group) {
    GroupEntry *group = object;
    if (strcmp(group->key, find->key) == 0) {
      find->found = group;
      return false;
    }
  } else {
    SubgroupEntry *subgroup = object;
    if (strcmp(subgroup->key, find->key) == 0) {
      find->found = subgroup;
      return false;
    }
  }
  return true;
}

// busca linear, achou devolve false pro visit parar
static void *find_group_value(GEN_LIST *list, const char *key, bool groups) {
  FindEntryContext context = {key, NULL, groups};
  (void)gen_list_visit(list, find_group_entry, &context);
  return context.found;
}

static int compare_item_pointers_callback(void *left, void *right) {
  return compare_item_pointers(left, right);
}

static int find_text_index(char **values, size_t count, const char *value) {
  for (size_t i = 0; i < count; ++i)
    if (strcmp(values[i], value) == 0)
      return (int)i;
  return -1;
}

// devolve o indice da chave, cria se ainda nao existir
static int add_text_index(char ***values, size_t *count, const char *value) {
  int index = find_text_index(*values, *count, value);
  if (index >= 0)
    return index;

  char *copy = copy_text(value);
  char **grown;
  if (copy == NULL)
    return -1;

  grown = realloc(*values, (*count + 1) * sizeof(**values));
  if (grown == NULL) {
    free(copy);
    return -1;
  }
  *values = grown;
  (*values)[*count] = copy;
  return (int)(*count)++;
}

static bool count_subgroup_items(void *object, void *context) {
  SubgroupEntry *subgroup = object;
  size_t *count = context;
  *count += (size_t)gen_list_size(subgroup->items);
  return true;
}

// sempre true pra nao parar nunca
static bool count_group_items(void *object, void *context) {
  GroupEntry *group = object;
  (void)gen_list_visit(group->subgroups, count_subgroup_items, context);
  return true;
}

COLLECTION *collection_create(COLLECTION_TYPE type,
                              const COLLECTION_OPTIONS *options) {
  COLLECTION *collection = malloc(sizeof(*collection));

  if (collection == NULL)
    return NULL;

  collection->type = type;
  collection->impl = NULL;
  collection->order_field = options == NULL ? NULL : options->order_field;
  collection->descending = options != NULL && options->descending;
  collection->group_field = options == NULL ? NULL : options->group_field;
  collection->subgroup_field = options == NULL ? NULL : options->subgroup_field;
  collection->cross_field = options == NULL ? NULL : options->cross_field;
  collection->cross_second_field = options == NULL ? NULL : options->cross_second_field;
  collection->cross_rows = NULL;
  collection->cross_row_count = 0;
  collection->cross_columns = NULL;
  collection->cross_column_count = 0;

  switch (type) {
    case COLLECTION_STACK:
      collection->impl = stack_init(NULL);
      break;

    case COLLECTION_QUEUE:
      collection->impl = queue_init(NULL);
      break;

    case COLLECTION_ARRAY_LIST:
      collection->impl = seq_list_init(NULL);
      break;

    case COLLECTION_LINKED_LIST:
      collection->impl = linked_list_init(NULL);
      break;

    case COLLECTION_SORTED_LIST:
      // a colecao vai de contexto pra o comparador ler order_field e descending
      collection->impl = ord_list_init_with_context(NULL, compare_item_fields,
                                                     collection);
      break;

    case COLLECTION_SENTINEL_LIST:
      collection->impl = dn_linked_list_init(NULL);
      break;

    case COLLECTION_CROSS_LIST:
      collection->impl = crossed_list_init(NULL);
      break;

    case COLLECTION_GROUPED_LIST:
      // destroy_group so libera as listas internas, nunca os itens
      collection->impl = gen_list_init(destroy_group);
      break;

    default:
      free(collection);
      return NULL;
  }

  if (collection->impl == NULL) {
    free(collection);
    return NULL;
  }

  return collection;
}

void collection_destroy(COLLECTION **collection) {
  if (collection == NULL || *collection == NULL)
    return;

  switch ((*collection)->type) {
    case COLLECTION_STACK: {
      STACK *impl = (*collection)->impl;
      stack_destroy(&impl);
      break;
    }

    case COLLECTION_QUEUE: {
      QUEUE *impl = (*collection)->impl;
      queue_destroy(&impl);
      break;
    }

    case COLLECTION_ARRAY_LIST: {
      SEQ_LIST *impl = (*collection)->impl;
      seq_list_destroy(&impl);
      break;
    }

    case COLLECTION_LINKED_LIST: {
      LINKED_LIST *impl = (*collection)->impl;
      linked_list_destroy(&impl);
      break;
    }

    case COLLECTION_SORTED_LIST: {
      ORD_LIST *impl = (*collection)->impl;
      ord_list_destroy(&impl);
      break;
    }

    case COLLECTION_SENTINEL_LIST: {
      DN_LINKED_LIST *impl = (*collection)->impl;
      dn_linked_list_destroy(&impl);
      break;
    }

    case COLLECTION_CROSS_LIST: {
      CROSSED_LIST *impl = (*collection)->impl;
      crossed_list_destroy(&impl);
      break;
    }

    case COLLECTION_GROUPED_LIST: {
      GEN_LIST *impl = (*collection)->impl;
      gen_list_destroy(&impl);
      break;
    }

    default:
      break;
  }

  for (size_t i = 0; i < (*collection)->cross_row_count; ++i)
    free((*collection)->cross_rows[i]);
  for (size_t i = 0; i < (*collection)->cross_column_count; ++i)
    free((*collection)->cross_columns[i]);
  free((*collection)->cross_rows);
  free((*collection)->cross_columns);

  free(*collection);
  *collection = NULL;
}

bool collection_insert(COLLECTION *collection, ITEM *item) {
  if (collection == NULL || item == NULL)
    return false;

  switch (collection->type) {
    case COLLECTION_STACK:
      return stack_push(collection->impl, item);

    case COLLECTION_QUEUE:
      return queue_push(collection->impl, item);

    case COLLECTION_ARRAY_LIST:
      return seq_list_push_back(collection->impl, item);

    case COLLECTION_LINKED_LIST:
      return linked_list_push_back(collection->impl, item);

    case COLLECTION_SORTED_LIST:
      return ord_list_insert(collection->impl, item);

    case COLLECTION_SENTINEL_LIST:
      return dn_linked_list_push_back(collection->impl, item);

    case COLLECTION_CROSS_LIST:
      {
        CROSSED_LIST *crossed = collection->impl;
        const char *row_value;
        const char *column_value;
        int row, column;

        if (collection->cross_field == NULL ||
            collection->cross_second_field == NULL)
          return false;

        // campo ausente vira "" pra todo item ter posicao
        row_value = item_get(item, collection->cross_field);
        column_value = item_get(item, collection->cross_second_field);
        // cresce a matriz sempre que aparece valor novo
        row = add_text_index(&collection->cross_rows,
                             &collection->cross_row_count,
                             row_value == NULL ? "" : row_value);
        column = add_text_index(&collection->cross_columns,
                                &collection->cross_column_count,
                                column_value == NULL ? "" : column_value);
        if (row < 0 || column < 0 ||
            !crossed_list_resize(crossed, (int)collection->cross_row_count, true) ||
            !crossed_list_resize(crossed, (int)collection->cross_column_count, false))
          return false;
        return crossed_list_insert(crossed, row, column, item);
      }

    case COLLECTION_GROUPED_LIST:
      {
        GEN_LIST *groups = collection->impl;
        const char *group_value;
        const char *subgroup_value;
        GroupEntry *group;
        SubgroupEntry *subgroup;

        if (collection->group_field == NULL ||
            collection->subgroup_field == NULL)
          return false;

        group_value = item_get(item, collection->group_field);
        subgroup_value = item_get(item, collection->subgroup_field);
        if (group_value == NULL)
          group_value = "";
        if (subgroup_value == NULL)
          subgroup_value = "";

        group = find_group_value(groups, group_value, true);
        if (group == NULL) {
          group = malloc(sizeof(*group));
          if (group == NULL)
            return false;
          group->key = copy_text(group_value);
          group->subgroups = gen_list_init(destroy_subgroup);
          if (group->key == NULL || group->subgroups == NULL ||
              !gen_list_push_back(groups, group, true)) {
            destroy_group(group);
            return false;
          }
        }

        subgroup = find_group_value(group->subgroups, subgroup_value, false);
        if (subgroup == NULL) {
          subgroup = malloc(sizeof(*subgroup));
          if (subgroup == NULL)
            return false;
          subgroup->key = copy_text(subgroup_value);
          subgroup->items = gen_list_init(NULL);
          if (subgroup->key == NULL || subgroup->items == NULL ||
              !gen_list_push_back(group->subgroups, subgroup, true)) {
            destroy_subgroup(subgroup);
            return false;
          }
        }
        return gen_list_push_back(subgroup->items, item, true);
      }

    default:
      return false;
  }
}

bool collection_remove(COLLECTION *collection, ITEM *item) {
  if (collection == NULL || item == NULL)
    return false;

  switch (collection->type) {
    // pilha e fila so deixam tirar o item do topo e do inicio
    case COLLECTION_STACK:
      if (stack_top(collection->impl) != item) return false;
      return stack_pop(collection->impl) == item;

    case COLLECTION_QUEUE:
      if (queue_front(collection->impl) != item) return false;
      return queue_pop(collection->impl) == item;

    case COLLECTION_ARRAY_LIST:
      {
        int index = seq_list_find(collection->impl, item, compare_item_pointers);
        return index >= 0 && seq_list_erase(collection->impl, index) == item;
      }

    case COLLECTION_LINKED_LIST:
      return linked_list_erase(collection->impl, item, compare_item_pointers) == item;

    case COLLECTION_SORTED_LIST:
      return ord_list_erase(collection->impl, item) == item;

    case COLLECTION_SENTINEL_LIST:
      return dn_linked_list_erase(collection->impl, item, compare_item_pointers) == item;

    case COLLECTION_CROSS_LIST:
      {
        CROSSED_LIST *crossed = collection->impl;
        int row, column;
        if (crossed_list_find(crossed, item, compare_item_pointers, &row, &column) != item)
          return false;
        return crossed_list_remove(crossed, row, column) == item;
      }

    case COLLECTION_GROUPED_LIST:
      {
        GroupEntry *group;
        SubgroupEntry *subgroup;
        bool atom;
        const char *group_value;
        const char *subgroup_value;

        if (collection->group_field == NULL ||
            collection->subgroup_field == NULL)
          return false;

        group_value = item_get(item, collection->group_field);
        subgroup_value = item_get(item, collection->subgroup_field);
        group = find_group_value(collection->impl,
                                 group_value == NULL ? "" : group_value, true);
        if (group == NULL)
          return false;
        subgroup = find_group_value(group->subgroups,
                                    subgroup_value == NULL ? "" : subgroup_value,
                                    false);
        if (subgroup == NULL ||
            gen_list_erase(subgroup->items, item, true,
                           compare_item_pointers_callback, &atom) == NULL)
          return false;

        // nao deixa grupo ou subgrupo vazio pra tras
        if (gen_list_empty(subgroup->items)) {
          (void)gen_list_erase(group->subgroups, subgroup, true,
                               compare_item_pointers_callback, &atom);
          destroy_subgroup(subgroup);
        }
        if (gen_list_empty(group->subgroups)) {
          (void)gen_list_erase(collection->impl, group, true,
                               compare_item_pointers_callback, &atom);
          destroy_group(group);
        }
        return true;
      }

    default:
      return false;
  }
}

size_t collection_size(const COLLECTION *collection) {
  if (collection == NULL)
    return 0;

  switch (collection->type) {
    case COLLECTION_STACK:
      return stack_size(collection->impl);

    case COLLECTION_QUEUE:
      return queue_size(collection->impl);

    case COLLECTION_ARRAY_LIST:
      return seq_list_size(collection->impl);

    case COLLECTION_LINKED_LIST:
      return linked_list_size(collection->impl);

    case COLLECTION_SORTED_LIST:
      return ord_list_size(collection->impl);

    case COLLECTION_SENTINEL_LIST:
      return dn_linked_list_size(collection->impl);

    case COLLECTION_CROSS_LIST:
      return crossed_list_size(collection->impl);

    case COLLECTION_GROUPED_LIST:
      {
        size_t count = 0;
        (void)gen_list_visit(collection->impl, count_group_items, &count);
        return count;
      }

    default:
      return 0;
  }
}

typedef struct {
  COLLECTION_VISITOR visitor;
  void *context;
} COLLECTION_VISIT_CONTEXT;

// os tres visit abaixo descem grupo, subgrupo e item chamando o visitor da colecao
static bool visit_group_item(void *item, void *context) {
  COLLECTION_VISIT_CONTEXT *visit = context;
  return visit->visitor(item, visit->context);
}

static bool visit_group_subgroups(void *object, void *context) {
  SubgroupEntry *subgroup = object;
  return gen_list_visit(subgroup->items, visit_group_item, context);
}

static bool visit_groups(void *object, void *context) {
  GroupEntry *group = object;
  return gen_list_visit(group->subgroups, visit_group_subgroups, context);
}

static bool collection_cross_visit_adapter(int i, int j, void *item, void *context);

bool collection_visit(const COLLECTION *collection,
                      COLLECTION_VISITOR visitor,
                      void *context){
  if (collection == NULL || visitor == NULL)
    return false;

  switch (collection->type) {
    // o cast funciona porque ITEM * e void * tem a mesma representacao
    case COLLECTION_STACK:
      return stack_visit(collection->impl, (bool (*)(void *, void *))visitor, context);
    case COLLECTION_QUEUE:
      return queue_visit(collection->impl, (bool (*)(void *, void *))visitor, context);
    case COLLECTION_ARRAY_LIST:
      return seq_list_visit(collection->impl, (bool (*)(void *, void *))visitor, context);
    case COLLECTION_LINKED_LIST:
      return linked_list_visit(collection->impl, (bool (*)(void *, void *))visitor, context);
    case COLLECTION_SENTINEL_LIST:
      return dn_linked_list_visit(collection->impl, (bool (*)(void *, void *))visitor, context);
    case COLLECTION_SORTED_LIST:
      return ord_list_visit(collection->impl, (bool (*)(void *, void *))visitor, context);
    case COLLECTION_CROSS_LIST: {
      COLLECTION_VISIT_CONTEXT visit_context = {visitor, context};
      return crossed_list_visit(collection->impl, collection_cross_visit_adapter,
                                &visit_context);
    }
    case COLLECTION_GROUPED_LIST: {
      COLLECTION_VISIT_CONTEXT visit_context = {visitor, context};
      return gen_list_visit(collection->impl, visit_groups, &visit_context);
    }

    default:
      return false;
  }
}

static bool collection_cross_visit_adapter(int i, int j, void *item, void *context) {
  (void)i;
  (void)j;
  COLLECTION_VISITOR visitor = ((COLLECTION_VISIT_CONTEXT *)context)->visitor;
  return visitor(item, ((COLLECTION_VISIT_CONTEXT *)context)->context);
}

bool collection_visit_cross_index(const COLLECTION *collection,
                                  bool second_index,
                                  COLLECTION_VISITOR visitor,
                                  void *context) {
  if (collection == NULL || visitor == NULL || collection->type != COLLECTION_CROSS_LIST)
    return false;

  COLLECTION_VISIT_CONTEXT visit_context = {visitor, context};
  CROSSED_LIST *crossed = collection->impl;
  // primeiro indice percorre por linha, segundo por coluna
  bool is_row = !second_index;
  int line_count = crossed_list_capacity(crossed, is_row);
  for (int index = 0; index < line_count; index++) {
    if (!crossed_list_visit_line(crossed, index, is_row,
                                 collection_cross_visit_adapter, &visit_context))
      return false;
  }
  return true;
}

bool collection_type_parse(const char *name, COLLECTION_TYPE *type)
{
  if (name == NULL || type == NULL)
    return false;

  for (size_t i = 0; i < TYPE_COUNT; i++) {
    if (strcmp(type_table[i].name, name) == 0) {
      *type = type_table[i].type;
      return true;
    }
  }
  return false;
}

const char *collection_type_name(COLLECTION_TYPE type)
{
  for (size_t i = 0; i < TYPE_COUNT; i++) {
    if (type_table[i].type == type)
      return type_table[i].name;
  }
  return "desconhecido";
}
