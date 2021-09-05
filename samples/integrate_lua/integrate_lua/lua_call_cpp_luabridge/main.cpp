extern "C"
{
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}
#include "LuaBridge\LuaBridge.h"
#include "LuaBridge/detail/LuaException.h"
#include "LuaBridge/detail/LuaRef.h"
#include<string>

#include <stdio.h>
#include <string.h>

using namespace std;
using namespace luabridge;

class Player
{
public:
    int AddHp(int add)
    {
        hp_+=add;
        return hp_;
    }

    int hp_=0;
};

int main(int argc, char * argv[])
{
    lua_State* lua_state = luaL_newstate();
    luaL_openlibs(lua_state);

    luabridge::getGlobalNamespace(lua_state)
            .beginClass<Player>("Player")
            .addConstructor<void (*) (void)>()
            .addFunction("AddHp", &Player::AddHp)
            .addData("hp_",&Player::hp_)
            .endClass();

    luaL_dofile(lua_state, "../a.lua");

    LuaRef mainFunction = luabridge::getGlobal(lua_state,"fail");

    try {
        mainFunction();
    } catch (const luabridge::LuaException& e) {
        std::cout << e.what();
    }

    lua_close(lua_state);

    return 0;
}
