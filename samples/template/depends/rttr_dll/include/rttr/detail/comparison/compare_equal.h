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

#ifndef RTTR_COMPARE_EQUAL_H_
#define RTTR_COMPARE_EQUAL_H_

#include "rttr/detail/base/core_prerequisites.h"
#include "rttr/detail/misc/misc_type_traits.h"
#include "rttr/detail/comparison/comparable_types.h"
#include "rttr/detail/misc/template_type_trait.h"


#include <type_traits>
#include <cstring>

namespace rttr
{
namespace detail
{

/////////////////////////////////////////////////////////////////////////////////////////

RTTR_API bool compare_types_equal(const void* lhs, const void* rhs, const type& t, bool& ok);

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////


template<typename T>
using has_equal_operator_impl = std::integral_constant<bool, has_equal_operator<T>::value && !std::is_array<T>::value &&
                                                             !is_template_instance<T>::value>;

template<typename T>
using is_equal_comparable = std::integral_constant<bool, has_equal_operator_impl<T>::value ||
                                                         is_comparable_type<T>::value>;


/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
RTTR_INLINE typename std::enable_if<is_equal_comparable<T>::value && !std::is_array<T>::value, bool>::type
compare_equal(const T& lhs, const T& rhs, bool& ok);

/////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
RTTR_INLINE typename std::enable_if<!is_equal_comparable<T>::value && !std::is_array<T>::value, bool>::type
compare_equal(const T& lhs, const T& rhs, bool& ok);

/////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
RTTR_INLINE typename std::enable_if<!is_equal_comparable<T>::value && std::is_array<T>::value, bool>::type
compare_equal(const T& lhs, const T& rhs, bool& ok);

/////////////////////////////////////////////////////////////////////////////////////////

} // end namespace detail
} // end namespace rttr

#include "rttr/detail/comparison/compare_equal_impl.h"
// the include in this place is necessary, otherwise we get an error during compilation:
// "'compare_equal': identifier not found"
#include "rttr/detail/misc/template_type_trait_impl.h"

#endif // RTTR_COMPARE_EQUAL_H_
