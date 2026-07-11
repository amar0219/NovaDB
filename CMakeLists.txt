cmake_minimum_required(VERSION 3.20)
project(vecdb LANGUAGES CXX)

# ---------------------------------------------------------------------------
# Language standard
# ---------------------------------------------------------------------------
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# ---------------------------------------------------------------------------
# Global compiler options
# ---------------------------------------------------------------------------
add_library(vecdb_options INTERFACE)

target_compile_options(vecdb_options INTERFACE
    -Wall -Wextra -Wpedantic -Wshadow -Wconversion
    $<$<CONFIG:Release>:-O3 -march=native -DNDEBUG>
    $<$<CONFIG:Debug>:-O0 -g -fsanitize=address,undefined>
)
target_link_options(vecdb_options INTERFACE
    $<$<CONFIG:Debug>:-fsanitize=address,undefined>
)

option(VECDB_BUILD_TESTS "Build unit tests" ON)
option(VECDB_BUILD_BENCHMARKS "Build benchmark suite" ON)
option(VECDB_ENABLE_AVX512 "Enable AVX-512 kernels (falls back to AVX2 at runtime if unsupported)" OFF)

if(VECDB_ENABLE_AVX512)
    target_compile_definitions(vecdb_options INTERFACE VECDB_ENABLE_AVX512=1)
endif()

# ---------------------------------------------------------------------------
# Core library (storage, index, query, similarity, threading, wal, config, logging)
# ---------------------------------------------------------------------------
add_subdirectory(src)

# ---------------------------------------------------------------------------
# Tests and benchmarks
# ---------------------------------------------------------------------------
if(VECDB_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

if(VECDB_BUILD_BENCHMARKS)
    add_subdirectory(benchmarks)
endif()
