#ifndef SEQ_LIST_H
#define SEQ_LIST_H

#include <stdbool.h>

typedef struct seq_list_ SEQ_LIST;

/* Cria e inicializa a estrutura; retorna NULL se a alocação falhar. A função opcional destroy é
   chamada para os objetos na destruição. */
SEQ_LIST* seq_list_init(void (*destroy)(void*));
/* Libera a estrutura e seus nós ou vetores; chama destroy para os objetos se configurado. No
   projeto, ITEM_STORE é dono dos ITEMs. */
void seq_list_destroy(SEQ_LIST **sq);

/* Insere o ponteiro do objeto no fim; retorna false para argumento inválido ou falha de
   alocação. No projeto, ITEM_STORE continua dono do ITEM. */
bool seq_list_push_back(SEQ_LIST* sq, void *object);
/* Remove e devolve o objeto do fim; retorna NULL se a estrutura estiver vazia ou inválida. Não
   libera o objeto armazenado. */
void* seq_list_pop_back(SEQ_LIST *sq);

/* Insere o objeto no índice indicado; retorna false para índice ou argumentos inválidos ou falha
   de alocação. Armazena o ponteiro sem cópia. */
bool seq_list_insert(SEQ_LIST* sq, void *object, int idx);
/* Redimensiona a capacidade; retorna false para capacidade inválida ou falha de alocação. Ao
   encolher, remove as posições excedentes. */
bool seq_list_resize(SEQ_LIST *sq, int capacity);
/* Remove e devolve o objeto; retorna NULL se não encontrar ou se os argumentos forem inválidos.
   Não libera a memória do objeto. */
void* seq_list_erase(SEQ_LIST *sq, int idx);

/* Devolve sem remover o objeto na posição indicada; retorna NULL se a posição ou estrutura forem
   inválidas. Não transfere sua propriedade. */
void* seq_list_get(SEQ_LIST *sq, int idx);
/* Substitui o objeto encontrado e devolve o anterior; retorna NULL se o alvo não existir ou os
   argumentos forem inválidos. Não libera nenhum objeto. */
void* seq_list_set(SEQ_LIST *sq, void *object, int idx);
/* Procura o objeto usando cmp e devolve seu índice; retorna -1 se não encontrar ou se os
   argumentos forem inválidos. */
int seq_list_find(SEQ_LIST *sq, void *object, int (*cmp)(void*, void*));

/* Devolve a quantidade de elementos; retorna 0 se a estrutura for NULL. */
int seq_list_size(SEQ_LIST *sq);
/* Retorna true se a estrutura estiver vazia ou for NULL; false se contiver elementos. */
bool seq_list_empty(SEQ_LIST *sq);

/* Percorre os elementos; retorna true se visitar tudo e false para argumentos inválidos ou
   parada da função de visita. */
bool seq_list_visit(SEQ_LIST *sq, bool (*visitor)(void *object, void *context), void *context);

#endif
