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

#ifndef RTTR_ENUMERATION_WRAPPER_H_
#define RTTR_ENUMERATION_WRAPPER_H_

#include "rttr/detail/base/core_prerequisites.h"
#include "rttr/detail/enumeration/enumeration_wrapper_base.h"
#include "rttr/detail/enumeration/enum_data.h"
#include "rttr/argument.h"
#include "rttr/variant.h"
#include "rttr/string_view.h"

#include <utility>
#include <type_traits>

namespace rttr
{
namespace detail
{

template<typename Enum_Type, std::size_t N, std::size_t Metadata_Count>
class enumeration_wrapper : public enumeration_wrapper_base, public metadata_handler<Metadata_Count>
{
    public:
        enumeration_wrapper(std::array< enum_data<Enum_Type>, N > data,
                            std::array<metadata, Metadata_Count> metadata_list) RTTR_NOEXCEPT
        :   metadata_handler<Metadata_Count>(std::move(metadata_list))
        {
            int index = 0;
            for (const auto& item : data)
            {
                 m_enum_names[index]    = item.get_name();
                 m_enum_values[index]   = item.get_value();
                 m_enum_variant_values[index] = item.get_value();
                 ++index;
            }
            static_assert(std::is_enum<Enum_Type>::value, "No enum type provided, please create an instance of this class only for enum types!");
        }

        bool is_valid() const RTTR_NOEXCEPT { return true; }
        type get_type() const RTTR_NOEXCEPT { return type::get<Enum_Type>(); }
        type get_underlying_type() const RTTR_NOEXCEPT { return type::get<typename std::underlying_type<Enum_Type>::type>(); }

        array_range<string_view> get_names() const RTTR_NOEXCEPT
        {
            return array_range<string_view>(m_enum_names.data(), N);
        }

        array_range<variant> get_values() const RTTR_NOEXCEPT
        {
            return array_range<variant>(m_enum_variant_values.data(), N);
        }

        string_view value_to_name(argument& value) const
        {
            if (!value.is_type<Enum_Type>() &&
                !value.is_type<typename std::underlying_type<Enum_Type>::type>())
            {
                return string_view();
            }

            const Enum_Type enum_value = value.get_value<Enum_Type>();
            int index = 0;
            for (const auto& item : m_enum_values)
            {
                if (item == enum_value)
                    return m_enum_names[index];

                ++index;
            }

            return string_view();
        }

        variant name_to_value(string_view name) const
        {
            int index = 0;
            for (const auto& item : m_enum_names)
            {
                if (item == name)
                    return m_enum_values[index];

                ++index;
            }
            return variant();
        }

        variant get_metadata(const variant& key) const { return metadata_handler<Metadata_Count>::get_metadata(key); }

    private:
        std::array< string_view, N >    m_enum_names;
        std::array< Enum_Type, N >      m_enum_values;
        std::array< variant, N >        m_enum_variant_values;
};

/////////////////////////////////////////////////////////////////////////////////////////

} // end namespace detail
} // end namespace rttr

#endif // RTTR_ENUMERATION_WRAPPER_H_
