//
// Created by captain on 2021/6/9.
//

#include "game_object.h"
#include <assert.h>
#include <rttr/registration>
#include "component.h"
#include "utils/debug.h"

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
    if(t.is_valid()==false){
        DEBUG_LOG_ERROR("type::get_by_name({}) failed",component_type_name);
        return nullptr;
    }

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

luabridge::LuaRef GameObject::AddComponent(luabridge::LuaRef component_type) {

    luabridge::LuaRef component_type_table=luabridge::getGlobal(LuaBinding::lua_state(),component_type_name.c_str());
    luabridge::LuaRef new_table=component_type_table();

    type t = type::get_by_name(component_type_name);
    if(t.is_valid()){
        Component* component=new_table.cast<Component*>();
        if(component== nullptr){
            DEBUG_LOG_ERROR("invalid component_type_name:{}",component_type_name);
            return luabridge::LuaRef(LuaBinding::lua_state());
        }

        return luabridge::LuaRef(LuaBinding::lua_state(),component);
    }

    if(lua_component_type_instance_map_.find(component_type_name)==lua_component_type_instance_map_.end()){
        std::vector<luabridge::LuaRef> component_vec;
        component_vec.push_back(new_table);
        lua_component_type_instance_map_[component_type_name]=component_vec;
    }else{
        lua_component_type_instance_map_[component_type_name].push_back(new_table);
    }

    luabridge::LuaRef awake_function=new_table["Awake"];
    awake_function();

    return new_table;
}

//按理说，cpp这边是不会去getcomponent lua 的 componet的。
//但是现在从将cpp class注册到lua，lua调用addcomponent，例如Camera，存到gameobject里的是lua的table，不是Component了。
//从c++端就getcomponenet不到了这个Camera。
//除非是说，上面的AddComponent，返回的new_table，仍然可以转换为cpp的Component，这样就可以兼容到cpp端去getComponent。
//居然真的可以Cast过来
//luabridge::LuaRef player_table=luabridge::getGlobal(lua_state,"player");
//if(player_table.isInstance<Player>()){
//Player* player=player_table.cast<Player*>();
//player->AddHp(4);
//}
//if(player_table.isInstance<Object>()){
//Object* object=player_table.cast<Object*>();
//object->OnDestroy();
//}
//这样的话，cpp端用模板做AddComponent GetComponent即可，既可以支持cpp的class，也可以支持注册到lua端的cpp class。当然，是不支持lua端的class的，不过完全没有这个需求。
//只有lua端才会需要get lua 的component。
//rttr可以废弃了。
//又少了一个依赖。

//有一个问题没法解决，GameObject用什么形式去存储cpp class类型以及多个实例的对英国厝。
//cpp class并不是一个变量。

//不过，lua的AddComponent直接传入字符串是最简单的。

//使用typeid可以解决这个问题，不管是cpp的类，cpp类的实例，注册到lua的cpp类的实例，都是相同的typeid，用typeid.hash作为key。
//cpp AddComponent的时候，对T获取typeid，然后new T，然后存入unorder_map。
//lua AddComponent的时候，对....传入过来的是LuaRef……获取的typeid并不是和cpp class一样的……
//这下AddComponent没法做了，和cpp的放不到一个map里面去。

//另外一种做法是，lua的Component单独存放为list，GetComponent的时候，再对这个list遍历一次，用isInstance<T>来检查是否是指定的T类型，然后返回符合T的数组。

//或者说在AddComponent的时候，对所有的cpp class都判断一遍，然后放到对应的map里面，感觉这个好一些吧，问题是要写很多if else，不过这样在get的时候就很方便了。

//另外是修改LuaBridge，在注册cpp class到lua的时候，将typeid().hashid写入到table中，这样AddComponent的时候只要获取出来就醒了。