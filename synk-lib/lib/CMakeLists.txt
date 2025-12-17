add_library(sync_primitives STATIC
    src/sync_primitives.c
)

target_include_directories(sync_primitives
    PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

set_target_properties(sync_primitives PROPERTIES
    OUTPUT_NAME sync_primitives
    C_STANDARD 11
)