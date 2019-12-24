#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif // _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "http_const.h"
#include "http_response.h"

int http_response_split(
    char *resp_str,
    char **firstline_end, char **headers_begin,
    char **headers_end, char **body_begin
) {
    fprintf(stderr, "[http_response_split] resp=``%s``\n", resp_str);
    *firstline_end = strstr(resp_str, gk_http_eol_seq);
    int error = *firstline_end == NULL;
    if (!error) {
        *headers_end = strstr(resp_str, gk_http_double_eol_seq);
        error = *headers_end == NULL;
        if (!error) {
            if (*headers_end != *firstline_end) // Headers present
                *headers_begin = *firstline_end + strlen(gk_http_eol_seq);
            else
                *headers_begin = *headers_end;

            *body_begin = *headers_end + 2 * strlen(gk_http_eol_seq);
        }
        else
            fprintf(stderr, "[http_response_split] 2\n");
    }
    else
        fprintf(stderr, "[http_response_split] 1\n");
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

http_response_t *http_response_init() {
    http_response_t *response = malloc(sizeof(http_response_t));
    response->version = NULL;
    response->status_code = 0;
    response->status_message = NULL;
    response->headers = NULL;
    response->body = NULL;
    return response;
}

void http_response_free(http_response_t *response) {
    if (response == NULL)
        return;
    
    free(response->body);
    list_free(response->headers, http_header_t_free);
    free(response->status_message);
    free(response->version);
    free(response);
}

int http_response_parse_firstline(
    char *begin, char *end, http_response_t *response
) {
    char *version_end = strchr(begin, ' ');
    int error = version_end == NULL;
    
    if (!error) {
        char *status_code_begin = version_end;
        while (*status_code_begin == ' ')
            ++status_code_begin;
        error = *status_code_begin == 0;
        
        if (!error) {
            char *status_code_end = strchr(status_code_begin + 1, ' ');
            error = *status_code_end == 0;
            
            if (!error) {
                char *status_message_begin = status_code_end + 1;
                while (*status_message_begin == ' ')
                    ++status_message_begin;
                error = *status_message_begin == 0;
                
                if (!error) {
                    char *err_ptr = NULL;
                    *status_code_end = 0;
                    response->status_code = strtol(
                        status_code_begin, &err_ptr, 10
                    );
                    error = err_ptr != status_code_end;
                    if (!error) {
                        response->version = strndup(
                            begin, version_end - begin
                        );
                        response->status_message = strndup(
                            status_message_begin, end - status_message_begin
                        );
                    }
                    else
                        fprintf(stderr, "[http_response_parse_firstline] strtol() failed on field 3 \n");
                }
                else
                    fprintf(stderr, "[http_response_parse_firstline] field 3 missing (eol) \n");
            }
            else
                fprintf(stderr, "[http_response_parse_firstline] field 3 missing (space) \n");
        }
        else
            fprintf(stderr, "[http_response_parse_firstline] field 2 missing (eol) \n");
    }
    else
        fprintf(stderr, "[http_response_parse_firstline] filed 2 missing (space)\n");
    
    return error;
}

http_response_t *http_response_parse(char *raw) {
    http_response_t *response = http_response_init();
    char *firstline_end, *headers_begin, *headers_end, *body_begin;
    int error = http_response_split(
        raw,
        &firstline_end, &headers_begin, &headers_end, &body_begin
    );
    
    if (!error) {
        error = http_response_parse_firstline(raw, firstline_end, response);
        
        if (!error) {
            int headers_cnt = http_response_parse_headers(
                headers_begin, headers_end - headers_begin, &response->headers
            );
            error = headers_cnt < 0;
            
            if (!error)
                response->body = strdup(body_begin);
            else
                fprintf(stderr, "[http_response_parse] Error: http_response_parse_headers() failed\n");
        }
        else
            fprintf(stderr, "[http_response_parse] Error: http_response_parse_firstline() failed\n");
    }
    else {
        fprintf(stderr, "[http_response_parse] Error: http_response_split() failed\n");
    }
    
    if (error) {
        http_response_free(response);
        response = NULL;
    }
    return response;
}
