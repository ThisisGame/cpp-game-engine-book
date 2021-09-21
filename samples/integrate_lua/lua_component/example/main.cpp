#define GLFW_INCLUDE_NONE

#if USE_LUA_SCRIPT
#include "utils/debug.h"
#include "lua_binding/lua_binding.h"

int main(void)
{
    lua_State* lua_state = luaL_newstate();
    luaL_openlibs(lua_state);

    //加上大括号，为了LuaRef在lua_close之前自动析构。
    {
        //设置lua搜索目录
        luabridge::LuaRef package_ref = luabridge::getGlobal(lua_state,"package");
        luabridge::LuaRef path_ref=package_ref["path"];
        std::string path=path_ref.tostring();
        path.append(";..\\example\\?.lua;");
        package_ref["path"]=path;

        LuaBinding::BindLua(lua_state);//绑定引擎所有类到Lua

        luaL_dofile(lua_state, "../example/main.lua");

        luabridge::LuaRef main_function = luabridge::getGlobal(lua_state, "main");
        try {
            main_function();
        } catch (const luabridge::LuaException& e) {
            DEBUG_LOG_ERROR(e.what());
        }
    }


    lua_close(lua_state);

    return 0;
}

#else
#include "component/game_object.h"
#include "utils/application.h"

int main(void){
    Application::set_title("[loadbank] press s play event,press 1 2 3 set param");
    Application::set_data_path("../data/");//设置资源目录

    Application::Init();//初始化引擎

    GameObject* go=new GameObject("LoginSceneGo");
    go->AddComponent("Transform");
    go->AddComponent("LoginScene");

    Application::Run();//开始引擎主循环
}
#endif

