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

#ifndef RTTR_ACCESSOR_TYPE_H_
#define RTTR_ACCESSOR_TYPE_H_

#include "rttr/detail/base/core_prerequisites.h"
#include "rttr/detail/misc/function_traits.h"
#include "rttr/detail/misc/misc_type_traits.h"

#include <type_traits>

namespace rttr
{
namespace detail
{
    /////////////////////////////////////////////////////////////////////////////////////

    struct member_func_ptr
    {
        using type = member_func_ptr;
    };

    struct function_ptr
    {
        using type = function_ptr;
    };

    struct member_object_ptr
    {
        using type = member_object_ptr;
    };

    struct object_ptr
    {
        using type = object_ptr;
    };

    template<typename T>
    struct property_type : conditional_t< std::is_member_function_pointer<T>::value,
                                          member_func_ptr,
                                          conditional_t< std::is_member_object_pointer<T>::value,
                                                         member_object_ptr,
                                                         conditional_t< is_functor<T>::value,
                                                                        function_ptr,
                                                                        conditional_t< std::is_pointer<T>::value,
                                                                                       object_ptr,
                                                                                       void
                                                                                     >
                                                                      >
                                                       >
                                        >
    {
    };

    /////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////

    struct void_member_func
    {
        using type = void_member_func;
    };

    struct return_member_func
    {
        using type = return_member_func;
    };

    struct void_func
    {
        using type = void_func;
    };

    struct return_func
    {
        using type = return_func;
    };

    template<typename T>
    struct method_type : conditional_t<std::is_member_function_pointer<T>::value,
                                       conditional_t< is_void_func<T>::value,
                                                      void_member_func,
                                                      return_member_func
                                                     >,
                                       conditional_t< is_functor<T>::value,
                                                      conditional_t< is_void_func<T>::value,
                                                                     void_func,
                                                                     return_func
                                                                   >,
                                                      void
                                                                   >
                                         >
    {
    };

    /////////////////////////////////////////////////////////////////////////////////////

    struct class_ctor
    {};

} // end namespace detail
} // end namespace rttr

#endif // RTTR_ACCESSOR_TYPE_H_
