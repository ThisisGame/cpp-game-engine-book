cmake_minimum_required(VERSION 3.17)
project(untitled)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXE_LINKER_FLAGS "-static")

#拷贝depends目录
file(COPY "../../template/depends/glfw-3.3.3.bin.WIN64/" DESTINATION "../depends/glfw-3.3.3.bin.WIN64/")
file(COPY "../../template/depends/glm/" DESTINATION "../depends/glm/")
file(COPY "../../template/depends/timetool/" DESTINATION "../depends/timetool/")
file(COPY "../../template/depends/rapidxml/" DESTINATION "../depends/rapidxml/")
file(COPY "../../template/depends/rttr/" DESTINATION "../depends/rttr/")
file(COPY "../../template/depends/stb/" DESTINATION "../depends/stb/")
file(COPY "../../template/depends/spdlog/include/spdlog/" DESTINATION "../depends/spdlog/")
file(COPY "../../template/depends/fmod/" DESTINATION "../depends/fmod/")

#拷贝data文件
file(COPY "../../template/data/audio/fmod_play_wav/" DESTINATION "../data/audio/")
file(COPY "../../template/data/images/audio/audio_source_3d/" DESTINATION "../data/images/")
file(GLOB audio_source_3d_cube_materials "../../template/data/material/audio_source_3d_cube_*")
file(COPY ${audio_source_3d_cube_materials} DESTINATION "../data/material/")
file(COPY "../../template/data/model/cube_audio_source.mesh" DESTINATION "../data/model/")
file(GLOB unlit_shaders "../../template/data/shader/unlit.*")
file(COPY ${unlit_shaders} DESTINATION "../data/shader/")

#头文件目录
include_directories("depends/glfw-3.3.3.bin.WIN64/include")
include_directories("depends/glfw-3.3.3.bin.WIN64/deps")
include_directories("depends/rttr/include")
include_directories("depends")
include_directories("source")

#链接目录
link_directories("depends/glfw-3.3.3.bin.WIN64/lib-static-ucrt")
link_directories("depends/rttr/lib")
link_directories("depends/fmod/api/core/lib/x64")

#链接lib文件
link_libraries(opengl32 glfw3dll rttr_core_d fmod_vc)

file(GLOB_RECURSE engine_cpp source/*.cpp)
file(GLOB_RECURSE fmod_cpp depends/fmod/api/*.cpp)
file(GLOB_RECURSE glfw_c depends/glfw-3.3.3.bin.WIN64/deps/glad_gl.c)

file(GLOB_RECURSE example_cpp example/*.cpp)
add_executable(untitled ${glfw_c} ${engine_cpp} ${example_cpp})


#拷贝dll到exe目录
file(COPY "depends/glfw-3.3.3.bin.WIN64/lib-static-ucrt/glfw3.dll" DESTINATION "./")
file(COPY "depends/fmod/api/core/lib/x64/fmod.dll" DESTINATION "./")


