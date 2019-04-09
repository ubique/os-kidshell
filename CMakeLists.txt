cmake_minimum_required(VERSION 3.10)
project(os_kidshell)

set(CMAKE_CXX_STANDARD 17)

add_executable(os_kidshell main.cpp KidShell.cpp KidShell.h EnvironmentManager.cpp EnvironmentManager.h)
