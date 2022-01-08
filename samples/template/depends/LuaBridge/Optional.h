// https://github.com/vinniefalco/LuaBridge
// Copyright 2021, Stefan Frings
// SPDX-License-Identifier: MIT

#pragma once

#include <LuaBridge/detail/Stack.h>

#include <optional>

namespace luabridge {

template<class T>
struct Stack<std::optional<T>>
{
    static void push(lua_State* L, std::optional<T> const& optional)
    {
        if (optional)
        {
            Stack<T>::push(L, *optional);
        }
        else
        {
            lua_pushnil(L);
        }
    }

    static std::optional<T> get(lua_State* L, int index)
    {
        if (lua_isnil(L, index))
        {
            lua_pop(L, 1);

            return std::nullopt;
        }

        return Stack<T>::get(L, index);
    }

    static bool isInstance(lua_State* L, int index)
    {
        return lua_isnil(L, index) || Stack<T>::isInstance(L, index);
    }
};

} // namespace luabridge
