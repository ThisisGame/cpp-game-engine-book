cmake_minimum_required(VERSION 3.17)
project(untitled)

set(CMAKE_CXX_STANDARD 17)

#拷贝depends目录
file(COPY "../../template/depends/glfw-3.3.3.bin.WIN64/" DESTINATION "../depends/glfw-3.3.3.bin.WIN64/")
file(COPY "../../template/depends/glm/" DESTINATION "../depends/glm/")
file(COPY "../../template/depends/timetool/" DESTINATION "../depends/timetool/")
file(COPY "../../template/depends/rapidxml/" DESTINATION "../depends/rapidxml/")
file(COPY "../../template/depends/rttr-0.9.6/src/rttr/" DESTINATION "../depends/rttr/")
file(COPY "../../template/depends/fmod/" DESTINATION "../depends/fmod/")

#拷贝data文件
file(COPY "../../template/data/images/diffuse_fishsoup_pot.cpt" DESTINATION "../data/images/")
file(COPY "../../template/data/images/urban_128.cpt" DESTINATION "../data/images/")
file(COPY "../../template/data/images/fmod_play_2d_sound_tips.cpt" DESTINATION "../data/images/")
file(COPY "../../template/data/model/fishsoup_pot.mesh" DESTINATION "../data/model/")
file(COPY "../../template/data/shader/unlit.vs" DESTINATION "../data/shader/")
file(COPY "../../template/data/shader/unlit.fs" DESTINATION "../data/shader/")
file(COPY "../../template/data/material/fishsoup_pot.mat" DESTINATION "../data/material/")
file(COPY "../../template/data/material/fmod_play_2d_sound_tips.mat" DESTINATION "../data/material/")
file(COPY "../../template/data/font/" DESTINATION "../data/font/")
file(COPY "../../template/data/audio/fmod_play_wav/" DESTINATION "../data/audio/")


#头文件目录
include_directories("depends/glfw-3.3.3.bin.WIN64/include")
include_directories("depends/glfw-3.3.3.bin.WIN64/deps")
include_directories("depends")
include_directories("source")

#链接目录
link_directories("depends/glfw-3.3.3.bin.WIN64/lib-static-ucrt")
link_directories("depends/fmod/api/core/lib/x64")

#链接lib文件
link_libraries(opengl32 glfw3dll fmod_vc)

file(GLOB_RECURSE engine_cpp source/*.cpp)
file(GLOB_RECURSE rttr_cpp "depends/rttr/*.cpp")
file(GLOB_RECURSE glfw_c depends/glfw-3.3.3.bin.WIN64/deps/glad_gl.c)
file(GLOB_RECURSE example_cpp example/*.cpp)

add_executable(untitled ${glfw_c} ${rttr_cpp} ${engine_cpp} ${example_cpp})


#拷贝dll到exe目录
file(COPY "depends/glfw-3.3.3.bin.WIN64/lib-static-ucrt/glfw3.dll" DESTINATION "./")

