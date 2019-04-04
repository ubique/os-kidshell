cmake_minimum_required(VERSION 2.8)

project(kidshell)

set(CMAKE_CXX_STANDARD 14)

add_executable(kidshell main.cpp)
add_executable(kidshell_asan main.cpp)

add_compile_options(kidshell_asan PUBLIC -fsanitize=address)
add_link_options(kidshell_asan -lasan)
