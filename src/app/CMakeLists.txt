set (SOURCE_FILES_APP
        cli.c
)
add_library(mod_app OBJECT ${SOURCE_FILES_APP})
set (MODULES_ALL ${MODULES_ALL} "$<TARGET_OBJECTS:mod_app>" PARENT_SCOPE)
