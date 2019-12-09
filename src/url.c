#include <regex.h>
#include <string.h>
#include <common.h>
#include "url.h"

#define URL_INFO_FIELD_CNT 4

static const char *gk_url_split_pattern =
    "^([a-z]*?):\\/\\/([^\\/]+)([^?]*)\\??(.*)";

int url_split(const char *url, url_info_t *url_info) {
    regex_t reg;
    regmatch_t matches[URL_INFO_FIELD_CNT];
    regcomp(&reg, gk_url_split_pattern, REG_ICASE);
    int match_result = regexec(&reg, url, URL_INFO_FIELD_CNT, matches, 0);
    regfree(&reg);

    if (match_result == 0) {
        url_info->proto = str_dup_by_match(url, &matches[0]);
        url_info->host = str_dup_by_match(url, &matches[1]);
        url_info->path = str_dup_by_match(url, &matches[2]);
        url_info->form_data = str_dup_by_match(url, &matches[3]);
    }
    return match_result;
}
