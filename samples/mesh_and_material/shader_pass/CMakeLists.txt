cmake_minimum_required(VERSION 3.17)
project(untitled)

set(CMAKE_CXX_STANDARD 17)

#拷贝depends目录
file(COPY "../../template/depends/glfw-3.3.3.bin.WIN64/" DESTINATION "../depends/glfw-3.3.3.bin.WIN64/")
file(COPY "../../template/depends/glm/" DESTINATION "../depends/glm/")
file(COPY "../../template/depends/timetool/" DESTINATION "../depends/timetool/")
file(COPY "../../template/depends/rapidxml/" DESTINATION "../depends/rapidxml/")

#拷贝data文件
file(COPY "../../template/data/images/urban.cpt" DESTINATION "../data/images/")
file(COPY "../../template/data/images/urban_128.cpt" DESTINATION "../data/images/")
file(COPY "../../template/data/model/cube.mesh" DESTINATION "../data/model/")
file(COPY "../../template/data/shader/unlit.vs" DESTINATION "../data/shader/")
file(COPY "../../template/data/shader/unlit.fs" DESTINATION "../data/shader/")
file(COPY "../../template/data/material/cube.mat" DESTINATION "../data/material/")

#头文件目录
include_directories("depends/glfw-3.3.3.bin.WIN64/include")
include_directories("depends/glfw-3.3.3.bin.WIN64/deps")
include_directories("depends")

#链接目录
link_directories("depends/glfw-3.3.3.bin.WIN64/lib-static-ucrt")

#链接lib文件
link_libraries(opengl32 glfw3dll)

add_executable(untitled source/main.cpp depends/glfw-3.3.3.bin.WIN64/deps/glad_gl.c source/renderer/texture2d.h source/renderer/texture2d.cpp source/renderer/mesh_filter.cpp source/renderer/mesh_filter.h source/renderer/shader.cpp source/renderer/shader.h source/renderer/material.cpp source/renderer/material.h source/renderer/technique.cpp source/renderer/technique.h source/renderer/pass.cpp source/renderer/pass.h source/utils/application.cpp source/utils/application.h)


#拷贝dll到exe目录
file(COPY "depends/glfw-3.3.3.bin.WIN64/lib-static-ucrt/glfw3.dll" DESTINATION "./")


