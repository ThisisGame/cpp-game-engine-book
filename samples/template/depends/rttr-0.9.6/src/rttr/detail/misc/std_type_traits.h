/************************************************************************************
*                                                                                   *
*   Copyright (c) 2014 - 2018 Axel Menzel <info@rttr.org>                           *
*                                                                                   *
*   This file is part of RTTR (Run Time Type Reflection)                            *
*   License: MIT License                                                            *
*                                                                                   *
*   Permission is hereby granted, free of charge, to any person obtaining           *
*   a copy of this software and associated documentation files (the "Software"),    *
*   to deal in the Software without restriction, including without limitation       *
*   the rights to use, copy, modify, merge, publish, distribute, sublicense,        *
*   and/or sell copies of the Software, and to permit persons to whom the           *
*   Software is furnished to do so, subject to the following conditions:            *
*                                                                                   *
*   The above copyright notice and this permission notice shall be included in      *
*   all copies or substantial portions of the Software.                             *
*                                                                                   *
*   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR      *
*   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,        *
*   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE     *
*   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER          *
*   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,   *
*   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE   *
*   SOFTWARE.                                                                       *
*                                                                                   *
*************************************************************************************/

#ifndef RTTR_STD_TYPE_TRAITS_H_
#define RTTR_STD_TYPE_TRAITS_H_

#include "rttr/detail/base/core_prerequisites.h"

#include <type_traits>

namespace rttr
{

namespace detail
{
#if RTTR_COMPILER == RTTR_COMPILER_MSVC && RTTR_COMP_VER <= 1800
// workaround because msvc can't handle
// T (*const)(U)
//
// Does not bind to a <T*> or <T*const> partial specialization with VC10 and earlier

template <class T>
struct remove_pointer_imp
{
   using type = T;
};

template <class T>
struct remove_pointer_imp<T*>
{
   using type = T;
};

template <class T, bool b>
struct remove_pointer_imp3
{
   using type = typename remove_pointer_imp<typename std::remove_cv<T>::type>::type;
};

template <class T>
struct remove_pointer_imp3<T, false>
{
   using type = T;
};

template <class T>
struct remove_pointer_imp2
{
   using type = typename remove_pointer_imp3<T, std::is_pointer<T>::value>::type;
};

template< typename T >
struct remove_pointer
{
    using type = typename remove_pointer_imp2<T>::type;
};
#else

template<typename T>
using remove_pointer = std::remove_pointer<T>;

#endif

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

 #if RTTR_COMPILER == RTTR_COMPILER_MSVC && RTTR_COMP_VER <= 1800

template <typename T, size_t... Bounds>
struct recombobulator;

template <typename T>
struct recombobulator<T>
{
    using type = T;
};

template <typename T, size_t N, size_t... Bounds>
struct recombobulator<T, N, Bounds...> : recombobulator<T[N], Bounds...>
{
};

template <typename T>
struct remove_const_impl
{
    using type = T;
};

template <typename T>
struct remove_const_impl<const T>
{
    using type = T;
};

template <typename T, size_t... Bounds>
struct discombobulator_const : recombobulator<typename remove_const_impl<T>::type, Bounds...>
{
};

template <typename T, size_t N, size_t... Bounds>
struct discombobulator_const<T[N], Bounds...> : discombobulator_const<T, N, Bounds...>
{
};

template <typename T>
struct remove_const : discombobulator_const<T>
{
};

template <typename T>
struct remove_const<T[]>
{
    using U = typename remove_const<T>::type;
    using type = U[];
};

#else

template<typename T>
using remove_const = std::remove_const<T>;

#endif

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

#if RTTR_COMPILER == RTTR_COMPILER_MSVC && RTTR_COMP_VER <= 1800

template <typename T>
struct remove_volatile_impl
{
    using type = T;
};

template <typename T>
struct remove_volatile_impl<volatile T>
{
    using type = T;
};

template <typename T, size_t... Bounds>
struct discombobulator_volatile : recombobulator<typename remove_volatile_impl<T>::type, Bounds...>
{
};

template <typename T, size_t N, size_t... Bounds>
struct discombobulator_volatile<T[N], Bounds...> : discombobulator_volatile<T, N, Bounds...>
{
};

template <typename T>
struct remove_volatile : discombobulator_volatile<T>
{
};

template <typename T>
struct remove_volatile<T[]>
{
    using U = typename remove_volatile<T>::type;
    using type = U[];
};

#else

template<typename T>
using remove_volatile = std::remove_volatile<T>;

#endif

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

#if RTTR_COMPILER == RTTR_COMPILER_MSVC && RTTR_COMP_VER <= 1800

template<class T>
struct remove_cv
{
    using type = typename remove_const<typename remove_volatile<T>::type>::type;
};

#else

template<typename T>
using remove_cv = std::remove_cv<T>;

#endif

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
// shortcuts to avoid writing typename my_traits<T>::type over and over again
// C++14 has support for this, but we support at the moment C++11

template< bool B, class T, class F >
using conditional_t = typename std::conditional<B, T, F>::type;

template<typename T>
using remove_cv_t = typename remove_cv<T>::type;

template<typename T>
using remove_volatile_t = typename remove_volatile<T>::type;

template<typename T>
using remove_const_t = typename remove_const<T>::type;

template<typename T>
using remove_pointer_t = typename remove_pointer<T>::type;

template<typename T>
using remove_reference_t = typename std::remove_reference<T>::type;

template<typename T>
using add_pointer_t = typename std::add_pointer<T>::type;

template< bool B, class T = void >
using enable_if_t = typename std::enable_if<B, T>::type;

template<typename T>
using decay_t = typename std::decay<T>::type;

template<typename T>
using add_const_t = typename std::add_const<T>::type;

template<typename T>
using add_lvalue_reference_t = typename std::add_lvalue_reference<T>::type;

/////////////////////////////////////////////////////////////////////////////////////

} // end namespace detail
} // end namespace rttr

#endif // RTTR_STD_TYPE_TRAITS_H_
