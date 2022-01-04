cmake_minimum_required(VERSION 3.17)
project(hunter)

set(CMAKE_CXX_STANDARD 17)

add_definitions(-D GLM_SWIZZLE)#GLM开启重排，随意转换。 vec2 v2 = v4.xz();

if (MSVC)
    add_compile_options(/bigobj)
    add_compile_options(/wd4819)#忽略保存为unicode警告
endif ()


include(CMakeLists.txt.glfw)
include(CMakeLists.txt.FreeType)
include(CMakeLists.txt.Wwise)
include(CMakeLists.txt.easy_profile_core)
include(CMakeLists.txt.lua)

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
file(COPY "../../template/data/images/urban.cpt" DESTINATION "../data/images/")
file(COPY "../../template/data/material/cube.mat" DESTINATION "../data/material/")
file(GLOB unlit_shaders "../../template/data/shader/unlit.*")
file(COPY ${unlit_shaders} DESTINATION "../data/shader/")
file(COPY "../../template/data/animation/load_skeleton_animation/export.skeleton_anim" DESTINATION "../data/animation/")
file(COPY "../../template/data/model/load_weight_file/export.mesh" DESTINATION "../data/model/")
file(COPY "../../template/data/model/load_weight_file/export.weight" DESTINATION "../data/model/")
file(COPY "../../template/data/audio/integrate_wwise/windows/" DESTINATION "../data/audio/")

#头文件目录
include_directories("depends")
include_directories("source")

file(GLOB_RECURSE engine_cpp source/*.cpp)
file(GLOB_RECURSE rttr_cpp "depends/rttr/*.cpp")
file(GLOB_RECURSE example_cpp example/*.cpp)

add_executable(hunter ${glfw_sources} ${rttr_cpp} ${freetype_src} ${easy_profiler_core_source} ${lua_src} ${engine_cpp} ${example_cpp})