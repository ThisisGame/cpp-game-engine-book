cmake_minimum_required(VERSION 3.17)
project(untitled)

set(CMAKE_CXX_STANDARD 17)

#add_definitions(-D GLM_SWIZZLE)#GLM开启重排，随意转换。 vec2 v2 = v4.xz();

if (MSVC)
    add_compile_options(/bigobj)
    add_compile_options(/wd4819)#忽略保存为unicode警告
endif ()


#下载 fbx_sdk_2020.2.1.7z
function(Download7Z)
    MESSAGE("Download fbx_sdk_2020.2.1.7z")
    file(DOWNLOAD "https://github.com/ThisisGame/makegameengineatnight/releases/download/fbx_sdk_2020.2.1_mac_win/fbx_sdk_2020.2.1.7z" "../../../template/depends/fbx_sdk_2020.2.1.7z" SHOW_PROGRESS)
endfunction()

#检测 fbx_sdk_2020.2.1.7z 是否存在，不存在则下载
if(EXISTS "../../../template/depends/fbx_sdk_2020.2.1.7z")
    MESSAGE("found fbx_sdk_2020.2.1.7z")
    file(SIZE "../../../template/depends/fbx_sdk_2020.2.1.7z" SIZE_7Z)
    MESSAGE("size of fbx_sdk_2020.2.1.7z: ${SIZE_7Z}")
    #检查大小是否匹配
    if(${SIZE_7Z} EQUAL 81207855)
        MESSAGE("fbx_sdk_2020.2.1.7z is valid")
    else()
        MESSAGE("fbx_sdk_2020.2.1.7z is invalid")
        Download7Z()
    endif()
else()
    Download7Z()
endif()



#解压 fbx_sdk_2020.2.1.7z
if(EXISTS "../../../template/depends/fbx_sdk_2020.2.1/")
    MESSAGE("found fbx_sdk_2020.2.1")
else()
    MESSAGE("not find fbx_sdk_2020.2.1,extract fbx_sdk_2020.2.1.7z")
    file(ARCHIVE_EXTRACT INPUT "../../../template/depends/fbx_sdk_2020.2.1.7z" DESTINATION "../../../template/depends/" VERBOSE)
endif()

#拷贝depends目录
file(COPY "../../template/depends/glm/" DESTINATION "../depends/glm/")
file(COPY "../../template/depends/glm_ext/" DESTINATION "../depends/glm/")
file(COPY "../../template/depends/spdlog/include/spdlog/" DESTINATION "../depends/spdlog/")
file(COPY "../../template/depends/fbx_sdk_2020.2.1/" DESTINATION "../depends/fbx_sdk/")

#拷贝data文件
file(COPY "../../template/data/model/fbx_extra.fbx" DESTINATION "../data/model/")

#头文件目录
include_directories("depends")
include_directories("depends/fbx_sdk/include")
include_directories("source")

if(WIN32)
    #链接目录
    link_directories("depends/fbx_sdk/win/lib/vs2019/x64/debug")
    #链接lib文件
    link_libraries(libfbxsdk-md libxml2-md zlib-md)
    #拷贝dll到exe目录
    file(COPY "depends/fbx_sdk/win/lib/vs2019/x64/debug/libfbxsdk.dll" DESTINATION "./")
else()
    #链接目录
    link_directories("depends/fbx_sdk/mac/lib/clang/debug")
    #链接lib文件
    link_libraries(fbxsdk)
    #拷贝dll到exe目录
    file(COPY "depends/fbx_sdk/mac/lib/clang/debug/libfbxsdk.dylib" DESTINATION "./")
endif()

file(GLOB_RECURSE example_cpp example/*.cpp example/Common/*.cxx)

add_executable(untitled ${example_cpp})