cmake_minimum_required(VERSION 3.17)
project(test)

set(CMAKE_CXX_STANDARD 17)

if (MSVC)
    add_compile_options(/bigobj)
    add_compile_options(/wd4819)#忽略保存为unicode警告
endif ()

set(CMAKE_CXX_FLAGS "-Wno-inconsistent-missing-override")

#拷贝depends目录
file(COPY "../../template/depends/lua/" DESTINATION "../depends/lua/")
file(COPY "../../template/depends/luasocket-3.0.0/src/" DESTINATION "../depends/luasocket/")
file(COPY "../../template/depends/sol2-3.2.2/include/sol/" DESTINATION "../depends/sol/")
file(COPY "../../template/depends/glm/" DESTINATION "../depends/glm/")
file(COPY "../../template/depends/rttr-0.9.6/src/rttr/" DESTINATION "../depends/rttr/")
file(COPY "../../template/depends/spdlog/include/spdlog/" DESTINATION "../depends/spdlog/")

#头文件目录
include_directories("depends/lua/src")
include_directories("depends")

if (MSVC)
    set(luasocket_c depends/luasocket/auxiliar.c
            depends/luasocket/buffer.c
            depends/luasocket/compat.c
            depends/luasocket/except.c
            depends/luasocket/inet.c
            depends/luasocket/io.c
            depends/luasocket/luasocket.c
            depends/luasocket/options.c
            depends/luasocket/select.c
            depends/luasocket/tcp.c
            depends/luasocket/timeout.c
            depends/luasocket/udp.c
            depends/luasocket/wsocket.c
            )
    link_libraries(ws2_32)
else()
    set(luasocket_c depends/luasocket/auxiliar.c
            depends/luasocket/buffer.c
            depends/luasocket/compat.c
            depends/luasocket/except.c
            depends/luasocket/inet.c
            depends/luasocket/io.c
            depends/luasocket/luasocket.c
            depends/luasocket/options.c
            depends/luasocket/select.c
            depends/luasocket/tcp.c
            depends/luasocket/timeout.c
            depends/luasocket/udp.c
            depends/luasocket/usocket.c
            )
endif()

file(GLOB_RECURSE lua_c "depends/lua/src/*.c")
file(GLOB_RECURSE rttr_cpp "depends/rttr/*.cpp")
file(GLOB_RECURSE engine_cpp "source/*.cpp")

add_executable(test ${lua_c} ${luasocket_c} ${rttr_cpp} "example/main.cpp" ${engine_cpp})