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

#include "rttr/parameter_info.h"

#include "rttr/detail/parameter_info/parameter_info_wrapper_base.h"
#include "rttr/variant.h"

namespace rttr
{

/////////////////////////////////////////////////////////////////////////////////////////

parameter_info::parameter_info(const detail::parameter_info_wrapper_base* wrapper) RTTR_NOEXCEPT
:   m_wrapper(wrapper)
{

}

/////////////////////////////////////////////////////////////////////////////////////////

type parameter_info::get_type() const RTTR_NOEXCEPT
{
    return m_wrapper->get_type();
}

/////////////////////////////////////////////////////////////////////////////////////////

bool parameter_info::has_default_value() const RTTR_NOEXCEPT
{
    return m_wrapper->has_default_value();
}

/////////////////////////////////////////////////////////////////////////////////////////

variant parameter_info::get_default_value() const
{
    return m_wrapper->get_default_value();
}

/////////////////////////////////////////////////////////////////////////////////////////

string_view parameter_info::get_name() const RTTR_NOEXCEPT
{
    return m_wrapper->get_name();
}

/////////////////////////////////////////////////////////////////////////////////////////

uint32_t parameter_info::get_index() const RTTR_NOEXCEPT
{
    return m_wrapper->get_index();
}

/////////////////////////////////////////////////////////////////////////////////////////

bool parameter_info::operator==(const parameter_info& other) const RTTR_NOEXCEPT
{
    return (m_wrapper == other.m_wrapper);
}

/////////////////////////////////////////////////////////////////////////////////////////

bool parameter_info::operator!=(const parameter_info& other) const RTTR_NOEXCEPT
{
    return (m_wrapper != other.m_wrapper);
}

/////////////////////////////////////////////////////////////////////////////////////////


} // end namespace rttr
