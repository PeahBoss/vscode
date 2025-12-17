add_executable(example
    example.c
)

target_link_libraries(example
    PRIVATE 
        sync_primitives
        pthread
)

target_include_directories(example
    PRIVATE 
        ${CMAKE_SOURCE_DIR}/lib/include
)

set_target_properties(example PROPERTIES
    C_STANDARD 11
    OUTPUT_NAME "run_example"
)