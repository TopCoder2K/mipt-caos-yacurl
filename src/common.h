#pragma once
#include <ctype.h>

// List head is always value-empty and never null

typedef struct list {
    void *value;
    struct list *next;
} list_t;

list_t *list_empty();

// If free_fn is NULL, will free() all elements of ls, including head.
// Otherwise, will additionally call free_fn on .value field of all
// elements of ls.
void list_free(list_t *ls,  void (*free_fn)(const void *));

void str_lowercase(char *str);
