#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif // _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "http_response.h"

static const char *gk_http_eol_seq = "\r\n";
static const char *gk_http_double_eol_seq = "\r\n\r\n";

int http_response_split(char *resp_str,
    char **firstline_end, char **headers_begin,
    char **headers_end, char **body_begin
) {
    *firstline_end = strstr(resp_str, gk_http_eol_seq);
    int error = *firstline_end == NULL;
    if (!error) {
        *headers_begin = *firstline_end + strlen(gk_http_eol_seq);
        *headers_end = strstr(*headers_begin, gk_http_double_eol_seq);
        error = *headers_end == NULL;
        if (!error)
            *body_begin = *headers_end + strlen(gk_http_double_eol_seq);
    }
    return error;
}

int http_response_parse_headers(const char *raw, size_t length, list_t **dest) {
#ifdef DEBUG
    fprintf(
        stderr, "[http_response_parse_headers] raw=``%s`` length=%zu\n",
        raw, length
    );
#endif // DEBUG
    
    list_t *ls_head = list_empty();
    list_t *ls_current = ls_head;
    
    int headers_count = 0;
    int error = 0;
    const char *cur_line = raw;
    while (!error && cur_line != NULL && cur_line < raw + length) {
        const char *line_end = strstr(cur_line, gk_http_eol_seq);
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
        cur_line = line_end + strlen(gk_http_eol_seq);
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
