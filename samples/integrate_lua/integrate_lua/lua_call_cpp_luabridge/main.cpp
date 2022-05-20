extern "C"
{
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}
#include "LuaBridge\LuaBridge.h"
#include<string>

#include <stdio.h>
#include <string.h>

using namespace std;

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

    lua_close(lua_state);

    return 0;
}
