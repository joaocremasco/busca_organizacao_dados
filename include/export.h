#ifndef TRABALHO_EXPORT_H
#define TRABALHO_EXPORT_H

#include "collection.h"
#include "reader.h"

#include <stdbool.h>
#include <stddef.h>

/* Escreve até top_k registros e atualiza written se fornecido; retorna false em argumentos
   inválidos ou falha de arquivo/escrita. Não assume a propriedade dos ITEMs. */
bool export_write(const char *path, const READER *reader,
                  const COLLECTION *results, size_t top_k, size_t *written);

#endif
