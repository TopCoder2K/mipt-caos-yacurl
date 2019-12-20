#pragma once
#include "common.h"
#include "http_header.h"

typedef struct http_request {
    char *method;
    char *path;
    char *version;
    list_t *headers;
    char *body;
} http_request_t;

http_request_t *http_request_init();
void http_request_free(http_request_t *request);

void http_request_sethdr(http_request_t *request, http_header_t *header);

// Will set the following:
//   Host header and .path as specifed in url
// Return value:
//   zero iff no error
int http_request_seturl(http_request_t *request, const char *url, int set_body);

// Return value:
//   pointer to string, containing the request
//   NULL if request is malformed
// Well-formed request must include not-null method, path, version, body
// and Host header
char *http_request_write(http_request_t *request);

void http_request_set_body(const char *body, http_request_t *request);
