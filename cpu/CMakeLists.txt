cmake_minimum_required(VERSION 3.10)

# set the project name
project(Cpu)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D _DEBUG -lSDL2 -ggdb3 -std=c++17 -I/SFML-2.5.1/include")

# add the executable
add_executable(Cpu ../mlibs/stack/stack.h ../mlibs/stack/stack.cpp ../mlibs/stack/errorHandler.cpp cpu.h cpu.cpp main.cpp)