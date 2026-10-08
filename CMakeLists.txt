cmake_minimum_required(VERSION 3.16)
project(Persistent_key_value_store C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

option(SANITIZE "Build with AddressSanitizer and UBSan" ON)

# The store itself: every .c file in src/
file(GLOB KV_SOURCES CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/*.c")

add_library(kvstore STATIC ${KV_SOURCES})
target_include_directories(kvstore PUBLIC "${CMAKE_SOURCE_DIR}/include")
target_compile_options(kvstore PUBLIC -Wall -Wextra -Wpedantic -g)

if(SANITIZE)
    target_compile_options(kvstore PUBLIC -fsanitize=address,undefined)
    target_link_options(kvstore PUBLIC -fsanitize=address,undefined)
endif()

# The terminal app
add_executable(kv main.c
        include/kv_basic.h)
target_link_libraries(kv PRIVATE kvstore)

# The random stress test (only built once stress.c exists)
if(EXISTS "${CMAKE_SOURCE_DIR}/stress.c")
    add_executable(stress stress.c)
    target_link_libraries(stress PRIVATE kvstore)
endif()