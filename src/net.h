#pragma once
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>

#define DEBUG

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
net_request_t *net_request_init() {
    net_request_t *net_req_ptr = (net_request_t *)calloc(1, sizeof(net_request_t));

    net_req_ptr->hostname = NULL;
    net_req_ptr->port = 0;
    net_req_ptr->send_buf = NULL;
    net_req_ptr->send_buf_size = 0;
    net_req_ptr->recv_buf = NULL;
    net_req_ptr->recv_buf_size = 0;
    net_req_ptr->on_data = NULL;

    return net_req_ptr;
}

// Shouldn`t free struct fields.
void net_request_free(net_request_t *net_req) {
    free(net_req);
}

// Should establish a TCP connection, send contents of send_buf and
// save the server`s response to recv_buf, calling on_data (if not null)
// each time the buffer gets full.
// If recv_buf is null or recv_buf_size is non-positive, should return
// an error. If recv_buf is full and on_data is null, should return an error.
//
// Return value:
//   zero if no error ocurred, -1 otherwise.
int net_send_receive(net_request_t *request) {
    int sd = socket(AF_INET, SOCK_STREAM, 0);
    if (sd == -1) {
        fprintf(stderr, "Socket creation failed");
        return -1;
    }

    char port_num[] = {0, 0, 0, 0, 0, 0};
    itoa(request->port, port_num, 10);
    struct addrinfo hints;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;        // It's not necessary: IPv4 or IPv6.
    hints.ai_socktype = SOCK_STREAM;    // TCP stream-sockets.
    hints.ai_flags = AI_PASSIVE;        // Fill IP-address.
    struct addrinfo *res = NULL;

    int status = 0;
    if ((status = getaddrinfo(request->hostname, port_num, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo error: %sn", gai_strerror(status));
        return -1;
    }



    /*in_addr_t ip4_addr = inet_addr(request->);
    in_port_t port = htons(request->port);
    while (res != NULL) {
        
    }

    struct sockaddr_in full_addr;
    full_addr.sin_family = AF_INET;
    full_addr.sin_port = port;
    full_addr.sin_addr.s_addr = ip4_addr;*/

    if (connect(sd, (const struct sockaddr*) &full_addr, sizeof(full_addr)) != 0) {
        fprintf(stderr, "Connection failed");
        shutdown(sd, SHUT_RDWR);
        close(sd);
        return -1;
    }

    // Send the request.
    if (write(sd, request->send_buf, request->send_buf_size) < request->send_buf_size) {
        fprintf(stderr, "The request wasn't completely transmitted");
        close(sd);
        return -1;
    }

    long long was_read = 0;
    size_t remained_size = request->recv_buf_size;
    while ((was_read = read(sd, request->recv_buf + request->recv_buf_size - remained_size, remained_size)) > 0) {
        if (was_read == -1) {
            fprintf(stderr, "Read failed");
            shutdown(sd, SHUT_RDWR);
            close(sd);
            return -1;
        }

        remained_size -= was_read;
        if (remained_size == 0) {
            if (request->on_data == NULL) {
                fprintf(stderr, "recv_buf is full, but there is no function to free it");
                shutdown(sd, SHUT_RDWR);
                close(sd);
                return -1;
            } else {
                request->on_data(request, request->recv_buf_size);
            }
        }
        #ifdef DEBUG
            printf("%s\n", request->recv_buf);
            fflush(stdout);
        #endif // DEBUG
    }
    close(sd);

    return 0;
}
