cmake_minimum_required(VERSION 3.8 FATAL_ERROR)
project(main LANGUAGES CXX)

add_executable(client ./src/client.cpp ./include/labtools.h)
target_link_libraries(client zmq)
target_compile_options(client PRIVATE -fsanitize=address -g)
target_link_options(client PRIVATE -fsanitize=address -g)

add_executable(server ./src/server.cpp ./include/labtools.h)
target_link_libraries(server zmq)
target_compile_options(server PRIVATE -fsanitize=address -g)
target_link_options(server PRIVATE -fsanitize=address -g)