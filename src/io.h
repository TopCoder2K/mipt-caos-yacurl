#pragma once
#include <stddef.h>



typedef struct vector {
    char *buf;
    size_t buf_size;
    size_t capacity;
} vector_t;

vector_t *vector_init();
void vector_free(vector_t *ptr);
// Returns 0 on success, -1 on error.
void realloc_buf(vector_t *vector);

// Saves the whole server response.
// Copyies the new recieved information from net_request->recv_buf
// to context_buf if the first one had been filled.
// Also reallocates context->buf if it's necessary.
void reuse_recv_buf(const struct net_request *request, size_t recieved_bytes, void *context);
