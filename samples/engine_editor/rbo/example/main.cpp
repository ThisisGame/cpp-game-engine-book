#define GLFW_INCLUDE_NONE

#include "utils/debug.h"
#include "lua_binding/lua_binding.h"

int main(void){
    //设置lua搜索目录
    LuaBinding::Init(";../example/?.lua;../source_lua/?.lua;../source_lua/utils/?.lua;../source_lua/component/?.lua;../source_lua/renderer/?.lua");
    //绑定引擎所有类到Lua
    LuaBinding::BindLua();
    //执行lua
    LuaBinding::RunLuaFile("../example/main.lua");
    //调用lua main()
    LuaBinding::CallLuaFunction("main");

    return 0;
}


