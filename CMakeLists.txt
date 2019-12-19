cmake_minimum_required(VERSION 3.5)
project(mipt-caos-2019-yacurl)

set (CMAKE_C_STANDARD 11)
set (CMAKE_C_FLAGS "-O2 -Wall")

set (SOURCE_FILES
        src/common.c
        src/http_header.c
        src/http_request.c
        src/url.c
)

add_executable(yacurl_tests_http ${SOURCE_FILES} tests/tests_http.c)
add_executable(yacurl_tests_request ${SOURCE_FILES} tests/tests_request.c)
