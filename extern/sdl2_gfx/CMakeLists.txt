cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

project(sdl2_gfx)

set(CMAKE_BUILD_TYPE Release)

find_package(SDL2 REQUIRED)

include_directories(${SDL2_INCLUDE_DIRS})

add_library(${PROJECT_NAME} STATIC
 SDL2_framerate.c
 SDL2_gfxPrimitives.c
 SDL2_imageFilter.c
 SDL2_rotozoom.c)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES})

set(GCC_DEBUG_OPTIONS "-g;-O0")
set(GCC_RELEASE_OPTIONS "-O3;-DNDEBUG")

target_compile_options(${PROJECT_NAME} PRIVATE "$<$<CONFIG:DEBUG>:${GCC_DEBUG_OPTIONS}>")
target_compile_options(${PROJECT_NAME} PRIVATE "$<$<CONFIG:RELEASE>:${GCC_RELEASE_OPTIONS}>")