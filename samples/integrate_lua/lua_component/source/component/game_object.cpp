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
    type t = type::get_by_name(component_type_name);
    if(t.is_valid()==false){
        DEBUG_LOG_ERROR("type::get_by_name({}) failed",component_type_name);
        return nullptr;
    }

    variant var = t.create();    // 创建实例
    Component* component=var.get_value<Component*>();
    component->set_game_object(this);

    if(component_type_instance_map_.find(component_type_name)==component_type_instance_map_.end()){
        std::vector<Component*> component_vec;
        component_vec.push_back(component);
        component_type_instance_map_[component_type_name]=component_vec;
    }else{
        component_type_instance_map_[component_type_name].push_back(component);
    }

    component->Awake();
    return component;
}

std::vector<Component*> &GameObject::GetComponents(std::string component_type_name) {
    return component_type_instance_map_[component_type_name];
}

Component* GameObject::GetComponent(std::string component_type_name) {
    if(component_type_instance_map_.find(component_type_name)==component_type_instance_map_.end()){
        return nullptr;
    }
    if(component_type_instance_map_[component_type_name].size()==0){
        return nullptr;
    }
    return component_type_instance_map_[component_type_name][0];
}

void GameObject::ForeachComponent(std::function<void(Component *)> func) {
    for (auto& v : component_type_instance_map_){
        for (auto& iter : v.second){
            Component* component=iter;
            func(component);
        }
    }
}

void GameObject::Foreach(std::function<void(GameObject* game_object)> func) {
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
    auto new_table=component_type();//luabridge对c++的class注册为table，并实现了__call，所以可以直接带括号。

    if(new_table.isInstance<Component>()){
        Component* component=new_table.cast<Component*>();
        component->set_game_object(this);
        if(component_type_instance_map_.find(component_type_name)==component_type_instance_map_.end()){
            std::vector<Component*> component_vec;
            component_vec.push_back(component);
            component_type_instance_map_[component_type_name]=component_vec;
        }else{
            component_type_instance_map_[component_type_name].push_back(component);
        }
        component->Awake();
    }else{
        luabridge::LuaRef function_ref=new_table["set_game_object"];
        if(function_ref.isFunction()){
            function_ref(new_table,this);
        }

        if(lua_component_type_instance_map_.find(component_type_name)==lua_component_type_instance_map_.end()){
            std::vector<luabridge::LuaRef> component_vec;
            component_vec.push_back(new_table);
            lua_component_type_instance_map_[component_type_name]=component_vec;
        }else{
            lua_component_type_instance_map_[component_type_name].push_back(new_table);
        }
        new_table["Awake"](new_table);
    }

    return new_table;
}

luabridge::LuaRef GameObject::GetComponentFromLua(std::string component_type_name) {
    //首先尝试去获取cpp component
    Component* component=GetComponent(component_type_name);
    if(component!= nullptr){
        return luabridge::LuaRef(LuaBinding::lua_state(),component);
    }

    //再去lua component里搜索
    if(lua_component_type_instance_map_.find(component_type_name)==lua_component_type_instance_map_.end()){
        return nullptr;
    }
    if(lua_component_type_instance_map_[component_type_name].size()==0){
        return nullptr;
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
