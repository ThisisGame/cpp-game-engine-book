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

#include "rttr/detail/enumeration/enumeration_wrapper_base.h"

#include "rttr/argument.h"

namespace rttr
{
namespace detail
{
/////////////////////////////////////////////////////////////////////////////////////////

enumeration_wrapper_base::enumeration_wrapper_base() RTTR_NOEXCEPT
:   m_declaring_type(get_invalid_type())
{
}

/////////////////////////////////////////////////////////////////////////////////////////

enumeration_wrapper_base::~enumeration_wrapper_base() RTTR_NOEXCEPT
{
}

/////////////////////////////////////////////////////////////////////////////////////////

void enumeration_wrapper_base::set_declaring_type(type declaring_type) RTTR_NOEXCEPT
{
    m_declaring_type = declaring_type;
}

/////////////////////////////////////////////////////////////////////////////////////////

type enumeration_wrapper_base::get_declaring_type() const RTTR_NOEXCEPT
{
    return m_declaring_type;
}

/////////////////////////////////////////////////////////////////////////////////////////

bool enumeration_wrapper_base::is_valid() const RTTR_NOEXCEPT
{
    return false;
}

/////////////////////////////////////////////////////////////////////////////////////////

type enumeration_wrapper_base::get_underlying_type() const RTTR_NOEXCEPT
{
    return get_invalid_type();
}

/////////////////////////////////////////////////////////////////////////////////////////

type enumeration_wrapper_base::get_type() const RTTR_NOEXCEPT
{
    return get_invalid_type();
}

/////////////////////////////////////////////////////////////////////////////////////////

array_range<string_view> enumeration_wrapper_base::get_names() const RTTR_NOEXCEPT
{
    return array_range<string_view>();
}

/////////////////////////////////////////////////////////////////////////////////////////

array_range<variant> enumeration_wrapper_base::get_values() const RTTR_NOEXCEPT
{
    return array_range<variant>();
}

/////////////////////////////////////////////////////////////////////////////////////////

string_view enumeration_wrapper_base::value_to_name(argument& value) const
{
    return string_view();
}

/////////////////////////////////////////////////////////////////////////////////////////

variant enumeration_wrapper_base::name_to_value(string_view name) const
{
    return variant();
}

/////////////////////////////////////////////////////////////////////////////////////////

variant enumeration_wrapper_base::get_metadata(const variant& key) const
{
    return variant();
}

/////////////////////////////////////////////////////////////////////////////////////////

} // end namespace detail
} // end namespace rttr
