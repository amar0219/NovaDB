function(add_vecdb_test test_name)
    add_executable(${test_name} ${test_name}.cpp)
    target_link_libraries(${test_name} PRIVATE vecdb_core vecdb_options)
    add_test(NAME ${test_name} COMMAND ${test_name})
endfunction()

add_vecdb_test(test_heap)
add_vecdb_test(test_search)
add_vecdb_test(test_similarity)
