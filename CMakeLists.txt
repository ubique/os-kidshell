cmake_minimum_required(VERSION 3.10)

project(vshell)

if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS_RELEASE "-std=c++17 -O3")
    set(CMAKE_CXX_FLAGS_DEBUG "-std=c++17 -g -W -Wall")
else()
    message(STATUS "Use GNUCXX C++ compiler.")
endif()

include_directories(include src)

set(CMAKE_CXX_STANDARD 17)

set(EXECUTABLE_OUTPUT_PATH "${PROJECT_BINARY_DIR}/bin")

add_executable(vshell
               src/vshell.cpp
               include/vshell.h
               src/parser.cpp
               include/parser.h
               src/env_wrapper.cpp
               include/env_wrapper.h
               include/command.h)

install(TARGETS vshell
        RUNTIME DESTINATION bin)
