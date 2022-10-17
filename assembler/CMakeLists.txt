cmake_minimum_required(VERSION 3.10)

# set the project name
project(Assembler)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D _DEBUG -ggdb3 -std=c++17")

# add the executable
add_executable(Assembler ../config.h ../cmd.h assembler.h assembler.cpp ../mlibs/onegin/onegin.h ../mlibs/onegin/onegin.cpp main.cpp)