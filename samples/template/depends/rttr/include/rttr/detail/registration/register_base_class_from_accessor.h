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

#ifndef RTTR_REGISTER_BASE_CLASS_FROM_ACCESSOR_H_
#define RTTR_REGISTER_BASE_CLASS_FROM_ACCESSOR_H_

#include "rttr/detail/base/core_prerequisites.h"

#include "rttr/detail/type/base_classes.h"
#include "rttr/detail/type/type_register.h"
#include "rttr/detail/misc/misc_type_traits.h"

#include <type_traits>

namespace rttr
{
namespace detail
{

template<typename ClassType, typename AccClassType>
enable_if_t<contains<AccClassType, typename ClassType::base_class_list>::value, void>
register_member_accessor_class_type_when_needed_3()
{
}

template<typename ClassType, typename AccClassType>
enable_if_t<!contains<AccClassType, typename ClassType::base_class_list>::value, void>
register_member_accessor_class_type_when_needed_3()
{
    base_class_info baseClassInfo = { type::get<AccClassType>(), &rttr_cast_impl<ClassType, AccClassType> };
    type_register::register_base_class(type::get<ClassType>(), baseClassInfo);
}

template<typename ClassType, typename AccClassType>
enable_if_t<has_base_class_list<ClassType>::value, void>
register_member_accessor_class_type_when_needed_2()
{
    register_member_accessor_class_type_when_needed_3<ClassType, AccClassType>();
}

template<typename ClassType, typename AccClassType>
enable_if_t<!has_base_class_list<ClassType>::value, void>
register_member_accessor_class_type_when_needed_2()
{
    base_class_info baseClassInfo = { type::get<AccClassType>(), &rttr_cast_impl<ClassType, AccClassType> };
    type_register::register_base_class(type::get<ClassType>(), baseClassInfo);
}

template<typename ClassType, typename AccClassType>
enable_if_t<std::is_base_of<AccClassType, ClassType>::value, void>
register_member_accessor_class_type_when_needed_1()
{
    register_member_accessor_class_type_when_needed_2<ClassType, AccClassType>();
}

template<typename ClassType, typename AccClassType>
enable_if_t<!std::is_base_of<AccClassType, ClassType>::value, void>
register_member_accessor_class_type_when_needed_1()
{
}

template<typename ClassType, typename AccClassType>
enable_if_t<!std::is_same<ClassType, AccClassType>::value, void>
register_member_accessor_class_type_when_needed()
{
    register_member_accessor_class_type_when_needed_1<ClassType, AccClassType>();
}

template<typename ClassType, typename AccClassType>
enable_if_t<std::is_same<ClassType, AccClassType>::value, void>
register_member_accessor_class_type_when_needed()
{
}

/////////////////////////////////////////////////////////////////////////////////////////

template<typename ClassType, typename F>
enable_if_t<std::is_member_function_pointer<F>::value && !std::is_member_object_pointer<F>::value, void>
register_accessor_class_type_when_needed()
{
    register_member_accessor_class_type_when_needed<ClassType, typename function_traits<F>::class_type>();
}

template<typename ClassType, typename F>
enable_if_t<!std::is_member_function_pointer<F>::value && std::is_member_object_pointer<F>::value, void>
register_accessor_class_type_when_needed()
{
    register_member_accessor_class_type_when_needed<ClassType, typename property_traits<F>::class_type>();
}

template<typename ClassType, typename F>
enable_if_t<std::is_member_function_pointer<F>::value && std::is_member_object_pointer<F>::value, void>
register_accessor_class_type_when_needed()
{
}

template<typename ClassType, typename F>
enable_if_t<!std::is_member_function_pointer<F>::value && !std::is_member_object_pointer<F>::value, void>
register_accessor_class_type_when_needed()
{
}

/////////////////////////////////////////////////////////////////////////////////////////


} // end namespace detail
} // end namespace rttr

#endif // RTTR_REGISTER_BASE_CLASS_FROM_ACCESSOR_H_
