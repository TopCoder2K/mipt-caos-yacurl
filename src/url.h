#pragma once

typedef struct url_info {
    char *proto;
    char *host;
    char *path;
    char *form_data;
} url_info_t;

// Upon success, will fill url_info and return zero.
// Otherwise, will return value received from regexec().
//
// `url_info' fields can be freed with free().
int url_split(const char *url, url_info_t *url_info);
