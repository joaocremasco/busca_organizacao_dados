
#include "query.h"

#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *field;
    char *operator;
    char *expected_value;
} FilterRule;

typedef struct {
    char *field;
    double target;
    double tolerance;
    double weight;
} SimilarityRule;

struct query {
    FilterRule *filters;
    size_t filter_count;
    SimilarityRule *similarities;
    size_t similarity_count;
    double acceptance_threshold;
    bool has_acceptance_threshold;
    char *order_field;
    bool descending;
    char *group_field;
    char *subgroup_field;
    char *cross_field;
    char *cross_second_field;
};

static char *duplicate_text(const char *text) {
    size_t length = strlen(text) + 1;
    char *copy = malloc(length);

    if (copy != NULL)
        memcpy(copy, text, length);

    return copy;
}

static char *trim_whitespace(char *text) {
    char *end;

    while (isspace((unsigned char)*text))
        ++text;

    end = text + strlen(text);

    while (end > text && isspace((unsigned char)end[-1]))
        --end;

    *end = '\0';
    return text;
}

static bool parse_double(const char *text, double *value) {
    char *end;
    double parsed = strtod(text, &end);

    if (text == end || *end != '\0' || !isfinite(parsed))
        return false;

    *value = parsed;
    return true;
}

static bool equals_ignore_case(const char *left, const char *right) {
    while (*left != '\0' && *right != '\0') {
        if (tolower((unsigned char)*left) != tolower((unsigned char)*right))
            return false;
        ++left;
        ++right;
    }
    return *left == '\0' && *right == '\0';
}

static bool replace_string(char **destination, const char *source) {
    char *copy = duplicate_text(source);

    if (copy == NULL)
        return false;

    free(*destination);
    *destination = copy;
    return true;
}

/* Returns 1 for a line, 0 for EOF, -1 for a file error,
   and -2 for an allocation error. */
static int read_line(FILE *file, char **line, size_t *capacity) {
    size_t length = 0;
    int character;

    if (*line == NULL) {
        *capacity = 128;
        *line = malloc(*capacity);

        if (*line == NULL)
            return -2;
    }

    while ((character = fgetc(file)) != EOF && character != '\n') {
        if (length + 1 >= *capacity) {
            size_t new_capacity = *capacity * 2;
            char *new_line = realloc(*line, new_capacity);

            if (new_line == NULL)
                return -2;

            *line = new_line;
            *capacity = new_capacity;
        }

        (*line)[length++] = (char)character;
    }

    if (ferror(file))
        return -1;

    if (character == EOF && length == 0)
        return 0;

    /* Remove CR from Windows-style line endings. */
    if (length > 0 && (*line)[length - 1] == '\r')
        --length;

    (*line)[length] = '\0';
    return 1;
}

static bool append_filter_rule(QUERY *query, char **parts) {
    FilterRule *grown_filters;
    FilterRule *new_filter;

    grown_filters = realloc(
        query->filters,
        (query->filter_count + 1) * sizeof(*grown_filters)
    );

    if (grown_filters == NULL)
        return false;

    query->filters = grown_filters;
    new_filter = &query->filters[query->filter_count];

    new_filter->field = duplicate_text(parts[1]);
    new_filter->operator = duplicate_text(parts[2]);
    new_filter->expected_value = duplicate_text(parts[3]);

    if (new_filter->field == NULL ||
        new_filter->operator == NULL ||
        new_filter->expected_value == NULL) {
        free(new_filter->field);
        free(new_filter->operator);
        free(new_filter->expected_value);
        return false;
    }

    ++query->filter_count;
    return true;
}

static bool append_similarity_rule(QUERY *query, char **parts) {
    SimilarityRule *grown_rules;
    SimilarityRule *new_rule;
    double target;
    double tolerance;
    double weight;

    if (!parse_double(parts[2], &target) ||
        !parse_double(parts[3], &tolerance) ||
        !parse_double(parts[4], &weight) ||
        tolerance <= 0 || weight < 0) {
        return false;
    }

    grown_rules = realloc(
        query->similarities,
        (query->similarity_count + 1) * sizeof(*grown_rules)
    );

    if (grown_rules == NULL)
        return false;

    query->similarities = grown_rules;
    new_rule = &query->similarities[query->similarity_count];

    new_rule->field = duplicate_text(parts[1]);

    if (new_rule->field == NULL)
        return false;

    new_rule->target = target;
    new_rule->tolerance = tolerance;
    new_rule->weight = weight;

    ++query->similarity_count;
    return true;
}

static bool is_supported_operator(const char *operator) {
    return strcmp(operator, "==") == 0 ||
           strcmp(operator, "!=") == 0 ||
           strcmp(operator, "<") == 0 ||
           strcmp(operator, "<=") == 0 ||
           strcmp(operator, ">") == 0 ||
           strcmp(operator, ">=") == 0;
}

static void set_error(char *error, size_t error_size,
                      const char *message) {
    if (error != NULL && error_size > 0)
        (void)snprintf(error, error_size, "%s", message);
}

typedef enum {
    QUERY_LINE_OK,
    QUERY_LINE_INVALID,
    QUERY_LINE_OUT_OF_MEMORY
} QueryLineResult;

static QueryLineResult parse_query_line(QUERY *query, char *line) {
    char *parts[6] = {0};
    size_t part_count = 0;
    char *remaining = trim_whitespace(line);

    if (*remaining == '\0' || *remaining == '#')
        return QUERY_LINE_OK;

    while (part_count < sizeof(parts) / sizeof(parts[0])) {
        parts[part_count++] = remaining;
        remaining = strchr(remaining, '|');

        if (remaining == NULL)
            break;

        *remaining++ = '\0';
    }

    if (remaining != NULL)
        return QUERY_LINE_INVALID;

    for (size_t i = 0; i < part_count; ++i)
        parts[i] = trim_whitespace(parts[i]);

    if (strcmp(parts[0], "FILTER") == 0 &&
        part_count == 4 &&
        is_supported_operator(parts[2])) {
        return append_filter_rule(query, parts)
                   ? QUERY_LINE_OK : QUERY_LINE_OUT_OF_MEMORY;
    }

    if (strcmp(parts[0], "SIM") == 0 && part_count == 5) {
        if (append_similarity_rule(query, parts))
            return QUERY_LINE_OK;

        /* Invalid values and allocation failures are distinguished
           by the validation performed before allocating the rule. */
        double target, tolerance, weight;

        if (!parse_double(parts[2], &target) ||
            !parse_double(parts[3], &tolerance) ||
            !parse_double(parts[4], &weight) ||
            tolerance <= 0 || weight < 0) {
            return QUERY_LINE_INVALID;
        }

        return QUERY_LINE_OUT_OF_MEMORY;
    }

    if (strcmp(parts[0], "ACCEPT") == 0 && part_count == 2) {
        if (!parse_double(parts[1], &query->acceptance_threshold) ||
            query->acceptance_threshold < 0 ||
            query->acceptance_threshold > 1) {
            return QUERY_LINE_INVALID;
        }

        query->has_acceptance_threshold = true;
        return QUERY_LINE_OK;
    }

    if (strcmp(parts[0], "ORDER") == 0 &&
        part_count == 3 &&
        (strcmp(parts[2], "ASC") == 0 ||
         strcmp(parts[2], "DESC") == 0)) {
        if (!replace_string(&query->order_field, parts[1]))
            return QUERY_LINE_OUT_OF_MEMORY;

        query->descending = strcmp(parts[2], "DESC") == 0;
        return QUERY_LINE_OK;
    }

    if (strcmp(parts[0], "GROUP") == 0 && part_count == 3) {
        if (!replace_string(&query->group_field, parts[1]))
            return QUERY_LINE_OUT_OF_MEMORY;

        if (!replace_string(&query->subgroup_field, parts[2]))
            return QUERY_LINE_OUT_OF_MEMORY;

        return QUERY_LINE_OK;
    }

    if (strcmp(parts[0], "CROSS") == 0 && part_count == 3) {
        if (!replace_string(&query->cross_field, parts[1]))
            return QUERY_LINE_OUT_OF_MEMORY;

        if (!replace_string(&query->cross_second_field, parts[2]))
            return QUERY_LINE_OUT_OF_MEMORY;

        return QUERY_LINE_OK;
    }

    return QUERY_LINE_INVALID;
}

QUERY *query_load(const char *path, char *error, size_t error_size) {
    FILE *query_file;
    QUERY *query;
    char *line = NULL;
    size_t line_capacity = 0;
    size_t line_number = 0;
    int read_status;
    QueryLineResult line_result = QUERY_LINE_OK;

    if (path == NULL) {
        set_error(error, error_size, "caminho da consulta ausente");
        return NULL;
    }

    query_file = fopen(path, "r");

    if (query_file == NULL) {
        set_error(error, error_size, "não foi possível abrir consulta");
        return NULL;
    }

    query = calloc(1, sizeof(*query));

    if (query == NULL) {
        fclose(query_file);
        set_error(error, error_size, "sem memória ao criar consulta");
        return NULL;
    }

    while (1) {
        read_status = read_line(query_file, &line, &line_capacity);

        if (read_status == 0)
            break;

        if (read_status < 0) {
            set_error(error, error_size,
                      read_status == -2
                          ? "sem memória ao ler consulta"
                          : "erro ao ler consulta");

            free(line);
            fclose(query_file);
            query_destroy(&query);
            return NULL;
        }

        ++line_number;
        line_result = parse_query_line(query, line);

        if (line_result != QUERY_LINE_OK)
            break;
    }

    if (line_result != QUERY_LINE_OK) {
        if (line_result == QUERY_LINE_INVALID) {
            if (error != NULL && error_size > 0) {
                (void)snprintf(error, error_size,
                               "consulta inválida na linha %zu",
                               line_number);
            }
        } else {
            set_error(error, error_size, "sem memória ao ler consulta");
        }

        free(line);
        fclose(query_file);
        query_destroy(&query);
        return NULL;
    }

    free(line);
    fclose(query_file);

    if (query->similarity_count > 0) {
        double total_weight = 0;

        for (size_t i = 0; i < query->similarity_count; ++i)
            total_weight += query->similarities[i].weight;

        if (total_weight == 0) {
            set_error(error, error_size,
                      "os pesos SIM não podem ser todos zero");
            query_destroy(&query);
            return NULL;
        }
    }

    if (query->similarity_count > 0 &&
        !query->has_acceptance_threshold) {
        query->acceptance_threshold = 0;
    }

    return query;
}

void query_destroy(QUERY **query_ptr) {
    if (query_ptr == NULL || *query_ptr == NULL)
        return;

    for (size_t i = 0; i < (*query_ptr)->filter_count; ++i) {
        free((*query_ptr)->filters[i].field);
        free((*query_ptr)->filters[i].operator);
        free((*query_ptr)->filters[i].expected_value);
    }

    for (size_t i = 0; i < (*query_ptr)->similarity_count; ++i)
        free((*query_ptr)->similarities[i].field);

    free((*query_ptr)->filters);
    free((*query_ptr)->similarities);
    free((*query_ptr)->order_field);
    free((*query_ptr)->group_field);
    free((*query_ptr)->subgroup_field);
    free((*query_ptr)->cross_field);
    free((*query_ptr)->cross_second_field);
    free(*query_ptr);

    *query_ptr = NULL;
}

static bool filter_matches(const FilterRule *filter, const ITEM *item) {
    const char *actual_value = item_get(item, filter->field);
    double actual_number;
    double expected_number;
    int comparison;
    int actual_boolean = -1;
    int expected_boolean = -1;

    if (actual_value == NULL)
        return false;

    if (equals_ignore_case(actual_value, "true"))
        actual_boolean = 1;
    else if (equals_ignore_case(actual_value, "false"))
        actual_boolean = 0;

    if (equals_ignore_case(filter->expected_value, "true"))
        expected_boolean = 1;
    else if (equals_ignore_case(filter->expected_value, "false"))
        expected_boolean = 0;

    if (actual_boolean >= 0 && expected_boolean >= 0) {
        comparison = actual_boolean - expected_boolean;
    } else if (parse_double(actual_value, &actual_number) &&
        parse_double(filter->expected_value, &expected_number)) {
        comparison = actual_number < expected_number ? -1 :
                     actual_number > expected_number ? 1 : 0;
    } else {
        comparison = strcmp(actual_value, filter->expected_value);
    }

    if (strcmp(filter->operator, "==") == 0)
        return comparison == 0;

    if (strcmp(filter->operator, "!=") == 0)
        return comparison != 0;

    if (strcmp(filter->operator, "<") == 0)
        return comparison < 0;

    if (strcmp(filter->operator, "<=") == 0)
        return comparison <= 0;

    if (strcmp(filter->operator, ">") == 0)
        return comparison > 0;

    return comparison >= 0;
}

bool query_matches(const QUERY *query, ITEM *item, double *similarity) {
    double weighted_score = 0;
    double total_weight = 0;
    double overall_similarity;

    if (query == NULL || item == NULL)
        return false;

    for (size_t i = 0; i < query->filter_count; ++i) {
        if (!filter_matches(&query->filters[i], item))
            return false;
    }

    for (size_t i = 0; i < query->similarity_count; ++i) {
        const SimilarityRule *rule = &query->similarities[i];
        double actual_number;
        double rule_score;

        if (!item_get_number(item, rule->field, &actual_number)) {
            rule_score = 0;
        } else {
            double distance = actual_number > rule->target
                                  ? actual_number - rule->target
                                  : rule->target - actual_number;

            rule_score = 1.0 - distance / rule->tolerance;

            if (rule_score < 0)
                rule_score = 0;
        }

        weighted_score += rule_score * rule->weight;
        total_weight += rule->weight;
    }

    overall_similarity = total_weight == 0
                             ? 1.0
                             : weighted_score / total_weight;

    if (similarity != NULL)
        *similarity = overall_similarity;

    return query->similarity_count == 0 ||
           overall_similarity >= query->acceptance_threshold;
}

COLLECTION_OPTIONS query_collection_options(const QUERY *query) {
    COLLECTION_OPTIONS options = {0};

    if (query == NULL)
        return options;

    options.order_field = query->order_field;
    options.descending = query->descending;
    options.group_field = query->group_field;
    options.subgroup_field = query->subgroup_field;
    options.cross_field = query->cross_field;
    options.cross_second_field = query->cross_second_field;

    return options;
}

bool query_has_group(const QUERY *query) {
    return query != NULL &&
           query->group_field != NULL &&
           query->subgroup_field != NULL;
}

bool query_has_cross(const QUERY *query) {
    return query != NULL &&
           query->cross_field != NULL &&
           query->cross_second_field != NULL;
}

bool query_has_order(const QUERY *query) {
  return query != NULL && query->order_field != NULL;
}
