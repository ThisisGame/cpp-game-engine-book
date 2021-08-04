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

#ifndef RTTR_CAST_IMPL_H_
#define RTTR_CAST_IMPL_H_

#include "rttr/type.h"
#include "rttr/detail/misc/misc_type_traits.h"

#include <type_traits>

namespace rttr
{

template<typename Target_Type, typename Source_Type>
RTTR_INLINE Target_Type rttr_cast(Source_Type object) RTTR_NOEXCEPT
{
    static_assert(detail::pointer_count<Target_Type>::value == 1, "Target type must be a pointer");
    static_assert(detail::pointer_count<Source_Type>::value == 1, "Source argument must be a pointer");
    static_assert(rttr::detail::has_get_type_func<Source_Type>::value, "Class has not type defined - please use the macro RTTR_ENABLE().");

    using Return_Type = detail::remove_pointer_t<Target_Type>;
    using Arg_Type = detail::remove_pointer_t<Source_Type>;

    static_assert((std::is_volatile<Arg_Type>::value && std::is_volatile<Return_Type>::value) ||
                   (!std::is_volatile<Arg_Type>::value && std::is_volatile<Return_Type>::value) ||
                   (!std::is_volatile<Arg_Type>::value && !std::is_volatile<Return_Type>::value),  "Return type must have volatile qualifier");

    static_assert( (std::is_const<Arg_Type>::value && std::is_const<Return_Type>::value) ||
                   (!std::is_const<Arg_Type>::value && std::is_const<Return_Type>::value) ||
                   (!std::is_const<Arg_Type>::value && !std::is_const<Return_Type>::value), "Return type must have const qualifier");

    using source_type_no_cv = typename detail::remove_cv<typename detail::remove_pointer<Source_Type>::type>::type;
    return static_cast<Target_Type>(type::apply_offset(const_cast<source_type_no_cv*>(object)->get_ptr(), const_cast<source_type_no_cv*>(object)->get_type(), type::get<Target_Type>()));
}

}

#endif // RTTR_CAST_IMPL_H_
