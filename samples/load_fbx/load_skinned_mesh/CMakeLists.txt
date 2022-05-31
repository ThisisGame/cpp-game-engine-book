cmake_minimum_required(VERSION 3.17)
project(untitled)

set(CMAKE_CXX_STANDARD 17)

add_definitions(-D GLM_SWIZZLE)#GLM开启重排，随意转换。 vec2 v2 = v4.xz();

add_definitions(-D USE_FMOD_STUDIO)#是否使用fmod studio，加载bank文件的形式。


if (MSVC)
    add_compile_options(/bigobj)
    add_compile_options(/wd4819)#忽略保存为unicode警告
endif ()


include(CMakeLists.txt.glfw)
include(CMakeLists.txt.FreeType)
include(CMakeLists.txt.fmod)
include(CMakeLists.txt.easy_profile_core)
include(CMakeLists.txt.lua)
include(CMakeLists.txt.luasocket)

#拷贝depends目录
file(COPY "../../template/depends/glfw-3.3-3.4/" DESTINATION "../depends/glfw-3.3-3.4/")
file(COPY "../../template/depends/glm/" DESTINATION "../depends/glm/")
file(COPY "../../template/depends/glm_ext/" DESTINATION "../depends/glm/")
file(COPY "../../template/depends/timetool/" DESTINATION "../depends/timetool/")
file(COPY "../../template/depends/rapidxml/" DESTINATION "../depends/rapidxml/")
file(COPY "../../template/depends/stb/" DESTINATION "../depends/stb/")
file(COPY "../../template/depends/spdlog/include/spdlog/" DESTINATION "../depends/spdlog/")
file(COPY "../../template/depends/rttr-0.9.6/src/rttr/" DESTINATION "../depends/rttr/")
file(COPY "../../template/depends/easy_profiler-2.1.0/easy_profiler_core/" DESTINATION "../depends/easy_profiler_core/")

#拷贝data文件
file(COPY "../../template/data/images/fbx_extra_jiulian.cpt" DESTINATION "../data/images/")
file(COPY "../../template/data/model/fbx_extra_jiulian.mesh" DESTINATION "../data/model/")
file(COPY "../../template/data/model/fbx_extra_jiulian.weight" DESTINATION "../data/model/")
file(COPY "../../template/data/animation/fbx_extra_bip001_bip001_take_001_baselayer.skeleton_anim" DESTINATION "../data/animation/")
file(COPY "../../template/data/shader/unlit.vs" DESTINATION "../data/shader/")
file(COPY "../../template/data/shader/unlit.fs" DESTINATION "../data/shader/")
file(COPY "../../template/data/material/fbx_extra_jiulian.mat" DESTINATION "../data/material/")

#头文件目录
include_directories("depends")
include_directories("source")

file(GLOB_RECURSE engine_cpp source/*.cpp)
file(GLOB_RECURSE rttr_cpp "depends/rttr/*.cpp")
file(GLOB_RECURSE example_cpp example/*.cpp)

add_executable(untitled ${glfw_sources} ${rttr_cpp} ${freetype_src} ${easy_profiler_core_source} ${lua_src} ${luasocket_c} ${engine_cpp} ${example_cpp})