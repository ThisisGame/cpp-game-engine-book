//
// Created by captain on 2021/6/9.
// 使用std::weak_ptr无疑是最好的选择。然而用std::weak_ptr来打破std::shared_ptr引起的循环并不那么常见，所以这个方法也不值一提。
// 严格来讲层级数据结构，比如tree，孩子结点一般都只被父节点拥有，当父节点被销毁后，所有的孩子结点也都应该被销毁。
// 这样，从父节点到子节点的链接可以用std::unique_ptr来表示，而反过来从子节点到父节点的指针可以用原始指针来实现，因为子节点的生命周期不会比父节点的更长，所以不会出现子节点去解引用一个父节点的悬浮指针的情况。
//

#ifndef UNTITLED_COMPONENT_H
#define UNTITLED_COMPONENT_H

#include <rttr/registration>
#include <sol/sol.hpp>
#include "utils/debug.h"
using namespace rttr;

class GameObject;
class Component {
public:
    Component();
    virtual ~Component();

    GameObject* game_object(){return game_object_;}
    void set_game_object(GameObject* game_object){game_object_=game_object;}

    /// 设置对应的Lua组件
    /// \param lua_component_instance
    void set_lua_component_instance(sol::table& lua_component_instance){
        lua_component_instance_=lua_component_instance;
    }

private:
    /// 同步调用Lua组件函数
    /// \param function_name
    void SyncLuaComponent(const char* function_name,GameObject* game_object= nullptr);

public:
    virtual void OnEnable();
    virtual void Awake();
    virtual void Update();
    virtual void FixedUpdate();

    /// 渲染之前
    virtual void OnPreRender();
    /// 渲染之后
    virtual void OnPostRender();

    virtual void OnDisable();

    virtual void OnTriggerEnter(GameObject* game_object);

    virtual void OnTriggerExit(GameObject* game_object);

    virtual void OnTriggerStay(GameObject* game_object);
private:
    GameObject* game_object_;
    sol::table lua_component_instance_;

RTTR_ENABLE();
};


#endif //UNTITLED_COMPONENT_H
