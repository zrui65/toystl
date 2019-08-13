include_directories(..)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

add_executable(test_alloc test_alloc.cc) 
add_executable(test_vector test_vector.cc) 
add_executable(test_stl test_stl.cc) 
add_executable(test_string test_string.cc) 
add_executable(test_list test_list.cc)
add_executable(test_rbtree test_rbtree.cc)
add_executable(test_hashtable test_hashtable.cc)
add_executable(test_copy test_copy.cc)
add_executable(test_sort test_sort.cc)

