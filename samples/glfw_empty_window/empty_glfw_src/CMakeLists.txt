cmake_minimum_required(VERSION 3.17)
project(untitled)

set(CMAKE_CXX_STANDARD 17)

#拷贝depends目录
file(COPY "../../template/depends/glfw-3.3-3.4/" DESTINATION "../depends/glfw-3.3-3.4/")

include(CMakeLists.txt.glfw)

#头文件目录
include_directories("depends/glfw-3.3-3.4/include")

#链接文件目录
#link_directories("depends/glfw-3.3-3.4/lib-static-ucrt")



#链接库
#link_libraries(opengl32 glfw3dll)

#link_libraries(opengl32)

link_libraries(${glfw_LIBRARIES})

message(${glfw_SOURCES})

#源代码文件
add_executable(untitled main.cpp ${glfw_SOURCES} ${TINYCTHREAD} ${GLAD_GL})