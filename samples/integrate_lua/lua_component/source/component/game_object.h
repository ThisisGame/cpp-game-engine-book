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
#include <rttr/registration>
#include "component.h"
#include "data_structs/tree.h"
#include "lua_binding/lua_binding.h"

using namespace rttr;

class GameObject:public Tree::Node {
public:
    GameObject(std::string name);
    ~GameObject();

    std::string& name(){return name_;}
    void set_name(std::string name){name_=name;}

    unsigned char layer(){return layer_;}
    void set_layer(unsigned char layer){layer_=layer;}

    bool active(){return active_;}
    void set_active(bool active){active_=active;}

    /// 设置父节点
    /// \param parent
    /// \return
    bool SetParent(GameObject* parent);

    /// 全局查找GameObject
    /// \param name
    /// \return
    static GameObject* Find(std::string name);
public:
    /// 添加组件，仅用于C++中添加组件。
    /// \tparam T 组件类型
    /// \return 组件实例
    template <class T=Component>
    T* AddComponent();

    /// 附加组件实例
    /// \param component_instance_table
    void AttachComponent(Component* component);
private:
    std::string name_;

    unsigned char layer_;//将物体分不同的层，用于相机分层、物理碰撞分层等。

    bool active_=true;//是否激活

    static Tree game_object_tree_;//用树存储所有的GameObject。

    std::unordered_map<std::string,std::vector<Component*>> components_map_;
};


#endif //UNTITLED_GAME_OBJECT_H
