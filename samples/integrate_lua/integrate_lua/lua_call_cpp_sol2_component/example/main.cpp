#include <iostream>
#include <string>
#include <sol/sol.hpp>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "../source/game_object.h"
#include "../source/key_action.h"
#include "../source/animator.h"
#include "../source/camera.h"

sol::state sol_state;


void CompareGameObject(GameObject* a,GameObject* b){
    std::cout<<"CompareGameObject a==b: "<<(a==b)<<std::endl;
}

int main(int argc, char * argv[])
{
    sol_state.open_libraries(sol::lib::base);

    //绑定glm::vec3
    {
        auto glm_ns_table = sol_state["glm"].get_or_create<sol::table>();
        glm_ns_table.new_usertype<glm::vec3>("vec3",sol::constructors<glm::vec3(const float&, const float&, const float&)>(),
                "x", &glm::vec3::x,
                "y", &glm::vec3::y,
                "z", &glm::vec3::z,
                "r", &glm::vec3::r,
                "g", &glm::vec3::g,
                "b", &glm::vec3::b,
                sol::meta_function::to_string,[] (const glm::vec3* vec) -> std::string {return glm::to_string(*vec);},
                sol::meta_function::addition,[] (const glm::vec3* vec_a,const  glm::vec3* vec_b) {return (*vec_a)+(*vec_b);},
                sol::meta_function::subtraction,[] (const glm::vec3* vec_a,const  glm::vec3* vec_b) {return (*vec_a)-(*vec_b);},
                sol::meta_function::multiplication,[] (const glm::vec3* vec,const float a) {return (*vec)*a;},
                sol::meta_function::division,[] (const glm::vec3* vec,const float a) {return (*vec)/a;},
                sol::meta_function::unary_minus,[] (const glm::vec3* vec) {return (*vec)*-1;},
                sol::meta_function::equal_to,[] (const glm::vec3* vec_a,const  glm::vec3* vec_b) {return (*vec_a)==(*vec_b);}
                );
    }

    //绑定glm::vec4
    {
        auto glm_ns_table = sol_state["glm"].get_or_create<sol::table>();
        glm_ns_table.new_usertype<glm::vec4>("vec4",sol::constructors<glm::vec4(const float&, const float&, const float&, const float&)>(),
                "x", &glm::vec4::x,
                "y", &glm::vec4::y,
                "z", &glm::vec4::z,
                "w", &glm::vec4::w,
                "r", &glm::vec4::r,
                "g", &glm::vec4::g,
                "b", &glm::vec4::b,
                "a", &glm::vec4::a,
                sol::meta_function::to_string,[] (const glm::vec4* vec) {return glm::to_string(*vec);},
                sol::meta_function::addition,[] (const glm::vec4* vec_a,const  glm::vec4* vec_b) {return (*vec_a)+(*vec_b);},
                sol::meta_function::subtraction,[] (const glm::vec4* vec_a,const  glm::vec4* vec_b) {return (*vec_a)-(*vec_b);},
                sol::meta_function::multiplication,[] (const glm::vec4* vec,const float a) {return (*vec)*a;},
                sol::meta_function::division,[] (const glm::vec4* vec,const float a) {return (*vec)/a;},
                sol::meta_function::unary_minus,[] (const glm::vec4* vec) {return (*vec)*-1;},
                sol::meta_function::equal_to,[] (const glm::vec4* vec_a,const  glm::vec4* vec_b) {return (*vec_a)==(*vec_b);}
                );
    }

    //绑定glm::mat4
    {
        luabridge::getGlobalNamespace(lua_state)
                .beginNamespace("glm")
                .beginClass<glm::mat4>("mat4")
                .addConstructor<void(*)(const float&)>()
                .addFunction ("__tostring", std::function <std::string (const glm::mat4*)> ([] (const glm::mat4* m) {return glm::to_string(*m);}))
                .addFunction ("__add", std::function <glm::mat4 (const glm::mat4*,const glm::mat4*)> ([] (const glm::mat4* m_a,const  glm::mat4* m_b) {return (*m_a)+(*m_b);}))
                .addFunction ("__sub", std::function <glm::mat4 (const glm::mat4*,const glm::mat4*)> ([] (const glm::mat4* m_a,const  glm::mat4* m_b) {return (*m_a)-(*m_b);}))
                .addFunction ("__mul", std::function <glm::vec4 (const glm::mat4*,const glm::vec4*)> ([] (const glm::mat4* m,const glm::vec4* v) {return (*m)*(*v);}))
                .addFunction ("__div", std::function <glm::mat4 (const glm::mat4*,const float)> ([] (const glm::mat4* m,const float a) {return (*m)/a;}))
                .addFunction ("__unm", std::function <glm::mat4 (const glm::mat4*)> ([] (const glm::mat4* m) {return (*m)*-1;}))
                .addFunction ("__eq", std::function <bool (const glm::mat4*,const glm::mat4*)> ([] (const glm::mat4* m_a,const  glm::mat4* m_b) {return (*m_a)==(*m_b);}))
                .endClass();
    }

    //绑定glm函数
    {
        luabridge::getGlobalNamespace(lua_state)
                .beginNamespace("glm")
//            .addFunction("to_string",std::function <std::string (const glm::mat4*)> ([] (const glm::mat4* m) {return glm::to_string((*m));}))
//            .addFunction("to_string",std::function <std::string (const glm::vec3*)> ([] (const glm::vec3* v) {return glm::to_string((*v));}))//同名覆盖，不支持函数重载。应该使用 __tostring注册。
                .addFunction("rotate",std::function <glm::mat4 (const glm::mat4*,const float,const glm::vec3*)> ([] (const glm::mat4* m,const float f,const glm::vec3* v) {return glm::rotate(*m,f,*v);}))
                .addFunction("radians",std::function <float (const float)> ([] (const float f) {return glm::radians(f);}));
    }

    //绑定 GameObject
    {
        luabridge::getGlobalNamespace(lua_state)
                .beginClass<GameObject>("GameObject")
                .addConstructor<void (*) ()>() //绑定 构造函数
                .addFunction("__eq", &GameObject::operator==)// 操作符重载
                .addFunction("AddComponent", &GameObject::AddComponentFromLua)//绑定 成员函数
                .addFunction("GetComponent",&GameObject::GetComponentFromLua)//绑定 成员函数
                .endClass();
    }

    //绑定 Component
    {
        luabridge::getGlobalNamespace(lua_state)
                .beginClass<Component>("Component")
                .addConstructor<void (*) ()>()
                .addFunction("Awake",&Component::Awake)
                .addFunction("Update",&Component::Update)
                .addFunction("game_object",&Component::game_object)
                .addFunction("set_game_object",&Component::set_game_object)
                .endClass();
    }

    //绑定 Animator
    {
        luabridge::getGlobalNamespace(lua_state)
                .deriveClass<Animator,Component>("Animator")
                .addConstructor<void (*) ()>()
                .endClass();
    }
    //绑定 Camera
    {
        luabridge::getGlobalNamespace(lua_state)
                .deriveClass<Camera,Component>("Camera")//指明继承自 Component
                .addConstructor<void (*) ()>()//绑定 构造函数
                .addFunction("position",&Camera::position)//绑定 子类函数
                .addFunction("set_position",&Camera::set_position)
                .endClass();
    }


    //绑定普通函数
    {
        luabridge::getGlobalNamespace(lua_state)
                .addFunction("CompareGameObject", &CompareGameObject);
    }

    //绑定常量
    {
        const int const_value=12;

        luabridge::getGlobalNamespace(lua_state)
                .beginNamespace("Test")
                .addConstant("const_value",const_value)
                .endNamespace();
    }

    //绑定枚举
    {
        luabridge::getGlobalNamespace(lua_state)
                .beginNamespace("KeyAction")
                .addConstant<std::size_t>("UP",KeyAction::UP)
                .addConstant<std::size_t>("DOWN",KeyAction::DOWN)
                .endNamespace();

        luabridge::getGlobalNamespace(lua_state)
                .addFunction("GetKeyActionUp",&GetKeyActionUp)
                .addFunction("GetKeyActionDown",&GetKeyActionDown);
    }



    //设置lua搜索目录
    {
        luabridge::LuaRef package_ref = luabridge::getGlobal(lua_state,"package");
        luabridge::LuaRef path_ref=package_ref["path"];
        std::string path=path_ref.tostring();
        path.append(";../example/?.lua;");
        package_ref["path"]=path;
    }

    luaL_dofile(lua_state, "../example/main.lua");

    //加上大括号，为了LuaRef在lua_close之前自动析构。
    {
        luabridge::LuaRef main_function = luabridge::getGlobal(lua_state, "main");
        try {
            main_function();
        } catch (const luabridge::LuaException& e) {
            std::cout<<"lua error: "<<e.what()<<std::endl;
        }
    }

    for(int i=0;i<3;i++){
        GameObject::Foreach([](GameObject* game_object){
            game_object->ForeachLuaComponent([](LuaRef lua_ref){
                LuaRef update_function_ref=lua_ref["Update"];
                if(update_function_ref.isFunction()){
                    update_function_ref(lua_ref);
                }
            });
        });
    }

    lua_close(lua_state);

    return 0;
}
