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

#ifndef RTTR_COMPARE_ARRAY_LESS_IMPL_H_
#define RTTR_COMPARE_ARRAY_LESS_IMPL_H_

#include "rttr/detail/base/core_prerequisites.h"
#include "rttr/detail/misc/misc_type_traits.h"
#include "rttr/detail/comparison/compare_less.h"

#include <type_traits>
#include <cstring>

namespace rttr
{
namespace detail
{

/////////////////////////////////////////////////////////////////////////////////////////

template<typename ElementType>
struct compare_array_less_impl
{
    int operator()(const ElementType &lhs, const ElementType &rhs)
    {
        int result = 0;
        compare_less_than(lhs, rhs, result);
        return result;
    }
};

/////////////////////////////////////////////////////////////////////////////////////////

template<typename ElementType, std::size_t Count>
struct compare_array_less_impl<ElementType[Count]>
{
    int operator()(const ElementType (&lhs)[Count], const ElementType (&rhs)[Count])
    {
        int flag = 0;
        for(std::size_t i = 0; i < Count; ++i)
        {
            if ((flag = compare_array_less_impl<ElementType>()(lhs[i], rhs[i])) != 0)
                return flag;
        }

        return 0;
    }
};

/////////////////////////////////////////////////////////////////////////////////////////

template<typename ElementType, std::size_t Count>
RTTR_INLINE bool compare_array_less(const ElementType (&lhs)[Count], const ElementType (&rhs)[Count])
{
    if (compare_array_less_impl<ElementType[Count]>()(lhs, rhs) == -1)
        return true;

    return false;
}

/////////////////////////////////////////////////////////////////////////////////////////

} // end namespace detail
} // end namespace rttr

#endif // RTTR_COMPARE_ARRAY_LESS_IMPL_H_
