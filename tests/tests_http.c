#include <stdio.h>
#include <string.h>
#include "http.h"

const char test_case_1[] =
    "Host: mipt.ru\n"
    "Content-Length: 0\n"
    "Content-Type:    text/plain    \n"
    "X-CSRF-Token:8000000000000009\n"
;

int main() {
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
            printf("\"%s\": \"%s\"\n", ps_header->key, ps_header->value);
            cur_header = cur_header->next;
        }
        
        list_free(headers, http_header_t_free);
    }
    else {
        printf("Error processing headers: %d\n", headers_count);
    }
    
    return 0;
}
