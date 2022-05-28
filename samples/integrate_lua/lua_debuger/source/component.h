//
// Created by captainchen on 2021/9/16.
//

#ifndef TEST_COMPONENT_H
#define TEST_COMPONENT_H

#include <rttr/registration>
#include <sol/sol.hpp>
#include <spdlog/spdlog.h>

using namespace rttr;

class GameObject;
class Component {
public:
    Component():game_object_(nullptr){};
    virtual ~Component(){};

    GameObject* game_object(){return game_object_;}
    void set_game_object(GameObject* game_object){game_object_=game_object;}

    /// 设置对应的Lua组件
    /// \param lua_component_instance
    void set_lua_component_instance(sol::table& lua_component_instance){
        lua_component_instance_=lua_component_instance;
    }

    /// 同步调用Lua组件函数
    /// \param function_name
    void SyncLuaComponent(const char* function_name){
        if (!lua_component_instance_.valid()){
            return;
        }
        sol::protected_function function_awake=lua_component_instance_[function_name];
        if(!function_awake.valid()){
            return;
        }
        auto result=function_awake();
        if(result.valid()== false){
            sol::error err = result;
            type t=type::get(this);
            std::string component_type_name=t.get_name().to_string();
            SPDLOG_ERROR("\n---- RUN LUA_FUNCTION ERROR ----\nComponent call Awake error,type:{}\n{}\n------------------------",component_type_name,err.what());
        }
    }

    virtual void Awake(){
        SyncLuaComponent("Awake");
    };
    virtual void Update(){
        SyncLuaComponent("Update");
    };
private:
    GameObject* game_object_;
    sol::table lua_component_instance_;

RTTR_ENABLE();
};


#endif //TEST_COMPONENT_H
