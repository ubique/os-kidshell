cmake_minimum_required(VERSION 3.02)
project(kidshell)

set(CMAKE_CXX_STANDARD 14)

add_executable(
        kidshell
        shell.cpp
        ShellHelper.cpp
        ShellHelper.h
        ParseUtils.cpp
        ParseUtils.h
)