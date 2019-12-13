#pragma once
#include <stddef.h>

typedef struct net_request {
    char *hostname;
    int port;
    char *send_buf;
    size_t send_buf_size;
    char *recv_buf;
    size_t recv_buf_size;
    void (*on_data)(const net_request_t *, size_t);
} net_request_t;

// Should set defaults as:
//   recv_buf <- NULL, recv_buf_size <- 0, on_data <- NULL
net_request_t *net_request_init();

// Shouldn`t free struct fields
void net_request_free();

// Should establish a TCP connection, send contents of send_buf and
// save the server`s response to recv_buf, calling on_data (if not null)
// each time the buffer gets full.
// If recv_buf is null or recv_buf_size is non-positive, should return
// an error. If recv_buf is full and on_data is null, should return an error.
//
// Return value:
//   zero if no error ocurred
int net_send_receive(net_request_t *request);
