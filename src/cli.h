#pragma once
#include "common.h"
#include "http_request.h"

typedef struct cmdline {
    int include_response_headers;
    int same_name_outfile;
    char *method;
    list_t *headers;
    char *body;
    char *dst_file;
    char *url;
} cmdline_t;

void print_help();

// Parameters:
//   argc, argv -- as passed to main()
// Return value:
//   zero if no error ocurred; otherwise index of failing argument
int parse_cmdline(int argc, char **argv, cmdline_t *cmdline);

void free_cmdline(cmdline_t *cmdline);
