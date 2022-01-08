#include "../source/game_object.h"
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "../source/key_action.h"
#include "../source/animator.h"
#include "../source/camera.h"

lua_State* lua_state;



// 注册枚举
template <typename T>
struct EnumWrapper {
    static typename std::enable_if<std::is_enum<T>::value, void>::type push(lua_State* L, T value){
        lua_pushnumber (L, static_cast<std::size_t> (value));
    }
    static typename std::enable_if<std::is_enum<T>::value, T>::type get(lua_State* L, int index){
        return static_cast <T> (lua_tointeger (L, index));
    }
};

namespace luabridge {
    template <>
    struct Stack<KeyAction> : EnumWrapper<KeyAction>{};
} // namespace luabridge


void CompareGameObject(GameObject* a,GameObject* b){
    std::cout<<"CompareGameObject a==b: "<<(a==b)<<std::endl;
}

int main(int argc, char * argv[])
{
    lua_state = luaL_newstate();
    luaL_openlibs(lua_state);

    //绑定glm::vec3
    {
        luabridge::getGlobalNamespace(lua_state)
                .beginNamespace("glm")//指定NameSpace
                .beginClass<glm::vec3>("vec3")//绑定类
                .addConstructor<void(*)(const float&, const float&, const float&)>()//绑定 构造函数
                .addData("x", &glm::vec3::x)//绑定 成员变量
                .addData("y", &glm::vec3::y)
                .addData("z", &glm::vec3::z)
                .addData("r", &glm::vec3::r)
                .addData("g", &glm::vec3::g)
                .addData("b", &glm::vec3::b)
                .addFunction ("__tostring", std::function <std::string (const glm::vec3*)> ([] (const glm::vec3* vec) {return glm::to_string(*vec);}))//指定Lua Meta Function
                .addFunction ("__add", std::function <glm::vec3 (const glm::vec3*,const glm::vec3*)> ([] (const glm::vec3* vec_a,const  glm::vec3* vec_b) {return (*vec_a)+(*vec_b);}))
                .addFunction ("__sub", std::function <glm::vec3 (const glm::vec3*,const glm::vec3*)> ([] (const glm::vec3* vec_a,const  glm::vec3* vec_b) {return (*vec_a)-(*vec_b);}))
                .addFunction ("__mul", std::function <glm::vec3 (const glm::vec3*,const float)> ([] (const glm::vec3* vec,const float a) {return (*vec)*a;}))
                .addFunction ("__div", std::function <glm::vec3 (const glm::vec3*,const float)> ([] (const glm::vec3* vec,const float a) {return (*vec)/a;}))
                .addFunction ("__unm", std::function <glm::vec3 (const glm::vec3*)> ([] (const glm::vec3* vec) {return (*vec)*-1;}))
                .addFunction ("__eq", std::function <bool (const glm::vec3*,const glm::vec3*)> ([] (const glm::vec3* vec_a,const  glm::vec3* vec_b) {return (*vec_a)==(*vec_b);}))
                .endClass();
    }

    //绑定glm::vec4
    {
        luabridge::getGlobalNamespace(lua_state)
                .beginNamespace("glm")
                .beginClass<glm::vec4>("vec4")
                .addConstructor<void(*)(const float&, const float&, const float&, const float&)>()
                .addData("x", &glm::vec4::x)
                .addData("y", &glm::vec4::y)
                .addData("z", &glm::vec4::z)
                .addData("w", &glm::vec4::w)
                .addData("r", &glm::vec4::r)
                .addData("g", &glm::vec4::g)
                .addData("b", &glm::vec4::b)
                .addData("a", &glm::vec4::a)
                .addFunction ("__tostring", std::function <std::string (const glm::vec4*)> ([] (const glm::vec4* vec) {return glm::to_string(*vec);}))
                .addFunction ("__add", std::function <glm::vec4 (const glm::vec4*,const glm::vec4*)> ([] (const glm::vec4* vec_a,const  glm::vec4* vec_b) {return (*vec_a)+(*vec_b);}))
                .addFunction ("__sub", std::function <glm::vec4 (const glm::vec4*,const glm::vec4*)> ([] (const glm::vec4* vec_a,const  glm::vec4* vec_b) {return (*vec_a)-(*vec_b);}))
                .addFunction ("__mul", std::function <glm::vec4 (const glm::vec4*,const float)> ([] (const glm::vec4* vec,const float a) {return (*vec)*a;}))
                .addFunction ("__div", std::function <glm::vec4 (const glm::vec4*,const float)> ([] (const glm::vec4* vec,const float a) {return (*vec)/a;}))
                .addFunction ("__unm", std::function <glm::vec4 (const glm::vec4*)> ([] (const glm::vec4* vec) {return (*vec)*-1;}))
                .addFunction ("__eq", std::function <bool (const glm::vec4*,const glm::vec4*)> ([] (const glm::vec4* vec_a,const  glm::vec4* vec_b) {return (*vec_a)==(*vec_b);}))
                .endClass();
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
