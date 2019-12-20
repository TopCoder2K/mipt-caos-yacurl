#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "../net.h"
#include "../io.h"


#define DEBUG


const unsigned int BUF_SIZE = 4096 * 4;

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

    // TEST 1.
    send_buf_size = read(STDIN_FILENO, to_send, BUF_SIZE);

    net_request_t *net_req_ptr = net_request_init();
    net_req_ptr->hostname = hostname;
    net_req_ptr->port = port;
    net_req_ptr->send_buf = to_send;
    net_req_ptr->send_buf_size = send_buf_size;
    net_req_ptr->recv_buf = to_recv;
    net_req_ptr->recv_buf_size = recv_buf_size;

    printf("Request1 was created.\n");

    int status = net_send_receive(net_req_ptr);
    if (status == -1) {
        return 1;
    }
    printf("\nGOT:\n%s\n", net_req_ptr->recv_buf);
    // -------------------------------------------------------------

    // TEST 2.
    net_req_ptr->on_data = on_data;
    vector_t *response = vector_init();
    net_req_ptr->user_context = response;

    memset(to_send, 0, BUF_SIZE);
    memset(to_recv, 0, BUF_SIZE);

    for (int i = 1; i <= 4096; ++i) {
        sprintf(to_send + i * 4, "%d", i);
    }
    net_req_ptr->send_buf_size = BUF_SIZE;
    net_req_ptr->recv_buf_size = BUF_SIZE;
    printf("Request2 was created.\n");

    status = net_send_receive(net_req_ptr);
    if (status == -1) {
        return 1;
    }
    printf("\nGOT:\n%s\n", net_req_ptr->recv_buf);
    // -------------------------------------------------------------

    vector_free(response);
    net_request_free(net_req_ptr);
    free(to_send);
    free(to_recv);

    return 0;
}