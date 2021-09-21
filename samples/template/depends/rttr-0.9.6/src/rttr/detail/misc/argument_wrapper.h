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

#ifndef RTTR_ARGUMENT_WRAPPER_H_
#define RTTR_ARGUMENT_WRAPPER_H_

#include "rttr/detail/base/core_prerequisites.h"

#include <memory>
#include <type_traits>

namespace rttr
{
namespace detail
{

/*!
 * This class holds the address to any given data in m_data.
 *
 * What kind of data it holds, depends on the operation it will be used for.
 * That means there will be also kind of check what kind of data it contains.
 * The caller has to make sure the data it will forward, matches the expected data in the invoke function.
 */
struct argument_wrapper
{
    argument_wrapper() : m_data(nullptr) {}

    template<typename T, typename Tp = typename std::enable_if<!std::is_same<T, argument_wrapper>::value, T>::type>
    argument_wrapper(T&& data) : m_data(const_cast<void*>(reinterpret_cast<const void*>(std::addressof(data)))) {}

    template<typename T>
    T& get_value() const
    {
        using raw_type = typename std::remove_reference<T>::type;
        return (*reinterpret_cast<raw_type*>(const_cast<void *>(m_data)));
    }

    void* m_data;
};

} // end namespace detail
} // end namespace rttr

#endif // RTTR_ARGUMENT_WRAPPER_H_
