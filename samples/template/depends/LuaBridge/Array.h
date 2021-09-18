// https://github.com/vinniefalco/LuaBridge
//
// Copyright 2020, Dmitry Tarakanov
// SPDX-License-Identifier: MIT

#pragma once

#include <LuaBridge/detail/Stack.h>

#include <array>

namespace luabridge {

template<class T, std::size_t s>
struct Stack<std::array<T, s>>
{
    static void push(lua_State* L, std::array<T, s> const& array)
    {
        lua_createtable(L, static_cast<int>(s), 0);
        for (std::size_t i = 0; i < s; ++i)
        {
            lua_pushinteger(L, static_cast<lua_Integer>(i + 1));
            Stack<T>::push(L, array[i]);
            lua_settable(L, -3);
        }
    }

    static std::array<T, s> get(lua_State* L, int index)
    {
        if (!lua_istable(L, index))
        {
            luaL_error(L, "#%d argments must be table", index);
        }

        std::size_t const tableSize = static_cast<std::size_t>(get_length(L, index));

        if (tableSize != s)
        {
            luaL_error(L, "array size should be %d ", s);
        }

        std::array<T, s> array;

        int const absindex = lua_absindex(L, index);
        lua_pushnil(L);
        int arrayIndex = 0;
        while (lua_next(L, absindex) != 0)
        {
            array[arrayIndex] = Stack<T>::get(L, -1);
            lua_pop(L, 1);
            ++arrayIndex;
        }
        return array;
    }
};

} // namespace luabridge
