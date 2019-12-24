set (SOURCE_FILES_HTTP
        tests_http.c
)
add_executable(tests_http ${SOURCE_FILES_HTTP})
target_link_libraries(tests_http ${MODULES_ALL})

set (SOURCE_FILES_REQUEST
        tests_request.c
)
add_executable(tests_request ${SOURCE_FILES_REQUEST})
target_link_libraries(tests_request ${MODULES_ALL})

set (SOURCE_FILES_RESPONSE
        tests_response.c
)
add_executable(tests_response ${SOURCE_FILES_RESPONSE})
target_link_libraries(tests_response ${MODULES_ALL})
