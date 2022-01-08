// https://github.com/vinniefalco/LuaBridge
// Copyright 2020, Dmitry Tarakanov
// Copyright 2012, Vinnie Falco <vinnie.falco@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include <LuaBridge/detail/Config.h>
#include <LuaBridge/detail/TypeList.h>

#include <functional>

namespace luabridge {

namespace detail {

/**
  Since the throw specification is part of a function signature, the FuncTraits
  family of templates needs to be specialized for both types. The
  LUABRIDGE_THROWSPEC macro controls whether we use the 'throw ()' form, or
  'noexcept' (if C++11 is available) to distinguish the functions.
*/
#if defined(__APPLE_CPP__) || defined(__APPLE_CC__) || defined(__clang__) || defined(__GNUC__) || \
    (defined(_MSC_VER) && (_MSC_VER >= 1700))
// Do not define LUABRIDGE_THROWSPEC since the Xcode and gcc  compilers do not
// distinguish the throw specification in the function signature.
#define LUABRIDGE_THROWSPEC
#else
// Visual Studio 10 and earlier pay too much mind to useless throw () spec.
//
#define LUABRIDGE_THROWSPEC throw()
#endif

//==============================================================================
/**
 * Traits class for unrolling the type list values into function arguments.
 */
template<class ReturnType, size_t NUM_PARAMS>
struct Caller;

template<class ReturnType>
struct Caller<ReturnType, 0>
{
    template<class Fn, class Params>
    static ReturnType f(Fn& fn, TypeListValues<Params>& params)
    {
        return fn();
    }

    template<class T, class MemFn, class Params>
    static ReturnType f(T* obj, MemFn& fn, TypeListValues<Params>&)
    {
        return (obj->*fn)();
    }
};

template<class ReturnType>
struct Caller<ReturnType, 1>
{
    template<class Fn, class Params>
    static ReturnType f(Fn& fn, TypeListValues<Params>& tvl)
    {
        return fn(tvl.hd);
    }

    template<class T, class MemFn, class Params>
    static ReturnType f(T* obj, MemFn& fn, TypeListValues<Params>& tvl)
    {
        return (obj->*fn)(tvl.hd);
    }
};

template<class ReturnType>
struct Caller<ReturnType, 2>
{
    template<class Fn, class Params>
    static ReturnType f(Fn& fn, TypeListValues<Params>& tvl)
    {
        return fn(tvl.hd, tvl.tl.hd);
    }

    template<class T, class MemFn, class Params>
    static ReturnType f(T* obj, MemFn& fn, TypeListValues<Params>& tvl)
    {
        return (obj->*fn)(tvl.hd, tvl.tl.hd);
    }
};

template<class ReturnType>
struct Caller<ReturnType, 3>
{
    template<class Fn, class Params>
    static ReturnType f(Fn& fn, TypeListValues<Params>& tvl)
    {
        return fn(tvl.hd, tvl.tl.hd, tvl.tl.tl.hd);
    }

    template<class T, class MemFn, class Params>
    static ReturnType f(T* obj, MemFn& fn, TypeListValues<Params>& tvl)
    {
        return (obj->*fn)(tvl.hd, tvl.tl.hd, tvl.tl.tl.hd);
    }
};

template<class ReturnType>
struct Caller<ReturnType, 4>
{
    template<class Fn, class Params>
    static ReturnType f(Fn& fn, TypeListValues<Params>& tvl)
    {
        return fn(tvl.hd, tvl.tl.hd, tvl.tl.tl.hd, tvl.tl.tl.tl.hd);
    }

    template<class T, class MemFn, class Params>
    static ReturnType f(T* obj, MemFn& fn, TypeListValues<Params>& tvl)
    {
        return (obj->*fn)(tvl.hd, tvl.tl.hd, tvl.tl.tl.hd, tvl.tl.tl.tl.hd);
    }
};

template<class ReturnType>
struct Caller<ReturnType, 5>
{
    template<class Fn, class Params>
    static ReturnType f(Fn& fn, TypeListValues<Params>& tvl)
    {
        return fn(tvl.hd, tvl.tl.hd, tvl.tl.tl.hd, tvl.tl.tl.tl.hd, tvl.tl.tl.tl.tl.hd);
    }

    template<class T, class MemFn, class Params>
    static ReturnType f(T* obj, MemFn& fn, TypeListValues<Params>& tvl)
    {
        return (obj->*fn)(tvl.hd, tvl.tl.hd, tvl.tl.tl.hd, tvl.tl.tl.tl.hd, tvl.tl.tl.tl.tl.hd);
    }
};

template<class ReturnType>
struct Caller<ReturnType, 6>
{
    template<class Fn, class Params>
    static ReturnType f(Fn& fn, TypeListValues<Params>& tvl)
    {
        return fn(tvl.hd,
                  tvl.tl.hd,
                  tvl.tl.tl.hd,
                  tvl.tl.tl.tl.hd,
                  tvl.tl.tl.tl.tl.hd,
                  tvl.tl.tl.tl.tl.tl.hd);
    }

    template<class T, class MemFn, class Params>
    static ReturnType f(T* obj, MemFn& fn, TypeListValues<Params>& tvl)
    {
        return (obj->*fn)(tvl.hd,
                          tvl.tl.hd,
                          tvl.tl.tl.hd,
                          tvl.tl.tl.tl.hd,
                          tvl.tl.tl.tl.tl.hd,
                          tvl.tl.tl.tl.tl.tl.hd);
    }
};

template<class ReturnType>
struct Caller<ReturnType, 7>
{
    template<class Fn, class Params>
    static ReturnType f(Fn& fn, TypeListValues<Params>& tvl)
    {
        return fn(tvl.hd,
                  tvl.tl.hd,
                  tvl.tl.tl.hd,
                  tvl.tl.tl.tl.hd,
                  tvl.tl.tl.tl.tl.hd,
                  tvl.tl.tl.tl.tl.tl.hd,
                  tvl.tl.tl.tl.tl.tl.tl.hd);
    }

    template<class T, class MemFn, class Params>
    static ReturnType f(T* obj, MemFn& fn, TypeListValues<Params>& tvl)
    {
        return (obj->*fn)(tvl.hd,
                          tvl.tl.hd,
                          tvl.tl.tl.hd,
                          tvl.tl.tl.tl.hd,
                          tvl.tl.tl.tl.tl.hd,
                          tvl.tl.tl.tl.tl.tl.hd,
                          tvl.tl.tl.tl.tl.tl.tl.hd);
    }
};

template<class ReturnType>
struct Caller<ReturnType, 8>
{
    template<class Fn, class Params>
    static ReturnType f(Fn& fn, TypeListValues<Params>& tvl)
    {
        return fn(tvl.hd,
                  tvl.tl.hd,
                  tvl.tl.tl.hd,
                  tvl.tl.tl.tl.hd,
                  tvl.tl.tl.tl.tl.hd,
                  tvl.tl.tl.tl.tl.tl.hd,
                  tvl.tl.tl.tl.tl.tl.tl.hd,
                  tvl.tl.tl.tl.tl.tl.tl.tl.hd);
    }

    template<class T, class MemFn, class Params>
    static ReturnType f(T* obj, MemFn& fn, TypeListValues<Params>& tvl)
    {
        return (obj->*fn)(tvl.hd,
                          tvl.tl.hd,
                          tvl.tl.tl.hd,
                          tvl.tl.tl.tl.hd,
                          tvl.tl.tl.tl.tl.hd,
                          tvl.tl.tl.tl.tl.tl.hd,
                          tvl.tl.tl.tl.tl.tl.tl.hd,
                          tvl.tl.tl.tl.tl.tl.tl.tl.hd);
    }
};

template<class ReturnType>
struct Caller<ReturnType, 9>
{
    template<class Fn, class Params>
    static ReturnType f(Fn& fn, TypeListValues<Params>& tvl)
    {
        return fn(tvl.hd,
                  tvl.tl.hd,
                  tvl.tl.tl.hd,
                  tvl.tl.tl.tl.hd,
                  tvl.tl.tl.tl.tl.hd,
                  tvl.tl.tl.tl.tl.tl.hd,
                  tvl.tl.tl.tl.tl.tl.tl.hd,
                  tvl.tl.tl.tl.tl.tl.tl.tl.hd,
                  tvl.tl.tl.tl.tl.tl.tl.tl.tl.hd);
    }

    template<class T, class MemFn, class Params>
    static ReturnType f(T* obj, MemFn& fn, TypeListValues<Params>& tvl)
    {
        return (obj->*fn)(tvl.hd,
                          tvl.tl.hd,
                          tvl.tl.tl.hd,
                          tvl.tl.tl.tl.hd,
                          tvl.tl.tl.tl.tl.hd,
                          tvl.tl.tl.tl.tl.tl.hd,
                          tvl.tl.tl.tl.tl.tl.tl.hd,
                          tvl.tl.tl.tl.tl.tl.tl.tl.hd,
                          tvl.tl.tl.tl.tl.tl.tl.tl.tl.hd);
    }
};

template<class ReturnType, class Fn, class Params>
ReturnType doCall(Fn& fn, TypeListValues<Params>& tvl)
{
    return Caller<ReturnType, TypeListSize<Params>::value>::f(fn, tvl);
}

template<class ReturnType, class T, class MemFn, class Params>
ReturnType doCall(T* obj, MemFn& fn, TypeListValues<Params>& tvl)
{
    return Caller<ReturnType, TypeListSize<Params>::value>::f(obj, fn, tvl);
}

//==============================================================================
/**
    Traits for function pointers.

    There are three types of functions: global, non-const member, and const
    member. These templates determine the type of function, which class type it
    belongs to if it is a class member, the const-ness if it is a member
    function, and the type information for the return value and argument list.

    Expansions are provided for functions with up to 8 parameters. This can be
    manually extended, or expanded to an arbitrary amount using C++11 features.
*/
template<class MemFn, class D = MemFn>
struct FuncTraits
{
};

/* Ordinary function pointers. */

template<class R, class... ParamList>
struct FuncTraits<R (*)(ParamList...)>
{
    static bool const isMemberFunction = false;
    using DeclType = R (*)(ParamList...);
    using ReturnType = R;
    using Params = typename MakeTypeList<ParamList...>::Result;

    static R call(const DeclType& fp, TypeListValues<Params>& tvl) { return doCall<R>(fp, tvl); }
};

/* Windows: WINAPI (a.k.a. __stdcall) function pointers. */

#ifdef _M_IX86 // Windows 32bit only

template<class R, class... ParamList>
struct FuncTraits<R(__stdcall*)(ParamList...)>
{
    static bool const isMemberFunction = false;
    using DeclType = R(__stdcall*)(ParamList...);
    using ReturnType = R;
    using Params = typename MakeTypeList<ParamList...>::Result;

    static R call(const DeclType& fp, TypeListValues<Params>& tvl) { return doCall<R>(fp, tvl); }
};

#endif // _M_IX86

/* Non-const member function pointers. */

template<class T, class R, class... ParamList>
struct FuncTraits<R (T::*)(ParamList...)>
{
    static bool const isMemberFunction = true;
    static bool const isConstMemberFunction = false;
    using DeclType = R (T::*)(ParamList...);
    using ClassType = T;
    using ReturnType = R;
    using Params = typename MakeTypeList<ParamList...>::Result;

    static R call(ClassType* obj, const DeclType& fp, TypeListValues<Params>& tvl)
    {
        return doCall<R>(obj, fp, tvl);
    }
};

/* Const member function pointers. */

template<class T, class R, class... ParamList>
struct FuncTraits<R (T::*)(ParamList...) const>
{
    static bool const isMemberFunction = true;
    static bool const isConstMemberFunction = true;
    using DeclType = R (T::*)(ParamList...) const;
    using ClassType = T;
    using ReturnType = R;
    using Params = typename MakeTypeList<ParamList...>::Result;

    static R call(const ClassType* obj, const DeclType& fp, TypeListValues<Params>& tvl)
    {
        return doCall<R>(obj, fp, tvl);
    }
};

/* std::function */

template<class R, class... ParamList>
struct FuncTraits<std::function<R(ParamList...)>>
{
    static bool const isMemberFunction = false;
    static bool const isConstMemberFunction = false;
    using DeclType = std::function<R(ParamList...)>;
    using ReturnType = R;
    using Params = typename MakeTypeList<ParamList...>::Result;

    static ReturnType call(DeclType& fn, TypeListValues<Params>& tvl)
    {
        return doCall<ReturnType>(fn, tvl);
    }
};

template<class ReturnType, class Params, int startParam>
struct Invoke
{
    template<class Fn>
    static int run(lua_State* L, Fn& fn)
    {
        try
        {
            ArgList<Params, startParam> args(L);
            Stack<ReturnType>::push(L, FuncTraits<Fn>::call(fn, args));
            return 1;
        }
        catch (const std::exception& e)
        {
            return luaL_error(L, e.what());
        }
    }

    template<class T, class MemFn>
    static int run(lua_State* L, T* object, const MemFn& fn)
    {
        try
        {
            ArgList<Params, startParam> args(L);
            Stack<ReturnType>::push(L, FuncTraits<MemFn>::call(object, fn, args));
            return 1;
        }
        catch (const std::exception& e)
        {
            return luaL_error(L, e.what());
        }
    }
};

template<class Params, int startParam>
struct Invoke<void, Params, startParam>
{
    template<class Fn>
    static int run(lua_State* L, Fn& fn)
    {
        try
        {
            ArgList<Params, startParam> args(L);
            FuncTraits<Fn>::call(fn, args);
            return 0;
        }
        catch (const std::exception& e)
        {
            return luaL_error(L, e.what());
        }
    }

    template<class T, class MemFn>
    static int run(lua_State* L, T* object, const MemFn& fn)
    {
        try
        {
            ArgList<Params, startParam> args(L);
            FuncTraits<MemFn>::call(object, fn, args);
            return 0;
        }
        catch (const std::exception& e)
        {
            return luaL_error(L, e.what());
        }
    }
};

} // namespace detail

} // namespace luabridge
