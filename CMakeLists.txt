cmake_minimum_required(VERSION 2.8)

project(kidshell)

set(CMAKE_CXX_FLAGS_DEBUG "-fsanitize=address,undefined")
set(CMAKE_CXX_FLAGS_RELEASE "-O2")

set(CMAKE_CXX_STANDARD 17)

add_executable(${PROJECT_NAME} "kidshell.cpp")
