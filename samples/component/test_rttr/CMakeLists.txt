cmake_minimum_required(VERSION 3.17)
project(testrttr)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXE_LINKER_FLAGS "-static")

#拷贝depends目录
file(COPY "../../template/depends/rttr/" DESTINATION "../depends/rttr/")

include_directories(depends/rttr/include)

link_directories(depends/rttr/lib)

link_libraries(rttr_core_d)

add_executable(testrttr main.cpp)