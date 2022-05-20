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

    }

    ~GameObject() {
        std::cout<<"~GameObject"<<std::endl;
    }

    Component* AddComponent(sol::table component_instance_table){
        Component* component=component_instance_table.as<Component*>();
        const char* component_type_name=component->type_name();
        if(components_map_.find(component_type_name) == components_map_.end()){
            std::vector<Component*> component_vec;
            components_map_[component_type_name]=component_vec;
        }
        components_map_[component_type_name].push_back(component);
    }

    Component* GetComponent(std::string component_type_name) {

    }

private:
    std::unordered_map<std::string,std::vector<Component*>> components_map_;
};
#endif //TEST_GAME_OBJECT_H
