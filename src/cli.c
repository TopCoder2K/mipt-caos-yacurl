#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif // _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "cli.h"
#include "http_methods.h"

static const char *gk_optstring = "iOX:H:d:o:";
static const char *gk_help_fmt = ""
    "Usage: %s [-iO] [-X method] [-H header...] [-d body] [-o filename] url\n"
    "    -i: include response headers to output\n"
    "    -O: output to file named after remote\n";
static const char *gk_err_cmdline_method_fmt = "Unsupported method: %s\n";
static const char *gk_err_cmdline_header_fmt = "Bad header format: %s\n";
static const char *gk_err_cmdline_url_missing = "Missing parameter: url\n";
static const char *gk_err_cmdline_extra_param_fmt = "Extra parameter(s): %s\n";

void print_help(const char *progname) {
    fprintf(stderr, gk_help_fmt, progname);
}

int init_cmdline_t(cmdline_t *cmdline) {
    cmdline->include_response_headers = 0;
    cmdline->same_name_outfile = 0;
    cmdline->method = strdup("GET");
    cmdline->headers = list_empty();
    cmdline->body = strdup("");
    cmdline->dst_file = NULL;
    cmdline->url = NULL;
}

int cmdline_set_method(cmdline_t *cmdline, const char *method) {
    int error = !http_methods_is_supported(method);
    if (!error) {
        free(cmdline->method);
        cmdline->method = strdup(method);
        // fprintf(stderr, "method=``%s``\n", method);
    }
    else
        fprintf(stderr, gk_err_cmdline_method_fmt, method);
    
    return error;
}

int cmdline_add_header(cmdline_t *cmdline, const char *keyval) {
    int error = 0;
    char *key = NULL, *value = NULL;
    
    const char *pos_colon = strchr(keyval, ':');
    if (pos_colon != NULL) {
        // Colon found => ``key: value`` format
        value = pos_colon + 1;
        while (*value == ' ')
            ++value;
        if (*(value)) {
            key = strndup(keyval, pos_colon - keyval);
            value = strdup(value);
        }
        else
            error = 1;
    }
    else {
        // No colon => ``key;`` format
        const char *pos_semicolon = strchr(keyval, ';');
        if (pos_semicolon != NULL) {
            key = strndup(keyval, pos_semicolon - keyval);
            value = strdup("");
        }
        else
            error = 1;
    }
    
    if (!error) {
        str_lowercase(key);
        http_header_code_t code = http_header_code(key);
        fprintf(stderr, "[debug] header=``%s``(code=%d):``%s``\n", key, code, value);
        http_header_t *header = malloc(sizeof(http_header_t));
        header->key.k_code = code;
        if (code != HTTP_HDR_OTHER) {
            header->key.k_str = NULL;
            free(key);
        }
        else
            header->key.k_str = key;
        header->value = value;
        http_header_set(cmdline->headers, header);
    }
    else
        fprintf(stderr, gk_err_cmdline_header_fmt, keyval);
    
    return error;
}

int parse_cmdline(int argc, char **argv, cmdline_t *cmdline) {
    init_cmdline_t(cmdline);
    int optchar;
    int error = 0;
    while ((optchar = getopt(argc, argv, gk_optstring)) != -1 && error == 0) {
        switch (optchar) {
            case 'i':
                cmdline->include_response_headers = 1;
                // fprintf(stderr, "include_response_headers=1\n");
                break;
            case 'O':
                cmdline->same_name_outfile = 1;
                // fprintf(stderr, "same_name_outfile=1\n");
                break;
            case 'X':
                error = cmdline_set_method(cmdline, optarg);
                break;
            case 'H':
                error = cmdline_add_header(cmdline, optarg);
                break;
            case 'd':
                free(cmdline->body);
                cmdline->body = strdup(optarg);
                // fprintf(stderr, "body=``%s``\n", cmdline->body);
                break;
            case 'o':
                free(cmdline->dst_file);
                cmdline->dst_file = strdup(optarg);
                // fprintf(stderr, "dst_file=``%s``\n", cmdline->dst_file);
                break;
            case ':':
            case '?':
            default:
                error = 1;
                break;
        }
    }
    if (optchar == -1 && error == 0) {
        error = 1;
        if (optind + 1 == argc) {
            cmdline->url = strdup(argv[optind]);
            error = 0;
        }
        else if (optind + 1 > argc)
            fprintf(stderr, gk_err_cmdline_url_missing);
        else
            fprintf(stderr, gk_err_cmdline_extra_param_fmt, argv[optind + 1]);
    }
    if (error) {
        print_help(argv[0]);
        fflush(stderr);
    }
    return error;
}

void free_cmdline(cmdline_t *cmdline) {
    free(cmdline->method);
    list_free(cmdline->headers, http_header_t_free);
    free(cmdline->body);
    free(cmdline->dst_file);
    free(cmdline->url);
}
