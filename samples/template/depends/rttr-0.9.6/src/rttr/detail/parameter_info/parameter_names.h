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

#ifndef RTTR_PARAMETER_NAMES_H_
#define RTTR_PARAMETER_NAMES_H_

#include "rttr/detail/base/core_prerequisites.h"
#include "rttr/detail/misc/misc_type_traits.h"
#include <tuple>

namespace rttr
{
namespace detail
{

/////////////////////////////////////////////////////////////////////////////////////////

template<typename...T>
struct parameter_names
{
    parameter_names() {}
    parameter_names(T&&... args) : m_param_names(std::forward<T>(args)...) {}

    std::tuple<T...> m_param_names;

    RTTR_STATIC_CONSTEXPR std::size_t count = sizeof...(T);
};

/////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
struct is_parameter_names : std::false_type { };

template<typename... TArgs>
struct is_parameter_names<parameter_names<TArgs...> > : std::true_type { };

/////////////////////////////////////////////////////////////////////////////////////////

template<typename...TArgs>
using count_parameter_names = count_if<is_parameter_names, type_list< raw_type_t<TArgs>... > >;

} // end namespace detail
} // end namespace rttr

#endif // RTTR_PARAMETER_NAMES_H_
