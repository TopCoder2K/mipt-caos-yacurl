set (SOURCE_FILES_NET
        tests_net.c
)
add_executable(tests_net ${SOURCE_FILES_NET})
target_link_libraries(tests_net ${MODULES_ALL})
