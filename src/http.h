#pragma once
#include "common.h"

typedef struct http_header {
    char *key;
    char *value;
} http_header_t;

void http_header_t_free(const void *header_ptr);

// `raw' line format:
//   ^header:[ ]+value[ ]+$
// return value:
//   number of headers
// headers obtained can be freed by calls to http_header_t_free()
int http_parse_headers(const char *raw, size_t length, list_t **dest);
