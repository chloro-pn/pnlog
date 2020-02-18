cmake_minimum_required(VERSION 3.1)

project(pnlog CXX C)

enable_testing()

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE "Release")
endif()

add_subdirectory("${PROJECT_SOURCE_DIR}/src")

add_subdirectory("${PROJECT_SOURCE_DIR}/test")

add_subdirectory("${PROJECT_SOURCE_DIR}/examples")
