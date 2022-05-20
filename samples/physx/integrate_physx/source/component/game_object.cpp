//
// Created by captain on 2021/6/9.
//

#include "game_object.h"
#include <assert.h>
#include <rttr/registration>
#include "component.h"
#include "utils/debug.h"

using namespace rttr;

Tree GameObject::game_object_tree_;//用树存储所有的GameObject。

GameObject::GameObject(const char* name): Tree::Node(),layer_(0x01) {
    set_name(name);
    game_object_tree_.root_node()->AddChild(this);
}

GameObject::~GameObject() {
    DEBUG_LOG_INFO("GameObject::~GameObject");
}

Component* GameObject::AddComponent(std::string component_type_name) {
    type t = type::get_by_name(component_type_name);
    if(t.is_valid()==false){
        std::cout<<"type::get_by_name({}) failed:"<<component_type_name<<std::endl;
        return nullptr;
    }

    sol::table component_table=AddComponentFromLua(component_type_name);
    Component* component=component_table.as<Component*>();
    return component;
}

Component* GameObject::GetComponent(std::string component_type_name) {
    sol::table component_table=GetComponentFromLua(component_type_name);
    if(!component_table){
        //没有找到组件,就去查找子组件
        type t = type::get_by_name(component_type_name);
        if(t.is_valid()==false){
            std::cout<<"type::get_by_name({}) failed:"<<component_type_name<<std::endl;
            return nullptr;
        }
        auto derived_classes = t.get_derived_classes();
        for(auto derived_class:derived_classes){
            component_table=GetComponentFromLua(derived_class.get_name().to_string());
            if(component_table){
                break;
            }
        }
    }
    if(!component_table){
        return nullptr;
    }
    Component* component=component_table.as<Component*>();
    return component;
}

bool GameObject::SetParent(GameObject* parent){
    if(parent== nullptr){
        DEBUG_LOG_ERROR("parent null");
        return false;
    }
    parent->AddChild(this);
    return true;
}

void GameObject::Foreach(std::function<void(GameObject* game_object)> func) {
    game_object_tree_.Post(game_object_tree_.root_node(),[&func](Tree::Node* node){
        auto n=node;
        GameObject* game_object= dynamic_cast<GameObject *>(n);
        func(game_object);
    });
}

GameObject* GameObject::Find(const char* name) {
    GameObject* game_object_find= nullptr;
    game_object_tree_.Find(game_object_tree_.root_node(), [&name](Tree::Node* node){
        GameObject* game_object=dynamic_cast<GameObject*>(node);
        if(strcmp(game_object->name(),name)==0){
            return true;
        }
        return false;
    }, reinterpret_cast<Node **>(&game_object_find));
    return game_object_find;
}

//LuaScript
sol::table GameObject::AddComponentFromLua(std::string component_type_name) {
    sol::protected_function component_type_construct_function=LuaBinding::sol_state()[component_type_name];//对c++的class注册为table，并实现了__call，所以可以直接带括号。
    auto result=component_type_construct_function();
    if(result.valid()== false){
        sol::error err = result;
        DEBUG_LOG_ERROR("\n---- RUN LUA_FUNCTION ERROR ----\nAddComponentFromLua call type construct error,type:{},not register in lua binding?\n{}\n------------------------",component_type_name,err.what());
    }
    sol::table new_table=result;

    result=new_table["set_game_object"](new_table,this);
    if(result.valid()== false){
        sol::error err = result;
        DEBUG_LOG_ERROR("\n---- RUN LUA_FUNCTION ERROR ----\nAddComponentFromLua call set_game_object error,type:{}\n{}\n------------------------",component_type_name,err.what());
    }

    if(lua_component_type_instance_map_.find(component_type_name)==lua_component_type_instance_map_.end()){
        std::vector<sol::table> component_vec;
        component_vec.push_back(new_table);
        lua_component_type_instance_map_[component_type_name]=component_vec;
    }else{
        lua_component_type_instance_map_[component_type_name].push_back(new_table);
    }
    result=new_table["Awake"](new_table);
    if(result.valid()== false){
        sol::error err = result;
        DEBUG_LOG_ERROR("\n---- RUN LUA_FUNCTION ERROR ----\nAddComponentFromLua call Awake error,type:{}\n{}\n------------------------",component_type_name,err.what());
    }
    return new_table;
}

sol::table GameObject::GetComponentFromLua(std::string component_type_name) {
    if(lua_component_type_instance_map_.find(component_type_name)==lua_component_type_instance_map_.end()){
        return sol::lua_nil;
    }
    if(lua_component_type_instance_map_[component_type_name].size()==0){
        return sol::lua_nil;
    }
    return lua_component_type_instance_map_[component_type_name][0];
}

void GameObject::ForeachLuaComponent(std::function<void(std::string,sol::table)> func) {
    for (auto& v : lua_component_type_instance_map_){
        for (auto& iter : v.second){
            sol::table lua_component_instance_table=iter;
            func(v.first,lua_component_instance_table);
        }
    }
}
