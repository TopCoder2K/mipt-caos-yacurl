#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../src/http_header.h"
#include "../src/http_request.h"

const char *gk_test_1_url = "http://acm.mipt.ru/twiki/bin/view/Algorithms/GeneralAlgorithmsLinks";
const char *gk_test_1_method = "GET";
const char *gk_test_1_version = "HTTP/1.1";
const char *gk_test_1_path = "/twiki/bin/view/Algorithms/GeneralAlgorithmsLinks";
const char *gk_test_1_host = "acm.mipt.ru";

const char *gk_test_2_url = "https://lainon.life/graphs/d/000000001/lainon-life?orgId=1&from=now-7d&to=now";
const char *gk_test_2_method = "POST";
const char *gk_test_2_version = "HTTP/2";
const char *gk_test_2_path = "/graphs/d/000000001/lainon-life";
const char *gk_test_2_host = "lainon.life";
const char *gk_test_2_hdr_upgrade_key = "upgrade";
const char *gk_test_2_hdr_upgrade_value = "h2";
const char *gk_test_2_hdr_x_lets_all_love_lain_key = "x-lets-all-love-lain";
const char *gk_test_2_hdr_x_lets_all_love_lain_value = "";
const char *gk_test_2_request_full = ""
    "POST /graphs/d/000000001/lainon-life HTTP/2\n" // first line & headers
    "host: lainon.life\n"
    "upgrade: h2\n"
    "x-lets-all-love-lain: \n"
    "\n"
    ""; // body (empty)

void test_1() {
    http_request_t *req = http_request_init();
    req->method = strdup("GET");
    req->version = strdup(gk_test_1_version);
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

void test_2(int argc, char **argv) {
    http_request_t *req = http_request_init();
    req->method = strdup(gk_test_2_method);
    req->version = strdup(gk_test_2_version);
    req->body = strdup("");
    if (! http_request_seturl(req, gk_test_2_url)) {
        http_header_t *hdr_set_upgrade = malloc(sizeof(http_header_t));
        hdr_set_upgrade->key.k_code = HTTP_HDR_OTHER;
        hdr_set_upgrade->key.k_str = strdup(gk_test_2_hdr_upgrade_key);
        hdr_set_upgrade->value = strdup(gk_test_2_hdr_upgrade_value);
        http_header_set(req->headers, hdr_set_upgrade);
        
        http_header_t *hdr_set_x_lets_all_love_lain = malloc(sizeof(http_header_t));
        hdr_set_x_lets_all_love_lain->key.k_code = HTTP_HDR_OTHER;
        hdr_set_x_lets_all_love_lain->key.k_str = strdup(gk_test_2_hdr_x_lets_all_love_lain_key);
        hdr_set_x_lets_all_love_lain->value = strdup(gk_test_2_hdr_x_lets_all_love_lain_value);
        http_header_set(req->headers, hdr_set_x_lets_all_love_lain);
        
        fprintf(
            stderr, "[test_2] method=``%s``, path=``%s``, version=``%s``\n",
            req->method, req->path, req->version
        );
        
        fprintf(stderr, "[test_2] headers:\n");
        list_t *cnode = req->headers->next;
        while (cnode != NULL) {
            http_header_t *header = cnode->value;
            const char *key;
            if (header->key.k_code == HTTP_HDR_OTHER)
                key = header->key.k_str;
            else
                key = http_header_str(header->key.k_code);
            fprintf(stderr, "[test_2] -- ``%s``: ``%s``\n", key, header->value);
            cnode = cnode->next;
        }
        
        assert(! strcmp(req->method, gk_test_2_method));
        assert(! strcmp(req->path, gk_test_2_path));
        assert(! strcmp(req->version, gk_test_2_version));
        
        int is_found = 0;
        // Header: host
        http_header_key_t hdr_host_key = { .k_code = HTTP_HDR_HOST };
        list_t *hdr_check_host_node = list_find_equal(
            req->headers,
            &hdr_host_key,
            http_header_key_isequal,
            &is_found
        );
        assert(is_found);
        http_header_t *hdr_check_host = hdr_check_host_node->value;
        assert(hdr_check_host->key.k_code == HTTP_HDR_HOST);
        assert(hdr_check_host->key.k_str == NULL);
        assert(!strcmp(hdr_check_host->value, gk_test_2_host));
        
        // Header: upgrade
        is_found = 0;
        http_header_key_t hdr_upgrade_key = { .k_code = HTTP_HDR_OTHER, .k_str = gk_test_2_hdr_upgrade_key };
        list_t *hdr_check_upgrade_node = list_find_equal(
            req->headers,
            &hdr_upgrade_key,
            http_header_key_isequal,
            &is_found
        );
        assert(is_found);
        http_header_t *hdr_check_upgrade = hdr_check_upgrade_node->value;
        assert(hdr_check_upgrade->key.k_code == HTTP_HDR_OTHER);
        assert(!strcmp(hdr_check_upgrade->key.k_str, gk_test_2_hdr_upgrade_key));
        assert(!strcmp(hdr_check_upgrade->value, gk_test_2_hdr_upgrade_value));
        
        // Header: x-lets-all-love-lain
        is_found = 0;
        http_header_key_t hdr_x_lets_all_love_lain_key = { .k_code = HTTP_HDR_OTHER, .k_str = gk_test_2_hdr_x_lets_all_love_lain_key };
        list_t *hdr_check_x_lets_all_love_lain_node = list_find_equal(
            req->headers,
            &hdr_x_lets_all_love_lain_key,
            http_header_key_isequal,
            &is_found
        );
        assert(is_found);
        http_header_t *hdr_check_x_lets_all_love_lain = hdr_check_x_lets_all_love_lain_node->value;
        assert(hdr_check_x_lets_all_love_lain->key.k_code == HTTP_HDR_OTHER);
        assert(!strcmp(hdr_check_x_lets_all_love_lain->key.k_str, gk_test_2_hdr_x_lets_all_love_lain_key));
        assert(!strcmp(hdr_check_x_lets_all_love_lain->value, gk_test_2_hdr_x_lets_all_love_lain_value));
        
        char *req_formed = http_request_write(req);
        fprintf(
            stderr, "[test 2] full request=``%s``\n",
            req_formed
        );
        assert(!strcmp(req_formed, gk_test_2_request_full));
        
        free(req_formed);
        fputs("Passed\n", stderr);
    }
    else {
        fputs("Error setting url\n", stderr);
    }
    http_request_free(req);
}

int main(int argc, char **argv) {
    test_1();
    test_2(argc, argv);
    
    return 0;
}
