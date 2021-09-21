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

#include "rttr/constructor.h"
#include "rttr/detail/constructor/constructor_wrapper_base.h"
#include "rttr/argument.h"

#include <utility>


using namespace std;

namespace rttr
{
namespace detail
{

template<>
constructor create_item(const constructor_wrapper_base* wrapper)
{
    return constructor(wrapper);
}

template<>
constructor create_invalid_item()
{
    static const constructor_wrapper_base invalid_wrapper;
    return constructor(&invalid_wrapper);
}

} // end namespace detail

/////////////////////////////////////////////////////////////////////////////////////////

constructor::constructor(const detail::constructor_wrapper_base* wrapper) RTTR_NOEXCEPT
:   m_wrapper(wrapper)
{

}

/////////////////////////////////////////////////////////////////////////////////////////

bool constructor::is_valid() const RTTR_NOEXCEPT
{
    return m_wrapper->is_valid();
}

/////////////////////////////////////////////////////////////////////////////////////////

constructor::operator bool() const RTTR_NOEXCEPT
{
    return m_wrapper->is_valid();
}

/////////////////////////////////////////////////////////////////////////////////////////

access_levels constructor::get_access_level() const RTTR_NOEXCEPT
{
    return m_wrapper->get_access_level();
}

/////////////////////////////////////////////////////////////////////////////////////////

type constructor::get_instantiated_type() const RTTR_NOEXCEPT
{
    return m_wrapper->get_instantiated_type();
}

/////////////////////////////////////////////////////////////////////////////////////////

type constructor::get_declaring_type() const RTTR_NOEXCEPT
{
    return m_wrapper->get_declaring_type();
}


/////////////////////////////////////////////////////////////////////////////////////////

string_view constructor::get_signature() const RTTR_NOEXCEPT
{
    return m_wrapper->get_signature();
}

/////////////////////////////////////////////////////////////////////////////////////////

array_range<parameter_info> constructor::get_parameter_infos() const RTTR_NOEXCEPT
{
    return m_wrapper->get_parameter_infos();
}

/////////////////////////////////////////////////////////////////////////////////////////

variant constructor::get_metadata(const variant& key) const
{
    return m_wrapper->get_metadata(key);
}

/////////////////////////////////////////////////////////////////////////////////////////

variant constructor::invoke() const
{
    return m_wrapper->invoke();
}

/////////////////////////////////////////////////////////////////////////////////////////

variant constructor::invoke(argument arg1) const
{
    return m_wrapper->invoke(arg1);
}

/////////////////////////////////////////////////////////////////////////////////////////

variant constructor::invoke(argument arg1, argument arg2) const
{
    return m_wrapper->invoke(arg1, arg2);
}

/////////////////////////////////////////////////////////////////////////////////////////

variant constructor::invoke(argument arg1, argument arg2, argument arg3) const
{
    return m_wrapper->invoke(arg1, arg2, arg3);
}

/////////////////////////////////////////////////////////////////////////////////////////

variant constructor::invoke(argument arg1, argument arg2, argument arg3, argument arg4) const
{
    return m_wrapper->invoke(arg1, arg2, arg3, arg4);
}

/////////////////////////////////////////////////////////////////////////////////////////

variant constructor::invoke(argument arg1, argument arg2, argument arg3, argument arg4,
                            argument arg5) const
{
    return m_wrapper->invoke(arg1, arg2, arg3, arg4, arg5);
}

/////////////////////////////////////////////////////////////////////////////////////////

variant constructor::invoke(argument arg1, argument arg2, argument arg3, argument arg4,
                            argument arg5, argument arg6) const
{
    return m_wrapper->invoke(arg1, arg2, arg3, arg4, arg5, arg6);
}

/////////////////////////////////////////////////////////////////////////////////////////

variant constructor::invoke_variadic(std::vector<argument> args) const
{
    return m_wrapper->invoke_variadic(args);
}

/////////////////////////////////////////////////////////////////////////////////////////

bool constructor::operator==(const constructor& other) const RTTR_NOEXCEPT
{
    return (m_wrapper == other.m_wrapper);
}

/////////////////////////////////////////////////////////////////////////////////////////

bool constructor::operator!=(const constructor& other) const RTTR_NOEXCEPT
{
    return (m_wrapper != other.m_wrapper);
}

/////////////////////////////////////////////////////////////////////////////////////////


} // end namespace rttr
