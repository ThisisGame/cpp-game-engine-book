cmake_minimum_required(VERSION 3.17)
project(untitled)

set(CMAKE_CXX_STANDARD 17)

#拷贝depends目录
file(COPY "../../template/depends/glfw-3.3.3.bin.WIN64/" DESTINATION "../depends/glfw-3.3.3.bin.WIN64/")
file(COPY "../../template/depends/glm/" DESTINATION "../depends/glm/")
file(COPY "../../template/depends/timetool/" DESTINATION "../depends/timetool/")

#拷贝data文件
file(COPY "../../template/data/images/urban.cpt" DESTINATION "../data/images/")
file(COPY "../../template/data/model/cube.mesh" DESTINATION "../data/model/")
file(COPY "../../template/data/shader/unlit.vs" DESTINATION "../data/shader/")
file(COPY "../../template/data/shader/unlit.fs" DESTINATION "../data/shader/")

#头文件目录
include_directories("depends/glfw-3.3.3.bin.WIN64/include")
include_directories("depends/glfw-3.3.3.bin.WIN64/deps")
include_directories("depends")

#链接目录
link_directories("depends/glfw-3.3.3.bin.WIN64/lib-static-ucrt")

#链接lib文件
link_libraries(opengl32 glfw3dll)

add_executable(untitled main.cpp depends/glfw-3.3.3.bin.WIN64/deps/glad_gl.c texture2d.h texture2d.cpp renderer/mesh_filter.cpp renderer/mesh_filter.h renderer/shader.cpp renderer/shader.h)


#拷贝dll到exe目录
file(COPY "depends/glfw-3.3.3.bin.WIN64/lib-static-ucrt/glfw3.dll" DESTINATION "./")


