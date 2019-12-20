#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "../src/http_response.h"

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
    
    char *firstline_end = NULL, *headers_begin = NULL, *headers_end = NULL,
        *body_begin = NULL;
    error = http_response_split(
        response_str,
        &firstline_end, &headers_begin, &headers_end, &body_begin
    );
    if (!error) {
        fputs("First line: ``", stdout);
        for (char *pchr = response_str; pchr < firstline_end; ++pchr)
            putchar(*pchr);
        fputs("``\n", stdout);
        fputs("Body: ``", stdout);
        for (char *pchr = body_begin; *pchr; ++pchr)
            putchar(*pchr);
        fputs("``\n", stdout);
        
        list_t *headers_list = NULL;
        int headers_cnt = http_response_parse_headers(
            headers_begin, headers_end - headers_begin, &headers_list
        );
        error = headers_cnt == -1;
        if (!error) {
            printf("%d headers:\n", headers_cnt);
            dump_http_headers(stdout, headers_list);
            list_free(headers_list, http_header_t_free);
        }
        else
            fprintf(stderr, "Error: http_response_parse_headers() failed\n");
    }
    else
        fprintf(stderr, "Error: http_response_split() failed\n");
    
    free(response_str);
    return error;
}
