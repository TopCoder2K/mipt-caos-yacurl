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
