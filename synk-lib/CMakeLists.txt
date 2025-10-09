cmake_minimum_required(VERSION 3.10)
project(lab1_sync_primitives VERSION 1.0 LANGUAGES C)

# Настройка стандарта C
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Политики CMake
if(POLICY CMP0077)
    cmake_policy(SET CMP0077 NEW)
endif()

# Добавляем статическую библиотеку
add_subdirectory(lib)

# Добавляем пример
add_subdirectory(src)

# Добавляем тесты
add_subdirectory(test)