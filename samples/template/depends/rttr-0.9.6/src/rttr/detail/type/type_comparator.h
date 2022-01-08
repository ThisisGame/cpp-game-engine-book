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

#ifndef RTTR_TYPE_COMPARATOR_H_
#define RTTR_TYPE_COMPARATOR_H_

#include "rttr/detail/base/core_prerequisites.h"
#include "rttr/detail/type/type_register.h"

namespace rttr
{

namespace detail
{

/////////////////////////////////////////////////////////////////////////////////////////

struct type_comparator_base
{
    using cmp_func = bool (*)(const void* lhs, const void* rhs);

    type_comparator_base(cmp_func cmp_f = [](const void*, const void*) ->bool { return false; }, type t = get_invalid_type())
    :   cmp(cmp_f), cmp_type(t)
    {
    }

    cmp_func    cmp;
    type        cmp_type;
};

/////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
struct type_equal_comparator : type_comparator_base
{
    type_equal_comparator() : type_comparator_base(equal, type::get<T>()) {}

    static bool equal(const void* lhs, const void* rhs)
    {
        const T& l = *static_cast<const T*>(lhs);
        const T& r = *static_cast<const T*>(rhs);
        return (l == r);
    }
};

/////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
struct type_less_than_comparator : type_comparator_base
{
    type_less_than_comparator() : type_comparator_base(less_than, type::get<T>()) {}

    static bool less_than(const void* lhs, const void* rhs)
    {
        const T& l = *static_cast<const T*>(lhs);
        const T& r = *static_cast<const T*>(rhs);
        return (l < r);
    }
};

/////////////////////////////////////////////////////////////////////////////////////////

} // end namespace detail
} // end namespace rttr

#endif // RTTR_TYPE_COMPARATOR_H_
