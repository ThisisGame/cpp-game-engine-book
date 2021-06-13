cmake_minimum_required(VERSION 3.17)
project(testrttr)

set(CMAKE_CXX_STANDARD 14)

#拷贝depends目录
file(COPY "../../template/depends/rttr/" DESTINATION "../depends/rttr/")

include_directories(depends/rttr/include)

link_directories(depends/rttr/lib)

link_libraries(librttr_core_d)

add_executable(testrttr main.cpp)

#拷贝dll到exe目录
file(COPY "depends/rttr/dll/librttr_core_d.dll" DESTINATION "./")