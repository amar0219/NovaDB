cmake_minimum_required(VERSION 3.20)
project(vecdb LANGUAGES CXX)

# Language standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Compiler options
add_library(vecdb_options INTERFACE)

if(MSVC)
    target_compile_options(vecdb_options INTERFACE
        /W4
        $<$<CONFIG:Release>:/O2>
        $<$<CONFIG:Debug>:/Od>
        $<$<CONFIG:Debug>:/Zi>
    )
else()
    target_compile_options(vecdb_options INTERFACE
        -Wall -Wextra -Wpedantic -Wshadow -Wconversion
        $<$<CONFIG:Release>:-O3>
        $<$<CONFIG:Release>:-march=native>
        $<$<CONFIG:Release>:-DNDEBUG>
        $<$<CONFIG:Debug>:-O0>
        $<$<CONFIG:Debug>:-g>
        $<$<CONFIG:Debug>:-fsanitize=address,undefined>
    )

    target_link_options(vecdb_options INTERFACE
        $<$<CONFIG:Debug>:-fsanitize=address,undefined>
    )
endif()

# Project options
option(VECDB_BUILD_TESTS "Build unit tests" ON)
option(VECDB_BUILD_BENCHMARKS "Build benchmark suite" ON)
option(VECDB_ENABLE_AVX512 "Enable AVX-512 kernels" OFF)

if(VECDB_ENABLE_AVX512)
    target_compile_definitions(vecdb_options INTERFACE
        VECDB_ENABLE_AVX512=1
    )
endif()

# Core library and executable
add_subdirectory(src)

# Tests
if(VECDB_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# Benchmarks
if(VECDB_BUILD_BENCHMARKS)
    add_subdirectory(benchmarks)
endif()