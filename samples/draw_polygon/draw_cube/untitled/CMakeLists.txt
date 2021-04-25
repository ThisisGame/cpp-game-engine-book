cmake_minimum_required(VERSION 3.17)
project(untitled)

set(CMAKE_CXX_STANDARD 17)


include_directories("depends/glfw-3.3.3.bin.WIN64/include")
include_directories("depends/glfw-3.3.3.bin.WIN64/deps")
include_directories("depends")

link_directories("depends/glfw-3.3.3.bin.WIN64/lib-static-ucrt")

link_libraries(opengl32 glfw3dll)

add_executable(untitled main.cpp depends/glfw-3.3.3.bin.WIN64/deps/glad_gl.c VertexData.h ShaderSource.h)