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
#include "LuaBridge\LuaBridge.h"
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
    }

    bool operator==(GameObject* rhs) const {
        return rhs == this;
    }

    Component* AddComponent(std::string component_type_name){
        type t = type::get_by_name(component_type_name);
        if(t.is_valid()==false){
            std::cout<<"type::get_by_name({}) failed:"<<component_type_name<<std::endl;
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

    std::vector<Component*>& GetComponents(std::string component_type_name) {
        return component_type_instance_map_[component_type_name];
    }


    Component* GetComponent(std::string component_type_name) {
        if(component_type_instance_map_.find(component_type_name)==component_type_instance_map_.end()){
            return nullptr;
        }
        if(component_type_instance_map_[component_type_name].size()==0){
            return nullptr;
        }
        return dynamic_cast<Component*>(component_type_instance_map_[component_type_name][0]);
    }

    void ForeachComponent(std::function<void(Component *)> func) {
        for (auto& v : component_type_instance_map_){
            for (auto& iter : v.second){
                Component* component=iter;
                func(component);
            }
        }
    }



    static void Foreach(std::function<void(GameObject* game_object)> func) {
        for (auto iter=game_object_list_.begin();iter!=game_object_list_.end();iter++){
            auto game_object=*iter;
            func(game_object);
        }
    }

private:
    std::unordered_map<std::string,std::vector<Component*>> component_type_instance_map_;
    static std::list<GameObject*> game_object_list_;//存储所有的GameObject。

public:
    luabridge::LuaRef AddComponentFromLua(std::string component_type_name) {
        luabridge::LuaRef component_type=luabridge::getGlobal(lua_state,component_type_name.c_str());
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
        }

        return new_table;
    }

    luabridge::LuaRef GetComponentFromLua(std::string component_type_name) {
        //首先尝试去获取cpp component
        Component* component=GetComponent(component_type_name);
        if(component!= nullptr){
            return luabridge::LuaRef(lua_state,component);
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

    void ForeachLuaComponent(std::function<void(LuaRef)> func) {
        for (auto& v : lua_component_type_instance_map_){
            for (auto& iter : v.second){
                LuaRef lua_ref=iter;
                func(lua_ref);
            }
        }
    }
private:
    std::unordered_map<std::string,std::vector<luabridge::LuaRef>> lua_component_type_instance_map_;
};
#endif //TEST_GAME_OBJECT_H
