# Создаем статическую библиотеку
add_library(sync_primitives STATIC
    src/sync_primitives.c
)

# Устанавливаем заголовочные файлы
target_include_directories(sync_primitives
    PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Устанавливаем свойства для библиотеки
set_target_properties(sync_primitives PROPERTIES
    OUTPUT_NAME sync_primitives
    C_STANDARD 11
)