#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../net.h"


#define DEBUG


const unsigned int BUF_SIZE = 4096;

    /*char *hostname;
    int port;
    char *send_buf;
    size_t send_buf_size;
    char *recv_buf;
    size_t recv_buf_size;
    void (*on_data)(const net_request_t *, size_t);*/

int main(int argc, char **argv) {
    char hostname[] = "localhost";
    int port = atoi(argv[1]);
    char *to_send = calloc(BUF_SIZE, sizeof(char));
    size_t send_buf_size = BUF_SIZE;
    char *to_recv = calloc(BUF_SIZE, sizeof(char));
    size_t recv_buf_size = BUF_SIZE;

    send_buf_size = read(STDIN_FILENO, to_send, BUF_SIZE);

    net_request_t *net_req_ptr = net_request_init();
    net_req_ptr->hostname = hostname;
    net_req_ptr->port = port;
    net_req_ptr->send_buf = to_send;
    net_req_ptr->send_buf_size = send_buf_size;
    net_req_ptr->recv_buf = to_recv;
    net_req_ptr->recv_buf_size = recv_buf_size;
    net_req_ptr->on_data = NULL;

    printf("Request was created.\n");

    int status = net_send_receive(net_req_ptr);
    if (status == -1) {
        return 1;
    }
    printf("\nGOT:\n%s\n", net_req_ptr->recv_buf);

    net_request_free(net_req_ptr);
    free(to_send);
    free(to_recv);

    return 0;
}