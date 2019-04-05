project(os_kidshell)

set(CMAKE_EXPORT_COMPILE_COMMANDS 1) # For ccls

add_executable(toy_shell main.cpp Runner.cpp Command.cpp)
