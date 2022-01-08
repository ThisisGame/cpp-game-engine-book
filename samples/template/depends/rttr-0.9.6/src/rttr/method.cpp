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

#include "rttr/method.h"
#include "rttr/detail/method/method_wrapper_base.h"
#include "rttr/argument.h"
#include "rttr/instance.h"

using namespace std;

namespace rttr
{

namespace detail
{

template<>
method create_item(const method_wrapper_base* wrapper)
{
    return method(wrapper);
}

template<>
method create_invalid_item()
{
    static const method_wrapper_base invalid_wrapper(string_view(), detail::get_invalid_type());
    return method(&invalid_wrapper);
}

} // end namespace detail;

/////////////////////////////////////////////////////////////////////////////////////////

method::method(const detail::method_wrapper_base* wrapper) RTTR_NOEXCEPT
:   m_wrapper(wrapper)
{

}

/////////////////////////////////////////////////////////////////////////////////////////

bool method::is_valid() const RTTR_NOEXCEPT
{
    return m_wrapper->is_valid();
}

/////////////////////////////////////////////////////////////////////////////////////////

method::operator bool() const RTTR_NOEXCEPT
{
    return m_wrapper->is_valid();
}

/////////////////////////////////////////////////////////////////////////////////////////

string_view method::get_name() const RTTR_NOEXCEPT
{
    return m_wrapper->get_name();
}

/////////////////////////////////////////////////////////////////////////////////////////

access_levels method::get_access_level() const RTTR_NOEXCEPT
{
    return m_wrapper->get_access_level();
}

/////////////////////////////////////////////////////////////////////////////////////////

bool method::is_static() const RTTR_NOEXCEPT
{
    return m_wrapper->is_static();
}

/////////////////////////////////////////////////////////////////////////////////////////

type method::get_return_type() const RTTR_NOEXCEPT
{
    return m_wrapper->get_return_type();
}

/////////////////////////////////////////////////////////////////////////////////////////

type method::get_declaring_type() const RTTR_NOEXCEPT
{
    return m_wrapper->get_declaring_type();
}

/////////////////////////////////////////////////////////////////////////////////////////

array_range<parameter_info> method::get_parameter_infos() const RTTR_NOEXCEPT
{
    return m_wrapper->get_parameter_infos();
}

/////////////////////////////////////////////////////////////////////////////////////////

string_view method::get_signature() const RTTR_NOEXCEPT
{
    return m_wrapper->get_signature();
}

/////////////////////////////////////////////////////////////////////////////////////////

variant method::get_metadata(const variant& key) const
{
    return m_wrapper->get_metadata(key);
}

/////////////////////////////////////////////////////////////////////////////////////////

variant method::invoke(instance object) const
{
    return m_wrapper->invoke(object);
}

/////////////////////////////////////////////////////////////////////////////////////////

variant method::invoke(instance object, argument arg1) const
{
    return m_wrapper->invoke(object, arg1);
}

/////////////////////////////////////////////////////////////////////////////////////////

variant method::invoke(instance object, argument arg1, argument arg2) const
{
    return m_wrapper->invoke(object, arg1, arg2);
}

/////////////////////////////////////////////////////////////////////////////////////////

variant method::invoke(instance object, argument arg1, argument arg2, argument arg3) const
{
    return m_wrapper->invoke(object, arg1, arg2, arg3);
}

/////////////////////////////////////////////////////////////////////////////////////////

variant method::invoke(instance object, argument arg1, argument arg2, argument arg3, argument arg4) const
{
    return m_wrapper->invoke(object, arg1, arg2, arg3, arg4);
}

/////////////////////////////////////////////////////////////////////////////////////////

variant method::invoke(instance object, argument arg1, argument arg2, argument arg3, argument arg4,
                       argument arg5) const
{
    return m_wrapper->invoke(object, arg1, arg2, arg3, arg4, arg5);
}

/////////////////////////////////////////////////////////////////////////////////////////

variant method::invoke(instance object, argument arg1, argument arg2, argument arg3, argument arg4,
                       argument arg5, argument arg6) const
{
    return m_wrapper->invoke(object, arg1, arg2, arg3, arg4, arg5, arg6);
}

/////////////////////////////////////////////////////////////////////////////////////////

variant method::invoke_variadic(instance object, std::vector<argument> args) const
{
    return m_wrapper->invoke_variadic(object, args);
}

/////////////////////////////////////////////////////////////////////////////////////////

bool method::operator==(const method& other) const RTTR_NOEXCEPT
{
    return (m_wrapper == other.m_wrapper);
}

/////////////////////////////////////////////////////////////////////////////////////////

bool method::operator!=(const method& other) const RTTR_NOEXCEPT
{
    return (m_wrapper != other.m_wrapper);
}

/////////////////////////////////////////////////////////////////////////////////////////

} // end namespace rttr
