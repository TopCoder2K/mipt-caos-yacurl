#pragma once
#include "common/common.h"
#include "http_header.h"

typedef struct http_response {
    char *version;
    int status_code;
    char *status_message;
    list_t *headers;
    char *body;
} http_response_t;

// Implies UNIX end-of-line (LF)
int http_response_split(
    char *resp_str,
    char **firstline_end, char **headers_begin,
    char **headers_end, char **body_begin
);

// `raw' line format:
//   ^header:[ ]+value[ ]+$
// return value:
//   number of headers
// implies UNIX end-of-line (LF)
// headers obtained can be freed by calls to http_header_t_free()
int http_response_parse_headers(const char *raw, size_t length, list_t **dest);

http_response_t *http_response_init();
void http_response_free(http_response_t *response);

http_response_t *http_response_parse(char *raw);
