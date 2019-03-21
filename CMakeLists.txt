cmake_minimum_required(VERSION 2.8)

project(task1)
set(CMAKE_CXX_FLAGS "-fsanitize=address,undefined")
# set(CMAKE_CXX_FLAGS "-O3")
add_executable(${PROJECT_NAME} "kidshell.cpp")
