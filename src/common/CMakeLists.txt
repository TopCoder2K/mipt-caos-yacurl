set (SOURCE_FILES_COMMON
        common.c
)
add_library(mod_common OBJECT ${SOURCE_FILES_COMMON})
set (MODULES_ALL ${MODULES_ALL} "$<TARGET_OBJECTS:mod_common>" PARENT_SCOPE)
