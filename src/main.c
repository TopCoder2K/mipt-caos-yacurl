#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "cli.h"
#include "http_request.h"

#ifdef DEBUG
void dump_cmdline(FILE *stream, cmdline_t *cmdline) {
    fprintf(stream, "[dump_cmdline] include_response_headers: %d\n", cmdline->include_response_headers);
    fprintf(stream, "[dump_cmdline] same_name_outfile: %d\n", cmdline->same_name_outfile);
    fprintf(stream, "[dump_cmdline] method: ``%s``\n", cmdline->method);
    
    fprintf(stream, "[dump_cmdline] headers:\n");
    list_t *cnode = cmdline->headers->next;
    while (cnode != NULL) {
        http_header_t *header = cnode->value;
        const char *key;
        if (header->key.k_code == HTTP_HDR_OTHER)
            key = header->key.k_str;
        else
            key = http_header_str(header->key.k_code);
        fprintf(stream, "[dump_cmdline] -- ``%s``: ``%s``\n", key, header->value);
        cnode = cnode->next;
    }
    
    fprintf(stream, "[dump_cmdline] body: ``%s``\n", cmdline->body);
    fprintf(stream, "[dump_cmdline] dst_file: ``%s``\n", cmdline->dst_file);
    fprintf(stream, "[dump_cmdline] url: ``%s``\n", cmdline->url);
}

void dump_http_request(FILE *stream, http_request_t *request) {
    fprintf(
        stream, "[dump_http_request] method=``%s``, path=``%s``, version=``%s``\n",
        request->method, request->path, request->version
    );
    
    fprintf(stream, "[dump_http_request] headers:\n");
    list_t *cnode = request->headers->next;
    while (cnode != NULL) {
        http_header_t *header = cnode->value;
        const char *key;
        if (header->key.k_code == HTTP_HDR_OTHER)
            key = header->key.k_str;
        else
            key = http_header_str(header->key.k_code);
        fprintf(
            stream, "[dump_http_request] -- ``%s``: ``%s``\n",
            key, header->value
        );
        cnode = cnode->next;
    }
}

void dump_full_http_requst(FILE *stream, http_request_t *request) {
    char *full = http_request_write(request);
    fprintf(stream, "[dump_full_http_requst] full=``%s``\n", full);
    free(full);
}
#endif // DEBUG

http_request_t *request_from_cmdline(cmdline_t *cmdline) {
    http_request_t *request = http_request_init();
    
    request->method = cmdline->method;
    cmdline->method = NULL;
    request->version = strdup("HTTP/1.1");
    http_request_set_body(cmdline->body, request);
    cmdline->body = NULL;
    
    assert(request->headers->next == NULL);
    request->headers->next = cmdline->headers->next;
    cmdline->headers->next = NULL;
    
    http_request_seturl(request, cmdline->url, 1);
    
    return request;
}

int main(int argc, char **argv) {
    cmdline_t cmdline;
    int retval;
    retval = parse_cmdline(argc, argv, &cmdline);
    
    if (retval == 0) {
#ifdef DEBUG
        dump_cmdline(stderr, &cmdline);
#endif // DEBUG
        
        http_request_t *request = request_from_cmdline(&cmdline);
#ifdef DEBUG
        dump_http_request(stdout, request);
        dump_full_http_requst(stdout, request);
#endif // DEBUG
        
        http_request_free(request);
    }
    else {
        fprintf(stderr, "Invalid command line\n");
    }
    
    free_cmdline(&cmdline);
    return 0;
}
