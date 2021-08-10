cmake_minimum_required(VERSION 3.17)
project(untitled)

set(CMAKE_CXX_STANDARD 17)
#set(CMAKE_EXE_LINKER_FLAGS "-static")
message("CMAKE_EXE_LINKER_FLAGS:" ${CMAKE_EXE_LINKER_FLAGS})
IF(CMAKE_EXE_LINKER_FLAGS STREQUAL "-static")
    set(RTTR_DIR "rttr")
    set(RTTR_LIB "rttr_core_d")
ELSE()
    set(RTTR_DIR "rttr_dll")
    set(RTTR_LIB "rttr_core_d.dll")
ENDIF()

#拷贝depends目录
file(COPY "../../template/depends/glfw-3.3.3.bin.WIN64/" DESTINATION "../depends/glfw-3.3.3.bin.WIN64/")
file(COPY "../../template/depends/glm/" DESTINATION "../depends/glm/")
file(COPY "../../template/depends/timetool/" DESTINATION "../depends/timetool/")
file(COPY "../../template/depends/rapidxml/" DESTINATION "../depends/rapidxml/")
file(COPY "../../template/depends/stb/" DESTINATION "../depends/stb/")
file(COPY "../../template/depends/spdlog/include/spdlog/" DESTINATION "../depends/spdlog/")
file(COPY "../../template/depends/fmod/" DESTINATION "../depends/fmod/")
file(COPY "../../template/depends/${RTTR_DIR}/" DESTINATION "../depends/${RTTR_DIR}/")


#拷贝data文件
file(COPY "../../template/data/audio/load_bank/" DESTINATION "../data/audio/")
file(COPY "../../template/data/images/listen.cpt" DESTINATION "../data/images/")
file(COPY "../../template/data/images/music.cpt" DESTINATION "../data/images/")
file(GLOB audio_source_3d_cube_materials "../../template/data/material/sphere_audio_source_3d_*")
file(COPY ${audio_source_3d_cube_materials} DESTINATION "../data/material/")
file(COPY "../../template/data/model/sphere.mesh" DESTINATION "../data/model/")
file(GLOB unlit_shaders "../../template/data/shader/unlit.*")
file(COPY ${unlit_shaders} DESTINATION "../data/shader/")

#头文件目录
include_directories("depends/glfw-3.3.3.bin.WIN64/include")
include_directories("depends/glfw-3.3.3.bin.WIN64/deps")
include_directories("depends/${RTTR_DIR}/include")
include_directories("depends")
include_directories("source")

#链接目录
link_directories("depends/glfw-3.3.3.bin.WIN64/lib-static-ucrt")
link_directories("depends/${RTTR_DIR}/lib")
link_directories("depends/fmod/api/core/lib/x64")

#链接lib文件
link_libraries(opengl32 glfw3dll ${RTTR_LIB} fmod_vc)

file(GLOB_RECURSE engine_cpp source/*.cpp)
file(GLOB_RECURSE fmod_cpp depends/fmod/api/*.cpp)
file(GLOB_RECURSE glfw_c depends/glfw-3.3.3.bin.WIN64/deps/glad_gl.c)

file(GLOB_RECURSE example_cpp example/*.cpp)
add_executable(untitled ${glfw_c} ${engine_cpp} ${example_cpp} source/audio/studio/audio_studio.cpp source/audio/studio/audio_studio.h source/audio/studio/audio_studio_event.cpp source/audio/studio/audio_studio_event.h)


#拷贝dll到exe目录
file(COPY "depends/glfw-3.3.3.bin.WIN64/lib-static-ucrt/glfw3.dll" DESTINATION "./")
file(COPY "depends/fmod/api/core/lib/x64/fmod.dll" DESTINATION "./")
IF(CMAKE_EXE_LINKER_FLAGS STREQUAL "")
    file(COPY "depends/${RTTR_DIR}/bin/librttr_core_d.dll" DESTINATION "./")
ENDIF()
