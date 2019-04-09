cmake_minimum_required(VERSION 2.9)

project(os_kidshell)

set(CMAKE_EXPORT_COMPILE_COMMANDS 1) # For ccls

include_directories(inc)

add_executable(toy_shell src/main.cpp src/Environment.cpp src/ExecArgumentsHolder.cpp src/Command.cpp)
