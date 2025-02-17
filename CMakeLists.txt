cmake_minimum_required(VERSION 3.5)

project(
	SGM_Specification
	LANGUAGES CXX
)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

option(BUILD_TEST_PROJ "Build SGM_Specification's example suite" ON)

add_subdirectory(lib)

if(BUILD_TEST_PROJ)
	add_subdirectory(example)
endif()