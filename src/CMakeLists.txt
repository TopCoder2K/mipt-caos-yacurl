add_subdirectory(app)
add_subdirectory(common)
add_subdirectory(http)
add_subdirectory(io)

add_executable(yacurl main.c ${MODULES_ALL})
set (MODULES_ALL ${MODULES_ALL} PARENT_SCOPE)
