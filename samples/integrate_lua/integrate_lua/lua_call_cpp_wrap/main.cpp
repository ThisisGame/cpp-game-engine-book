
extern "C"
{
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}
#include<string>
#include <memory>

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

static int CreatePlayer(lua_State* L)
{
    Player** pData = (Player**)lua_newuserdata(L, sizeof(Player*));
    *pData = new Player();
    luaL_getmetatable(L, "Player");
    lua_setmetatable(L, -2);
    return 1;
}

static int DestroyPlayer(lua_State* L)
{
    delete *(Player**)lua_topointer(L, 1);
    return 0;
}

static int CallAddHp(lua_State* L)
{
    Player* pPlayer = *(Player**)lua_topointer(L, 1);
    lua_pushnumber(L, pPlayer->AddHp(lua_tonumber(L, 2)));
    return 1;
}

static int lua_index(lua_State* L)
{
    Player* pPlayer = *(Player**)lua_topointer(L, 1);

    if (strcmp(lua_tostring(L, 2), "hp_") == 0)
    {
        lua_pushnumber(L, pPlayer->hp_);
    }
    else if (strcmp(lua_tostring(L, 2), "AddHp") == 0)
    {
        lua_pushcfunction(L, CallAddHp);
    }
    else
    {
        return 0;
    }
    return 1;
}

int main(int argc, char * argv[])
{
    lua_State *L = luaL_newstate();
    luaopen_base(L);

    lua_pushcfunction(L, CreatePlayer);
    lua_setglobal(L, "Player");

    luaL_newmetatable(L, "Player");

    lua_pushstring(L, "__gc");
    lua_pushcfunction(L, DestroyPlayer);
    lua_settable(L, -3);

    lua_pushstring(L, "__index");
    lua_pushcfunction(L, lua_index);
    lua_settable(L, -3);

    lua_pop(L, 1);

    luaL_dofile(L, "../a.lua");

    lua_close(L);

    return 0;
}
