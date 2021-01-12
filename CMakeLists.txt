cmake_minimum_required(VERSION 3.0.0)
project(NESEmulator VERSION 0.1.0)

include(CTest)
enable_testing()

# SDL 2
find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

# OpenGL
find_package(OpenGL REQUIRED)

add_library(imgui
    src/imgui/imgui_draw.cpp
    src/imgui/imgui_impl_sdl.cpp
    src/imgui/imgui_impl_opengl3.cpp
    src/imgui/imgui_widgets.cpp
    src/imgui/imgui.cpp
    src/imgui/imgui_tables.cpp
    src/imgui/imgui_demo.cpp
)

# Make SDL2 available to Imgui
target_include_directories(imgui PUBLIC ${SDL2_INCLUDE_DIRS})
# imgui/backends/ contains the SDL implementation
target_include_directories(imgui PUBLIC src/imgui/)

add_library("glad" "lib/glad/src/glad.c")
include_directories("lib/glad/include")

add_executable(NESEmulator
    src/main.cpp
    src/screen.cpp
    src/emulator.cpp
    src/nes.cpp
    src/input.cpp
    src/bus.cpp
    src/cpu.cpp
    src/debugger.cpp
)

target_link_libraries(NESEmulator
    "glad"
    ${OPENGL_gl_LIBRARY}
    ${SDL2_LIBRARIES}
    imgui
    ${CMAKE_DL_LIBS}
)

target_precompile_headers(NESEmulator PRIVATE 
    <iostream>
    <memory>
    <vector>
    <chrono>
    [["glad/glad.h"]]
    [["SDL2/SDL.h"]]
    [["imgui.h"]]
    [["imgui_impl_sdl.h"]]
    [["imgui_impl_opengl3.h"]]
)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)