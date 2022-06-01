//
// Created by captainchen on 2021/9/16.
//

#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H
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


    /// 添加组件，仅用于C++中添加组件。
    /// \tparam T 组件类型
    /// \return 组件实例
    template <class T=Component>
    T* AddComponent(){
        T* component=new T();
        AttachComponent(component);
        component->Awake();
        return dynamic_cast<T*>(component);
    }

    /// 附加组件实例
    /// \param component_instance_table
    void AttachComponent(Component* component){
        component->set_game_object(this);
        //获取类名
        type t=type::get(*component);
        std::string component_type_name=t.get_name().to_string();

        if(components_map_.find(component_type_name)==components_map_.end()){
            std::vector<Component*> component_vec;
            component_vec.push_back(component);
            components_map_[component_type_name]=component_vec;
        }else{
            components_map_[component_type_name].push_back(component);
        }
    }

    void ForeachComponent(std::function<void(Component*)> func) {
        for (auto& v : components_map_){
            for (auto& iter : v.second){
                Component* component=iter;
                func(component);
            }
        }
    }

private:
    std::unordered_map<std::string,std::vector<Component*>> components_map_;

public:
    static void Foreach(std::function<void(GameObject* game_object)> func) {
        for (auto iter=game_object_list_.begin();iter!=game_object_list_.end();iter++){
            auto game_object=*iter;
            func(game_object);
        }
    }
private:
    static std::list<GameObject*> game_object_list_;//存储所有的GameObject。

};

#endif //GAME_OBJECT_H
