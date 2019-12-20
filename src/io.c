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

    for (int i = 0; i < new_capacity / 2; ++i) {
        new_buf[i] = vector->buf[i];
    }
    free(vector->buf);

    vector->buf = new_buf;
    vector->capacity = new_capacity;
}

void reuse_recv_buf(const struct net_request *request, size_t recieved_bytes, void *context) {
    size_t request_size = request->recv_buf_size;
    vector_t *vector = (vector_t *)context;

    while (vector->capacity < vector->buf_size + request_size) {
        realloc_buf(vector);
    }

    #ifdef DEBUG
        printf("Capacity: %lld, Request size: %lld\n", vector->capacity, request_size);
    #endif // DEBUG

    for (size_t i = 0; i < request_size; ++i) {
        vector->buf[vector->buf_size] = request->recv_buf[i];
        ++vector->buf_size;
    }

    #ifdef DEBUG
        printf("Full response: %s", vector->buf);
    #endif // DEBUG

    memset(request->recv_buf, 0, request->recv_buf_size);
}
