cmake_minimum_required(VERSION 3.17)
project(untitled)

set(CMAKE_CXX_STANDARD 17)

if (WIN32)
    add_compile_options(/wd4819)#忽略保存为unicode警告
endif ()

include(CMakeLists.txt.glfw)
include(CMakeLists.txt.FreeType)

#拷贝depends目录
file(COPY "../../template/depends/glfw-3.3-3.4/" DESTINATION "../depends/glfw-3.3-3.4/")
file(COPY "../../template/depends/glm/" DESTINATION "../depends/glm/")
file(COPY "../../template/depends/timetool/" DESTINATION "../depends/timetool/")
file(COPY "../../template/depends/rapidxml/" DESTINATION "../depends/rapidxml/")
file(COPY "../../template/depends/stb/" DESTINATION "../depends/stb/")
file(COPY "../../template/depends/spdlog/include/spdlog/" DESTINATION "../depends/spdlog/")
file(COPY "../../template/depends/rttr-0.9.6/src/rttr/" DESTINATION "../depends/rttr/")

#拷贝data文件
file(COPY "../../template/data/images/diffuse_fishsoup_pot.cpt" DESTINATION "../data/images/")
file(COPY "../../template/data/images/urban_128.cpt" DESTINATION "../data/images/")
file(COPY "../../template/data/images/mod_bag.cpt" DESTINATION "../data/images/")
file(COPY "../../template/data/model/fishsoup_pot.mesh" DESTINATION "../data/model/")
file(COPY "../../template/data/shader/" DESTINATION "../data/shader/")
file(COPY "../../template/data/material/fishsoup_pot.mat" DESTINATION "../data/material/")
file(COPY "../../template/data/material/quad_draw_font_color.mat" DESTINATION "../data/material/")
file(COPY "../../template/data/material/ui.mat" DESTINATION "../data/material/")
file(COPY "../../template/data/font/" DESTINATION "../data/font/")

#头文件目录
include_directories("depends")
include_directories("source")

file(GLOB_RECURSE rttr_cpp "depends/rttr/*.cpp")
file(GLOB_RECURSE engine_cpp source/*.cpp)
file(GLOB_RECURSE example_cpp example/*.cpp)

add_executable(untitled ${glfw_sources} ${rttr_cpp} ${freetype_src} ${engine_cpp} ${example_cpp})


