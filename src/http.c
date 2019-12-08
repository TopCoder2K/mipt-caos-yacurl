#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "http.h"

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

http_request_t *http_request_init() {
    http_request_t *request = malloc(sizeof(http_request_t));
    request->host = NULL;
    request->port = -1;
    request->headers = list_empty();
    request->body = NULL;
    return request;
}

void http_request_free(http_request_t *request) {
    free(request->body);
    list_free(request->headers, http_header_t_free);
    free(request->host);
    free(request);
}

void http_request_sethdr(
    http_request_t *request, http_header_t *new_header
) {
    if (new_header->key.k_code == HTTP_HDR_OTHER)
        str_lowercase(new_header->key.k_str);

    int is_found;
    list_t *found_node = list_find_equal(
        request->headers,
        &new_header->key,
        http_header_key_isequal,
        &is_found
    );

    if (is_found) {
        http_header_t *found_header = found_node->value;
        if (strcmp(found_header->value, new_header->value)) {
            free(found_header->value);
            found_header->value = strdup(new_header->value);
        }
    }
    else {
        list_t *tail = found_node;
        list_append(tail, new_header);
    }
}
