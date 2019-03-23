cmake_minimum_required(VERSION 2.8)

project(terminal)

set(CMAKE_CXX_STANDARD 17)

add_executable(${PROJECT_NAME} terminal.cpp)

find_package(Boost REQUIRED COMPONENTS system filesystem)
target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES})
