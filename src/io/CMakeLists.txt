set (SOURCE_FILES_IO
        io.c
        net.c
)
add_library(mod_io OBJECT ${SOURCE_FILES_IO})
set (MODULES_ALL ${MODULES_ALL} "$<TARGET_OBJECTS:mod_io>" PARENT_SCOPE)
