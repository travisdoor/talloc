cmake_minimum_required(VERSION 3.7)
enable_testing()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR})
add_executable(talloc_test talloc_test.c)

# Find check 
find_package(check REQUIRED)

target_include_directories(talloc_test PRIVATE ${CHECK_INCLUDE_DIRS})
target_link_libraries(talloc_test ${CHECK_LIBRARIES} talloc)

add_test(talloc_test ${CMAKE_CURRENT_BINARY_DIR}/talloc_test)
