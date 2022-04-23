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
#include "data_structs/tree.h"
#include "lua_binding/lua_binding.h"

class Component;
class GameObject:public Tree::Node {
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
    unsigned char layer(){return layer_;}
    void set_layer(unsigned char layer){layer_=layer;}

    bool active(){return active_;}
    void set_active(bool active){active_=active;}

    /// 设置父节点
    /// \param parent
    /// \return
    bool SetParent(GameObject* parent);
	
	/// 遍历所有Camera
    /// \param func
    static void Foreach(std::function<void(GameObject*)> func);

    /// 全局查找GameObject
    /// \param name
    /// \return
    static GameObject* Find(std::string name);
private:
    std::string name_;
    std::unordered_map<std::string,std::vector<Component*>> component_type_instance_map_;

    unsigned char layer_;//将物体分不同的层，用于相机分层、物理碰撞分层等。

    bool active_=true;//是否激活

    static Tree game_object_tree_;//用树存储所有的GameObject。

//LUA_SCRIPT
public:
    /// 重载==，用于LuaRef对象做比较
    /// \param rhs
    /// \return
    bool operator==(GameObject* rhs) const;

    /// 在Lua中添加组件
    /// \param component_type_name cpp、lua的组件名称
    /// \return
    sol::table AddComponentFromLua(std::string component_type_name);

    /// 获取组件
    /// \param component_type_name
    /// \return
    sol::table GetComponentFromLua(std::string component_type_name);

    /// 遍历组件 一般在Update中使用
    /// \param func
    void ForeachLuaComponent(std::function<void(sol::table)> func);

private:
    std::unordered_map<std::string,std::vector<sol::table>> lua_component_type_instance_map_;//所有lua component
};


#endif //UNTITLED_GAME_OBJECT_H
