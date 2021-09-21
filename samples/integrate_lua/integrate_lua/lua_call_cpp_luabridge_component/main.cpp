#include "game_object.h"
#include <glm/glm.hpp>
#include <glm/ext.hpp>

lua_State* lua_state;

class Camera:public Component
{
public:
    void Awake() override{
        std::cout<<"Camera Awake"<<std::endl;
    }

    void Update() override{
//        std::cout<<"Camera Update"<<std::endl;
    }

    void set_position(glm::vec3 position){
        std::cout<<"Camera set_position:"<<glm::to_string(position)<<std::endl;
        position_=position;
    }

    glm::vec3 position(){
        return position_;
    }

private:
    glm::vec3 position_;
};

class Animator:public Component
{
public:
    void Awake() override{
        std::cout<<"Animator Awake"<<std::endl;
    }

    void Update() override{
//        std::cout<<"Animator Update"<<std::endl;
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

const int const_value=12;

typedef enum KeyAction{
    UP=0,
    DOWN=1,
    REPEAT=2
}KeyAction;

KeyAction GetKeyActionUp(){
    return KeyAction::UP;
}

KeyAction GetKeyActionDown(){
    return KeyAction::DOWN;
}

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

// Lua CFunction wrapper for StartWith.
int Lua_StartWith(lua_State* L) {

    return 2;  // 返回值有两个
}

GameObject* game_object_;
void SetGameObject(GameObject* game_object){
    std::cout<<"SetGameObject: "<<std::endl;
    game_object_=game_object;
}

int main(int argc, char * argv[])
{
    lua_state = luaL_newstate();
    luaL_openlibs(lua_state);

    // binding
    {
        luabridge::getGlobalNamespace(lua_state)
            .addFunction("SetGameObject",&SetGameObject);

        luabridge::getGlobalNamespace(lua_state)
                .beginNamespace("Test")
                .addConstant("const_value",const_value)
                .endNamespace();

        luabridge::getGlobalNamespace(lua_state)
                .beginNamespace("KeyAction")
                .addConstant<std::size_t>("UP",KeyAction::UP)
                .addConstant<std::size_t>("DOWN",KeyAction::DOWN)
                .endNamespace();

        luabridge::getGlobalNamespace(lua_state)
                .addFunction("GetKeyActionUp",&GetKeyActionUp)
                .addFunction("GetKeyActionDown",&GetKeyActionDown);

        luabridge::getGlobalNamespace(lua_state)
                .beginNamespace("glm")
                .beginClass<glm::vec3>("vec3")
                .addConstructor<void(*)(const float&, const float&, const float&)>()
                .addData("x", &glm::vec3::x)
                .addData("y", &glm::vec3::y)
                .addData("z", &glm::vec3::z)
                .addData("r", &glm::vec3::r)
                .addData("g", &glm::vec3::g)
                .addData("b", &glm::vec3::b)
                .addFunction ("__tostring", std::function <std::string (const glm::vec3*)> ([] (const glm::vec3* vec) {return glm::to_string(*vec);}))
                .addFunction ("__add", std::function <glm::vec3 (const glm::vec3*,const glm::vec3*)> ([] (const glm::vec3* vec_a,const  glm::vec3* vec_b) {return (*vec_a)+(*vec_b);}))
                .addFunction ("__sub", std::function <glm::vec3 (const glm::vec3*,const glm::vec3*)> ([] (const glm::vec3* vec_a,const  glm::vec3* vec_b) {return (*vec_a)-(*vec_b);}))
                .addFunction ("__mul", std::function <glm::vec3 (const glm::vec3*,const float)> ([] (const glm::vec3* vec,const float a) {return (*vec)*a;}))
                .addFunction ("__div", std::function <glm::vec3 (const glm::vec3*,const float)> ([] (const glm::vec3* vec,const float a) {return (*vec)/a;}))
                .addFunction ("__unm", std::function <glm::vec3 (const glm::vec3*)> ([] (const glm::vec3* vec) {return (*vec)*-1;}))
                .addFunction ("__eq", std::function <bool (const glm::vec3*,const glm::vec3*)> ([] (const glm::vec3* vec_a,const  glm::vec3* vec_b) {return (*vec_a)==(*vec_b);}))
                .endClass();
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
        luabridge::getGlobalNamespace(lua_state)
                .beginNamespace("glm")
//            .addFunction("to_string",std::function <std::string (const glm::mat4*)> ([] (const glm::mat4* m) {return glm::to_string((*m));}))
//            .addFunction("to_string",std::function <std::string (const glm::vec3*)> ([] (const glm::vec3* v) {return glm::to_string((*v));}))//同名覆盖，不支持函数重载。应该使用 __tostring注册。
                .addFunction("rotate",std::function <glm::mat4 (const glm::mat4*,const float,const glm::vec3*)> ([] (const glm::mat4* m,const float f,const glm::vec3* v) {return glm::rotate(*m,f,*v);}))
                .addFunction("radians",std::function <float (const float)> ([] (const float f) {return glm::radians(f);}));

        luabridge::getGlobalNamespace(lua_state)
                .beginClass<GameObject>("GameObject")
                .addConstructor<void (*) ()>()
                .addFunction("__eq", &GameObject::operator==)
                .addFunction("AddComponent", (luabridge::LuaRef (GameObject::*)(std::string))&GameObject::AddComponentFromLua)
                .addFunction("GetComponent",&GameObject::GetComponentFromLua)
                .addFunction("test_set",&GameObject::test_set)
                .endClass();
        luabridge::getGlobalNamespace(lua_state)
                .beginClass<Component>("Component")
                .addConstructor<void (*) ()>()
                .addFunction("Awake",&Component::Awake)
                .addFunction("Update",&Component::Update)
                .addFunction("game_object",&Component::game_object)
                .addFunction("set_game_object",&Component::set_game_object)
                .endClass();
        luabridge::getGlobalNamespace(lua_state)
                .deriveClass<Animator,Component>("Animator")
                .addConstructor<void (*) ()>()
                .endClass();
        luabridge::getGlobalNamespace(lua_state)
                .deriveClass<Camera,Component>("Camera")
                .addConstructor<void (*) ()>()
                .addFunction("position",&Camera::position)
                .addFunction("set_position",&Camera::set_position)
                .endClass();
    }


    //设置lua搜索目录
    {
        luabridge::LuaRef package_ref = luabridge::getGlobal(lua_state,"package");
        luabridge::LuaRef path_ref=package_ref["path"];
        std::string path=path_ref.tostring();
        path.append(";../?.lua;");
        package_ref["path"]=path;
    }

    luaL_dofile(lua_state, "../a.lua");

    //加上大括号，为了LuaRef在lua_close之前自动析构。
    {
        luabridge::LuaRef main_function = luabridge::getGlobal(lua_state, "main");
        try {
            main_function();
        } catch (const luabridge::LuaException& e) {
            std::cout<<"lua error: "<<e.what()<<std::endl;
        }
    }

    while (true){
        GameObject::Foreach([](GameObject* game_object){
            game_object->ForeachLuaComponent([](LuaRef lua_ref){
                LuaRef update_function_ref=lua_ref["Update"];
                if(update_function_ref.isFunction()){
                    update_function_ref(lua_ref);
                }
            });
        });

        if(game_object_== nullptr){
            std::cout<<"game_object_ gc"<<std::endl;
        }
    }



//    auto game_object=new GameObject();
//    auto component=game_object->AddComponent("Animator");

    lua_close(lua_state);

    return 0;
}
