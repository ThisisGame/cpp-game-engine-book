#define GLFW_INCLUDE_NONE

extern "C"
{
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}
#include "LuaBridge/LuaBridge.h"
#include "utils/debug.h"
#include "lua_binding.h"


int main(void)
{
    lua_State* lua_state = luaL_newstate();
    luaL_openlibs(lua_state);

    BindLua(lua_state);//绑定引擎所有类到Lua

    luaL_dofile(lua_state, "../example/main.lua");

    luabridge::LuaRef main_function = luabridge::getGlobal(lua_state, "main");
    try {
        main_function();
    } catch (const luabridge::LuaException& e) {
        DEBUG_LOG_ERROR(e.what());
    }

    lua_close(lua_state);

    return 0;
}

