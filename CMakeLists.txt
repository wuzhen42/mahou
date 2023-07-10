cmake_minimum_required(VERSION 3.25)
project(mahou LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(TARGETS)

add_subdirectory(src)
