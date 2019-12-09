#define _GNU_SOURCE
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
    http_header_t *hdr = (http_header_t *)header_ptr;
    if (hdr->key.k_code == HTTP_HDR_OTHER)
        free(hdr->key.k_str);
    free(hdr->value);
    free(hdr);
}

// `raw' line format:
//   ^header:[ ]+value[ ]+$
// return value:
//   number of headers
// headers obtained can be freed by calls to http_header_t_free()
int http_parse_headers(const char *raw, size_t length, list_t **dest) {
#ifdef DEBUG
    fprintf(
        stderr, "[http_parse_headers] raw=%p length=%zu dest=%p\n",
        raw, length, dest
    );
#endif // DEBUG
    
    list_t *ls_head = list_empty();
    list_t *ls_current = ls_head;
    
    int headers_count = 0;
    int error = 0;
    const char *cur_line = raw;
    while (cur_line != NULL && cur_line < raw + length) {
        const char *line_end = strchr(cur_line, '\n');
        if (line_end == NULL || line_end == cur_line) {
#ifdef DEBUG
            fprintf(
                stderr, "headers_count=%d cur_line=%p line_end=%p diff=%ld\n",
                headers_count, cur_line, line_end, line_end - cur_line
            );
            fprintf(
                stderr, "unprocessed len=%ld content=\"%s\"\n",
                raw + length - cur_line, cur_line
            );
#endif // DEBUG
            error = -1;
            break;
        }
#ifdef DEBUG
        fprintf(stderr, "line: \"");
        for (const char *pc = cur_line; pc != line_end; ++pc)
            fputc(*pc, stderr);
        fprintf(stderr, "\"\n");
#endif // DEBUG
        
        const char *key_end = strchr(cur_line, ':');
        if (key_end == NULL || key_end == cur_line || key_end > line_end) {
            error = -1;
            break;
        }
#ifdef DEBUG
        fprintf(stderr, "key: \"");
        for (const char *pc = cur_line; pc != key_end; ++pc)
            fputc(*pc, stderr);
        fprintf(stderr, "\"\n");
#endif // DEBUG
        
        const char *value = key_end + 1;
        while (value < line_end && *value == ' ')
            ++value;
        if (value == key_end) {
            error = -1;
            break;
        }
        
        const char *value_end = line_end;
        while (value_end > value && *(value_end - 1) == ' ')
            --value_end;
#ifdef DEBUG
        fprintf(stderr, "value: \"");
        for (const char *pc = value; pc != value_end; ++pc)
            fputc(*pc, stderr);
        fprintf(stderr, "\"\n");
#endif // DEBUG
        
        char *key = strndup(cur_line, key_end - cur_line);
        str_lowercase(key);
        http_header_code_t key_code = http_header_code(key);
        if (key_code != HTTP_HDR_OTHER) {
            free(key);
            key = NULL;
        }
        
        http_header_t *s_header = malloc(sizeof(http_header_t));
        s_header->key.k_str = key;
        s_header->key.k_code = key_code;
        s_header->value = strndup(value, value_end - value);
        
        ls_current = list_append(ls_current, s_header);
        ++headers_count;
        cur_line = line_end + 1;
    }
    
    if (error == 0) {
        *dest = ls_head;
        return headers_count;
    }
    else {
        list_free(ls_head, http_header_t_free);
        return error;
    }
}
