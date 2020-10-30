cmake_minimum_required(VERSION 3.10)
set(CMAKE_VERBOSE_MAKEFILE ON)

project(ncvis)

set(CMAKE_BUILD_TYPE Debug)
message(STATUS "Build type: " ${CMAKE_BUILD_TYPE})

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_CXX_FLAGS "-Wall -Wextra -ffast-math")
set(CMAKE_CXX_FLAGS_DEBUG "-g3")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 ")

add_subdirectory(src)
add_subdirectory(lib)