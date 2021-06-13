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

#ifndef RTTR_PROP_POLICIES_H_
#define RTTR_PROP_POLICIES_H_

#include "rttr/detail/base/core_prerequisites.h"
#include "rttr/detail/misc/misc_type_traits.h"

namespace rttr
{
namespace detail
{

struct return_as_ptr
{};

struct set_as_ptr
{};

struct set_as_ref_wrapper
{};

struct get_as_ref_wrapper
{};

struct return_as_copy
{};

struct read_only
{};

struct set_value
{};


/////////////////////////////////////////////////////////////////////////////////////////

// default getter policy
template<typename T>
struct get_getter_policy
{
    using type = return_as_copy;
};

// default setter policy
template<typename T>
struct get_setter_policy
{
    using type = set_value;
};

/////////////////////////////////////////////////////////////////////////////////////////

struct bind_as_ptr
{};

/////////////////////////////////////////////////////////////////////////////////////////

template<>
struct get_getter_policy<bind_as_ptr>
{
    using type = return_as_ptr;
};

template<>
struct get_setter_policy<bind_as_ptr>
{
    using type = set_as_ptr;
};

/////////////////////////////////////////////////////////////////////////////////////////

struct as_reference_wrapper
{};

/////////////////////////////////////////////////////////////////////////////////////////

template<>
struct get_getter_policy<as_reference_wrapper>
{
    using type = get_as_ref_wrapper;
};

template<>
struct get_setter_policy<as_reference_wrapper>
{
    using type = set_as_ref_wrapper;
};

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

using property_policy_list = type_list<bind_as_ptr, read_only, as_reference_wrapper>;

} // end namespace detail;

} // end namespace rttr

#endif // RTTR_PROP_POLICIES_H_
