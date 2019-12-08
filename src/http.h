#pragma once
#include "common.h"
#include "http_header_list.h"

typedef struct http_header_key {
    http_header_code_t k_code;
    char *k_str;
} http_header_key_t;

typedef struct http_header {
    http_header_key_t key;
    char *value;
} http_header_t;

void http_header_t_free(const void *header_ptr);

// `raw' line format:
//   ^header:[ ]+value[ ]+$
// return value:
//   number of headers
// headers obtained can be freed by calls to http_header_t_free()
int http_parse_headers(const char *raw, size_t length, list_t **dest);

typedef struct http_request {
    const char *host;
    int port;
    http_header_t *headers;
    const char *body;
} http_request_t;

http_request_t *http_request_init();

void http_request_sethdr();