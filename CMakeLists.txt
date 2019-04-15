cmake_minimum_required(VERSION 3.13)
project(os-kidshell)

set(CMAKE_CXX_STANDARD 11)

add_compile_definitions(LOCAL)

if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -pedantic -fsanitize=address,undefined -D_GLIBCXX_DEBUG")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2 -D_GLIBCXX_RELEASE")
endif()

add_executable(kidshell source.cpp)