cmake_minimum_required(VERSION 3.13)
project(parallel_prog6)

set(CMAKE_CXX_STANDARD 17)

link_libraries(-lmpi)

add_executable(parallel_prog6 main.cpp)