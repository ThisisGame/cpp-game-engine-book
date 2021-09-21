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

#ifndef RTTR_PARAMETER_INFOS_COMPARE_H_
#define RTTR_PARAMETER_INFOS_COMPARE_H_

#include "rttr/detail/base/core_prerequisites.h"
#include "rttr/parameter_info.h"
#include "rttr/argument.h"
#include "rttr/type.h"
#include "rttr/array_range.h"

#include <vector>

namespace rttr
{

class parameter_info;
class type;

namespace detail
{

/////////////////////////////////////////////////////////////////////////////////////////

struct compare_with_type_list
{
    static RTTR_INLINE bool compare(const array_range<parameter_info>& param_list, const std::vector<type>& arg_types)
    {
        const auto param_count = param_list.size();
        if (param_count != arg_types.size())
            return false;

        auto itr = param_list.begin();
        std::size_t index = 0;
        for (;itr != param_list.end(); ++itr, ++index)
        {
            if ((itr->get_type() != arg_types[index]))
                return false;
        }

        return true;
    }
};

/////////////////////////////////////////////////////////////////////////////////////////

struct compare_with_arg_list
{
    static RTTR_INLINE bool compare(const array_range<parameter_info>& param_list, const std::vector<argument>& args)
    {
        const auto param_count = param_list.size();
        const auto arg_count = args.size();
        if (arg_count > param_count)
            return false;

        auto itr = param_list.begin();
        std::size_t index = 0;
        for (; index < arg_count; ++itr, ++index)
        {
            if ((itr->get_type() != args[index].get_type()))
                return false;
        }

        // when there are still some parameter left, check if they are default values or not
        for (;itr != param_list.end(); ++itr)
        {
            if (!itr->has_default_value())
                return false;
        }

        return true;
    }
};

/////////////////////////////////////////////////////////////////////////////////////////

} // end namespace detail
} // end namespace rttr

#endif // RTTR_PARAMETER_INFOS_COMPARE_H_
