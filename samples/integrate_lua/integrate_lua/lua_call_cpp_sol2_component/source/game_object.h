//
// Created by captainchen on 2021/9/16.
//

#ifndef TEST_GAME_OBJECT_H
#define TEST_GAME_OBJECT_H
#include <sol/sol.hpp>
#include <iostream>
#include <string>
#include <unordered_map>
#include <stdio.h>
#include <string.h>
#include <list>
#include <rttr/registration>

using namespace std;
using namespace rttr;

#include "component.h"

extern sol::state sol_state;;
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

        sol::table component_table=AddComponentFromLua(component_type_name);
        Component* component=component_table.as<Component*>();
        return component;
    }

    std::vector<Component*> GetComponents(std::string component_type_name) {
        std::vector<Component*> component_table_vec;
        for (auto& iter : lua_component_type_instance_map_[component_type_name]){
            Component* component=iter.as<Component*>();
            component_table_vec.push_back(component);
        }
        return component_table_vec;
    }


    Component* GetComponent(std::string component_type_name) {
        sol::table component_table=GetComponentFromLua(component_type_name);
        Component* component=component_table.as<Component*>();
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

    /// 根据传入的组件名，创建组件实例
    /// \param component_type_name
    /// \return
    sol::table AddComponentFromLua(std::string component_type_name) {
        sol::protected_function component_type_construct_function=sol_state[component_type_name];//对c++的class注册为table，并实现了__call，所以可以直接带括号。
        auto result=component_type_construct_function();
        if(result.valid()== false){
            sol::error err = result;
            std::cerr << "----- RUN LUA ERROR ----" << std::endl;
            std::cerr <<"AddComponentFromLua call type construct error,type:"<<component_type_name<< std::endl;
            std::cerr << err.what() << std::endl;
            std::cerr << "------------------------" << std::endl;
        }
        sol::table new_table=result;

        result=new_table["set_game_object"](new_table,this);
        if(result.valid()== false){
            sol::error err = result;
            std::cerr << "----- RUN LUA ERROR ----" << std::endl;
            std::cerr <<"AddComponentFromLua call set_game_object error,type:"<<component_type_name<< std::endl;
            std::cerr << err.what() << std::endl;
            std::cerr << "------------------------" << std::endl;
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
            std::cerr << "----- RUN LUA ERROR ----" << std::endl;
            std::cerr <<"AddComponentFromLua call Awake error,type:"<<component_type_name<< std::endl;
            std::cerr << err.what() << std::endl;
            std::cerr << "------------------------" << std::endl;
        }
        return new_table;
    }

    sol::table GetComponentFromLua(std::string component_type_name) {
        if(lua_component_type_instance_map_.find(component_type_name)==lua_component_type_instance_map_.end()){
            return nullptr;
        }
        if(lua_component_type_instance_map_[component_type_name].size()==0){
            return nullptr;
        }
        return lua_component_type_instance_map_[component_type_name][0];
    }

    void ForeachLuaComponent(std::function<void(sol::table)> func) {
        for (auto& v : lua_component_type_instance_map_){
            for (auto& iter : v.second){
                sol::table lua_component_instance_table=iter;
                func(lua_component_instance_table);
            }
        }
    }

private:
    std::unordered_map<std::string,std::vector<sol::table>> lua_component_type_instance_map_;
};
#endif //TEST_GAME_OBJECT_H
