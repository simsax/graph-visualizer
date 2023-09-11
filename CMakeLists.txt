cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

project(graph-visualizer C)

set(CMAKE_C_STANDARD 11)

add_executable(${PROJECT_NAME} main.c)

target_link_libraries(${PROJECT_NAME} m)