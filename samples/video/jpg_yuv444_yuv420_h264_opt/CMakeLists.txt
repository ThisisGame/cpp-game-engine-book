cmake_minimum_required(VERSION 3.17)
project(jpg_yuv444_yuv420_h264_opt)

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
file(COPY "../../template/depends/spdlog/include/spdlog/" DESTINATION "../depends/spdlog/")
file(COPY "../../template/depends/openh264/" DESTINATION "../depends/openh264/")

#拷贝data文件
file(COPY "../../template/data/images/fbi.jpg" DESTINATION "../data/images/")

#头文件目录
include_directories("depends")
include_directories("depends/openh264/codec/api/wels")

if(WIN32)
    #链接目录
    link_directories("depends/openh264/lib/win")
    #链接lib文件
    link_libraries(openh264 encoder common console_common decoder openh264_dll processing)
else()

endif()

add_executable(load_image ${glfw_sources} main.cpp texture2d.cpp)