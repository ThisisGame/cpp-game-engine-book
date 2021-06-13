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

#ifndef RTTR_TYPE_IMPL_H_
#define RTTR_TYPE_IMPL_H_

#include <type_traits>
#include "rttr/detail/misc/misc_type_traits.h"
#include "rttr/detail/misc/function_traits.h"
#include "rttr/detail/type/base_classes.h"
#include "rttr/detail/type/get_derived_info_func.h"
#include "rttr/detail/type/get_create_variant_func.h"
#include "rttr/detail/type/type_register.h"
#include "rttr/detail/misc/utility.h"
#include "rttr/wrapper_mapper.h"
#include "rttr/detail/type/type_comparator.h"
#include "rttr/detail/type/type_data.h"
#include "rttr/detail/type/type_name.h"
#include "rttr/detail/registration/registration_manager.h"
#include "rttr/detail/misc/register_wrapper_mapper_conversion.h"


namespace rttr
{

/////////////////////////////////////////////////////////////////////////////////////////

RTTR_INLINE type::type(detail::type_data* data) RTTR_NOEXCEPT
:  m_type_data(data)
{
}

/////////////////////////////////////////////////////////////////////////////////////////

RTTR_INLINE type::type(const type& other) RTTR_NOEXCEPT
:   m_type_data(other.m_type_data)
{
}

/////////////////////////////////////////////////////////////////////////////////////////

RTTR_INLINE type& type::operator=(const type& other) RTTR_NOEXCEPT
{
    m_type_data = other.m_type_data;
    return *this;
}

/////////////////////////////////////////////////////////////////////////////////////////

RTTR_INLINE bool type::operator<(const type& other) const RTTR_NOEXCEPT
{
    return (m_type_data < other.m_type_data);
}

/////////////////////////////////////////////////////////////////////////////////////////

RTTR_INLINE bool type::operator>(const type& other) const RTTR_NOEXCEPT
{
    return (m_type_data > other.m_type_data);
}

/////////////////////////////////////////////////////////////////////////////////////////

RTTR_INLINE bool type::operator>=(const type& other) const RTTR_NOEXCEPT
{
    return (m_type_data >= other.m_type_data);
}

/////////////////////////////////////////////////////////////////////////////////////////

RTTR_INLINE bool type::operator<=(const type& other) const RTTR_NOEXCEPT
{
    return (m_type_data <= other.m_type_data);
}

/////////////////////////////////////////////////////////////////////////////////////////

RTTR_INLINE bool type::operator==(const type& other) const RTTR_NOEXCEPT
{
    return (m_type_data == other.m_type_data);
}

/////////////////////////////////////////////////////////////////////////////////////////

RTTR_INLINE bool type::operator!=(const type& other) const RTTR_NOEXCEPT
{
    return (m_type_data != other.m_type_data);
}

/////////////////////////////////////////////////////////////////////////////////////////

RTTR_INLINE type::type_id type::get_id() const RTTR_NOEXCEPT
{
    return reinterpret_cast<type::type_id>(m_type_data);
}

/////////////////////////////////////////////////////////////////////////////////////////

RTTR_INLINE bool type::is_valid() const RTTR_NOEXCEPT
{
    return m_type_data->is_valid;
}

/////////////////////////////////////////////////////////////////////////////////////////

RTTR_INLINE type::operator bool() const RTTR_NOEXCEPT
{
    return m_type_data->is_valid;
}

/////////////////////////////////////////////////////////////////////////////////////////

RTTR_INLINE type type::get_raw_type() const RTTR_NOEXCEPT
{
    return type(m_type_data->raw_type_data);
}

/////////////////////////////////////////////////////////////////////////////////////////

RTTR_INLINE type type::get_wrapped_type() const RTTR_NOEXCEPT
{
    return type(m_type_data->wrapped_type);
}

/////////////////////////////////////////////////////////////////////////////////////////

RTTR_INLINE type type::get_raw_array_type() const RTTR_NOEXCEPT
{
    return type(m_type_data->array_raw_type);
}

/////////////////////////////////////////////////////////////////////////////////////////

RTTR_INLINE string_view type::get_name() const RTTR_NOEXCEPT
{
    return m_type_data->name;
}

/////////////////////////////////////////////////////////////////////////////////////////

RTTR_INLINE string_view type::get_full_name() const RTTR_NOEXCEPT
{
    return m_type_data->type_name;
}

/////////////////////////////////////////////////////////////////////////////////////////

RTTR_INLINE std::size_t type::get_sizeof() const RTTR_NOEXCEPT
{
    return m_type_data->get_sizeof;
}

/////////////////////////////////////////////////////////////////////////////////////////

RTTR_INLINE std::size_t type::get_pointer_dimension() const RTTR_NOEXCEPT
{
    return m_type_data->get_pointer_dimension;
}

/////////////////////////////////////////////////////////////////////////////////////////

RTTR_INLINE bool type::is_class() const RTTR_NOEXCEPT
{
    return m_type_data->type_trait_value(detail::type_trait_infos::is_class);
}

/////////////////////////////////////////////////////////////////////////////////////////

RTTR_INLINE bool type::is_template_instantiation() const RTTR_NOEXCEPT
{
    return m_type_data->type_trait_value(detail::type_trait_infos::is_template_instantiation);
}

/////////////////////////////////////////////////////////////////////////////////////////

RTTR_INLINE bool type::is_enumeration() const RTTR_NOEXCEPT
{
    return m_type_data->type_trait_value(detail::type_trait_infos::is_enum);
}

/////////////////////////////////////////////////////////////////////////////////////////

RTTR_INLINE bool type::is_array() const RTTR_NOEXCEPT
{
    return m_type_data->type_trait_value(detail::type_trait_infos::is_array);
}

/////////////////////////////////////////////////////////////////////////////////////////

RTTR_INLINE bool type::is_associative_container() const RTTR_NOEXCEPT
{
    return m_type_data->type_trait_value(detail::type_trait_infos::is_associative_container);
}

/////////////////////////////////////////////////////////////////////////////////////////

RTTR_INLINE bool type::is_sequential_container() const RTTR_NOEXCEPT
{
    return m_type_data->type_trait_value(detail::type_trait_infos::is_sequential_container);
}

/////////////////////////////////////////////////////////////////////////////////////////

RTTR_INLINE bool type::is_pointer() const RTTR_NOEXCEPT
{
    return m_type_data->type_trait_value(detail::type_trait_infos::is_pointer);
}

/////////////////////////////////////////////////////////////////////////////////////////

RTTR_INLINE bool type::is_arithmetic() const RTTR_NOEXCEPT
{
    return m_type_data->type_trait_value(detail::type_trait_infos::is_arithmetic);
}

/////////////////////////////////////////////////////////////////////////////////////////

RTTR_INLINE bool type::is_function_pointer() const RTTR_NOEXCEPT
{
    return m_type_data->type_trait_value(detail::type_trait_infos::is_function_pointer);
}

/////////////////////////////////////////////////////////////////////////////////////////

RTTR_INLINE bool type::is_member_object_pointer() const RTTR_NOEXCEPT
{
    return m_type_data->type_trait_value(detail::type_trait_infos::is_member_object_pointer);
}

/////////////////////////////////////////////////////////////////////////////////////////

RTTR_INLINE bool type::is_member_function_pointer() const RTTR_NOEXCEPT
{
    return m_type_data->type_trait_value(detail::type_trait_infos::is_member_function_pointer);
}

/////////////////////////////////////////////////////////////////////////////////////////

RTTR_INLINE bool type::is_wrapper() const RTTR_NOEXCEPT
{
    return m_type_data->wrapped_type->is_valid;
}


/////////////////////////////////////////////////////////////////////////////////////////

RTTR_INLINE variant type::create_variant(const argument& data) const
{
    return m_type_data->create_variant(data);
}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

namespace detail
{

RTTR_INLINE static type get_invalid_type() RTTR_NOEXCEPT { return create_type(nullptr); }

/////////////////////////////////////////////////////////////////////////////////////////

RTTR_INLINE type create_type(type_data* data) RTTR_NOEXCEPT
{
    return data ? type(data) : type();
}

/////////////////////////////////////////////////////////////////////////////////
template<typename T>
using is_complete_type = std::integral_constant<bool, !std::is_function<T>::value && !std::is_same<T, void>::value>;

template<typename T>
RTTR_LOCAL RTTR_INLINE enable_if_t<is_complete_type<T>::value, type>
create_or_get_type() RTTR_NOEXCEPT
{
    // when you get an error here, then the type was not completely defined
    // (a forward declaration is not enough because base_classes will not be found)
    using type_must_be_complete = char[ sizeof(T) ? 1: -1 ];
    (void) sizeof(type_must_be_complete);
    static const type val = create_type(get_registration_manager().add_item(make_type_data<T>()));
    return val;
}

/////////////////////////////////////////////////////////////////////////////////

template<typename T>
RTTR_LOCAL RTTR_INLINE enable_if_t<!is_complete_type<T>::value, type>
create_or_get_type() RTTR_NOEXCEPT
{
    static const type val = create_type(get_registration_manager().add_item(make_type_data<T>()));
    return val;
}

/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////

template<typename T>
RTTR_LOCAL RTTR_INLINE type get_type_from_instance(const T*) RTTR_NOEXCEPT
{
    return detail::create_or_get_type<T>();
}

/////////////////////////////////////////////////////////////////////////////////

template<typename T, bool>
struct type_from_instance;

//! Specialization for retrieving the type from the instance directly
template<typename T>
struct type_from_instance<T, false> // the typeInfo function is not available
{
    static RTTR_INLINE type get(T&&) RTTR_NOEXCEPT
    {
        using non_ref_type = typename std::remove_cv<typename std::remove_reference<T>::type>::type;
        return create_or_get_type<non_ref_type>();
    }
};

/////////////////////////////////////////////////////////////////////////////////////////

//! Specialization for retrieving the type from the instance directly
template<typename T>
struct type_from_instance<T, true>
{
    static RTTR_INLINE type get(T&& object) RTTR_NOEXCEPT
    {
        return object.get_type();
    }
};

/////////////////////////////////////////////////////////////////////////////////////////

template<typename TargetType, typename SourceType, typename F>
struct type_converter;

} // end namespace detail

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
RTTR_INLINE type type::get() RTTR_NOEXCEPT
{
    using non_ref_type = typename std::remove_cv<typename std::remove_reference<T>::type>::type;
    return detail::create_or_get_type<non_ref_type>();
}

/////////////////////////////////////////////////////////////////////////////////////////

template<>
RTTR_INLINE type type::get<detail::invalid_type>() RTTR_NOEXCEPT
{
    return detail::get_invalid_type();
}

/////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
RTTR_INLINE type type::get(T&& object) RTTR_NOEXCEPT
{
    using remove_ref = typename std::remove_reference<T>::type;
    return detail::type_from_instance<T, detail::has_get_type_func<T>::value && !std::is_pointer<remove_ref>::value>::get(std::forward<T>(object));
}

/////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
RTTR_INLINE bool type::is_derived_from() const RTTR_NOEXCEPT
{
    return is_derived_from(type::get<T>());
}

/////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
RTTR_INLINE bool type::is_base_of() const RTTR_NOEXCEPT
{
    return is_base_of(type::get<T>());
}

/////////////////////////////////////////////////////////////////////////////////////////

template<typename F>
RTTR_INLINE void type::register_converter_func(F func)
{
    using namespace detail;

    using target_type_orig = typename function_traits<F>::return_type;
    using target_type = remove_cv_t<remove_reference_t<target_type_orig>>;

    const std::size_t arg_count = function_traits<F>::arg_count;

    static_assert(arg_count == 2, "Invalid argument count! The converter function signature must be: <target_type(source_type, bool&)>");
    static_assert(!std::is_same<void, target_type>::value, "Return type cannot be void!");
    static_assert(std::is_same<bool&, typename param_types<F, 1>::type>::value, "Second argument type must be a bool reference(bool&).");

    using source_type_orig = param_types_t<F, 0>;
    using source_type = remove_cv_t<remove_reference_t<source_type_orig>>;

    get_registration_manager().add_item(::rttr::detail::make_unique<type_converter<target_type, source_type, F>>(func));
}

/////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
RTTR_INLINE void type::register_wrapper_converter_for_base_classes()
{
    detail::reg_wrapper_converter_for_base_classes<T>();
}

/////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
void type::register_comparators()
{
    register_equal_comparator<T>();
    register_less_than_comparator<T>();
}

/////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
void type::register_equal_comparator()
{
    static_assert(detail::has_equal_operator<T>::value, "No equal operator for given type found.");

    detail::get_registration_manager().add_equal_cmp(::rttr::detail::make_unique<detail::type_equal_comparator<T>>());
}

/////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
void type::register_less_than_comparator()
{
    static_assert(detail::has_less_than_operator<T>::value, "No less-than operator for given type found.");

    detail::get_registration_manager().add_less_than_cmp(::rttr::detail::make_unique<detail::type_less_than_comparator<T>>());
}

/////////////////////////////////////////////////////////////////////////////////////////

} // end namespace rttr


namespace std
{
    template <>
    struct hash<rttr::type>
    {
    public:
        size_t operator()(const rttr::type& info) const
        {
            return hash<rttr::type::type_id>()(info.get_id());
        }
    };
} // end namespace std

#define RTTR_CAT_IMPL(a, b) a##b
#define RTTR_CAT(a, b) RTTR_CAT_IMPL(a, b)

#define RTTR_REGISTRATION_STANDARD_TYPE_VARIANTS(T) rttr::type::get<T>();       \
                                                    rttr::type::get<T*>();      \
                                                    rttr::type::get<const T*>();
#endif // RTTR_TYPE_IMPL_H_
