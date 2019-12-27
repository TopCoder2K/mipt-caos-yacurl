#include <stdio.h>
#include "app/cli.h"

int main(int argc, char **argv) {
    cmdline_t cmdline;
    int retval;
    retval = parse_cmdline(argc, argv, &cmdline);
    
    if (retval == 0) {
        printf("include_response_headers: %d\n", cmdline.include_response_headers);
        printf("same_name_outfile: %d\n", cmdline.same_name_outfile);
        printf("method: ``%s``\n", cmdline.method);
        
        printf("headers:\n");
        list_t *cnode = cmdline.headers->next;
        while (cnode != NULL) {
            http_header_t *header = cnode->value;
            const char *key;
            if (header->key.k_code == HTTP_HDR_OTHER)
                key = header->key.k_str;
            else
                key = http_header_str(header->key.k_code);
            printf("-- ``%s``: ``%s``\n", key, header->value);
            cnode = cnode->next;
        }
        
        printf("body: ``%s``\n", cmdline.body);
        printf("dst_file: ``%s``\n", cmdline.dst_file);
        printf("url: ``%s``\n", cmdline.url);
    }
    
    free_cmdline(&cmdline);
    return 0;
}
