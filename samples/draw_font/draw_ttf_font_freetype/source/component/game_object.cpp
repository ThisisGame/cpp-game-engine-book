//
// Created by captain on 2021/6/9.
//

#include "game_object.h"
#include <assert.h>
#include <rttr/registration>
#include "component.h"

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
    assert(t.is_valid());
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

