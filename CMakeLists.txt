cmake_minimum_required(VERSION 3.10)
project(OS)

set(CMAKE_CXX_STANDARD 11)

add_library(shell Logger.cpp Logger.h Executor.cpp Executor.h)

add_executable(main main.cpp)
target_link_libraries(main shell)

add_executable(test test.cpp)
target_link_libraries(test shell)
#target_compile_options(test PUBLIC $<$<COMPILE_LANGUAGE:CXX>:-std=c++17 -static-libasan -fsanitize=address,undefined,leak>)