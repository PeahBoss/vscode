# Находим cmocka
find_package(PkgConfig REQUIRED)
pkg_check_modules(CMOCKA REQUIRED cmocka)

# Создаем исполняемый файл тестов
add_executable(run_tests
    test_sync.c
)

# Линкуем (передача данных) с нашей библиотекой и cmocka
target_link_libraries(run_tests
    PRIVATE 
        sync_primitives
        pthread
        ${CMOCKA_LIBRARIES}
)

# Добавляем include директории
target_include_directories(run_tests
    PRIVATE 
        ${CMAKE_SOURCE_DIR}/lib/include
        ${CMOCKA_INCLUDE_DIRS}
)

# Устанавливаем свойства
set_target_properties(run_tests PROPERTIES
    C_STANDARD 11
)

# Добавляем цель тестирования
enable_testing()
add_test(NAME sync_primitives_tests COMMAND run_tests)