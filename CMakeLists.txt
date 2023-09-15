cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

project(graph-visualizer C)

set(CMAKE_BUILD_TYPE Release)

set(CMAKE_C_STANDARD 11)

# check the submodules
if(NOT EXISTS "${PROJECT_SOURCE_DIR}/extern/sdl2_ttf/CMakeLists.txt")
    message(FATAL_ERROR "The sdl2_ttf submodule was not downloaded! GIT_SUBMODULE was turned off or failed. Please update submodules and try again.")
endif()

add_executable(${PROJECT_NAME}
    src/main.c
    src/text.c
)

add_subdirectory(extern/sdl2_gfx)
add_subdirectory(extern/sdl2_ttf)

target_include_directories(
        ${PROJECT_NAME}
        PUBLIC
        ${PROJECT_SOURCE_DIR}/extern/sdl2_gfx
        ${PROJECT_SOURCE_DIR}/extern/sdl2_ttf
)

target_link_libraries(${PROJECT_NAME} SDL2_gfx SDL2_ttf m)

set(GCC_COMPILE_OPTIONS "-Wall;-Wextra;-pedantic;-std=c11")
set(GCC_DEBUG_OPTIONS "${GCC_COMPILE_OPTIONS};-g;-O0")
set(GCC_RELEASE_OPTIONS "${GCC_COMPILE_OPTIONS};-O3;-DNDEBUG")

target_compile_options(${PROJECT_NAME} PUBLIC "$<$<CONFIG:DEBUG>:${GCC_DEBUG_OPTIONS}>")
target_compile_options(${PROJECT_NAME} PUBLIC "$<$<CONFIG:RELEASE>:${GCC_RELEASE_OPTIONS}>")