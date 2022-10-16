cmake_minimum_required(VERSION 3.10)

# set the project name
project(Disasm)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D _DEBUG -ggdb3 -std=c++17")

# add the executable
add_executable(Disasm disassembler.h disassembler.cpp main.cpp)