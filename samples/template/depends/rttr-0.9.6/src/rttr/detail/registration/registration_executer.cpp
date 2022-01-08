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

#include "rttr/detail/registration/registration_executer.h"

#include <algorithm>

namespace rttr
{
namespace detail
{

/////////////////////////////////////////////////////////////////////////////////////////

registration_executer::registration_executer()
{
}

/////////////////////////////////////////////////////////////////////////////////////////

registration_executer::~registration_executer()
{
    for (auto&& item : m_list)
    {
        item.second();
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

void registration_executer::add_registration_func(const void* key, std::function<void(void)> func)
{
    auto itr = std::find_if(m_list.begin(), m_list.end(), [key](const item_type& item) { return (item.first == key); });
    if (itr != m_list.end())
    {
        itr->second = std::move(func);
    }
    else
    {
        m_list.emplace_back(std::make_pair(key, func));
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

} // end namespace detail
} // end namespace rttr
