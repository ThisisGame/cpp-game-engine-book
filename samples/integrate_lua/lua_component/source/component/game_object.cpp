//
// Created by captain on 2021/6/9.
//

#include "game_object.h"
#include <assert.h>
#include <rttr/registration>
#include "component.h"
#include "utils/debug.h"

using namespace rttr;

std::list<GameObject*> GameObject::game_object_list_;

GameObject::GameObject(std::string name):layer_(0x01) {
    set_name(name);

    game_object_list_.push_back(this);
}

GameObject::~GameObject() {

}

Component* GameObject::AddComponent(std::string component_type_name) {
    luabridge::LuaRef component_table=AddComponentFromLua(component_type_name);
    if(component_table.isNil()){
        DEBUG_LOG_ERROR("{} not register to lua",component_type_name);
        return nullptr;
    }
    Component* component=component_table.cast<Component*>();
    return component;
}

std::vector<Component*> GameObject::GetComponents(std::string component_type_name) {
    std::vector<Component*> component_table_vec;
    for (auto& iter : lua_component_type_instance_map_[component_type_name]){
        Component* component=iter.cast<Component*>();
        component_table_vec.push_back(component);
    }
    return component_table_vec;
}



Component* GameObject::GetComponent(std::string component_type_name) {
    luabridge::LuaRef component_table=GetComponentFromLua(component_type_name);
    if(component_table.isNil()){
        DEBUG_LOG_ERROR("{} not register to lua",component_type_name);
        return nullptr;
    }
    Component* component=component_table.cast<Component*>();
    return component;
}

void GameObject::Foreach(std::function<void(GameObject*)> func) {
    for (auto iter=game_object_list_.begin();iter!=game_object_list_.end();iter++){
        auto game_object=*iter;
        func(game_object);
    }
}

/******************** BEGIN LUA COMPONENT ******************/

bool GameObject::operator==(GameObject* rhs) const {
    return rhs == this;
}

luabridge::LuaRef GameObject::AddComponentFromLua(std::string component_type_name) {
    luabridge::LuaRef component_type=luabridge::getGlobal(LuaBinding::lua_state(),component_type_name.c_str());
    if(component_type.isNil()){
        DEBUG_LOG_ERROR("{} not register to lua",component_type_name);
        return luabridge::LuaRef(LuaBinding::lua_state());
    }
    auto new_table=component_type();//luabridge对c++的class注册为table，并实现了__call，所以可以直接带括号。

    {
        luabridge::LuaRef function_ref=new_table["set_game_object"];
        if(function_ref.isFunction()==false){
            DEBUG_LOG_ERROR("{} has no function {}",component_type_name,"set_game_object");
            return luabridge::LuaRef(LuaBinding::lua_state());
        }
        luabridge::LuaRef function_return_ref= function_ref(new_table,this);
    }


    if(lua_component_type_instance_map_.find(component_type_name)==lua_component_type_instance_map_.end()){
        std::vector<luabridge::LuaRef> component_vec;
        component_vec.push_back(new_table);
        lua_component_type_instance_map_[component_type_name]=component_vec;
    }else{
        lua_component_type_instance_map_[component_type_name].push_back(new_table);
    }

    {
        luabridge::LuaRef function_ref=new_table["Awake"];
        if(function_ref.isFunction()==false){
            DEBUG_LOG_ERROR("{} has no function {}",component_type_name,"Awake");
            return luabridge::LuaRef(LuaBinding::lua_state());
        }
        luabridge::LuaRef function_return_ref= function_ref(new_table,this);
    }

    return new_table;
}

luabridge::LuaRef GameObject::GetComponentFromLua(std::string component_type_name) {
    if(lua_component_type_instance_map_.find(component_type_name)==lua_component_type_instance_map_.end()){
        return luabridge::LuaRef(LuaBinding::lua_state());
    }
    if(lua_component_type_instance_map_[component_type_name].size()==0){
        return luabridge::LuaRef(LuaBinding::lua_state());
    }
    return lua_component_type_instance_map_[component_type_name][0];
}

void GameObject::ForeachLuaComponent(std::function<void(luabridge::LuaRef)> func) {
    for (auto& v : lua_component_type_instance_map_){
        for (auto& iter : v.second){
            luabridge::LuaRef lua_ref=iter;
            func(lua_ref);
        }
    }
}
