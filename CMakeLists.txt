cmake_minimum_required(VERSION 3.10)
project(OS)

set(CMAKE_CXX_STANDARD 11)

add_library(shell Logger.cpp Logger.h Executor.cpp Executor.h)

add_executable(main main.cpp)
target_link_libraries(main shell)

add_executable(test test.cpp)
target_link_libraries(test shell)
set_target_properties(shell test main PROPERTIES CXX_STANDARD 17)