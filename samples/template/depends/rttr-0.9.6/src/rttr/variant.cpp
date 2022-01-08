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

#include "rttr/variant.h"

#include "rttr/detail/variant/variant_data_policy.h"
#include "rttr/variant_associative_view.h"
#include "rttr/variant_sequential_view.h"
#include "rttr/argument.h"

#include <algorithm>
#include <limits>
#include <string>
#include <set>

namespace rttr
{

/////////////////////////////////////////////////////////////////////////////////////////

variant::variant(const variant& other)
:   m_policy(other.m_policy)
{
    m_policy(detail::variant_policy_operation::CLONE, other.m_data, m_data);
}

/////////////////////////////////////////////////////////////////////////////////////////

variant::variant(variant&& other)
:   m_policy(other.m_policy)
{
    other.m_policy(detail::variant_policy_operation::SWAP, other.m_data, m_data);
    other.m_policy = &detail::variant_data_policy_empty::invoke;
}

/////////////////////////////////////////////////////////////////////////////////////////

void variant::swap(variant& other)
{
    if (this == &other)
        return;

    const bool is_this_valid = is_valid();
    const bool is_other_valid = other.is_valid();

    if (!is_this_valid && !is_other_valid)
        return;

    if (is_this_valid && is_other_valid)
    {
        detail::variant_data tmp_data;
        detail::variant_policy_func tmp_policy_func = other.m_policy;
        other.m_policy(detail::variant_policy_operation::SWAP, other.m_data, tmp_data);

        m_policy(detail::variant_policy_operation::SWAP, m_data, other.m_data);
        other.m_policy = m_policy;

        tmp_policy_func(detail::variant_policy_operation::SWAP, tmp_data, m_data);
        m_policy = tmp_policy_func;
    }
    else
    {
        detail::variant_data& full_data = is_this_valid ? m_data : other.m_data;
        detail::variant_data& empty_data = is_this_valid ? other.m_data : m_data;
        detail::variant_policy_func full_policy_func = is_this_valid ? m_policy : other.m_policy;

        full_policy_func(detail::variant_policy_operation::SWAP, full_data, empty_data);

        std::swap(m_policy, other.m_policy);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

variant& variant::operator=(const variant& other)
{
    if (this == &other)
        return *this;

    m_policy(detail::variant_policy_operation::DESTROY, m_data, detail::argument_wrapper());
    other.m_policy(detail::variant_policy_operation::CLONE, other.m_data, m_data);
    m_policy = other.m_policy;

    return *this;
}

/////////////////////////////////////////////////////////////////////////////////////////

variant& variant::operator=(variant&& other)
{
    m_policy(detail::variant_policy_operation::DESTROY, m_data, detail::argument_wrapper());
    other.m_policy(detail::variant_policy_operation::SWAP, other.m_data, m_data);
    m_policy = other.m_policy;
    other.m_policy = &detail::variant_data_policy_empty::invoke;

    return *this;
}

/////////////////////////////////////////////////////////////////////////////////////////

bool variant::compare_equal(const variant& other, bool& ok) const
{
    ok = false;
    return m_policy(detail::variant_policy_operation::COMPARE_EQUAL, m_data, std::tie(*this, other, ok));
}

/////////////////////////////////////////////////////////////////////////////////////////

bool variant::compare_less(const variant& other, bool& ok) const
{
    return m_policy(detail::variant_policy_operation::COMPARE_LESS, m_data,  std::tie(*this, other, ok));
}

/////////////////////////////////////////////////////////////////////////////////////////

void variant::clear()
{
    m_policy(detail::variant_policy_operation::DESTROY, m_data, detail::argument_wrapper());
    m_policy = &detail::variant_data_policy_empty::invoke;
}

/////////////////////////////////////////////////////////////////////////////////////////

bool variant::is_valid() const
{
    return m_policy(detail::variant_policy_operation::IS_VALID, m_data, detail::argument_wrapper());
}

/////////////////////////////////////////////////////////////////////////////////////////

variant::operator bool() const
{
    return m_policy(detail::variant_policy_operation::IS_VALID, m_data, detail::argument_wrapper());
}

/////////////////////////////////////////////////////////////////////////////////////////

bool variant::is_associative_container() const
{
    return m_policy(detail::variant_policy_operation::IS_ASSOCIATIVE_CONTAINER, m_data, detail::argument_wrapper());
}

/////////////////////////////////////////////////////////////////////////////////////////

bool variant::is_sequential_container() const
{
    return m_policy(detail::variant_policy_operation::IS_SEQUENTIAL_CONTAINER, m_data, detail::argument_wrapper());
}

/////////////////////////////////////////////////////////////////////////////////////////

type variant::get_type() const
{
    type src_type = detail::get_invalid_type();
    m_policy(detail::variant_policy_operation::GET_TYPE, m_data, src_type);
    return src_type;
}

/////////////////////////////////////////////////////////////////////////////////////////

variant variant::extract_wrapped_value() const
{
    variant var;
    m_policy(detail::variant_policy_operation::EXTRACT_WRAPPED_VALUE, m_data, var);
    return var;
}

/////////////////////////////////////////////////////////////////////////////////////////

variant variant::create_wrapped_value(const type& wrapped_type) const
{
    variant var;
    m_policy(detail::variant_policy_operation::CREATE_WRAPPED_VALUE, m_data, std::tie(var, wrapped_type));
    return var;
}

/////////////////////////////////////////////////////////////////////////////////////////

variant_associative_view variant::create_associative_view() const
{
    variant_associative_view result;
    m_policy(detail::variant_policy_operation::CREATE_ASSOCIATIV_VIEW, m_data, result.m_view);
    return result;
}

/////////////////////////////////////////////////////////////////////////////////////////

variant_sequential_view variant::create_sequential_view() const
{
    variant_sequential_view result;
    m_policy(detail::variant_policy_operation::CREATE_SEQUENTIAL_VIEW, m_data, result.m_view);
    return result;
}

/////////////////////////////////////////////////////////////////////////////////////////

bool variant::can_convert(const type& target_type) const
{
    if (!is_valid())
        return false;

    type source_type = get_type();
    source_type = (source_type.is_wrapper() && !target_type.is_wrapper()) ? source_type.get_wrapped_type() : source_type;

    if (source_type == target_type)
        return true;

    if (source_type.get_pointer_dimension() == 1 && target_type.get_pointer_dimension() == 1)
    {
        if (type::apply_offset(get_raw_ptr(), source_type, target_type))
            return true;
    }

    if (!source_type.is_wrapper() && target_type.is_wrapper())
    {
        if (target_type.get_wrapped_type() == source_type && target_type.m_type_data->create_wrapper)
            return true;
    }

    if (source_type.get_type_converter(target_type))
        return true;

    if (target_type == type::get<std::nullptr_t>() && is_nullptr())
        return true;

    const bool source_is_arithmetic = source_type.is_arithmetic();
    const bool target_is_arithmetic = target_type.is_arithmetic();
    const bool target_is_enumeration = target_type.is_enumeration();
    const type string_type = type::get<std::string>();

    return ((source_is_arithmetic && target_is_arithmetic) ||
            (source_is_arithmetic && target_type == string_type) ||
            (source_type == string_type && target_is_arithmetic) ||
            (source_type.is_enumeration() && target_is_arithmetic) ||
            (source_is_arithmetic && target_is_enumeration) ||
            (source_type == string_type && target_is_enumeration));
}

/////////////////////////////////////////////////////////////////////////////////////////

bool variant::convert(const type& target_type, variant& target_var) const
{
    if (!is_valid())
        return false;

    bool ok = false;

    const type source_type = get_type();
    const bool source_is_arithmetic = source_type.is_arithmetic();
    const bool target_is_arithmetic = target_type.is_arithmetic();
    const type string_type = type::get<std::string>();
    if (target_type == source_type)
    {
        target_var = *this;
        return true; // the current variant is already the target type, we don't need to do anything
    }
    else if (!source_type.is_wrapper() && target_type.is_wrapper() &&
             target_type.get_wrapped_type() == source_type)
    {
        target_var = create_wrapped_value(target_type);
        ok = target_var.is_valid();
    }
    else if (source_type.is_wrapper() && !target_type.is_wrapper())
    {
        variant var = extract_wrapped_value();
        ok = var.convert(target_type);
        target_var = var;
    }
    else if ((source_is_arithmetic && target_is_arithmetic) ||
            (source_is_arithmetic && target_type == string_type) ||
            (source_type == string_type && target_is_arithmetic) ||
            (source_type.is_enumeration() && target_is_arithmetic) ||
            (source_type.is_enumeration() && target_type == string_type))
    {
        if (target_type == type::get<bool>())
        {
            bool value;
            if ((ok = try_basic_type_conversion(value)) == true)
                target_var = value;
        }
        else if (target_type == type::get<char>())
        {
            char value;
            if ((ok = try_basic_type_conversion(value)) == true)
                target_var = value;
        }
        else if (target_type == type::get<int8_t>())
        {
            int8_t value;
            if ((ok = try_basic_type_conversion(value)) == true)
                target_var = value;
        }
        else if (target_type == type::get<int16_t>())
        {
            int16_t value;
            if ((ok = try_basic_type_conversion(value)) == true)
                target_var = value;
        }
        else if (target_type == type::get<int32_t>())
        {
            int32_t value;
            if ((ok = try_basic_type_conversion(value)) == true)
                target_var = value;
        }
        else if (target_type == type::get<int64_t>())
        {
            int64_t value;
            if ((ok = try_basic_type_conversion(value)) == true)
                target_var = value;
        }
        else if (target_type == type::get<uint8_t>())
        {
            uint8_t value;
            if ((ok = try_basic_type_conversion(value)) == true)
                target_var = value;
        }
        else if (target_type == type::get<uint16_t>())
        {
            uint16_t value;
            if ((ok = try_basic_type_conversion(value)) == true)
                target_var = value;
        }
        else if (target_type == type::get<uint32_t>())
        {
            uint32_t value;
            if ((ok = try_basic_type_conversion(value)) == true)
                target_var = value;
        }
        else if (target_type == type::get<uint64_t>())
        {
            uint64_t value;
            if ((ok = try_basic_type_conversion(value)) == true)
                target_var = value;
        }
        else if (target_type == type::get<float>())
        {
            float value;
            if ((ok = try_basic_type_conversion(value)) == true)
                target_var = value;
        }
        else if (target_type == type::get<double>())
        {
            double value;
            if ((ok = try_basic_type_conversion(value)) == true)
                target_var = value;
        }
        else if (target_type == string_type)
        {
            std::string value;
            if ((ok = try_basic_type_conversion(value)) == true)
                target_var = std::move(value);
        }
    }
    else if ((source_is_arithmetic || source_type == string_type)
             && target_type.is_enumeration())
    {
        variant var = target_type;
        auto wrapper = std::ref(var);
        if ((ok = try_basic_type_conversion(wrapper)) == true)
            target_var = std::move(var);
    }
    else
    {
        if (const auto& converter = source_type.get_type_converter(target_type))
        {
            void* ptr = get_ptr();
            target_var = converter->to_variant(ptr, ok);
        }
        else if (target_type == type::get<std::nullptr_t>() && is_nullptr())
        {
            target_var = nullptr;
            ok = true;
        }
        else if (source_type.is_pointer() &&
                 (source_type.get_pointer_dimension() == 1 && target_type.get_pointer_dimension() == 1))
        {
            void* raw_ptr = get_raw_ptr();
            if (void* casted_ptr = type::apply_offset(raw_ptr, source_type, target_type))
            {
                // although we forward a void* to create a variant,
                // it will create a variant for the specific class type
                target_var = target_type.create_variant(casted_ptr);
                if (target_var.is_valid())
                    ok = true;
            }
        }
    }

    return ok;
}

/////////////////////////////////////////////////////////////////////////////////////////

bool variant::convert(const type& target_type)
{
    return convert(target_type, *this);
}

/////////////////////////////////////////////////////////////////////////////////////////

bool variant::to_bool() const
{
    return convert<bool>(nullptr);
}

/////////////////////////////////////////////////////////////////////////////////////////

int variant::to_int(bool *ok) const
{
    return convert<int>(ok);
}

/////////////////////////////////////////////////////////////////////////////////////////

std::string variant::to_string(bool *ok) const
{
    return convert<std::string>(ok);
}

/////////////////////////////////////////////////////////////////////////////////////////

float variant::to_float(bool* ok) const
{
    return convert<float>(ok);
}

/////////////////////////////////////////////////////////////////////////////////////////

double variant::to_double(bool* ok) const
{
    return convert<double>(ok);
}

/////////////////////////////////////////////////////////////////////////////////////////

int8_t variant::to_int8(bool *ok) const
{
    return convert<int8_t>(ok);
}

/////////////////////////////////////////////////////////////////////////////////////////

int16_t variant::to_int16(bool *ok) const
{
    return convert<int16_t>(ok);
}

/////////////////////////////////////////////////////////////////////////////////////////

int32_t variant::to_int32(bool *ok) const
{
    return convert<int32_t>(ok);
}

/////////////////////////////////////////////////////////////////////////////////////////

int64_t variant::to_int64(bool *ok) const
{
    return convert<int64_t>(ok);
}

/////////////////////////////////////////////////////////////////////////////////////////

uint8_t variant::to_uint8(bool *ok) const
{
    return convert<uint8_t>(ok);
}

/////////////////////////////////////////////////////////////////////////////////////////

uint16_t variant::to_uint16(bool *ok) const
{
    return convert<uint16_t>(ok);
}

/////////////////////////////////////////////////////////////////////////////////////////

uint32_t variant::to_uint32(bool *ok) const
{
    return convert<uint32_t>(ok);
}

/////////////////////////////////////////////////////////////////////////////////////////

uint64_t variant::to_uint64(bool *ok) const
{
    return convert<uint64_t>(ok);
}

/////////////////////////////////////////////////////////////////////////////////////////

} // end namespace rttr
