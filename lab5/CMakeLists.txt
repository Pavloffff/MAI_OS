cmake_minimum_required(VERSION 3.8 FATAL_ERROR)
project(main LANGUAGES C)

set(BUILD_WITH_ASAN 1)

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

add_executable(main2 ./src/main1.c)
target_include_directories(main2 PRIVATE ./include)
target_link_libraries(main2 PRIVATE lib2 m)

add_executable(main ./src/main2.c)
target_include_directories(main PRIVATE ./include m)

if (${BUILD_WITH_ASAN})
    message("-- Adding sanitizers")
    target_compile_options(main PRIVATE -fsanitize=address)
    target_link_options(main PRIVATE -fsanitize=address)
    target_compile_options(main1 PRIVATE -fsanitize=address)
    target_link_options(main1 PRIVATE -fsanitize=address)
    target_compile_options(main2 PRIVATE -fsanitize=address)
    target_link_options(main2 PRIVATE -fsanitize=address)
endif()
