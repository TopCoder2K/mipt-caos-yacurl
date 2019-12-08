#pragma once

typedef enum http_header_code {
    HTTP_HDR_HOST = 0,
    HTTP_HDR_USER_AGENT = 1,
    HTTP_HDR_OTHER = 2
} http_header_code_t;

const char *http_header_str(http_header_code_t code);

// str must be lowercase
http_header_code_t http_header_code(const char *str);
