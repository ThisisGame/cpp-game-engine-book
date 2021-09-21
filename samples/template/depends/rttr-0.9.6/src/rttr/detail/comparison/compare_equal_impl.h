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

#ifndef RTTR_COMPARE_EQUAL_IMPL_H_
#define RTTR_COMPARE_EQUAL_IMPL_H_

#include "rttr/detail/base/core_prerequisites.h"
#include "rttr/detail/misc/misc_type_traits.h"
#include "rttr/type.h"
#include "rttr/detail/comparison/compare_array_equal.h"

#include <type_traits>
#include <cstring>

namespace rttr
{
namespace detail
{

/////////////////////////////////////////////////////////////////////////////////////////

/*!
 * \brief This function return the result of the expression `lhs == rhs` when the type \p T has the equal operator defined,
 *         otherwise this function will return false.
 */
template<typename T>
RTTR_INLINE typename std::enable_if<is_equal_comparable<T>::value && !std::is_array<T>::value, bool>::type
compare_equal(const T& lhs, const T& rhs, bool& ok)
{
    ok = true;
    return (lhs == rhs);
}

/////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
RTTR_INLINE typename std::enable_if<!is_equal_comparable<T>::value && !std::is_array<T>::value, bool>::type
compare_equal(const T& lhs, const T& rhs, bool& ok)
{
    return compare_types_equal(&lhs, &rhs, type::get<T>(), ok);
}

/////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
RTTR_INLINE typename std::enable_if<!is_equal_comparable<T>::value && std::is_array<T>::value, bool>::type
compare_equal(const T& lhs, const T& rhs, bool& ok)
{
    return compare_array_equal(lhs, rhs, ok);
}

/////////////////////////////////////////////////////////////////////////////////////////

} // end namespace detail
} // end namespace rttr

#endif // RTTR_COMPARE_EQUAL_IMPL_H_
