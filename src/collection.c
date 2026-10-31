// Adapta as oito estruturas para uma interface comum usada pelo restante do programa.

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
  void *impl; // Implementação concreta selecionada por type.
  const char *order_field;
  bool descending;
  const char *group_field;
  const char *subgroup_field;
  const char *cross_field;
  const char *cross_second_field;
  // Os valores distintos dos campos viram índices de linha e coluna.
  char **cross_rows;
  size_t cross_row_count;
  char **cross_columns;
  size_t cross_column_count;
  size_t cross_item_count;
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
    // Também aceita os nomes usados no enunciado.
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

// Compara os endereços para localizar a mesma referência, não apenas um item equivalente.
static int compare_item_pointers(void *left, void *right) {
  uintptr_t a = (uintptr_t)left;
  uintptr_t b = (uintptr_t)right;
  return (a > b) - (a < b);
}

/* Compara numericamente quando os dois campos são números e como texto nos
   demais casos; sem ORDER, usa os endereços para manter uma ordem definida. */
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

// Copia uma chave porque a coleção mantém seus índices além da chamada atual.
static char *copy_text(const char *text) {
  size_t length = strlen(text) + 1;
  char *copy = malloc(length);
  if (copy != NULL)
    memcpy(copy, text, length);
  return copy;
}

// A organização interna segue grupo -> subgrupo -> referências aos ITEMs.
typedef struct {
  char *key;
  GEN_LIST *subgroups;
} GroupEntry;

typedef struct {
  char *key;
  GEN_LIST *items;
} SubgroupEntry;

// Libera o bucket da célula; os ITEMs guardados nele continuam no ITEM_STORE.
static void destroy_cross_cell(void *object) {
  GEN_LIST *items = object;
  gen_list_destroy(&items);
}

// Remove apenas os nós e metadados do subgrupo; os ITEMs pertencem ao ITEM_STORE.
static void destroy_subgroup(void *object) {
  SubgroupEntry *subgroup = object;
  if (subgroup == NULL)
    return;
  gen_list_destroy(&subgroup->items);
  free(subgroup->key);
  free(subgroup);
}

// Libera a lista de subgrupos e a chave do grupo, sem tocar nos ITEMs.
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

// find_group escolhe qual tipo de entrada será comparado no visitor.
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

// Procura uma chave no nível indicado; o visitor para assim que encontra a entrada.
// Devolve NULL quando a chave não estiver nessa lista.
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

// Reutiliza o índice existente ou acrescenta uma cópia da chave ao vetor.
// Devolve -1 se não conseguir reservar memória.
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

// Soma ao acumulador a quantidade de ITEMs armazenados no subgrupo.
static bool count_subgroup_items(void *object, void *context) {
  SubgroupEntry *subgroup = object;
  size_t *count = context;
  *count += (size_t)gen_list_size(subgroup->items);
  return true;
}

// Continua o percurso após somar todos os subgrupos do grupo atual.
static bool count_group_items(void *object, void *context) {
  GroupEntry *group = object;
  (void)gen_list_visit(group->subgroups, count_subgroup_items, context);
  return true;
}

// Cria a estrutura pedida e guarda as opções de organização por referência.
// Devolve NULL para tipo inválido ou falha de alocação.
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
  collection->cross_item_count = 0;

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
      // O comparador recebe a coleção para consultar o campo e a direção de ORDER.
      collection->impl = ord_list_init_with_context(NULL, compare_item_fields,
                                                     collection);
      break;

    case COLLECTION_SENTINEL_LIST:
      collection->impl = dn_linked_list_init(NULL);
      break;

    case COLLECTION_CROSS_LIST:
      collection->impl = crossed_list_init(destroy_cross_cell);
      break;

    case COLLECTION_GROUPED_LIST:
      // A destruição recursiva libera grupos e subgrupos, não os ITEMs.
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

// Libera a estrutura concreta e os índices auxiliares; os ITEMs ficam no ITEM_STORE.
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

// Insere uma referência sem assumir a propriedade do ITEM.
// Devolve false se faltarem opções obrigatórias ou se uma alocação falhar.
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

        // Campos ausentes usam a mesma chave vazia e continuam indexáveis.
        row_value = item_get(item, collection->cross_field);
        column_value = item_get(item, collection->cross_second_field);
        // Cada chave nova amplia o eixo correspondente antes da inserção da célula.
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
        GEN_LIST *items = crossed_list_get(crossed, row, column);
        if (items != NULL) {
          if (!gen_list_push_back(items, item, true))
            return false;
        } else {
          items = gen_list_init(NULL);
          if (items == NULL)
            return false;
          if (!gen_list_push_back(items, item, true) ||
              !crossed_list_insert(crossed, row, column, items)) {
            gen_list_destroy(&items);
            return false;
          }
        }
        collection->cross_item_count++;
        return true;
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

// Remove a referência do item sem liberá-lo; pilha e fila só permitem retirar
// o elemento acessível no topo ou no início, respectivamente.
bool collection_remove(COLLECTION *collection, ITEM *item) {
  if (collection == NULL || item == NULL)
    return false;

  switch (collection->type) {
    // Pilha e fila só permitem retirar o item do topo ou do início.
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
      {
        ORD_LIST *ordered = collection->impl;
        for (int index = 0; index < ord_list_size(ordered); ++index)
          if (ord_list_get(ordered, index) == item)
            return ord_list_erase_at(ordered, index) == item;
        return false;
      }

    case COLLECTION_SENTINEL_LIST:
      return dn_linked_list_erase(collection->impl, item, compare_item_pointers) == item;

    case COLLECTION_CROSS_LIST:
      {
        CROSSED_LIST *crossed = collection->impl;
        const char *row_value = item_get(item, collection->cross_field);
        const char *column_value = item_get(item, collection->cross_second_field);
        int row = find_text_index(collection->cross_rows,
                                  collection->cross_row_count,
                                  row_value == NULL ? "" : row_value);
        int column = find_text_index(collection->cross_columns,
                                     collection->cross_column_count,
                                     column_value == NULL ? "" : column_value);
        bool atom;
        GEN_LIST *items;
        if (row < 0 || column < 0)
          return false;
        items = crossed_list_get(crossed, row, column);
        if (items == NULL || gen_list_erase(items, item, true,
                                            compare_item_pointers_callback,
                                            &atom) == NULL)
          return false;
        collection->cross_item_count--;
        if (gen_list_empty(items)) {
          (void)crossed_list_remove(crossed, row, column);
          destroy_cross_cell(items);
        }
        return true;
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

        // Remove níveis vazios para manter a hierarquia ocupada por itens.
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

// Quantos itens a coleção guarda. A lista cruzada mantém um contador próprio e a
// generalizada conta percorrendo grupos e subgrupos, já que os itens ficam em sublistas.
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
      return collection->cross_item_count;

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

// Estes visitors descem grupo e subgrupo até entregar cada ITEM ao visitor externo.
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

// Percorre a coleção na ordem natural de cada estrutura e entrega um ITEM por vez ao
// visitor, parando quando ele devolve false. Devolve false se o percurso foi
// interrompido ou se algum argumento for inválido; context segue sem alteração.
bool collection_visit(const COLLECTION *collection,
                      COLLECTION_VISITOR visitor,
                      void *context){
  if (collection == NULL || visitor == NULL)
    return false;

  switch (collection->type) {
    // Os TADs recebem void *, enquanto a interface pública entrega ITEM *.
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

// A célula armazena um bucket GEN_LIST; este adaptador visita seus ITEMs.
static bool collection_cross_visit_adapter(int i, int j, void *item, void *context) {
  (void)i;
  (void)j;
  return gen_list_visit(item, visit_group_item, context);
}

// Só vale para a lista cruzada: com second_index falso percorre todas as linhas e,
// com verdadeiro, todas as colunas, em ordem de índice. Para os outros tipos devolve
// false, e também quando o visitor interrompe o percurso.
bool collection_visit_cross_index(const COLLECTION *collection,
                                  bool second_index,
                                  COLLECTION_VISITOR visitor,
                                  void *context) {
  if (collection == NULL || visitor == NULL || collection->type != COLLECTION_CROSS_LIST)
    return false;

  COLLECTION_VISIT_CONTEXT visit_context = {visitor, context};
  CROSSED_LIST *crossed = collection->impl;
  // second_index seleciona o eixo: false percorre linhas, true percorre colunas.
  bool is_row = !second_index;
  int line_count = crossed_list_capacity(crossed, is_row);
  for (int index = 0; index < line_count; index++) {
    if (!crossed_list_visit_line(crossed, index, is_row,
                                 collection_cross_visit_adapter, &visit_context))
      return false;
  }
  return true;
}

// Converte o nome recebido em --base ou --result para o tipo da coleção, aceitando os
// nomes em inglês e os do enunciado. Devolve false se o nome não estiver na tabela.
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

// Nome do tipo para mensagens. Como a tabela lista os nomes em inglês primeiro, é um
// deles que sai; devolve "desconhecido" para um tipo fora da tabela.
const char *collection_type_name(COLLECTION_TYPE type)
{
  for (size_t i = 0; i < TYPE_COUNT; i++) {
    if (type_table[i].type == type)
      return type_table[i].name;
  }
  return "desconhecido";
}
