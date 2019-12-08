#include <stdio.h>
#include <stdlib.h>
#include "common.h"

list_t *list_empty() {
    list_t *head = malloc(sizeof(list_t));
    head->value = NULL;
    head->next= NULL;
    return head;
}

// If free_fn is NULL, will free() all elements of ls, including head.
// Otherwise, will additionally call free_fn on .value field of all
// elements of ls.
void list_free(list_t *ls, void (*free_fn)(const void *)) {
#ifdef DEBUG
    if (free_fn != NULL)
        fprintf(stderr, "[list_free] will free value\n");
#endif // DEBUG
    list_t *next;
    while (ls != NULL) {
#ifdef DEBUG
        fprintf(stderr, "[list_free] free %p\n", ls);
#endif // DEBUG
        next = ls->next;
        if (free_fn != NULL && ls->value != NULL)
            free_fn(ls->value);
        free(ls);
        ls = next;
    }
}

list_t *list_append(list_t *tail, void *value) {
    list_t *new_tail = list_empty();
    new_tail->value = value;
    tail->next = new_tail;
    return new_tail;
}

list_t *list_find_equal(
    list_t *ls,
    const void *value,
    int (*equal_fn)(const void *, const void *),
    int *found_ptr
) {
    list_t *node = ls->next, *prev_node = ls;
    while (node != NULL && !equal_fn(node->value, value)) {
        prev_node = node;
        node = node->next;
    }

    if (node == NULL) {
        *found_ptr = 0;
        return prev_node;
    }
    else {
        *found_ptr = 1;
        return node;
    }
}

void str_lowercase(char *str) {
    while (*str) {
        *str = tolower(*str);
        ++str;
    }
}
