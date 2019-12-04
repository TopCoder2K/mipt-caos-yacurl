#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "http.h"

void http_header_t_free(const void *header_ptr) {
    http_header_t *hdr = (http_header_t *)header_ptr;
    free(hdr->key);
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
        
        http_header_t *s_header = malloc(sizeof(http_header_t));
        s_header->key = strndup(cur_line, key_end - cur_line);
        s_header->value = strndup(value, value_end - value);
        
        list_t *ls_next = list_empty();
        ls_next->value = s_header;
        ls_current = ls_current->next = ls_next;
        
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
