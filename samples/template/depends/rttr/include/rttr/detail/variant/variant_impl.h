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

#ifndef RTTR_VARIANT_IMPL_H_
#define RTTR_VARIANT_IMPL_H_

#include "rttr/type.h"
#include "rttr/detail/misc/misc_type_traits.h"
#include "rttr/detail/misc/utility.h"
#include "rttr/detail/type/type_converter.h"
#include "rttr/detail/misc/data_address_container.h"
#include "rttr/detail/variant/variant_data_policy.h"
#include "rttr/variant_associative_view.h"
#include "rttr/variant_sequential_view.h"

namespace rttr
{
namespace detail
{
template<typename T>
using variant_t = remove_cv_t<remove_reference_t<T>>;
}

/////////////////////////////////////////////////////////////////////////////////////////

RTTR_INLINE variant::variant()
:   m_policy(&detail::variant_data_policy_empty::invoke)
{
}

/////////////////////////////////////////////////////////////////////////////////////////

template<typename T, typename Tp>
RTTR_INLINE variant::variant(T&& val)
:   m_policy(&detail::variant_policy<Tp>::invoke)
{
    static_assert(std::is_copy_constructible<Tp>::value || std::is_array<Tp>::value,
                  "The given value is not copy constructible, try to add a copy constructor to the class.");

    detail::variant_policy<Tp>::create(std::forward<T>(val), m_data);
}

/////////////////////////////////////////////////////////////////////////////////////////

RTTR_INLINE variant::~variant()
{
   m_policy(detail::variant_policy_operation::DESTROY, m_data, detail::argument_wrapper());
}

/////////////////////////////////////////////////////////////////////////////////////////

template<typename T, typename Tp>
RTTR_INLINE variant& variant::operator=(T&& other)
{
    *this = variant(std::forward<T>(other));
    return *this;
}

/////////////////////////////////////////////////////////////////////////////////////////

RTTR_INLINE bool variant::operator==(const variant& other) const
{
    auto ok = false;
    return compare_equal(other, ok);
}

/////////////////////////////////////////////////////////////////////////////////////////

RTTR_INLINE bool variant::operator!=(const variant& other) const
{
    auto ok = false;
    return !compare_equal(other, ok);
}

/////////////////////////////////////////////////////////////////////////////////////////

RTTR_INLINE bool variant::operator<(const variant& other) const
{
    bool ok = false;
    return compare_less(other, ok);
}

/////////////////////////////////////////////////////////////////////////////////////////

RTTR_INLINE bool variant::operator<=(const variant& other) const
{
    auto ok_equal = false, ok_less = false;
    return ((compare_equal(other, ok_equal) && ok_equal) ||
            (compare_less(other, ok_less) && ok_less));
}

/////////////////////////////////////////////////////////////////////////////////////////

RTTR_INLINE bool variant::operator>=(const variant& other) const
{
    auto ok_equal = false, ok_less = false;
    return ( ((compare_equal(other, ok_equal) && ok_equal) ||
              (!compare_less(other, ok_less) && ok_less))
            && is_valid() && other.is_valid());
}

/////////////////////////////////////////////////////////////////////////////////////////

RTTR_INLINE bool variant::operator>(const variant& other) const
{
    auto ok_equal = false, ok_less = false;
    return ((!compare_equal(other, ok_equal) && ok_equal) &&
            (!compare_less(other, ok_less) && ok_less));
}

/////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
RTTR_INLINE T& variant::get_value()
{
    using namespace detail;
    auto result = unsafe_variant_cast<variant_t<T>>(this);

    return *result;
}

/////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
RTTR_INLINE const T& variant::get_value() const
{
    using namespace detail;
    auto result = unsafe_variant_cast<variant_t<T>>(this);

    return *result;
}

/////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
RTTR_INLINE const T& variant::get_wrapped_value() const
{
    detail::data_address_container result{detail::get_invalid_type(), detail::get_invalid_type(), nullptr, nullptr};
    m_policy(detail::variant_policy_operation::GET_ADDRESS_CONTAINER, m_data, result);
    using nonRef = detail::remove_cv_t<T>;
    return *reinterpret_cast<const nonRef*>(result.m_data_address_wrapped_type);
}

/////////////////////////////////////////////////////////////////////////////////////////

RTTR_INLINE void* variant::get_ptr() const
{
    void* value;
    m_policy(detail::variant_policy_operation::GET_PTR, m_data, value);
    return value;
}

/////////////////////////////////////////////////////////////////////////////////////////

RTTR_INLINE type variant::get_raw_type() const
{
    type result = detail::get_invalid_type();
    m_policy(detail::variant_policy_operation::GET_RAW_TYPE, m_data, result);
    return result;
}

/////////////////////////////////////////////////////////////////////////////////////////

RTTR_INLINE void* variant::get_raw_ptr() const
{
    void* result;
    m_policy(detail::variant_policy_operation::GET_RAW_PTR, m_data, result);
    return result;
}

/////////////////////////////////////////////////////////////////////////////////////////

RTTR_INLINE detail::data_address_container variant::get_data_address_container() const
{
    detail::data_address_container result{detail::get_invalid_type(), detail::get_invalid_type(), nullptr, nullptr};
    m_policy(detail::variant_policy_operation::GET_ADDRESS_CONTAINER, m_data, result);
    return result;
}

/////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
RTTR_INLINE bool variant::is_type() const
{
    type src_type = detail::get_invalid_type();
    m_policy(detail::variant_policy_operation::GET_TYPE, m_data, src_type);
    return (type::get<T>() == src_type);
}

/////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
RTTR_INLINE bool variant::can_convert() const
{
    return can_convert(type::get<T>());
}

/////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
RTTR_INLINE bool variant::try_basic_type_conversion(T& to) const
{
    return m_policy(detail::variant_policy_operation::CONVERT, m_data, argument(to));
}

/////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
typename std::enable_if<detail::pointer_count<T>::value == 1, bool>::type
RTTR_INLINE variant::try_pointer_conversion(T& to, const type& source_type, const type& target_type) const
{
    if (!source_type.is_pointer())
        return false;

    auto ptr = get_raw_ptr();

    if (ptr)
    {
        if ((ptr = type::apply_offset(ptr, source_type, target_type)) != nullptr)
        {
            to = reinterpret_cast<T>(ptr);
            return true;
        }
    }
    else // a nullptr
    {
        // check if a down cast is possible
        if (source_type.is_derived_from(target_type))
        {
            to = reinterpret_cast<T>(ptr);
            return true;
        }
    }

    return false;
}

/////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
typename std::enable_if<detail::pointer_count<T>::value != 1, bool>::type
RTTR_INLINE variant::try_pointer_conversion(T& to, const type& source_type, const type& target_type) const
{
    return false;
}

/////////////////////////////////////////////////////////////////////////////////////////

RTTR_INLINE bool variant::is_nullptr() const
{
    return m_policy(detail::variant_policy_operation::IS_NULLPTR, m_data, detail::argument_wrapper());
}

/////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
typename std::enable_if<detail::is_nullptr_t<T>::value, bool>::type
static RTTR_INLINE ptr_to_nullptr(T& to)
{
    to = nullptr;
    return true;
}

/////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
typename std::enable_if<!detail::is_nullptr_t<T>::value, bool>::type
static RTTR_INLINE ptr_to_nullptr(T& to)
{
    return false;
}

/////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
RTTR_INLINE bool variant::convert(T& value) const
{
    bool ok = false;

    const type source_type = get_type();
    const type target_type = type::get<T>();
    if (source_type.is_wrapper() && !target_type.is_wrapper())
    {
        variant var = extract_wrapped_value();
        return var.convert<T>(value);
    }
    else if (!source_type.is_wrapper() && target_type.is_wrapper() &&
             target_type.get_wrapped_type() == source_type)
    {
        variant var = create_wrapped_value(target_type);
        if ((ok = var.is_valid()) == true)
            value = var.get_value<T>();
    }
    else if (target_type == source_type)
    {
        value = const_cast<variant&>(*this).get_value<T>();
        ok = true;
    }
    else if(try_basic_type_conversion(value))
    {
        ok = true;
    }
    else if (const auto& converter = source_type.get_type_converter(target_type))
    {
        const auto target_converter = static_cast<const detail::type_converter_target<T>*>(converter);
        value = target_converter->convert(get_ptr(), ok);
    }
    else if (target_type == type::get<std::nullptr_t>())
    {
        if (is_nullptr())
            ok = ptr_to_nullptr(value);
    }
    else
    {
        ok = try_pointer_conversion(value, source_type, target_type);
    }

    return ok;
}

/////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
RTTR_INLINE detail::enable_if_t<std::is_arithmetic<T>::value, T> variant::convert_impl(bool* ok) const
{
    T result = 0;
    const bool could_convert = convert<T>(result);
    if (ok)
        *ok = could_convert;

    return result;
}

/////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
RTTR_INLINE detail::enable_if_t<!std::is_arithmetic<T>::value && !std::is_enum<T>::value, T> variant::convert_impl(bool* ok) const
{
    static_assert(std::is_default_constructible<T>::value, "The given type T has no default constructor."
                                                           "You can only convert to a type, with a default constructor.");
    T result;
    const bool could_convert = convert<T>(result);
    if (ok)
        *ok = could_convert;

    return result;
}

/////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
RTTR_INLINE detail::enable_if_t<std::is_enum<T>::value, T> variant::convert_impl(bool* ok) const
{
    const auto target_type = type::get<T>();
    if (get_type() == target_type)
    {
        T result;
        const auto could_convert = convert<T>(result);
        if (ok)
            *ok = could_convert;

        return result;
    }
    else
    {
        variant var = type::get<T>();
        auto wrapper = std::ref(var);
        const auto could_convert = convert<std::reference_wrapper<variant>>(wrapper);
        if (ok)
            *ok = could_convert;

        return var.get_value<T>();
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
RTTR_INLINE T variant::convert(bool* ok) const
{
    return convert_impl<T>(ok);
}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

namespace detail
{

/////////////////////////////////////////////////////////////////////////////////////////

template<class T>
RTTR_INLINE T* unsafe_variant_cast(variant* operand) RTTR_NOEXCEPT
{
    const void* value;
    operand->m_policy(detail::variant_policy_operation::GET_VALUE, operand->m_data, value);
    return reinterpret_cast<T*>(const_cast<void*>(value));
}

/////////////////////////////////////////////////////////////////////////////////////////

template<class T>
RTTR_INLINE const T* unsafe_variant_cast(const variant* operand) RTTR_NOEXCEPT
{
    return unsafe_variant_cast<const T>(const_cast<variant*>(operand));
}

} // end namespace detail

/////////////////////////////////////////////////////////////////////////////////////////

template<class T>
RTTR_INLINE T variant_cast(const variant& operand)
{
    using namespace detail;
    static_assert(std::is_constructible<T, const variant_t<T>&>::value,
                  "variant_cast<T>(variant&) requires T to be constructible from const remove_cv_t<remove_reference_t<T>>&");

    auto result = unsafe_variant_cast<variant_t<T>>(&operand);

    using ref_type = conditional_t<std::is_reference<T>::value, T, add_lvalue_reference_t<T>>;
    return static_cast<ref_type>(*result);
}

/////////////////////////////////////////////////////////////////////////////////////////

template<class T>
RTTR_INLINE T variant_cast(variant& operand)
{
    using namespace detail;
    static_assert(std::is_constructible<T, variant_t<T>&>::value,
                  "variant_cast<T>(variant&) requires T to be constructible from remove_cv_t<remove_reference_t<T>>&");

    auto result = unsafe_variant_cast<variant_t<T>>(&operand);

    using ref_type = conditional_t<std::is_reference<T>::value, T, add_lvalue_reference_t<T>>;
    return static_cast<ref_type>(*result);
}

/////////////////////////////////////////////////////////////////////////////////////////

template<class T>
RTTR_INLINE T variant_cast(variant&& operand)
{
    using namespace detail;
    static_assert(std::is_constructible<T, variant_t<T>>::value,
                  "variant_cast<T>(variant&&) requires T to be constructible from remove_cv_t<remove_reference_t<T>>");
    auto result = unsafe_variant_cast<variant_t<T>>(&operand);
    return std::move(*result);
}

/////////////////////////////////////////////////////////////////////////////////////////

template<class T>
RTTR_INLINE T* variant_cast(variant* operand) RTTR_NOEXCEPT
{
    using namespace detail;
    return (type::get<T>() == operand->get_type()) ?
            unsafe_variant_cast<T>(operand) : nullptr;
}

/////////////////////////////////////////////////////////////////////////////////////////

template<class T>
RTTR_INLINE const T* variant_cast(const variant* operand) RTTR_NOEXCEPT
{
    return variant_cast<T>(const_cast<variant*>(operand));
}

/////////////////////////////////////////////////////////////////////////////////////////

} // end namespace rttr

#endif // RTTR_VARIANT_IMPL_H_
