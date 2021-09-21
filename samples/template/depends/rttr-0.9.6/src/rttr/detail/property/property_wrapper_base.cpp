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

#include "rttr/detail/property/property_wrapper_base.h"

namespace rttr
{
namespace detail
{

/////////////////////////////////////////////////////////////////////////////////////////

property_wrapper_base::property_wrapper_base(string_view name, type declaring_type) RTTR_NOEXCEPT
:   m_name(name),
    m_declaring_type(declaring_type)
{
}

/////////////////////////////////////////////////////////////////////////////////////////

property_wrapper_base::~property_wrapper_base()
{
}

/////////////////////////////////////////////////////////////////////////////////////////

void property_wrapper_base::init() RTTR_NOEXCEPT
{
    get_type();
}

/////////////////////////////////////////////////////////////////////////////////////////

bool property_wrapper_base::is_valid() const RTTR_NOEXCEPT
{
    return false;
}

/////////////////////////////////////////////////////////////////////////////////////////

string_view property_wrapper_base::get_name() const RTTR_NOEXCEPT
{
    return m_name;
}

/////////////////////////////////////////////////////////////////////////////////////////

type property_wrapper_base::get_declaring_type() const RTTR_NOEXCEPT
{
    return m_declaring_type;
}

/////////////////////////////////////////////////////////////////////////////////////////

access_levels property_wrapper_base::get_access_level() const RTTR_NOEXCEPT
{
    return access_levels::public_access;
}

/////////////////////////////////////////////////////////////////////////////////////////

bool property_wrapper_base::is_readonly() const RTTR_NOEXCEPT
{
    return false;
}

/////////////////////////////////////////////////////////////////////////////////////////

bool property_wrapper_base::is_static() const RTTR_NOEXCEPT
{
    return false;
}

/////////////////////////////////////////////////////////////////////////////////////////

type property_wrapper_base::get_type() const RTTR_NOEXCEPT
{
    return get_invalid_type();
}

/////////////////////////////////////////////////////////////////////////////////////////

variant property_wrapper_base::get_metadata(const variant& key) const
{
    return variant();
}

/////////////////////////////////////////////////////////////////////////////////////////

bool property_wrapper_base::set_value(instance& object, argument& arg) const
{
    return false;
}

/////////////////////////////////////////////////////////////////////////////////////////

variant property_wrapper_base::get_value(instance& object) const
{
    return variant();
}

/////////////////////////////////////////////////////////////////////////////////////////

} // end namespace detail
} // end namespace rttr
