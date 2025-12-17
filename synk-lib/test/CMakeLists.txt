find_package(PkgConfig REQUIRED)
pkg_check_modules(CMOCKA REQUIRED cmocka)

add_executable(run_tests
    test_sync.c
)

target_link_libraries(run_tests
    PRIVATE 
        sync_primitives
        pthread
        ${CMOCKA_LIBRARIES}
)

target_include_directories(run_tests
    PRIVATE 
        ${CMAKE_SOURCE_DIR}/lib/include
        ${CMOCKA_INCLUDE_DIRS}
)

set_target_properties(run_tests PROPERTIES
    C_STANDARD 11
)

enable_testing()
add_test(NAME sync_primitives_tests COMMAND run_tests)