#include "game_object.h"

lua_State* lua_state;

class Camera:public Component
{
public:
    void Awake() override{
        std::cout<<"Camera Awake"<<std::endl;
    }

    void Update() override{
        std::cout<<"Camera Update"<<std::endl;
    }
};

class Animator:public Component
{
public:
    void Awake() override{
        std::cout<<"Animator Awake"<<std::endl;
    }

    void Update() override{
        std::cout<<"Animator Update"<<std::endl;
    }
};
//注册反射
RTTR_REGISTRATION
{
    registration::class_<Camera>("Camera")
            .constructor<>()(rttr::policy::ctor::as_raw_ptr);
    registration::class_<Animator>("Animator")
            .constructor<>()(rttr::policy::ctor::as_raw_ptr);
}


int main(int argc, char * argv[])
{
    lua_state = luaL_newstate();
    luaL_openlibs(lua_state);

    luabridge::getGlobalNamespace(lua_state)
            .beginClass<Component>("Component")
            .addConstructor<void (*) ()>()
            .addFunction("Awake",&Component::Awake)
            .addFunction("Update",&Component::Update)
            .addFunction("game_object",&Component::game_object)
            .addFunction("set_game_object",&Component::set_game_object)
            .endClass();
    luabridge::getGlobalNamespace(lua_state)
            .beginClass<GameObject>("GameObject")
            .addConstructor<void (*) ()>()
            .addFunction("__eq", &GameObject::operator==)
            .addFunction("AddComponent", (luabridge::LuaRef (GameObject::*)(std::string))&GameObject::AddComponentFromLua)
            .addFunction("GetComponent",&GameObject::GetComponentFromLua)
            .endClass();
    luabridge::getGlobalNamespace(lua_state)
            .deriveClass<Animator,Component>("Animator")
            .addConstructor<void (*) ()>()
            .endClass();
    luabridge::getGlobalNamespace(lua_state)
            .deriveClass<Camera,Component>("Camera")
            .addConstructor<void (*) ()>()
            .endClass();

    luaL_dofile(lua_state, "../a.lua");

    //加上大括号，为了LuaRef在lua_close之前自动析构。
    {
        luabridge::LuaRef main_function = luabridge::getGlobal(lua_state, "main");
        try {
            main_function();
        } catch (const luabridge::LuaException& e) {
            std::cout<<e.what()<<std::endl;
        }
    }

    GameObject::Foreach([](GameObject* game_object){
        game_object->ForeachComponent([](Component* component){
            component->Update();
        });

        game_object->ForeachLuaComponent([](LuaRef lua_ref){
            LuaRef update_function_ref=lua_ref["Update"];
            if(update_function_ref.isFunction()){
                update_function_ref();
            }
        });
    });

    lua_close(lua_state);

    return 0;
}
