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

#include "rttr/detail/enumeration/enumeration_helper.h"
#include "rttr/enumeration.h"
#include "rttr/argument.h"

namespace rttr
{
namespace detail
{

/////////////////////////////////////////////////////////////////////////////////////////

string_view get_enumeration_name(const argument& arg)
{
    return arg.get_type().get_enumeration().value_to_name(arg);
}

/////////////////////////////////////////////////////////////////////////////////////////

bool to_enumeration(string_view from, argument& to)
{
    auto& var_ref = to.get_value<std::reference_wrapper<variant>>();
    variant& var = var_ref.get();
    const type enum_type = var.get_value<type>();
    if (variant var_tmp = enum_type.get_enumeration().name_to_value(from))
    {
        var = var_tmp;
        return var.is_valid();
    }
    else
    {
        return false;
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

bool to_enumeration(const variant& from, argument& to)
{
    auto& var_ref = to.get_value<std::reference_wrapper<variant>>();
    variant& var = var_ref.get();
    const type enum_type = var.get_value<type>();
    const enumeration e = enum_type.get_enumeration();
    const type underlying_enum_type = e.get_underlying_type();
    for (const auto& value : e.get_values())
    {
        variant var_cpy = value;
        const bool ret = var_cpy.convert(underlying_enum_type);
        if (ret && var_cpy == from)
        {
            var = value;
            return var.is_valid();
        }
    }

    return false;
}

/////////////////////////////////////////////////////////////////////////////////////////

bool is_variant_with_enum(const argument& arg)
{
    if (arg.is_type<std::reference_wrapper<variant>>())
    {
        const auto& var = arg.get_value<std::reference_wrapper<variant>>().get();
        return (var.is_type<type>() && var.get_value<type>().is_enumeration());
    }
    else
    {
        return false;
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

} // end namespace detail
} // end namespace rttr
