#pragma once
#include "common.h"

typedef enum http_header_code {
    HTTP_HDR_HOST = 0,
    HTTP_HDR_USER_AGENT = 1,
    HTTP_HDR_OTHER = 2
} http_header_code_t;

const char *http_header_str(http_header_code_t code);

// str must be lowercase
http_header_code_t http_header_code(const char *str);



// k_str == NULL iff k_code != HTTP_HDR_OTHER
// else k_str points to header title
typedef struct http_header_key {
    http_header_code_t k_code;
    char *k_str;
} http_header_key_t;

typedef struct http_header {
    http_header_key_t key;
    char *value;
} http_header_t;

// will free() .value, .key.k_str (if HTTP_HDR_OTHER) and the argument itself
void http_header_t_free(const void *header_ptr);

// return value:
//   true iff keys are equal
int http_header_key_isequal(
    const http_header_key_t *first,
    const http_header_key_t *second
);

// `raw' line format:
//   ^header:[ ]+value[ ]+$
// return value:
//   number of headers
// headers obtained can be freed by calls to http_header_t_free()
int http_parse_headers(const char *raw, size_t length, list_t **dest);

// Will call free() on new_header and its fields, so it shouldn`t be used
// after
void http_header_set(list_t *headers, http_header_t *new_header);
