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

#ifndef RTTR_FILTER_ITEM_FUNCS_H_
#define RTTR_FILTER_ITEM_FUNCS_H_

#include "rttr/detail/base/core_prerequisites.h"
#include "rttr/filter_item.h"
#include "rttr/constructor.h"
#include "rttr/array_range.h"

namespace rttr
{
namespace detail
{

/////////////////////////////////////////////////////////////////////////////////////////

static bool is_valid_filter_item(filter_items filter)
{
    if ((filter.test_flag(filter_item::public_access) ||
         filter.test_flag(filter_item::non_public_access)) &&
        (filter.test_flag(filter_item::instance_item) ||
        filter.test_flag(filter_item::static_item)))
    {
        return true;
    }

    return false;
}

/////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
RTTR_INLINE bool filter_member_item(const T& item, const type& t, filter_items filter)
{
    bool result = true;

    if (filter.test_flag(filter_item::public_access) && filter.test_flag(filter_item::non_public_access))
    {
        result &= true;
    }
    else if (filter.test_flag(filter_item::public_access))
    {
        result &= (item.get_access_level() == access_levels::public_access);
    }
    else if (filter.test_flag(filter_item::non_public_access))
    {
        const auto access_level = item.get_access_level();
        result &= (access_level == access_levels::private_access || access_level == access_levels::protected_access);
    }

    if (filter.test_flag(filter_item::instance_item) && filter.test_flag(filter_item::static_item))
        result &= true;
    else if (filter.test_flag(filter_item::instance_item) && !filter.test_flag(filter_item::static_item))
        result &= !item.is_static();
    else if (!filter.test_flag(filter_item::instance_item) && filter.test_flag(filter_item::static_item))
        result &= item.is_static();

    if (filter.test_flag(filter_item::declared_only))
        result &= (item.get_declaring_type() == t);

    return result;
}

/////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
detail::default_predicate<T> get_filter_predicate(const type& t, filter_items filter)
{
    if (!is_valid_filter_item(filter))
    {
        return {[](const T&){ return false; }};
    }
    else
    {
        return {[filter, t](const T& item)
        {
            return filter_member_item<T>(item, t, filter);
        }};
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

template<>
RTTR_INLINE detail::default_predicate<constructor> get_filter_predicate(const type& t, filter_items filter)
{
    if (!is_valid_filter_item(filter))
    {
        return {[](const constructor&){ return false; }};
    }
    else
    {
        return {[filter](const constructor& item)
        {
            bool result = true;

            if (filter.test_flag(filter_item::public_access) && filter.test_flag(filter_item::non_public_access))
            {
                result &= true;
            }
            else if (filter.test_flag(filter_item::public_access))
            {
                result &= (item.get_access_level() == access_levels::public_access);
            }
            else if (filter.test_flag(filter_item::non_public_access))
            {
                const auto access_level = item.get_access_level();
                result &= (access_level == access_levels::private_access || access_level == access_levels::protected_access);
            }

            return result;
        }};
    }
}
/////////////////////////////////////////////////////////////////////////////////////////

} // end namespace detail
} // end namespace rttr


#endif // RTTR_FILTER_ITEM_FUNCS_H_
