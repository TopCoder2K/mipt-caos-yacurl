#include <stdio.h>
#include <string.h>
#include <stddef.h>

static const size_t http_methods_supported_cnt = 5;

static const char *http_methods_supported[5] = {
    "GET",
    "POST",
    "PUT",
    "DELETE",
    "HEAD"
};

int http_methods_is_supported(const char *method) {
    int match = 0;
    for (size_t i = 0; !match && (i < http_methods_supported_cnt); ++i) {
#ifdef DEBUG
        fprintf(
            stderr, "[http_methods_is_supported] compare=``%s``\n",
            http_methods_supported[i]
        );
#endif // DEBUG
        if (!strcmp(method, http_methods_supported[i]))
            match = 1;
    }
    return match;
}
