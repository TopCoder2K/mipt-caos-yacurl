#include <assert.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "http_const.h"
#include "http_request.h"
#include "url.h"

http_request_t *http_request_init() {
    http_request_t *request = malloc(sizeof(http_request_t));
    request->version = NULL;
    // request->host = NULL;
    // request->port = -1;
    request->path = NULL;
    request->headers = list_empty();
    request->body = NULL;
    return request;
}

void http_request_free(http_request_t *request) {
    if (request == NULL)
        return;
    
    free(request->method);
    free(request->path);
    free(request->version);
    list_free(request->headers, http_header_t_free);
    // free(request->host);
    free(request->body);
    free(request);
}

void http_request_sethdr(
    http_request_t *request, http_header_t *new_header
) {
    http_header_set(request->headers, new_header);
}

int http_request_seturl(http_request_t *request, const char *url, int set_body) {
    url_info_t url_info;
    char *split_error = url_split(url, &url_info);
    if (split_error) {
        fprintf(
            stderr, "http_request_seturl(): url_split(): regexec() error: %s\n",
            split_error
        );
        free(split_error);
        return 1;
    }
    
    request->path = url_info.path;
    if (strlen(request->path) == 0) { // No path specified
        free(request->path);
        request->path = strdup("/");
    }
    
    http_header_t *hdr_host = malloc(sizeof(http_header_t));
    hdr_host->key.k_code = HTTP_HDR_HOST;
    hdr_host->key.k_str = NULL;
    hdr_host->value = url_info.host;
    http_request_sethdr(request, hdr_host);
    
    // TODO handle form-urlencoded (and proto?)
    if (set_body)
        http_request_set_body(url_info.form_data, request);
    free(url_info.form_data);
    free(url_info.proto);

    return 0;
}

static void http_request_set_contentlength(
    http_request_t *request, size_t len
) {
    char len_str[100] = "";
    snprintf(len_str, 100, "%zu", len);

    http_header_t *content_length = malloc(sizeof(http_header_t));
    content_length->key.k_code = HTTP_HDR_CONTENT_LENGTH;
    content_length->key.k_str = NULL;
    content_length->value = strdup(len_str);
    http_request_sethdr(request, content_length);
}

static void http_request_set_contenttype(
    http_request_t *request, const char *type
) {
    http_header_t *content_type = malloc(sizeof(http_header_t));
    content_type->key.k_code = HTTP_HDR_CONTENT_TYPE;
    content_type->key.k_str = NULL;
    content_type->value = strdup(type);
    http_request_sethdr(request, content_type);
}

void http_request_set_body(const char *body, http_request_t *request) {
#ifdef DEBUG
    fprintf(stderr, "[http_request_set_body] body=``%s``\n", body);
#endif // DEBUG
    size_t content_length = strlen(body);
    free(request->body);
    request->body = strdup(body);
    
    http_header_key_t key;
    key.k_code = HTTP_HDR_CONTENT_LENGTH;
    key.k_str = NULL;
    int is_found = 0;
    list_t *hdr_node = list_find_equal(
        request->headers,
        &key,
        http_header_key_isequal,
        &is_found
    );
    if (!is_found)
        http_request_set_contentlength(request, content_length);
    
    key.k_code = HTTP_HDR_CONTENT_TYPE;
    key.k_str = NULL;
    is_found = 0;
    hdr_node = list_find_equal(
        request->headers,
        &key,
        http_header_key_isequal,
        &is_found
    );
    if (!is_found)
        http_request_set_contenttype(request, "x-www-form-urlencoded");
}

// Return value:
//   zero iff ok
int http_request_validate_basic(http_request_t *request) {
    int error = 0;
    error |= request->method == NULL;
    error |= request->path == NULL;
    error |= request->version == NULL;
    error |= request->body == NULL;

    http_header_key_t hdr_host_key = { .k_code = HTTP_HDR_HOST };
    int is_found;
    list_t *hdr_host_node = list_find_equal(
        request->headers,
        &hdr_host_key,
        http_header_key_isequal,
        &is_found
    );
    error |= !is_found;
    if (!error) {
        http_header_t *hdr_host = hdr_host_node->value;
        error |= strlen(hdr_host->value) == 0;
    }
    
#ifdef DEBUG
    fprintf(
        stderr, "[http_request_validate_basic] error=%d\n",
        error
    );
#endif // DEBUG
    return error;
}

// Return value:
//   number of bytes enough to store the request string
size_t http_request_required_size(http_request_t *request) {
    size_t required_size = 1; // 0-terminator
    required_size += strlen(request->method) + 1;
    required_size += strlen(request->path) + 1;
    required_size += strlen(request->version) + strlen(gk_http_eol_seq);
    
    list_t *cnode = request->headers->next;
    while (cnode != NULL) {
        http_header_t *cheader = cnode->value;
        const char *key;
#ifdef DEBUG
        fprintf(
            stderr, "[http_request_required_size] header loop code=%d\n",
            cheader->key.k_code
        );
#endif // DEBUG
        if (cheader->key.k_code == HTTP_HDR_OTHER)
            key = cheader->key.k_str;
        else
            key = http_header_str(cheader->key.k_code);
        required_size += strlen(key) + 2 + strlen(cheader->value) + strlen(gk_http_eol_seq);
        cnode = cnode->next;
    }
    
    required_size += strlen(gk_http_eol_seq) + strlen(request->body);
    
#ifdef DEBUG
        fprintf(
            stderr, "[http_request_required_size] return=%zu\n",
            required_size
        );
#endif // DEBUG
    return required_size;
}

char *http_request_write(http_request_t *request) {
    char *request_buffer = NULL;
    if (http_request_validate_basic(request) == 0) {

        size_t required_size = http_request_required_size(request);
        request_buffer = malloc(required_size);
        memset(request_buffer, 0, required_size);
        
        request_buffer = strcat(request_buffer, request->method);
        request_buffer = strcat(request_buffer, " ");
        request_buffer = strcat(request_buffer, request->path);
        request_buffer = strcat(request_buffer, " ");
        request_buffer = strcat(request_buffer, request->version);
        request_buffer = strcat(request_buffer, gk_http_eol_seq);
        
        list_t *cnode = request->headers->next;
        while (cnode != NULL) {
            http_header_t *cheader = cnode->value;
            const char *key;
            if (cheader->key.k_code == HTTP_HDR_OTHER)
                key = cheader->key.k_str;
            else
                key = http_header_str(cheader->key.k_code);
#ifdef DEBUG
            fprintf(
                stderr, "[http_request_write] header loop key=``%s`` value=``%s``\n",
                key, cheader->value
            );
#endif // DEBUG
            request_buffer = strcat(request_buffer, key);
            request_buffer = strcat(request_buffer, ": ");
            request_buffer = strcat(request_buffer, cheader->value);
            request_buffer = strcat(request_buffer, gk_http_eol_seq);
            cnode = cnode->next;
        };
        
        request_buffer = strcat(request_buffer, gk_http_eol_seq);
        request_buffer = strcat(request_buffer, request->body);
        
#ifdef DEBUG
        fprintf(
            stderr, "[http_request_write] written len=%zu request=``%s``\n",
            strlen(request_buffer), request_buffer
        );
#endif // DEBUG
        assert(strlen(request_buffer) + 1 == required_size);
    }
    
    return request_buffer;
}
