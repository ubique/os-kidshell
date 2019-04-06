cmake_minimum_required(VERSION 3.1)
project(os_kidshell)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_GLIBCXX_DEBUG -fsanitize=address,undefined")
set(CMAKE_CXX_FLAGS_RELEASE "-O2")

add_executable(os_kidshell kidshell.cpp)
