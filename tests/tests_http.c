#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../src/http_header.h"
#include "../src/http_request.h"

const char test_case_1[] =
    "Host: mipt.ru\n"
    "Content-Length: 0\n"
    "Content-Type:    text/plain    \n"
    "X-CSRF-Token:8000000000000009\n"
;

void test_1() {
    printf("Test 1\n");
    list_t *headers = NULL;
    int headers_count = http_parse_headers(
        test_case_1,
        strlen(test_case_1),
        &headers
    );
    
    
    if (headers_count >= 0) {
        printf("Headers count: %d\n", headers_count);
        list_t *cur_header = headers->next;
        while (cur_header != NULL) {
            http_header_t *ps_header = cur_header->value;
            const char *key;
            if (ps_header->key.k_code == HTTP_HDR_OTHER) {
                printf("[UNK] ");
                key = ps_header->key.k_str;
            }
            else {
                printf("[%3d] ", ps_header->key.k_code);
                key = http_header_str(ps_header->key.k_code);
            }
            printf(" \"%s\": \"%s\"\n", key, ps_header->value);
            cur_header = cur_header->next;
        }
        
        list_free(headers, http_header_t_free);
    }
    else {
        printf("Error processing headers: %d\n", headers_count);
    }
    printf("End test 1\n");
}

void test_2() {
    printf("Test 2\n");
    http_request_t *request = http_request_init();

    http_header_t *hdr_host = malloc(sizeof(http_header_t));
    hdr_host->key.k_code = HTTP_HDR_HOST;
    hdr_host->value = strdup("ident.me");
    http_request_sethdr(request, hdr_host);
    hdr_host = NULL;

    http_header_t *hdr_accept_encoding = malloc(sizeof(http_header_t));
    hdr_accept_encoding->key.k_code = HTTP_HDR_OTHER;
    hdr_accept_encoding->key.k_str = strdup("Accept-Encoding");
    hdr_accept_encoding->value = strdup("gzip");
    http_request_sethdr(request, hdr_accept_encoding);
    hdr_accept_encoding = NULL;

    http_header_t *hdr_user_agent = malloc(sizeof(http_header_t));
    hdr_user_agent->key.k_code = HTTP_HDR_USER_AGENT;
    hdr_user_agent->value = strdup("yacurl");
    http_request_sethdr(request, hdr_user_agent);
    hdr_user_agent = NULL;

    list_t *cur_header = request->headers->next;
    while (cur_header != NULL) {
        http_header_t *ps_header = cur_header->value;
        const char *key;
        if (ps_header->key.k_code == HTTP_HDR_OTHER) {
            printf("[UNK]");
            key = ps_header->key.k_str;
        }
        else {
            printf("[%3d]", ps_header->key.k_code);
            key = http_header_str(ps_header->key.k_code);
        }
        printf(" \"%s\": \"%s\"\n", key, ps_header->value);
        cur_header = cur_header->next;
    }
    
    http_request_free(request);
    printf("End test 2\n");
}

int main() {
    test_1();
    test_2();
    
    return 0;
}
