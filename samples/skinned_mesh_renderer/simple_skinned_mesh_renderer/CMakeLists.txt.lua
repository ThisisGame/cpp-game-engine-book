#头文件目录
include_directories("depends/lua/src")

#源文件
file(GLOB_RECURSE lua_src depends/lua/src/*.c)