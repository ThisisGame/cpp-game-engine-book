cmake_minimum_required(VERSION 3.17)
project(jpg_Motion-JPEG)

set(CMAKE_CXX_STANDARD 17)

if (WIN32)
    add_compile_options(/wd4819)#忽略保存为unicode警告
endif ()

include(CMakeLists.txt.glfw)

#拷贝depends目录
file(COPY "../../template/depends/stb/" DESTINATION "../depends/stb/")
file(COPY "../../template/depends/timetool/" DESTINATION "../depends/timetool/")
file(COPY "../../template/depends/spdlog/include/spdlog/" DESTINATION "../depends/spdlog/")

#拷贝data文件
file(COPY "../../template/data/images/fbi.jpg" DESTINATION "../data/images/")

#头文件目录
include_directories("depends")
include_directories("depends/openh264/codec/api/wels")
include_directories("C:/Users/captainchen/Downloads/opencv/build/include")

if(WIN32)
    #链接目录
    link_directories("depends/openh264/lib/win")
    link_directories("C:/Users/captainchen/Downloads/opencv/build/x64/vc16/lib")
    #链接lib文件
    link_libraries(opencv_world480d)
else()

endif()

add_executable(load_image main.cpp)