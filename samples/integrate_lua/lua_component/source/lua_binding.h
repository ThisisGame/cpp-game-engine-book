//
// Created by captain on 2021/9/6.
//

#ifndef UNTITLED_LUA_BINDING_H
#define UNTITLED_LUA_BINDING_H

#define BIND_CPP_TO_LUA_NAMESPACE "Cpp"

extern "C"
{
#include "lua.h"
}
#include "LuaBridge/LuaBridge.h"
#include "component/game_object.h"
#include "utils/application.h"

/// 绑定引擎所有类到Lua
/// \param lua_state
void BindLua(lua_State* lua_state){
    luabridge::getGlobalNamespace(lua_state)
            .beginNamespace(BIND_CPP_TO_LUA_NAMESPACE)
            .beginClass<Application>("Application")
            .addStaticFunction("set_title",&Application::set_title)
            .addStaticFunction("set_data_path",&Application::set_data_path)
            .addStaticFunction("Init",&Application::Init)
            .addStaticFunction("Run",&Application::Run)
            .endClass();

    luabridge::getGlobalNamespace(lua_state)
            .beginNamespace(BIND_CPP_TO_LUA_NAMESPACE)
            .beginClass<GameObject>("GameObject")
            .addConstructor<void (*) (std::string)>()
            .addFunction("AddComponent", (luabridge::LuaRef (GameObject::*)(luabridge::LuaRef))&GameObject::AddComponent)
            .endClass();

    luabridge::getGlobalNamespace(lua_state)
            .beginNamespace(BIND_CPP_TO_LUA_NAMESPACE)
            .beginClass<Component>("Component")
            .endClass();
}

#endif //UNTITLED_LUA_BINDING_H
