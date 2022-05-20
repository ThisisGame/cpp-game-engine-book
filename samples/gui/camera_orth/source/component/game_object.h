//
// Created by captain on 2021/6/9.
//

#ifndef UNTITLED_GAME_OBJECT_H
#define UNTITLED_GAME_OBJECT_H

#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <memory>
#include <list>
#include <functional>

class Component;
class GameObject {
public:
    GameObject(std::string name);
    ~GameObject();

    std::string& name(){return name_;}
    void set_name(std::string name){name_=name;}

    /// 添加组件
    /// \param component_type_name 组件类名
    /// \return
    Component* AddComponent(std::string component_type_name);

    /// 获取组件
    /// \param component_type_name 组件类名
    /// \return
    Component* GetComponent(std::string component_type_name);

    /// 获取所有同名组件
    /// \param component_type_name 组件类名
    /// \return
    std::vector<Component*>& GetComponents(std::string component_type_name);

    /// 遍历所有Component
    /// \param func
    void ForeachComponent(std::function<void(Component* component)> func);

    unsigned char layer(){return layer_;}
    void set_layer(unsigned char layer){layer_=layer;}

    /// 遍历所有Camera
    /// \param func
    static void Foreach(std::function<void(GameObject* game_object)> func);
private:
    std::string name_;
    std::unordered_map<std::string,std::vector<Component*>> component_type_instance_map_;

    unsigned char layer_;//将物体分不同的层，用于相机分层、物理碰撞分层等。

    static std::list<GameObject*> game_object_list_;//存储所有的GameObject。
};


#endif //UNTITLED_GAME_OBJECT_H
