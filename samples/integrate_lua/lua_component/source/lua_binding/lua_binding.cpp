//
// Created by captainchen on 2021/9/13.
//

#include "lua_binding.h"
#include "utils/application.h"
#include "component/game_object.h"
#include "component/transform.h"
#include "control/input.h"


lua_State* LuaBinding::lua_state_;

void LuaBinding::BindLua(lua_State *lua_state) {
    lua_state_=lua_state;

    // depends
    {
        luabridge::getGlobalNamespace(lua_state)
                .beginNamespace("glm")
                .beginClass<glm::vec3>("vec3")
                .endClass();
    }

    // component
    {
        luabridge::getGlobalNamespace(lua_state)
                .beginClass<Component>("Component")
                .addFunction("Awake",&Component::Awake)
                .addFunction("Update",&Component::Update)
                .addFunction("game_object",&Component::game_object)
                .addFunction("set_game_object",&Component::set_game_object)
                .endClass();

        luabridge::getGlobalNamespace(lua_state)
                .beginClass<GameObject>("GameObject")
                .addConstructor<void (*) (std::string)>()
                .addFunction("__eq", &GameObject::operator==)
                .addFunction("AddComponent", (luabridge::LuaRef (GameObject::*)(luabridge::LuaRef))&GameObject::AddComponent)
                .addFunction("GetComponent",&GameObject::GetComponentFromLua)
                .endClass();

        luabridge::getGlobalNamespace(lua_state)
                .beginClass<Transform>("Transform")
                .addConstructor<void (*) ()>()
                .addFunction("position", &Transform::position)
                .addFunction("rotation", &Transform::rotation)
                .addFunction("scale", &Transform::scale)
                .addFunction("set_position", &Transform::set_position)
                .addFunction("set_rotation", &Transform::set_rotation)
                .addFunction("set_scale", &Transform::set_scale)
                .endClass();
    }

    // control
    {
        luabridge::getGlobalNamespace(lua_state)
                .beginClass<vec2_ushort>("vec2_ushort")
                .addProperty("x_",&vec2_ushort::x_)
                .addProperty("y_",&vec2_ushort::y_)
                .endClass();

        luabridge::getGlobalNamespace(lua_state)
                .beginClass<Input>("Input")
                .addStaticFunction("RecordKey",&Input::RecordKey)
                .addStaticFunction("GetKey",&Input::GetKey)
                .addStaticFunction("GetKeyDown",&Input::GetKeyDown)
                .addStaticFunction("GetKeyUp",&Input::GetKeyUp)
                .addStaticFunction("Update",&Input::Update)
                .addStaticFunction("GetMouseButton",&Input::GetMouseButton)
                .addStaticFunction("GetMouseButtonDown",&Input::GetMouseButtonDown)
                .addStaticFunction("GetMouseButtonUp",&Input::GetMouseButtonUp)
                .addStaticFunction("mousePosition",&Input::mousePosition)
                .addStaticFunction("set_mousePosition",&Input::set_mousePosition)
                .addStaticFunction("mouse_scroll",&Input::mouse_scroll)
                .addStaticFunction("RecordScroll",&Input::RecordScroll)
                .endClass();
    }

    // utils
    {
        luabridge::getGlobalNamespace(lua_state)
                .beginClass<Application>("Application")
                .addStaticFunction("set_title",&Application::set_title)
                .addStaticFunction("set_data_path",&Application::set_data_path)
                .addStaticFunction("Init",&Application::Init)
                .addStaticFunction("Run",&Application::Run)
                .endClass();
    }




}