cmake_minimum_required(VERSION 3.8)

project("TestingFilev")

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_C_COMPILER "/usr/bin/gcc")
set(CMAKE_CXX_COMPILER "/usr/bin/g++")

find_package(Threads REQUIRED)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# сборка libnl
add_custom_target(configure
    COMMAND "${CMAKE_CURRENT_SOURCE_DIR}/Third-party/libnl-3.0/configure"
    WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/libnl"
)
add_custom_target(build
    COMMAND make
    WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/libnl"
    DEPENDS configure
)
add_custom_target(libnl3
    COMMAND make install
    WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/libnl"
    DEPENDS build
)
add_custom_target(default
    DEPENDS build
)

link_directories("${CMAKE_BINARY_DIR}/libnl")

include_directories(
    ${CMAKE_CURRENT_LIST_DIR}
    "${CMAKE_CURRENT_LIST_DIR}/Third-party/libnl-3.0/include"
)

add_compile_options(
    -Wall -Wextra -Wpedantic
    -Wfloat-conversion
    -Werror
    -Wno-unused-parameter
)

#libraries
add_subdirectory("Common/Actions")
add_subdirectory("Common/NlWrapper")
add_subdirectory("Common/SimpleLogger")
add_subdirectory("Third-party/inih")

#executable
add_subdirectory("Client")
add_subdirectory("Server")
