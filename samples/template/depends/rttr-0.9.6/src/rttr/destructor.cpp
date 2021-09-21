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

#include "rttr/destructor.h"
#include "rttr/detail/destructor/destructor_wrapper_base.h"

using namespace std;

namespace rttr
{

namespace detail
{

template<>
destructor create_item(const destructor_wrapper_base* wrapper)
{
    return destructor(wrapper);
}

template<>
destructor create_invalid_item()
{
    static const destructor_wrapper_base invalid_wrapper;
    return destructor(&invalid_wrapper);
}

} // end namespace detail

/////////////////////////////////////////////////////////////////////////////////////////

destructor::destructor(const detail::destructor_wrapper_base* wrapper) RTTR_NOEXCEPT
:   m_wrapper(wrapper)
{

}

/////////////////////////////////////////////////////////////////////////////////////////

bool destructor::is_valid() const RTTR_NOEXCEPT
{
    return m_wrapper->is_valid();
}

/////////////////////////////////////////////////////////////////////////////////////////

destructor::operator bool() const RTTR_NOEXCEPT
{
    return m_wrapper->is_valid();
}

/////////////////////////////////////////////////////////////////////////////////////////

type destructor::get_declaring_type() const RTTR_NOEXCEPT
{
    return m_wrapper->get_declaring_type();
}

/////////////////////////////////////////////////////////////////////////////////////////

type destructor::get_destructed_type() const RTTR_NOEXCEPT
{
    return m_wrapper->get_destructed_type();
}

/////////////////////////////////////////////////////////////////////////////////////////

bool destructor::invoke(variant& obj) const RTTR_NOEXCEPT
{
    return m_wrapper->invoke(obj);
}

/////////////////////////////////////////////////////////////////////////////////////////

bool destructor::operator==(const destructor& other) const RTTR_NOEXCEPT
{
    return (m_wrapper == other.m_wrapper);
}

/////////////////////////////////////////////////////////////////////////////////////////

bool destructor::operator!=(const destructor& other) const RTTR_NOEXCEPT
{
    return (m_wrapper != other.m_wrapper);
}

/////////////////////////////////////////////////////////////////////////////////////////

} // end namespace rttr
