#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "http_request.h"
#include "url.h"

http_request_t *http_request_init() {
    http_request_t *request = malloc(sizeof(http_request_t));
    // request->host = NULL;
    // request->port = -1;
    request->headers = list_empty();
    request->body = NULL;
    return request;
}

void http_request_free(http_request_t *request) {
    free(request->body);
    list_free(request->headers, http_header_t_free);
    // free(request->host);
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

void http_request_seturl(http_request_t *request, const char *url) {
    url_info_t url_info;
    int split_result = url_split(url, &url_info);
    if (split_result) {
        fprintf(
            stderr, "http_request_seturl(): url_split(): regexec() error\n"
        );
    }
    
    request->version = strdup("1.1");
    request->path = url_info.path;
    
    http_header_t *hdr_host = malloc(sizeof(http_header_t));
    hdr_host->key.k_code = HTTP_HDR_HOST;
    hdr_host->key.k_str = NULL;
    hdr_host->value = url_info.host;
    http_request_sethdr(request, hdr_host);
    
    // TODO handle form-urlencoded
    free(url_info.form_data);
}
