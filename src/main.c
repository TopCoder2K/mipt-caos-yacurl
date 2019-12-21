#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cli.h"
#include "http_response.h"
#include "http_request.h"

void display_headers(FILE *stream, const char *fmt, list_t *headers_list) {
    list_t *cnode = headers_list->next;
    while (cnode != NULL) {
        http_header_t *header = cnode->value;
        const char *key;
        if (header->key.k_code == HTTP_HDR_OTHER)
            key = header->key.k_str;
        else
            key = http_header_str(header->key.k_code);
        fprintf(stream, fmt, key, header->value);
        cnode = cnode->next;
    }
}

#ifdef DEBUG
void dump_cmdline(FILE *stream, cmdline_t *cmdline) {
    fprintf(stream, "[dump_cmdline] include_response_headers: %d\n", cmdline->include_response_headers);
    fprintf(stream, "[dump_cmdline] same_name_outfile: %d\n", cmdline->same_name_outfile);
    fprintf(stream, "[dump_cmdline] method: ``%s``\n", cmdline->method);

    fprintf(stream, "[dump_cmdline] headers:\n");
    display_headers(stream, "[dump_cmdline] -- ``%s``: ``%s``\n", cmdline->headers);

    fprintf(stream, "[dump_cmdline] body: ``%s``\n", cmdline->body);
    fprintf(stream, "[dump_cmdline] dst_file: ``%s``\n", cmdline->dst_file);
    fprintf(stream, "[dump_cmdline] url: ``%s``\n", cmdline->url);
}

void dump_http_request(FILE *stream, http_request_t *request) {
    fprintf(
        stream, "[dump_http_request] method=``%s``, path=``%s``, version=``%s``\n",
        request->method, request->path, request->version
    );
    fprintf(
        stream, "[dump_http_request] body=``%s``\n",
        request->body
    );

    fprintf(stream, "[dump_http_request] headers:\n");
    display_headers(stream, "[dump_http_request] -- ``%s``: ``%s``\n", request->headers);
}

void dump_full_http_requst(FILE *stream, http_request_t *request) {
    char *full = http_request_write(request);
    fprintf(stream, "[dump_full_http_requst] full=``%s``\n", full);
    free(full);
}
#endif // DEBUG

http_request_t *request_from_cmdline(cmdline_t *cmdline) {
    http_request_t *request = http_request_init();

    http_request_seturl(
        request, cmdline->url, !cmdline->has_nonempty_body
    );

    request->method = cmdline->method;
    cmdline->method = NULL;
    request->version = strdup("HTTP/1.1");
    if (cmdline->has_nonempty_body)
        http_request_set_body(cmdline->body, request);
    free(cmdline->body);
    cmdline->body = NULL;

    list_t *cnode = cmdline->headers->next;
    while (cnode != NULL) {
        http_header_t *chdr = cnode->value;
        http_request_sethdr(request, chdr); // BUG memory leak
        cnode->value = NULL;
        cnode = cnode->next;
    }

    return request;
}

void display_response(http_response_t *response, int show_headers) {
    if (show_headers) {
        printf("%d %s\n", response->status_code, response->body);
        display_headers(stdout, "%s: %s\n", response->headers);
    }
    fputs(response->body, stdout);
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
