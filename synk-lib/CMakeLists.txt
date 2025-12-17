cmake_minimum_required(VERSION 3.10)
project(lab1_sync_primitives VERSION 1.0 LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

if(POLICY CMP0077)
    cmake_policy(SET CMP0077 NEW)
endif()

add_subdirectory(lib)

add_subdirectory(src)

add_subdirectory(test)