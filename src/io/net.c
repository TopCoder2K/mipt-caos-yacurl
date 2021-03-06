#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "net.h"

#define DEBUG



net_request_t *net_request_init() {
    net_request_t *net_req_ptr = (net_request_t *)calloc(1, sizeof(net_request_t));

    net_req_ptr->hostname = NULL;
    net_req_ptr->port = 0;
    net_req_ptr->send_buf = NULL;
    net_req_ptr->send_buf_size = 0;
    net_req_ptr->recv_buf = NULL;
    net_req_ptr->recv_buf_size = 0;
    net_req_ptr->user_context = NULL;
    net_req_ptr->on_data = NULL;

    return net_req_ptr;
}

void net_request_free(net_request_t *net_req) {
    free(net_req);
}

int net_send_receive(net_request_t *request) {
    if (request->on_data == NULL) {
        fprintf(stderr, "[net_send_receive] There is no function to save the server response\n");
        return -1;
    }

    // Bad moment?????????????????????????????????????????????????????????
    char port_num[10];
    sprintf(port_num, "%d", request->port);

    struct addrinfo hints;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;        // Most web-servers support IPv4
    hints.ai_socktype = SOCK_STREAM;    // TCP stream-sockets.
    struct addrinfo *addrinfo_list = NULL;

    int status = 0;
    if ((status = getaddrinfo(request->hostname, port_num, &hints, &addrinfo_list)) != 0) {
        fprintf(stderr, "[net_send_receive] getaddrinfo error: %s\n", gai_strerror(status));
        return -1;
    }

    // Create socket.
    int sock_fd = socket(addrinfo_list->ai_family, addrinfo_list->ai_socktype, addrinfo_list->ai_protocol);
    if (sock_fd == -1) {
        fprintf(stderr, "[net_send_receive] Socket creation failed\n");
        return -1;
    }

    if (connect(sock_fd, addrinfo_list->ai_addr, addrinfo_list->ai_addrlen) != 0) {
        fprintf(stderr, "[net_send_receive] Connection failed\n");
        close(sock_fd);
        return -1;
    }



    // Send the request.
    #ifdef DEBUG
        fprintf(stderr, "[net_send_receive] Start sending the request.\n");
    #endif // DEBUG

    long long bytes_sent = 0;
    size_t remaining_bytes = request->send_buf_size;
    while ((bytes_sent = send(sock_fd, request->send_buf + request->send_buf_size - remaining_bytes, remaining_bytes, 0)) > 0) {
        remaining_bytes -= bytes_sent;
    }

    if (remaining_bytes > 0) {
        if (bytes_sent < 0) {
            fprintf(stderr, "[net_send_receive] Error occured during the transmition (e. g. server closed the connection)\n");
        } else {
            fprintf(stderr, "[net_send_receive] The transmition was strangely interrupted\n");
        }
        close(sock_fd);
        return -1;
    }


    // Read the response.
    #ifdef DEBUG
        fprintf(stderr, "[net_send_receive] Start reading the response.\n");
    #endif // DEBUG

    long long was_read = 0;
    remaining_bytes = request->recv_buf_size;
    while ((was_read = recv(sock_fd, request->recv_buf + request->recv_buf_size - remaining_bytes, remaining_bytes, 0)) > 0) {
        remaining_bytes -= was_read;

        if (remaining_bytes == 0) {
            request->on_data(request->recv_buf, request->recv_buf_size, request->user_context);
            remaining_bytes = request->recv_buf_size;
            memset(request->recv_buf, 0, request->recv_buf_size);
        }
    }

    if (was_read == -1) {
        fprintf(stderr, "[net_send_receive] recv failed\n");
        shutdown(sock_fd, SHUT_RDWR);
        close(sock_fd);
        return -1;
    }

    #ifdef DEBUG
        fprintf(stderr, "[net_send_receive] Start saving the response.\n");
    #endif // DEBUG

    request->on_data(request->recv_buf, request->recv_buf_size - remaining_bytes, request->user_context);

    #ifdef DEBUG
        fprintf(stderr, "[net_send_receive] Finished.\n");
    #endif // DEBUG

    close(sock_fd);
    freeaddrinfo(addrinfo_list);

    return 0;
}
