#pragma once
#include "common.h"
#include "http_header.h"

typedef struct http_request {
    char *host;
    int port;
    list_t *headers;
    char *body;
} http_request_t;

http_request_t *http_request_init();
void http_request_free(http_request_t *request);

void http_request_sethdr(http_request_t *request, http_header_t *header);
