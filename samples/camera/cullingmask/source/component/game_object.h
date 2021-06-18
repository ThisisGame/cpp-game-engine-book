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

class Component;
class GameObject {
public:
    GameObject();
    GameObject(std::string name);
    ~GameObject();

    std::string& name(){return name_;}
    void set_name(std::string name){name_=name;}

    /// 添加组件
    /// \param component_type_name 组件类名
    /// \return
    std::shared_ptr<Component> AddComponent(std::string component_type_name);

    /// 获取组件
    /// \param component_type_name 组件类名
    /// \return
    std::shared_ptr<Component> GetComponent(std::string component_type_name);

    /// 获取所有同名组件
    /// \param component_type_name 组件类名
    /// \return
    std::vector<std::shared_ptr<Component>>& GetComponents(std::string component_type_name);

private:
    std::string name_;
    std::unordered_map<std::string,std::vector<std::shared_ptr<Component>>> component_type_instance_map_;

    unsigned char layer_;//用于Camera 确认是否要渲染这个
};


#endif //UNTITLED_GAME_OBJECT_H
