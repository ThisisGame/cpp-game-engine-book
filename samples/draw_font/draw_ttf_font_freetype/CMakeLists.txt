cmake_minimum_required(VERSION 3.17)
project(untitled)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXE_LINKER_FLAGS "-static")

#拷贝depends目录
file(COPY "../../template/depends/glfw-3.3.3.bin.WIN64/" DESTINATION "../depends/glfw-3.3.3.bin.WIN64/")
file(COPY "../../template/depends/glm/" DESTINATION "../depends/glm/")
file(COPY "../../template/depends/timetool/" DESTINATION "../depends/timetool/")
file(COPY "../../template/depends/rapidxml/" DESTINATION "../depends/rapidxml/")
file(COPY "../../template/depends/rttr/" DESTINATION "../depends/rttr/")
file(COPY "../../template/depends/stb/" DESTINATION "../depends/stb/")
file(COPY "../../template/depends/spdlog/include/spdlog/" DESTINATION "../depends/spdlog/")
file(COPY "../../template/depends/freetype-2.11.0/" DESTINATION "../depends/freetype/")

#拷贝data文件
file(COPY "../../template/data/images/diffuse_fishsoup_pot.cpt" DESTINATION "../data/images/")
file(COPY "../../template/data/images/urban_128.cpt" DESTINATION "../data/images/")
file(COPY "../../template/data/model/fishsoup_pot.mesh" DESTINATION "../data/model/")
file(COPY "../../template/data/shader/unlit.vs" DESTINATION "../data/shader/")
file(COPY "../../template/data/shader/unlit.fs" DESTINATION "../data/shader/")
file(COPY "../../template/data/material/fishsoup_pot.mat" DESTINATION "../data/material/")
file(COPY "../../template/data/material/quad_draw_font.mat" DESTINATION "../data/material/")
file(COPY "../../template/data/font/" DESTINATION "../data/font/")

#头文件目录
include_directories("depends/glfw-3.3.3.bin.WIN64/include")
include_directories("depends/glfw-3.3.3.bin.WIN64/deps")
include_directories("depends/rttr/include")
include_directories("depends")
include_directories("source")

#链接目录
link_directories("depends/glfw-3.3.3.bin.WIN64/lib-static-ucrt")
link_directories("depends/rttr/lib")

#链接lib文件
link_libraries(opengl32 glfw3dll rttr_core_d)

file(GLOB_RECURSE engine_cpp source/*.cpp)
add_executable(untitled depends/glfw-3.3.3.bin.WIN64/deps/glad_gl.c ${engine_cpp}
        example/main.cpp
        example/login_scene.cpp)


#拷贝dll到exe目录
file(COPY "depends/glfw-3.3.3.bin.WIN64/lib-static-ucrt/glfw3.dll" DESTINATION "./")


