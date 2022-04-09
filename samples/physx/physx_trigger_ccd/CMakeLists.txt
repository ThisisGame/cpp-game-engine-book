cmake_minimum_required(VERSION 3.17)
project(untitled)

set(CMAKE_CXX_STANDARD 17)

#~en is high-speed motion enabled.
#~zh 高速运动是否启用。
add_definitions(-D HIGH_SPEED)

if (MSVC)
    add_compile_options(/bigobj)
    add_compile_options(/wd4819)#忽略保存为unicode警告
endif ()


include(CMakeLists.txt.Physx)

#头文件目录
include_directories("depends")

file(GLOB_RECURSE example_cpp example/*.cpp)

add_executable(untitled ${example_cpp})
set_property(TARGET untitled PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")