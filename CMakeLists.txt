cmake_minimum_required(VERSION 3.5)
project(mipt-caos-2019-yacurl)

set (CMAKE_C_STANDARD 11)
set (CMAKE_C_FLAGS "-O2 -Wall")
set (CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS} -g -DDEBUG")
include_directories("src")

add_subdirectory(src)
MESSAGE(MODULES_ALL ${MODULES_ALL})

add_subdirectory(tests)
