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

#ifndef RTTR_ARGUMENT_IMPL_H_
#define RTTR_ARGUMENT_IMPL_H_

#include "rttr/type.h"
#include "rttr/variant.h"

namespace rttr
{

/////////////////////////////////////////////////////////////////////////////////////////

RTTR_INLINE argument::argument() RTTR_NOEXCEPT : m_data(nullptr), m_variant(nullptr), m_type(detail::get_invalid_type()) {}

/////////////////////////////////////////////////////////////////////////////////////////

RTTR_INLINE argument::argument(argument&& arg) RTTR_NOEXCEPT : m_data(arg.m_data), m_variant(arg.m_variant), m_type(arg.m_type) {}

/////////////////////////////////////////////////////////////////////////////////////////

RTTR_INLINE argument::argument(const argument& other) RTTR_NOEXCEPT : m_data(other.m_data), m_variant(other.m_variant), m_type(other.m_type) {}

/////////////////////////////////////////////////////////////////////////////////////////

RTTR_INLINE argument::argument(variant& var) RTTR_NOEXCEPT : m_data(var.get_ptr()), m_variant(&var), m_type(var.get_type()) {}

/////////////////////////////////////////////////////////////////////////////////////////

RTTR_INLINE argument::argument(const variant& var) RTTR_NOEXCEPT : m_data(var.get_ptr()),  m_variant(&var), m_type(var.get_type()) {}

/////////////////////////////////////////////////////////////////////////////////////////

template<typename T, typename Tp>
argument::argument(const T& data) RTTR_NOEXCEPT
:   m_data(reinterpret_cast<const void*>(std::addressof(data))),
    m_variant(nullptr),
    m_type(rttr::type::get<T>())
{
    static_assert(!std::is_same<instance, T>::value, "Don't use the argument class for forwarding an instance!");
}

/////////////////////////////////////////////////////////////////////////////////////////

template<typename T, typename Tp>
argument::argument(T& data) RTTR_NOEXCEPT
:   m_data(reinterpret_cast<const void*>(std::addressof(data))),
    m_variant(nullptr),
    m_type(rttr::type::get<T>())
{
    static_assert(!std::is_same<instance, T>::value, "Don't use the argument class for forwarding an instance!");
}

/////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
RTTR_INLINE argument::ptr_type<T> argument::is_type() const RTTR_NOEXCEPT
{
    return ((rttr::type::get<T>() == m_type) ||
             m_type == type::get<std::nullptr_t>() ||
             (m_variant && type::get<variant*>() == type::get<T>()));
}

/////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
RTTR_INLINE argument::non_ptr_type<T> argument::is_type() const RTTR_NOEXCEPT
{
    return (rttr::type::get<T>() == m_type ||
            (m_variant && type::get<variant>() == type::get<T>()));
}

/////////////////////////////////////////////////////////////////////////////////////////

RTTR_INLINE type argument::get_type() const RTTR_NOEXCEPT
{
    return m_type;
}

/////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
RTTR_INLINE argument::arg_value_t<T>& argument::get_value() const RTTR_NOEXCEPT
{
    using raw_type = typename std::remove_reference<T>::type;
    return (*reinterpret_cast<raw_type*>(const_cast<void *>(m_data)));
}

/////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
RTTR_INLINE argument::arg_rvalue_t<T>&& argument::get_value() const RTTR_NOEXCEPT
{
    using raw_type = typename std::remove_reference<T>::type;
    return std::move(*reinterpret_cast<raw_type*>(const_cast<void *>(m_data)));
}

/////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
RTTR_INLINE argument::is_variant_t<T>& argument::get_value() const RTTR_NOEXCEPT
{
    using raw_type = typename std::remove_reference<T>::type;
    return (*reinterpret_cast<raw_type*>(const_cast<variant *>(m_variant)));
}

/////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
RTTR_INLINE argument::is_variant_ref_t<T>&& argument::get_value() const RTTR_NOEXCEPT
{
    using raw_type = typename std::remove_reference<T>::type;
    return std::move(*reinterpret_cast<raw_type*>(const_cast<variant *>(m_variant)));
}

/////////////////////////////////////////////////////////////////////////////////////////

RTTR_INLINE argument& argument::operator=(const argument& other) RTTR_NOEXCEPT
{
    m_data = other.m_data;
    const_cast<rttr::type&>(m_type) = other.m_type;
    m_variant = other.m_variant;
    return *this;
}

/////////////////////////////////////////////////////////////////////////////////////////

} // end namespace rttr

#endif // RTTR_ARGUMENT_IMPL_H_
