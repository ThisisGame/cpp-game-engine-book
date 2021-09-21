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
#include "lua_binding/lua_binding.h"

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



    unsigned char layer(){return layer_;}
    void set_layer(unsigned char layer){layer_=layer;}

    /// 遍历所有Camera
    /// \param func
    static void Foreach(std::function<void(GameObject*)> func);

private:
    std::string name_;

    unsigned char layer_;//将物体分不同的层，用于相机分层、物理碰撞分层等。

    static std::list<GameObject*> game_object_list_;//存储所有的GameObject。

#ifdef USE_LUA_SCRIPT
public:
    /// 获取所有同名组件
    /// \param component_type_name 组件类名
    /// \return
    std::vector<Component*> GetComponents(std::string component_type_name);
public:
    /// 重载==，用于LuaRef对象做比较
    /// \param rhs
    /// \return
    bool operator==(GameObject* rhs) const;

    /// 在Lua中添加组件
    /// \param component_type_name cpp、lua的组件名称
    /// \return
    luabridge::LuaRef AddComponentFromLua(std::string component_type_name);

    /// 获取组件
    /// \param component_type_name
    /// \return
    luabridge::LuaRef GetComponentFromLua(std::string component_type_name);

    /// 遍历组件 一般在Update中使用
    /// \param func
    void ForeachLuaComponent(std::function<void(luabridge::LuaRef)> func);

private:
    std::unordered_map<std::string,std::vector<luabridge::LuaRef>> lua_component_type_instance_map_;//所有lua component
#else
public:
    /// 获取所有同名组件
    /// \param component_type_name 组件类名
    /// \return
    std::vector<Component*>& GetComponents(std::string component_type_name);

    /// 遍历所有Component
    /// \param func
    void ForeachComponent(std::function<void(Component* component)> func);
private:
    std::unordered_map<std::string,std::vector<Component*>> component_type_instance_map_;
#endif
};


#endif //UNTITLED_GAME_OBJECT_H
