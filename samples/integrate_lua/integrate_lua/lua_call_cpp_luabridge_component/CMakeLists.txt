cmake_minimum_required(VERSION 3.17)
project(test)

set(CMAKE_CXX_STANDARD 14)

#set(CMAKE_EXE_LINKER_FLAGS "-static")
#message("CMAKE_EXE_LINKER_FLAGS:" ${CMAKE_EXE_LINKER_FLAGS})
#IF(CMAKE_EXE_LINKER_FLAGS STREQUAL "-static")
#    set(RTTR_DIR "rttr")
#    set(RTTR_LIB "rttr_core_d")
#ELSE()
#    set(RTTR_DIR "rttr_dll")
#    set(RTTR_LIB "rttr_core_d.dll")
#ENDIF()

#拷贝depends目录
file(COPY "../../../template/depends/lua/" DESTINATION "../depends/lua/")
file(COPY "../../../template/depends/LuaBridge/" DESTINATION "../depends/LuaBridge/")
#file(COPY "../../../template/depends/${RTTR_DIR}/" DESTINATION "../depends/${RTTR_DIR}/")
file(COPY "../../../template/depends/glm/" DESTINATION "../depends/glm/")
file(COPY "../../../template/depends/rttr-0.9.6/src/rttr/" DESTINATION "../depends/rttr/")

#头文件目录
include_directories("depends/lua/src")
#include_directories("depends/${RTTR_DIR}/include")
include_directories("depends")

#链接目录
#link_directories("depends/${RTTR_DIR}/lib")

#链接lib文件
link_libraries(${RTTR_LIB})

file(GLOB_RECURSE lua_c "depends/lua/src/*.c")
file(GLOB_RECURSE rttr_cpp "depends/rttr/*.cpp")

add_executable(test ${lua_c} ${rttr_cpp} "example/main.cpp" source/component.cpp source/game_object.cpp)

#IF(CMAKE_EXE_LINKER_FLAGS STREQUAL "")
#    file(COPY "depends/${RTTR_DIR}/bin/librttr_core_d.dll" DESTINATION "./")
#ENDIF()