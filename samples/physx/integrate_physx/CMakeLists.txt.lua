file(COPY "../../template/depends/lua/" DESTINATION "../depends/lua/")
file(COPY "../../template/depends/sol2-3.2.2/include/sol/" DESTINATION "../depends/sol/")

#头文件目录
include_directories("depends/lua/src")

#源文件
file(GLOB_RECURSE lua_src depends/lua/src/*.c)