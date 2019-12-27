set (SOURCE_FILES_HTTP
        http_header.c
        http_const.c
        http_methods.c
        http_response.c
        http_request.c
        url.c
)
add_library(mod_http OBJECT ${SOURCE_FILES_HTTP})
set (MODULES_ALL ${MODULES_ALL} "$<TARGET_OBJECTS:mod_http>" PARENT_SCOPE)
