cmake_minimum_required(VERSION 3.17)
project(untitled)

set(CMAKE_CXX_STANDARD 17)

if (WIN32)
    add_compile_options(/wd4819)#忽略保存为unicode警告
endif ()

include(CMakeLists.txt.glfw)

#拷贝depends目录
file(COPY "../../template/depends/glfw-3.3-3.4/" DESTINATION "../depends/glfw-3.3-3.4/")
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
include_directories("depends")
include_directories("source")

file(GLOB_RECURSE engine_cpp source/*.cpp)
add_executable(untitled ${glfw_c} ${engine_cpp})