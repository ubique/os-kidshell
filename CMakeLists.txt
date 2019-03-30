cmake_minimum_required(VERSION 3.1)

set(CMAKE_CXX_STANDARD 11)

project(kidshell)

add_executable(kidshell kidshell.cpp
                        environment.cpp
                        environment.h
                        programconditions.cpp
                        programconditions.h)

install(TARGETS kidshell
        RUNTIME DESTINATION bin)
