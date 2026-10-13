
file(GLOB_RECURSE VECDB_CORE_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)

# Exclude standalone entry points from the shared library.
list(FILTER VECDB_CORE_SOURCES
    EXCLUDE REGEX "(main|hnsw_main)\\.cpp$"
)

add_library(vecdb_core ${VECDB_CORE_SOURCES})

target_include_directories(vecdb_core
    PUBLIC "${PROJECT_SOURCE_DIR}/include"
)

target_link_libraries(vecdb_core PUBLIC vecdb_options)

# Existing exact-search CLI
add_executable(novadb_cli "${PROJECT_SOURCE_DIR}/app/cli.cpp")
target_link_libraries(novadb_cli PRIVATE vecdb_core vecdb_options)

# Separate HNSW CLI
add_executable(novadb_hnsw "${PROJECT_SOURCE_DIR}/app/hnsw_cli.cpp")
target_link_libraries(novadb_hnsw PRIVATE vecdb_core vecdb_options)

# Keep executable paths compatible with api/service.py.
set_target_properties(novadb_cli novadb_hnsw PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${PROJECT_SOURCE_DIR}"
)
