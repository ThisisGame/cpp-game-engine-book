//
// Created by captain on 2021/9/6.
//

#ifndef UNTITLED_LUA_BINDING_H
#define UNTITLED_LUA_BINDING_H

extern "C"
{
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}
#include "LuaBridge/LuaBridge.h"

class LuaBinding{
public:
    /// 绑定引擎所有类到Lua
    /// \param lua_state
    static void BindLua(lua_State* lua_state);

    static lua_State* lua_state(){return lua_state_;}

private:
    static lua_State* lua_state_;
};



#endif //UNTITLED_LUA_BINDING_H
