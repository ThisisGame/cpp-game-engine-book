cmake_minimum_required(VERSION 3.17)
project(untitled)

set(CMAKE_CXX_STANDARD 17)

#add_definitions(-D GLM_SWIZZLE)#GLM开启重排，随意转换。 vec2 v2 = v4.xz();

if (MSVC)
    add_compile_options(/bigobj)
    add_compile_options(/wd4819)#忽略保存为unicode警告
endif ()

#拷贝depends目录
file(COPY "../../template/depends/glm/" DESTINATION "../depends/glm/")
file(COPY "../../template/depends/glm_ext/" DESTINATION "../depends/glm/")
file(COPY "../../template/depends/spdlog/include/spdlog/" DESTINATION "../depends/spdlog/")
file(COPY "../../template/depends/assimp-5.0.1/win/" DESTINATION "../depends/assimp-5.0.1/")

#拷贝data文件
file(COPY "../../template/data/model/assimp_extra.fbx" DESTINATION "../data/model/")

#头文件目录
include_directories("depends")
include_directories("depends/assimp-5.0.1/include")
include_directories("source")


#链接目录
link_directories("depends/assimp-5.0.1/lib")

if(WIN32)
    #链接lib文件
    link_libraries(assimp-vc142-mtd)

    #拷贝dll到exe目录
    file(COPY "depends/assimp-5.0.1/bin/assimp-vc142-mtd.dll" DESTINATION "./")
else()

endif()

file(GLOB_RECURSE example_cpp example/*.cpp)

add_executable(untitled ${example_cpp})