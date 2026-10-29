/* Coordena a leitura dos argumentos, a busca e a exportação dos resultados. */
#include "collection.h"
#include "export.h"
#include "item.h"
#include "query.h"
#include "reader.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct {
    const QUERY *query;
    COLLECTION *result_collection;
    size_t matched_count;
    double result_insert_ms;
    bool failed;
} SearchContext;

/* Devolve o horário atual em milissegundos, ou zero se a leitura falhar. */
static double current_time_ms(void) {
    struct timespec timestamp;

    if (timespec_get(&timestamp, TIME_UTC) != TIME_UTC) {
        return 0;
    }
    return (double)timestamp.tv_sec * 1000.0 +
           (double)timestamp.tv_nsec / 1000000.0;
}

/* Avalia um ITEM e o insere na coleção de resultados quando aceito. */
static bool process_search_item(ITEM *item, void *context) {
    SearchContext *search = context;
    double similarity;
    double insert_start_ms;
    char similarity_text[64];

    if (!query_matches(search->query, item, &similarity)) {
        return true;
    }
    ++search->matched_count;

    (void)snprintf(similarity_text, sizeof(similarity_text), "%.17g", similarity);
    if (!item_set(item, "similarity", similarity_text)) {
        search->failed = true;
        return false;
    }

    insert_start_ms = current_time_ms();
    if (!collection_insert(search->result_collection, item)) {
        search->failed = true;
        return false;
    }
    search->result_insert_ms += current_time_ms() - insert_start_ms;
    return true;
}

/* Converte apenas dígitos decimais e rejeita valores que excedam size_t. */
static bool parse_size_argument(const char *text, size_t *value_out) {
    size_t parsed_value = 0;
    size_t digit_index;

    if (text == NULL || *text == '\0') {
        return false;
    }
    for (digit_index = 0; text[digit_index] != '\0'; ++digit_index) {
        size_t digit;

        if (text[digit_index] < '0' || text[digit_index] > '9') {
            return false;
        }
        digit = (size_t)(text[digit_index] - '0');
        if (parsed_value > ((size_t)-1 - digit) / 10) {
            return false;
        }
        parsed_value = parsed_value * 10 + digit;
    }
    *value_out = parsed_value;
    return true;
}

static void print_usage(const char *program_name) {
    fprintf(stderr,
            "Usage: %s --reader name --input file --base structure "
            "--result structure --query file --output file "
            "--top-k N --limit N\n",
            program_name);
}

/* Executa carga, busca, exportação e libera os recursos usados. */
static int run_program(const char *reader_name, const char *input_path,
                       const char *query_path, const char *output_path,
                       COLLECTION_TYPE base_type, COLLECTION_TYPE result_type,
                       size_t top_k, size_t input_limit) {
    size_t input_record_count = 0;
    size_t output_record_count = 0;
    ITEM_STORE *item_store = NULL;
    READER *reader = NULL;
    QUERY *query = NULL;
    COLLECTION *base_collection = NULL;
    COLLECTION *result_collection = NULL;
    char error_message[192] = {0};
    double load_ms = 0;
    double search_ms = 0;
    double result_insert_ms = 0;
    double write_ms = 0;
    int exit_status = 1;
    SearchContext search = {0};
    double start_ms;
    bool failed = false;

    query = query_load(query_path, error_message, sizeof(error_message));
    if (query == NULL) {
        fprintf(stderr, "Erro na consulta: %s\n", error_message);
        failed = true;
    }

    if (!failed &&
        (((base_type == COLLECTION_GROUPED_LIST ||
           result_type == COLLECTION_GROUPED_LIST) && !query_has_group(query)) ||
         ((base_type == COLLECTION_CROSS_LIST ||
           result_type == COLLECTION_CROSS_LIST) && !query_has_cross(query)) ||
         ((base_type == COLLECTION_SORTED_LIST ||
           result_type == COLLECTION_SORTED_LIST) && !query_has_order(query)))) {
        fprintf(stderr,
                "The selected structures require ORDER, GROUP, or CROSS "
                "fields in the query.\n");
        failed = true;
    }

    if (!failed) {
        reader = reader_open(input_path, reader_name);
        if (reader == NULL) {
            fprintf(stderr, "Erro ao abrir dataset: %s\n", input_path);
            failed = true;
        }
    }

    if (!failed) {
        item_store = item_store_create();
        if (item_store == NULL) {
            fprintf(stderr, "Sem memória para armazenar registros.\n");
            failed = true;
        }
    }

    if (!failed) {
        COLLECTION_OPTIONS options = query_collection_options(query);
        base_collection = collection_create(base_type, &options);
        result_collection = collection_create(result_type, &options);
        if (base_collection == NULL || result_collection == NULL) {
            fprintf(stderr, "Não foi possível criar as estruturas.\n");
            failed = true;
        }
    }

    if (!failed) {
        start_ms = current_time_ms();
        while (input_limit == 0 || input_record_count < input_limit) {
            ITEM *item = NULL;
            READER_STATUS status = reader_next(reader, item_store, &item);

            if (status == READER_EOF)
                break;
            if (status == READER_ERROR) {
                fprintf(stderr, "Erro de leitura: %s\n", reader_error(reader));
                failed = true;
                break;
            }
            if (!collection_insert(base_collection, item)) {
                fprintf(stderr, "Falha ao inserir registro na base.\n");
                failed = true;
                break;
            }
            ++input_record_count;
        }
        load_ms = current_time_ms() - start_ms;
    }

    if (!failed) {
        search.query = query;
        search.result_collection = result_collection;
        start_ms = current_time_ms();
        if (!collection_visit(base_collection, process_search_item, &search) ||
            search.failed) {
            fprintf(stderr, "Falha durante busca ou inserção de resultados.\n");
            failed = true;
        } else {
            search_ms = current_time_ms() - start_ms - search.result_insert_ms;
            if (search_ms < 0)
                search_ms = 0;
            result_insert_ms = search.result_insert_ms;
        }
    }

    if (!failed) {
        start_ms = current_time_ms();
        if (!export_write(output_path, reader, result_collection, top_k,
                          &output_record_count)) {
            fprintf(stderr, "Falha ao escrever arquivo de saída.\n");
            failed = true;
        } else {
            write_ms = current_time_ms() - start_ms;
        }
    }

    if (!failed) {
        printf("STATUS=OK\nINPUT_RECORDS=%zu\nMATCHED_RECORDS=%zu\n"
               "OUTPUT_RECORDS=%zu\nLOAD_MS=%.3f\nSEARCH_MS=%.3f\n"
               "RESULT_INSERT_MS=%.3f\nWRITE_MS=%.3f\n",
               input_record_count, search.matched_count, output_record_count,
               load_ms, search_ms, result_insert_ms, write_ms);
        exit_status = 0;
    }

    collection_destroy(&result_collection);
    collection_destroy(&base_collection);
    reader_close(&reader);
    query_destroy(&query);
    item_store_destroy(&item_store);
    return exit_status;
}

/* Valida os argumentos da linha de comando e inicia o processamento. */
int main(int argc, char **argv) {
    const char *reader_name = NULL;
    const char *input_path = NULL;
    const char *base_type_name = NULL;
    const char *result_type_name = NULL;
    const char *query_path = NULL;
    const char *output_path = NULL;
    size_t top_k = 0;
    size_t input_limit = 0;
    bool has_top_k = false;
    bool has_input_limit = false;
    COLLECTION_TYPE base_type;
    COLLECTION_TYPE result_type;
    int argument_index;

    for (argument_index = 1; argument_index < argc; ++argument_index) {
        const char *argument = argv[argument_index];

        if (argument_index + 1 >= argc) {
            print_usage(argv[0]);
            return 2;
        }
        if (strcmp(argument, "--reader") == 0) {
            reader_name = argv[++argument_index];
        } else if (strcmp(argument, "--input") == 0) {
            input_path = argv[++argument_index];
        } else if (strcmp(argument, "--base") == 0) {
            base_type_name = argv[++argument_index];
        } else if (strcmp(argument, "--result") == 0) {
            result_type_name = argv[++argument_index];
        } else if (strcmp(argument, "--query") == 0) {
            query_path = argv[++argument_index];
        } else if (strcmp(argument, "--output") == 0) {
            output_path = argv[++argument_index];
        } else if (strcmp(argument, "--top-k") == 0) {
            has_top_k = parse_size_argument(argv[++argument_index], &top_k);
            if (!has_top_k) {
                print_usage(argv[0]);
                return 2;
            }
        } else if (strcmp(argument, "--limit") == 0) {
            has_input_limit = parse_size_argument(argv[++argument_index], &input_limit);
            if (!has_input_limit) {
                print_usage(argv[0]);
                return 2;
            }
        } else {
            print_usage(argv[0]);
            return 2;
        }
    }

    if (reader_name == NULL || input_path == NULL || base_type_name == NULL ||
        result_type_name == NULL || query_path == NULL || output_path == NULL ||
        !has_top_k || !has_input_limit ||
        !collection_type_parse(base_type_name, &base_type) ||
        !collection_type_parse(result_type_name, &result_type)) {
        print_usage(argv[0]);
        return 2;
    }

    return run_program(reader_name, input_path, query_path, output_path,
                       base_type, result_type, top_k, input_limit);
}
