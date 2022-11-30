cmake_minimum_required(VERSION 3.8 FATAL_ERROR)
project(main LANGUAGES C)

set(CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE}
    FORCE)
set(CMAKE_C_FLAGS_ASAN
    "-fsanitize=address -fno-optimize-sibling-calls -fsanitize-address-use-after-scope -fno-omit-frame-pointer -g -O1"
    FORCE)

add_library(
        lib1 SHARED
        ./include/lib.h
        ./src/lib1.c
)
add_library(
        lib2 SHARED
        ./include/lib.h
        ./src/lib2.c
)

add_executable(main1 ./src/main1.c)
target_include_directories(main1 PRIVATE ./include)
target_link_libraries(main1 PRIVATE lib1 m)

add_executable(main2 ./src/main2.c)
target_include_directories(main2 PRIVATE ./include)
target_link_libraries(main2 PRIVATE lib2 m)

add_executable(main ./src/main2.c)
target_include_directories(main PRIVATE ./include m)
