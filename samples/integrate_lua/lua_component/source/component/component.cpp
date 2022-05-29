//
// Created by captain on 2021/6/9.
//

#include "component.h"

//注册反射
RTTR_REGISTRATION
{
    registration::class_<Component>("Component")
            .constructor<>()(rttr::policy::ctor::as_raw_ptr);
}

Component::Component() {

}

Component::~Component() {

}

/// 同步调用Lua组件函数
/// \param function_name
void Component::SyncLuaComponent(const char* function_name){
    if (!lua_component_instance_.valid()){
        return;
    }
    sol::protected_function function_awake=lua_component_instance_[function_name];
    if(!function_awake.valid()){
        return;
    }
    auto result=function_awake(lua_component_instance_);
    if(result.valid()== false){
        sol::error err = result;
        type t=type::get(this);
        std::string component_type_name=t.get_name().to_string();
        DEBUG_LOG_ERROR("\n---- RUN LUA_FUNCTION ERROR ----\nComponent call Awake error,type:{}\n{}\n------------------------",component_type_name,err.what());
    }
}

void Component::OnEnable() {
    DEBUG_LOG_INFO("Cpp.Component OnEnable");
    SyncLuaComponent("OnEnable");
}

void Component::Awake() {
    DEBUG_LOG_INFO("Cpp.Component Awake");
    SyncLuaComponent("Awake");
}

void Component::Update() {
    DEBUG_LOG_INFO("Cpp.Component Update");
    SyncLuaComponent("Update");
}

void Component::OnPreRender() {
    DEBUG_LOG_INFO("Cpp.Component OnPreRender");
    SyncLuaComponent("OnPreRender");
}

void Component::OnPostRender() {
    DEBUG_LOG_INFO("Cpp.Component OnPostRender");
    SyncLuaComponent("OnPostRender");
}

void Component::OnDisable() {
    DEBUG_LOG_INFO("Cpp.Component OnDisable");
    SyncLuaComponent("OnDisable");
}
