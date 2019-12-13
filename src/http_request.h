#pragma once
#include "common.h"
#include "http_header.h"

typedef struct http_request {
    char *version;
    char *path;
    list_t *headers;
    char *body;
} http_request_t;

http_request_t *http_request_init();
void http_request_free(http_request_t *request);

void http_request_sethdr(http_request_t *request, http_header_t *header);

// Will set the following:
//   .method <- GET
//   .version <- 1.1
//   Host header and .path as specifed in url
// Return value:
//   zero iff no error
int http_request_seturl(http_request_t *request, const char *url);
