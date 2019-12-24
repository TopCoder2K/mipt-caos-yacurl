set (SOURCE_FILES_CLI
        tests_cli.c
)
add_executable(tests_cli ${SOURCE_FILES_CLI})
target_link_libraries(tests_cli ${MODULES_ALL})
