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

#include "rttr/detail/registration/registration_state_saver.h"

namespace rttr
{
namespace detail
{

/////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
static std::vector<T> copy_range(const array_range<T>& range, typename array_range<T>::size_type from_index)
{
    std::vector<T> result;
    const auto size = range.size();
    result.reserve(size - from_index);
    typename array_range<T>::size_type index = 0;
    for (auto& item : range)
    {
        if (index >= from_index)
            result.push_back(item);

        ++index;
    }

    return result;
}

/////////////////////////////////////////////////////////////////////////////////////////

void registration_state_saver::save_state_begin()
{
    m_old_type_size     = type::get_types().size();
    m_old_methods_size  = type::get_global_methods().size();
    m_old_property_size = type::get_global_properties().size();
}

/////////////////////////////////////////////////////////////////////////////////////////

void registration_state_saver::save_state_end()
{
    m_types             = copy_range(type::get_types(), m_old_type_size);
    m_global_methods    = copy_range(type::get_global_methods(), m_old_methods_size);
    m_global_properties = copy_range(type::get_global_properties(), m_old_property_size);

    m_types.shrink_to_fit();
    m_global_methods.shrink_to_fit();
    m_global_properties.shrink_to_fit();
}

/////////////////////////////////////////////////////////////////////////////////////////

void registration_state_saver::reset()
{
    m_old_type_size = 0;
    m_old_property_size = 0;
    m_old_methods_size = 0;

    m_types.clear();
    m_global_properties.clear();
    m_global_methods.clear();
}

/////////////////////////////////////////////////////////////////////////////////////////

array_range<type> registration_state_saver::get_types() const RTTR_NOEXCEPT
{
    return {m_types.data(), m_types.size()};
}

/////////////////////////////////////////////////////////////////////////////////////////

array_range<property> registration_state_saver::get_global_properties() const RTTR_NOEXCEPT
{
    return {m_global_properties.data(), m_global_properties.size()};
}

/////////////////////////////////////////////////////////////////////////////////////////

array_range<method> registration_state_saver::get_global_methods() const RTTR_NOEXCEPT
{
    return {m_global_methods.data(), m_global_methods.size()};
}

/////////////////////////////////////////////////////////////////////////////////////////

} // end namespace detail
} // end namespace rttr

