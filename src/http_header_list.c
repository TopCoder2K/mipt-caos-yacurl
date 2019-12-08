#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include "http_header_list.h"

static const char *http_header_titles[] = {
    "host",
    "user-agent",
    NULL
};

const char *http_header_str(http_header_code_t code) {
    return http_header_titles[code];
}

http_header_code_t http_header_code(const char *str) {
    int cur_code = 0;
    const char *cur_title;
    do {
        cur_title = http_header_titles[cur_code];
        ++cur_code;
    } while (cur_title != NULL && strcmp(cur_title, str));
    
    #ifdef DEBUG
    fprintf(
        stderr, "[http_header_code] arg=\"%s\", ret=%d\n",
        str, cur_code - 1
    );
    #endif
    return cur_code - 1;
}
