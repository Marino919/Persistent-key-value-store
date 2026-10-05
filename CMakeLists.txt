cmake_minimum_required(VERSION 3.16)
project(kvstore C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# The store itself, shared by both programs
add_library(kvstore STATIC kvstore.c
        kvstore.h)
target_compile_options(kvstore PUBLIC
        -Wall -Wextra -Wpedantic -g -fsanitize=address,undefined)
target_link_options(kvstore PUBLIC
        -fsanitize=address,undefined)

# The terminal app
add_executable(kv main.c
        kvstore.h)
target_link_libraries(kv PRIVATE kvstore)

# The random stress test
add_executable(stress stress.c
        kvstore.h)
target_link_libraries(stress PRIVATE kvstore)