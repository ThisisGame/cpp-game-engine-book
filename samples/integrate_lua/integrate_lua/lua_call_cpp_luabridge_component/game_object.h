//
// Created by captainchen on 2021/9/16.
//

#ifndef TEST_GAME_OBJECT_H
#define TEST_GAME_OBJECT_H
extern "C"
{
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}
#include "LuaBridge/LuaBridge.h"
#include<string>
#include<unordered_map>
#include <stdio.h>
#include <string.h>
#include <list>
#include <rttr/registration>

using namespace std;
using namespace luabridge;
using namespace rttr;

#include "component.h"

extern lua_State* lua_state;
class GameObject{
public:
    GameObject() {
        game_object_list_.push_back(this);
    }

    ~GameObject() {
        std::cout<<"~GameObject"<<std::endl;
    }


    Component* AddComponent(std::string component_type_name){
        type t = type::get_by_name(component_type_name);
        if(t.is_valid()==false){
            std::cout<<"type::get_by_name({}) failed:"<<component_type_name<<std::endl;
            return nullptr;
        }

        luabridge::LuaRef component_table=AddComponentFromLua(component_type_name);
        Component* component=component_table.cast<Component*>();
        return component;
    }

    std::vector<Component*> GetComponents(std::string component_type_name) {
        std::vector<Component*> component_table_vec;
        for (auto& iter : lua_component_type_instance_map_[component_type_name]){
            Component* component=iter.cast<Component*>();
            component_table_vec.push_back(component);
        }
        return component_table_vec;
    }


    Component* GetComponent(std::string component_type_name) {
        luabridge::LuaRef component_table=GetComponentFromLua(component_type_name);
        Component* component=component_table.cast<Component*>();
        return component;
    }

    static void Foreach(std::function<void(GameObject* game_object)> func) {
        for (auto iter=game_object_list_.begin();iter!=game_object_list_.end();iter++){
            auto game_object=*iter;
            func(game_object);
        }
    }

private:
    static std::list<GameObject*> game_object_list_;//存储所有的GameObject。

public:

    bool operator==(GameObject* rhs) const {
        return rhs == this;
    }

    luabridge::LuaRef AddComponentFromLua(std::string component_type_name) {
        luabridge::LuaRef component_type=luabridge::getGlobal(lua_state,component_type_name.c_str());
        auto new_table=component_type();//luabridge对c++的class注册为table，并实现了__call，所以可以直接带括号。

        LuaRef function_ref=new_table["set_game_object"];
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

        return new_table;
    }

    luabridge::LuaRef GetComponentFromLua(std::string component_type_name) {
        if(lua_component_type_instance_map_.find(component_type_name)==lua_component_type_instance_map_.end()){
            return nullptr;
        }
        if(lua_component_type_instance_map_[component_type_name].size()==0){
            return nullptr;
        }
        return lua_component_type_instance_map_[component_type_name][0];
    }

    void ForeachLuaComponent(std::function<void(LuaRef)> func) {
        for (auto& v : lua_component_type_instance_map_){
            for (auto& iter : v.second){
                LuaRef lua_ref=iter;
                func(lua_ref);
            }
        }
    }

    void test_set(const std::string& a){
        std::cout<<a<<std::endl;
    }
private:
    std::unordered_map<std::string,std::vector<luabridge::LuaRef>> lua_component_type_instance_map_;
};
#endif //TEST_GAME_OBJECT_H
