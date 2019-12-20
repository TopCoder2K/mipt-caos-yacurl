#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "http_header.h"

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



int http_header_key_isequal(
    const http_header_key_t *first, const http_header_key_t *second
) {
    if (first->k_code == HTTP_HDR_OTHER && second->k_code == HTTP_HDR_OTHER) {
        int diff = strcmp(first->k_str, second->k_str);
        return diff == 0;
    }
    else
        return first->k_code == second->k_code;
}

void http_header_t_free(const void *header_ptr) {
    if (header_ptr == NULL)
        return;
    
    http_header_t *hdr = (http_header_t *)header_ptr;
    if (hdr->key.k_code == HTTP_HDR_OTHER)
        free(hdr->key.k_str);
    free(hdr->value);
    free(hdr);
}

void http_header_set(list_t *headers, http_header_t *new_header) {
    if (new_header->key.k_code == HTTP_HDR_OTHER)
        str_lowercase(new_header->key.k_str);

    int is_found;
    list_t *found_node = list_find_equal(
        headers,
        &new_header->key,
        http_header_key_isequal,
        &is_found
    );

    if (is_found) {
        http_header_t *found_header = found_node->value;
        free(found_header->value);
        found_header->value = strdup(new_header->value);
        http_header_t_free(new_header);
    }
    else {
        list_t *tail = found_node;
        list_append(tail, new_header);
    }
}
