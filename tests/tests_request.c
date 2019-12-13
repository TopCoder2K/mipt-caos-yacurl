#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../src/http_header.h"
#include "../src/http_request.h"

const char *gk_test_1_url = "http://acm.mipt.ru/twiki/bin/view/Algorithms/GeneralAlgorithmsLinks";
const char *gk_test_1_version = "1.1";
const char *gk_test_1_path = "/twiki/bin/view/Algorithms/GeneralAlgorithmsLinks";
const char *gk_test_1_host = "acm.mipt.ru";
void test_1() {
    http_request_t *req = http_request_init();
    if (! http_request_seturl(req, gk_test_1_url)) {
        list_t *node_hdr_host = req->headers->next;
        http_header_t *hdr_host = node_hdr_host->value;
        
        fprintf(
            stderr, "[test_1] version=\"%s\", host=\"%s\", path=\"%s\"\n",
            req->version, hdr_host->value, req->path
        );
        
        assert(! strcmp(req->version, gk_test_1_version));
        assert(! strcmp(req->path, gk_test_1_path));
        assert(hdr_host->key.k_code == HTTP_HDR_HOST);
        assert(hdr_host->key.k_str == NULL);
        assert(! strcmp(hdr_host->value, gk_test_1_host));
        
        fputs("Passed\n", stderr);
    }
    else {
        fputs("Error setting url\n", stderr);
    }
    http_request_free(req);
}

int main() {
    test_1();
    
    return 0;
}
