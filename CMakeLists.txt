cmake_minimum_required(VERSION 3.0)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_STANDARD 20)

project(MazeGenerator VERSION 0.1.0)

add_executable(MazeGenerator 
    src/main.cpp
    src/Graph.cpp
    src/Graph.h
    src/DisjointSets.cpp
    src/DisjointSets.h
    src/Timer.cpp
    src/Timer.h
    src/Maze.cpp
    src/Maze.h
    src/stb_image.h
)

find_package(glfw3 3.3 REQUIRED)
target_link_libraries(MazeGenerator glfw)
find_package(OpenGL REQUIRED)
target_link_libraries(MazeGenerator OpenGL::GL)