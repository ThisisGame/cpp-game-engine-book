cmake_minimum_required(VERSION 3.17)
project(untitled)

set(CMAKE_CXX_STANDARD 17)

#拷贝data文件
file(COPY "../../../template/data/images/diffuse_fishsoup_pot.cpt" DESTINATION "../data/images/")
file(COPY "../../../template/data/images/urban_128.cpt" DESTINATION "../data/images/")
file(COPY "../../../template/data/model/fishsoup_pot.mesh" DESTINATION "../data/model/")
file(COPY "../../../template/data/shader/unlit.vs" DESTINATION "../data/shader/")
file(COPY "../../../template/data/shader/unlit.fs" DESTINATION "../data/shader/")
file(COPY "../../../template/data/material/fishsoup_pot.mat" DESTINATION "../data/material/")

#头文件目录
include_directories("../../engine/depends/glfw-3.3.3.bin.WIN64/include")
include_directories("../../engine/depends/glfw-3.3.3.bin.WIN64/deps")
include_directories("../../engine/depends/rttr/include")
include_directories("../../engine/depends")
include_directories("source")

#链接目录
link_directories("../../engine/depends/glfw-3.3.3.bin.WIN64/lib-static-ucrt")
link_directories("../../engine/depends/rttr/lib")
link_directories("../../engine/cmake-build-debug")

#链接lib文件
link_libraries(opengl32 glfw3dll librttr_core_d engine)

add_executable(untitled source/main.cpp)


#拷贝dll到exe目录
file(COPY "../../engine/depends/glfw-3.3.3.bin.WIN64/lib-static-ucrt/glfw3.dll" DESTINATION "./")
file(COPY "../../engine/depends/rttr/dll/librttr_core_d.dll" DESTINATION "./")
