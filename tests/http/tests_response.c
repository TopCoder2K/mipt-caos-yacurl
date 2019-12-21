#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "http/http_response.h"

#define BUF_SIZE 0x1000

void dump_http_headers(FILE *stream, list_t *headers_list) {
    list_t *cnode = headers_list->next;
    while (cnode != NULL) {
        http_header_t *header = cnode->value;
        const char *key;
        if (header->key.k_code == HTTP_HDR_OTHER)
            key = header->key.k_str;
        else
            key = http_header_str(header->key.k_code);
        fprintf(
            stream, "Header: ``%s``: ``%s``\n",
            key, header->value
        );
        cnode = cnode->next;
    }
}

int main(int argc, char **argv) {
    int error = 0;
    int response_fd = open(argv[1], O_RDONLY);
    char *response_str = malloc(BUF_SIZE);
    memset(response_str, 0, BUF_SIZE);
    read(response_fd, response_str, BUF_SIZE);
    
    http_response_t *response = http_response_parse(response_str);
    if (response != NULL) {
        printf("Version: ``%s``\n", response->version);
        printf("Status code: %d\n", response->status_code);
        printf("Status message: ``%s``\n", response->status_message);
        printf("Headers:\n");
        dump_http_headers(stdout, response->headers);
        printf("Body: ``%s``\n", response->body);
    }
    else {
        fprintf(stderr, "Error: http_response_parse() failed\n");
    }
    
    http_response_free(response);
    free(response_str);
    
    return error;
}
