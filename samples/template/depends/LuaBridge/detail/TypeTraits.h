// https://github.com/vinniefalco/LuaBridge
// Copyright 2019, Dmitry Tarakanov
// Copyright 2012, Vinnie Falco <vinnie.falco@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include <LuaBridge/detail/Config.h>

#include <string>

namespace luabridge {

//------------------------------------------------------------------------------
/**
    Container traits.

    Unspecialized ContainerTraits has the isNotContainer typedef for SFINAE.
    All user defined containers must supply an appropriate specialization for
    ContinerTraits (without the typedef isNotContainer). The containers that
    come with LuaBridge also come with the appropriate ContainerTraits
    specialization. See the corresponding declaration for details.

    A specialization of ContainerTraits for some generic type ContainerType
    looks like this:

        template <class T>
        struct ContainerTraits <ContainerType <T>>
        {
          typedef typename T Type;

          static T* get (ContainerType <T> const& c)
          {
            return c.get (); // Implementation-dependent on ContainerType
          }
        };
*/
template<class T>
struct ContainerTraits
{
    typedef bool isNotContainer;
    typedef T Type;
};

namespace detail {

//------------------------------------------------------------------------------
/**
    Type traits.

    Specializations return information about a type.
*/
struct TypeTraits
{
    /** Determine if type T is a container.

        To be considered a container, there must be a specialization of
        ContainerTraits with the required fields.
    */
    template<typename T>
    class isContainer
    {
    private:
        typedef char yes[1]; // sizeof (yes) == 1
        typedef char no[2]; // sizeof (no)  == 2

        template<typename C>
        static no& test(typename C::isNotContainer*);

        template<typename>
        static yes& test(...);

    public:
        static const bool value = sizeof(test<ContainerTraits<T>>(0)) == sizeof(yes);
    };

    /** Determine if T is const qualified.
     */
    /** @{ */
    template<class T>
    struct isConst
    {
        static bool const value = false;
    };

    template<class T>
    struct isConst<T const>
    {
        static bool const value = true;
    };
    /** @} */

    /** Remove the const qualifier from T.
     */
    /** @{ */
    template<class T>
    struct removeConst
    {
        typedef T Type;
    };

    template<class T>
    struct removeConst<T const>
    {
        typedef T Type;
    };
    /**@}*/
};

} // namespace detail

} // namespace luabridge
