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
file(COPY "../../template/depends/stb/" DESTINATION "../depends/stb/")
file(COPY "../../template/depends/timetool/" DESTINATION "../depends/timetool/")

#拷贝data文件
file(COPY "../../template/data/images/urban.jpg" DESTINATION "../data/images/")

include_directories("depends")

add_executable(load_image ${glfw_sources} main.cpp texture2d.cpp)