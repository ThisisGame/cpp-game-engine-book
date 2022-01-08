// https://github.com/vinniefalco/LuaBridge
// Copyright 2020, Dmitry Tarakanov
// Copyright 2012, Vinnie Falco <vinnie.falco@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

namespace luabridge {

namespace detail {

/**
 * A unique key for a type name in a metatable.
 */
inline const void* getTypeKey()
{
#ifdef _NDEBUG
    static char value;
    return &value;
#else
    return reinterpret_cast<void*>(0x71);
#endif
}

/**
 * The key of a const table in another metatable.
 */
inline const void* getConstKey()
{
#ifdef _NDEBUG
    static char value;
    return &value;
#else
    return reinterpret_cast<void*>(0xc07);
#endif
}

/**
 * The key of a class table in another metatable.
 */
inline const void* getClassKey()
{
#ifdef _NDEBUG
    static char value;
    return &value;
#else
    return reinterpret_cast<void*>(0xc1a);
#endif
}

/**
 * The key of a propget table in another metatable.
 */
inline const void* getPropgetKey()
{
#ifdef _NDEBUG
    static char value;
    return &value;
#else
    return reinterpret_cast<void*>(0x6e7);
#endif
}

/**
 * The key of a propset table in another metatable.
 */
inline const void* getPropsetKey()
{
#ifdef _NDEBUG
    static char value;
    return &value;
#else
    return reinterpret_cast<void*>(0x5e7);
#endif
}

/**
 * The key of a static table in another metatable.
 */
inline const void* getStaticKey()
{
#ifdef _NDEBUG
    static char value;
    return &value;
#else
    return reinterpret_cast<void*>(0x57a);
#endif
}

/**
 * The key of a parent table in another metatable.
 */
inline const void* getParentKey()
{
#ifdef _NDEBUG
    static char value;
    return &value;
#else
    return reinterpret_cast<void*>(0xdad);
#endif
}

/**
    Get the key for the static table in the Lua registry.
    The static table holds the static data members, static properties, and
    static member functions for a class.
*/
template<class T>
void const* getStaticRegistryKey()
{
    static char value;
    return &value;
}

/** Get the key for the class table in the Lua registry.
    The class table holds the data members, properties, and member functions
    of a class. Read-only data and properties, and const member functions are
    also placed here (to save a lookup in the const table).
*/
template<class T>
void const* getClassRegistryKey()
{
    static char value;
    return &value;
}

/** Get the key for the const table in the Lua registry.
    The const table holds read-only data members and properties, and const
    member functions of a class.
*/
template<class T>
void const* getConstRegistryKey()
{
    static char value;
    return &value;
}

} // namespace detail

} // namespace luabridge
