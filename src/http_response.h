#pragma once
#include "common.h"
#include "http_header.h"

// Implies UNIX end-of-line (LF)
// BUG: fails when no headers are set, example: tests/tests_response.2.in
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
