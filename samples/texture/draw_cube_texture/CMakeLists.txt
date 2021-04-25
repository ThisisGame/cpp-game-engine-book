cmake_minimum_required(VERSION 3.17)
project(untitled)

set(CMAKE_CXX_STANDARD 17)

#拷贝depends目录
file(COPY "../../template/depends/glfw-3.3.3.bin.WIN64/" DESTINATION "../depends/glfw-3.3.3.bin.WIN64/")
file(COPY "../../template/depends/glm/" DESTINATION "../depends/glm/")
file(COPY "../../template/depends/stb/" DESTINATION "../depends/stb/")

#拷贝dll到exe目录
file(COPY "depends/glfw-3.3.3.bin.WIN64/lib-static-ucrt/glfw3.dll" DESTINATION "./")

#拷贝data文件
file(COPY "../../template/data/images/urban.jpg" DESTINATION "../data/images/")

include_directories("depends/glfw-3.3.3.bin.WIN64/include")
include_directories("depends/glfw-3.3.3.bin.WIN64/deps")
include_directories("depends")

link_directories("depends/glfw-3.3.3.bin.WIN64/lib-static-ucrt")

link_libraries(opengl32 glfw3dll)

add_executable(untitled main.cpp depends/glfw-3.3.3.bin.WIN64/deps/glad_gl.c vertex_data.h shader_source.h texture2d.h texture2d.cpp)

#拷贝dll到exe目录
file(COPY "depends/glfw-3.3.3.bin.WIN64/lib-static-ucrt/glfw3.dll" DESTINATION "./")