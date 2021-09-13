//
// Created by captainchen on 2021/9/13.
//

#include "lua_binding.h"
#include "utils/application.h"
#include "component/game_object.h"
#include "component/transform.h"


lua_State* LuaBinding::lua_state_;

void LuaBinding::BindLua(lua_State *lua_state) {
    lua_state_=lua_state;

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
            .beginClass<Transform>("Transform")
            .addConstructor<void (*) ()>()
            .addFunction("position", &Transform::position)
            .addFunction("set_position", &Transform::set_position)
            .endClass();

    luabridge::getGlobalNamespace(lua_state)
            .beginNamespace(BIND_CPP_TO_LUA_NAMESPACE)
            .beginNamespace("glm")
            .beginClass<glm::vec3>("vec3")
            .endClass();

    luabridge::getGlobalNamespace(lua_state)
            .beginNamespace(BIND_CPP_TO_LUA_NAMESPACE)
            .beginClass<Component>("Component")
            .endClass();
}