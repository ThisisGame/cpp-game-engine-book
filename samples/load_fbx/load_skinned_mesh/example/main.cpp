#define GLFW_INCLUDE_NONE

#include <iostream>
#include "utils/debug.h"
#include "lua_binding/lua_binding.h"

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtx/string_cast_beauty.hpp>

int main(void){
    //设置lua搜索目录
    LuaBinding::Init(";../example/?.lua;");
    //绑定引擎所有类到Lua
    LuaBinding::BindLua();
    //执行lua
    LuaBinding::RunLuaFile("../example/main.lua");
    //调用lua main()
    LuaBinding::CallLuaFunction("main");

    return 0;
}


