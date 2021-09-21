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

#ifndef RTTR_TEMPLATE_TYPE_TRAIT_IMPL_H_
#define RTTR_TEMPLATE_TYPE_TRAIT_IMPL_H_

#include "rttr/type.h"

namespace rttr
{
namespace detail
{

template<typename T>
std::vector<::rttr::type> template_type_trait<T>::get_template_arguments() { return {}; }

#if RTTR_COMPILER == RTTR_COMPILER_MSVC && RTTR_COMP_VER <= 1800
 // otherwise an internal compile error will occur
template<template <typename... > class T>
struct template_type_trait<T<>> : std::true_type
{
    static std::vector<::rttr::type> get_template_arguments() { return { }; }
};

template<template <typename... > class T, typename T1, typename...Args>
struct template_type_trait<T<T1, Args...>> : std::true_type
{
    static std::vector<::rttr::type> get_template_arguments() { return { ::rttr::type::get<T1>(), ::rttr::type::get<Args>()..., }; }
};

#else

template<template <typename... > class T, typename...Args>
struct template_type_trait<T<Args...>> : std::true_type
{
    static std::vector<::rttr::type> get_template_arguments() { return { ::rttr::type::get<Args>()..., }; }
};

#endif

} // end namespace detail
} // end namespace rttr


#define RTTR_ADD_TYPE_TRAIT_SPECIALIZATION_1(value_type)                                                                                    \
namespace rttr                                                                                                                              \
{                                                                                                                                           \
namespace detail                                                                                                                            \
{                                                                                                                                           \
    template<template <value_type...> class T, value_type...Args>                                                                           \
    struct template_type_trait<T<Args...>> : std::true_type                                                                                 \
    {                                                                                                                                       \
        static std::vector<::rttr::type> get_template_arguments() { return { ::rttr::type::get<Args>()..., }; }                             \
    };                                                                                                                                      \
}                                                                                                                                           \
}

#define RTTR_ADD_TYPE_TRAIT_SPECIALIZATION_2(value_type)                                                                                    \
namespace rttr                                                                                                                              \
{                                                                                                                                           \
namespace detail                                                                                                                            \
{                                                                                                                                           \
    template<template <typename, value_type > class T, typename T1, value_type N1>                                                          \
    struct template_type_trait<T<T1, N1>> : std::true_type                                                                                  \
    {                                                                                                                                       \
        static std::vector<::rttr::type> get_template_arguments() { return { ::rttr::type::get<T1>(), ::rttr::type::get<value_type>() }; }  \
    };                                                                                                                                      \
                                                                                                                                            \
    template<template <value_type, typename > class T, typename T1, value_type N1>                                                          \
    struct template_type_trait<T<N1, T1>> : std::true_type                                                                                  \
    {                                                                                                                                       \
        static std::vector<::rttr::type> get_template_arguments() { return { ::rttr::type::get<value_type>(), ::rttr::type::get<T1>() }; }  \
    };                                                                                                                                      \
}                                                                                                                                           \
}

#define RTTR_ADD_TYPE_TRAIT_SPECIALIZATION_3(value_type)                                                                                                        \
namespace rttr                                                                                                                                                  \
{                                                                                                                                                               \
namespace detail                                                                                                                                                \
{                                                                                                                                                               \
    template<template <typename, typename, value_type > class T, typename T1, typename T2, value_type N1>                                                       \
    struct template_type_trait<T<T1, T2, N1>> : std::true_type                                                                                                  \
    {                                                                                                                                                           \
        static std::vector<::rttr::type> get_template_arguments() { return { ::rttr::type::get<T1>(), ::rttr::type::get<T2>(), ::rttr::type::get<N1>() }; }     \
    };                                                                                                                                                          \
                                                                                                                                                                \
    template<template <typename, value_type, typename > class T, typename T1, typename T2, value_type N1>                                                       \
    struct template_type_trait<T<T1, N1, T2>> : std::true_type                                                                                                  \
    {                                                                                                                                                           \
        static std::vector<::rttr::type> get_template_arguments() { return { ::rttr::type::get<T1>(), ::rttr::type::get<N1>(), ::rttr::type::get<T2>() }; }     \
    };                                                                                                                                                          \
                                                                                                                                                                \
    template<template <value_type, typename, typename > class T, typename T1, typename T2, value_type N1>                                                       \
    struct template_type_trait<T<N1, T1, T2>> : std::true_type                                                                                                  \
    {                                                                                                                                                           \
        static std::vector<::rttr::type> get_template_arguments() { return { ::rttr::type::get<N1>(), ::rttr::type::get<T1>(), ::rttr::type::get<T2>() }; }     \
    };                                                                                                                                                          \
                                                                                                                                                                \
                                                                                                                                                                \
                                                                                                                                                                \
    template<template <value_type, value_type, typename > class T, typename T1, value_type N1, value_type N2>                                                   \
    struct template_type_trait<T<N1, N2, T1>> : std::true_type                                                                                                  \
    {                                                                                                                                                           \
        static std::vector<::rttr::type> get_template_arguments() { return { ::rttr::type::get<N1>(), ::rttr::type::get<N2>(), ::rttr::type::get<T1>() }; }     \
    };                                                                                                                                                          \
                                                                                                                                                                \
    template<template <value_type, typename, value_type > class T, typename T1, value_type N1, value_type N2>                                                   \
    struct template_type_trait<T<N1, T1, N2>> : std::true_type                                                                                                  \
    {                                                                                                                                                           \
        static std::vector<::rttr::type> get_template_arguments() { return { ::rttr::type::get<N1>(), ::rttr::type::get<T1>(), ::rttr::type::get<N2>() }; }     \
    };                                                                                                                                                          \
                                                                                                                                                                \
    template<template <typename, value_type, value_type > class T, typename T1, value_type N1, value_type N2>                                                   \
    struct template_type_trait<T<T1, N1, N2>> : std::true_type                                                                                                  \
    {                                                                                                                                                           \
        static std::vector<::rttr::type> get_template_arguments() { return { ::rttr::type::get<T1>(), ::rttr::type::get<N1>(), ::rttr::type::get<N2>() }; }     \
    };                                                                                                                                                          \
                                                                                                                                                                \
}                                                                                                                                                               \
}

#define RTTR_ADD_TYPE_TRAIT_SPECIALIZATION(value_type)  \
RTTR_ADD_TYPE_TRAIT_SPECIALIZATION_1(value_type)        \
RTTR_ADD_TYPE_TRAIT_SPECIALIZATION_2(value_type)        \
RTTR_ADD_TYPE_TRAIT_SPECIALIZATION_3(value_type)


RTTR_ADD_TYPE_TRAIT_SPECIALIZATION(std::size_t)
RTTR_ADD_TYPE_TRAIT_SPECIALIZATION(bool)
RTTR_ADD_TYPE_TRAIT_SPECIALIZATION(int)
RTTR_ADD_TYPE_TRAIT_SPECIALIZATION(char)

#endif // RTTR_TEMPLATE_TYPE_TRAIT_IMPL_H_
