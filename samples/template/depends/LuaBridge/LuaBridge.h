// https://github.com/vinniefalco/LuaBridge
// Copyright 2020, Dmitry Tarakanov
// Copyright 2012, Vinnie Falco <vinnie.falco@gmail.com>
// Copyright 2007, Nathan Reed
// SPDX-License-Identifier: MIT

#pragma once

// All #include dependencies are listed here
// instead of in the individual header files.
//

#define LUABRIDGE_MAJOR_VERSION 2
#define LUABRIDGE_MINOR_VERSION 7
#define LUABRIDGE_VERSION 207

#ifndef LUA_VERSION_NUM
#error "Lua headers must be included prior to LuaBridge ones"
#endif

#include <LuaBridge/detail/CFunctions.h>
#include <LuaBridge/detail/ClassInfo.h>
#include <LuaBridge/detail/Constructor.h>
#include <LuaBridge/detail/FuncTraits.h>
#include <LuaBridge/detail/Iterator.h>
#include <LuaBridge/detail/LuaException.h>
#include <LuaBridge/detail/LuaHelpers.h>
#include <LuaBridge/detail/LuaRef.h>
#include <LuaBridge/detail/Namespace.h>
#include <LuaBridge/detail/Security.h>
#include <LuaBridge/detail/Stack.h>
#include <LuaBridge/detail/TypeList.h>
#include <LuaBridge/detail/TypeTraits.h>
#include <LuaBridge/detail/Userdata.h>
