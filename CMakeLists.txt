cmake_minimum_required(VERSION 3.25)
project(mahou CXX)

set(CMAKE_CXX_STANDARD 17)

find_package(glm REQUIRED)
find_package(boost REQUIRED)
find_package(Eigen3 REQUIRED)
find_package(fmt REQUIRED)

add_subdirectory(src)
