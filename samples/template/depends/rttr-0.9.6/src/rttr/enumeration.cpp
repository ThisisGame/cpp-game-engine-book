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

#include "rttr/enumeration.h"
#include "rttr/detail/enumeration/enumeration_wrapper_base.h"
#include "rttr/argument.h"

#include <utility>

using namespace std;

namespace rttr
{

namespace detail
{
template<>
enumeration create_item(const enumeration_wrapper_base* wrapper)
{
    return enumeration(wrapper);
}

template<>
enumeration create_invalid_item()
{
    static const enumeration_wrapper_base invalid_wrapper;
    return enumeration(&invalid_wrapper);
}

} // end namespace detail

/////////////////////////////////////////////////////////////////////////////////////////

enumeration::enumeration(const detail::enumeration_wrapper_base* wrapper) RTTR_NOEXCEPT
:   m_wrapper(wrapper)
{

}

/////////////////////////////////////////////////////////////////////////////////////////

bool enumeration::is_valid() const RTTR_NOEXCEPT
{
    return m_wrapper->is_valid();
}

/////////////////////////////////////////////////////////////////////////////////////////

enumeration::operator bool() const RTTR_NOEXCEPT
{
    return m_wrapper->is_valid();
}

/////////////////////////////////////////////////////////////////////////////////////////

string_view enumeration::get_name() const RTTR_NOEXCEPT
{
    return m_wrapper->get_type().get_name();
}

/////////////////////////////////////////////////////////////////////////////////////////

type enumeration::get_underlying_type() const RTTR_NOEXCEPT
{
    return m_wrapper->get_underlying_type();
}

/////////////////////////////////////////////////////////////////////////////////////////

type enumeration::get_type() const RTTR_NOEXCEPT
{
    return m_wrapper->get_type();
}

/////////////////////////////////////////////////////////////////////////////////////////

type enumeration::get_declaring_type() const RTTR_NOEXCEPT
{
    return m_wrapper->get_declaring_type();
}

/////////////////////////////////////////////////////////////////////////////////////////

variant enumeration::get_metadata(const variant& key) const
{
    return m_wrapper->get_metadata(key);
}

/////////////////////////////////////////////////////////////////////////////////////////

array_range<string_view> enumeration::get_names() const RTTR_NOEXCEPT
{
    return m_wrapper->get_names();
}

/////////////////////////////////////////////////////////////////////////////////////////

array_range<variant> enumeration::get_values() const RTTR_NOEXCEPT
{
    return m_wrapper->get_values();
}

/////////////////////////////////////////////////////////////////////////////////////////

string_view enumeration::value_to_name(argument value) const
{
    return m_wrapper->value_to_name(value);
}

/////////////////////////////////////////////////////////////////////////////////////////

variant enumeration::name_to_value(string_view name) const
{
    return m_wrapper->name_to_value(name);
}

/////////////////////////////////////////////////////////////////////////////////////////

bool enumeration::operator==(const enumeration& other) const RTTR_NOEXCEPT
{
    return (m_wrapper == other.m_wrapper);
}

/////////////////////////////////////////////////////////////////////////////////////////

bool enumeration::operator!=(const enumeration& other) const RTTR_NOEXCEPT
{
    return (m_wrapper != other.m_wrapper);
}

/////////////////////////////////////////////////////////////////////////////////////////

} // end namespace rttr
