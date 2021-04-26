cmake_minimum_required(VERSION 3.7)

project(test-control-center)
find_package(DtkCMake REQUIRED)

#find_package(DtkCmake REQUIRED)

add_compile_definitions(CVERSION="${CVERSION}")

add_subdirectory("src/frame")

#find(GLOB HEADERS "include/*")
