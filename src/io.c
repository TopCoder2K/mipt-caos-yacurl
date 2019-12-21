#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "io.h"
#include "net.h"

#define DEBUG



vector_t *vector_init() {
    vector_t *new_vector = calloc(1, sizeof(vector_t));

    new_vector->buf_size = 0;
    new_vector->capacity = 256;
    new_vector->buf = calloc(new_vector->capacity, sizeof(char));
    memset(new_vector->buf, 0, new_vector->capacity);

    return new_vector;
}

void vector_free(vector_t *ptr) {
    free(ptr->buf);
    free(ptr);
}

void realloc_buf(vector_t *vector) {
    size_t new_capacity = 2 * vector->capacity;
    char *new_buf = calloc(new_capacity, sizeof(char));

    memset(new_buf, 0, new_capacity);

    for (size_t i = 0; i < new_capacity / 2; ++i) {
        new_buf[i] = vector->buf[i];
    }
    free(vector->buf);

    vector->buf = new_buf;
    vector->capacity = new_capacity;
}

void on_data(const char *recv_buf, size_t recieved_bytes, void *context) {
    vector_t *vector = (vector_t *)context;

    #ifdef DEBUG
        fprintf(stderr, "[on_data] Before reallocation:\n[on_data] Capacity: %ld, Buffer size: %ld\n", vector->capacity, vector->buf_size);
    #endif // DEBUG

    while (vector->capacity < vector->buf_size + recieved_bytes) {
        realloc_buf(vector);
    }

    #ifdef DEBUG
        fprintf(stderr, "[on_data] After:\n[on_data] Capacity: %ld, Request size: %ld\n", vector->capacity, recieved_bytes);
    #endif // DEBUG

    for (size_t i = 0; i < recieved_bytes; ++i) {
        vector->buf[vector->buf_size] = recv_buf[i];
        ++vector->buf_size;
    }
}
