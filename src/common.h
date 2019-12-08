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

// return value:
//   tail after insertion
list_t *list_append(list_t *tail, void *value);

// arguments:
//   ls - list
//   value - value to search
//   equal_fn - returns non-zero iff equal
//   found_ptr - pointer, at which 1 is assigned upon success, 0 otherwise
// return value:
//   pointer to first equal element if exists, tail otherwise
list_t *list_find_equal(
    list_t *ls,
    const void *value,
    int (*equal_fn)(const void *, const void *),
    int *found_ptr
);

void str_lowercase(char *str);
