#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "common.h"
#include "url.h"

#define URL_INFO_FIELD_CNT 6 // 5 fields plus one non-used group

static const char *gk_url_split_pattern =
    "^(([a-z]*?):\\/\\/)?([^\\/?]+)([^?]*)\\??(.*)";

char *url_split(const char *url, url_info_t *url_info) {
    regex_t reg;
    regmatch_t matches[URL_INFO_FIELD_CNT];
    /*
    fprintf(
        stderr, "[url.c] [url_split] regex=``%s`` arg=``%s``\n",
        gk_url_split_pattern, url
    );
    */
    int comp_result = regcomp(&reg, gk_url_split_pattern, REG_ICASE | REG_EXTENDED);
    if (comp_result) {
        return "regcomp failed";
    }
    int match_result = regexec(&reg, url, URL_INFO_FIELD_CNT, matches, 0);

    char *error = NULL;
    if (match_result == 0) {
        /*
        for (int i = 0; i < URL_INFO_FIELD_CNT; ++i) {
            fprintf(
                stderr, "match[%d] = ``%s``\n",
                &matches[i]
            );
        }
        */
        url_info->proto = str_dup_by_match(url, &matches[2]);
        url_info->host = str_dup_by_match(url, &matches[3]);
        url_info->path = str_dup_by_match(url, &matches[4]);
        url_info->form_data = str_dup_by_match(url, &matches[5]);
    }
    else {
        error = malloc(0x100);
        regerror(match_result, &reg, error, 0x100);
    }
    
    regfree(&reg);
    return error;
}
