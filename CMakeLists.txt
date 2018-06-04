cmake_minimum_required(VERSION 3.5)
project(talloc VERSION 1.4.0 LANGUAGES C)

set(HAVE_64_BIT 1)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)

enable_testing()
add_subdirectory(tests)

set(SOURCE_FILES src/talloc.c src/heap.c src/ptr_tools.c src/pool.c src/vector.c src/utils.c)
set(HEADER_FILES include/talloc/talloc.h include/talloc/talloc_config.h)

add_library(talloc SHARED ${SOURCE_FILES} ${HEADER_FILES})

target_include_directories(talloc PUBLIC include)

if (MSVC)
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} /Od")
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} /Ox")
else()
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O3 -DNDEBUG")
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g -Wall")
endif()

