cmake_minimum_required(VERSION 3.12)
project(os-kidshell-master)

set(CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address,undefined")

add_executable(shell shell.cpp)
