cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

project(main LANGUAGES CXX)
add_executable(child1 ./src/child1.cpp)
add_executable(child2 ./src/child2.cpp)
add_executable(main ./src/main.cpp)
