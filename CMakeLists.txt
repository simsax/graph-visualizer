cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

project(graph-visualizer C)

set(CMAKE_BUILD_TYPE Debug)

set(CMAKE_C_STANDARD 11)

add_subdirectory(extern/sdl2_gfx)

add_executable(${PROJECT_NAME} main.c)

target_link_libraries(${PROJECT_NAME} sdl2_gfx m)

set(GCC_COMPILE_OPTIONS "-Wall;-Wextra;-pedantic;-std=c11")
set(GCC_DEBUG_OPTIONS "${GCC_COMPILE_OPTIONS};-g;-O0")
set(GCC_RELEASE_OPTIONS "${GCC_COMPILE_OPTIONS};-O3;-DNDEBUG")

target_compile_options(${PROJECT_NAME} PUBLIC "$<$<CONFIG:DEBUG>:${GCC_DEBUG_OPTIONS}>")
target_compile_options(${PROJECT_NAME} PUBLIC "$<$<CONFIG:RELEASE>:${GCC_RELEASE_OPTIONS}>")