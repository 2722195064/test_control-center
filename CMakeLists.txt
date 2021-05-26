cmake_minimum_required(VERSION 3.7)

project(test-control-center)
find_package(DtkCMake REQUIRED)

#find_package(DtkCmake REQUIRED)

add_compile_definitions(CVERSION="${CVERSION}")

add_definitions(-DWINDOW_MODE)

add_subdirectory("src/frame")

#find(GLOB HEADERS "include/*")

set(INCLUDE_INSTALL_DIR include/dde-control-center)
install(DIRECTORY ${HEADERS} DESTINATION ${INCLUDE_INSTALL_DIR})
