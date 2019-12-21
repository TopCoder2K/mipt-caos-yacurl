#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cli.h"
#include "http_response.h"
#include "http_request.h"
#include "io.h"
#include "net.h"

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

void dump_full_http_request(FILE *stream, http_request_t *request) {
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

net_request_t *netreq_from_httpreq(http_request_t *httpreq, int port) {
    net_request_t *netreq = net_request_init();
    
    http_header_key_t key_host = { .k_code = HTTP_HDR_HOST, .k_str = NULL };
    int found = 0;
    list_t *node_host = list_find_equal(
        httpreq->headers,
        &key_host,
        http_header_key_isequal,
        &found
    );
    assert(found);
    http_header_t *hdr_host = node_host->value;
    netreq->hostname = hdr_host->value;
    netreq->port = port;
    
    return netreq;
}

vector_t *get_response(net_request_t *netreq) {
    vector_t *vec = vector_init();
    netreq->recv_buf = malloc(0x1000);
    netreq->recv_buf_size = 0x1000;
    netreq->user_context = vec;
    netreq->on_data = on_data;
    assert(net_send_receive(netreq) == 0);
    return vec;
}

void netreq_finalize(net_request_t *netreq) {
    free(netreq->recv_buf);
    free(netreq->send_buf);
    net_request_free(netreq);
}

void display_response(http_response_t *response, int show_headers) {
    if (show_headers) {
        printf("%d %s\n", response->status_code, response->status_message);
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
        dump_http_request(stderr, request);
        dump_full_http_request(stderr, request);
#endif // DEBUG

        net_request_t *netreq = netreq_from_httpreq(request, 80);
        netreq->send_buf = http_request_write(request);
        netreq->send_buf_size = strlen(netreq->send_buf);

        vector_t *response_raw = get_response(netreq);
        fprintf(stderr, "[main] response_raw=``%s``\n", response_raw->buf);
        http_response_t *resp = http_response_parse(response_raw->buf);
        display_response(resp, cmdline.include_response_headers);
        

        netreq_finalize(netreq);
        http_request_free(request);
    }
    else {
        fprintf(stderr, "Invalid command line\n");
    }
    
    free_cmdline(&cmdline);
    return 0;
}
