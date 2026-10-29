#ifndef TRABALHO_EXPORT_H
#define TRABALHO_EXPORT_H

#include "collection.h"
#include "reader.h"

#include <stdbool.h>
#include <stddef.h>

// zero exporta tudo e a contagem pode ser nula
bool export_write(const char *path, const READER *reader,
                  const COLLECTION *results, size_t top_k, size_t *written);

#endif
