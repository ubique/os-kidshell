project(Interpreter)

set(CMAKE_CXX_STANDARD 14)

add_executable(Interpreter Program.h Program.cpp Enviroment.h Enviroment.cpp Interpreter.cpp)