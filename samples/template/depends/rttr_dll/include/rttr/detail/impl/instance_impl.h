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

#ifndef RTTR_INSTANCE_IMPL_H_
#define RTTR_INSTANCE_IMPL_H_

#include "rttr/variant.h"
#include "rttr/type.h"
#include "rttr/wrapper_mapper.h"
#include "rttr/detail/misc/misc_type_traits.h"

namespace rttr
{

/////////////////////////////////////////////////////////////////////////////////////////

RTTR_INLINE instance::instance() RTTR_NOEXCEPT
:   m_data_container(detail::data_address_container{detail::get_invalid_type(), detail::get_invalid_type(), nullptr, nullptr})
{

}

/////////////////////////////////////////////////////////////////////////////////////////

RTTR_INLINE instance::instance(const variant& var) RTTR_NOEXCEPT
:   m_data_container(var.get_data_address_container())
{
}

/////////////////////////////////////////////////////////////////////////////////////////

RTTR_INLINE instance::instance(const instance& other) RTTR_NOEXCEPT
:   m_data_container(other.m_data_container)
{
}

/////////////////////////////////////////////////////////////////////////////////////////

template<typename T, typename Tp>
RTTR_INLINE instance::instance(T& data) RTTR_NOEXCEPT
:   m_data_container(detail::data_address_container{
                     rttr::type::get<T>(), rttr::type::get<detail::wrapper_mapper_t<T>>(),
                     detail::as_void_ptr(detail::raw_addressof(data)), detail::as_void_ptr(detail::wrapped_raw_addressof(data))})
{
    static_assert(!std::is_same<argument, T>::value, "Don't use the instance class for forwarding an argument!");
}

/////////////////////////////////////////////////////////////////////////////////////////

template<typename Target_Type>
RTTR_INLINE Target_Type* instance::try_convert() const RTTR_NOEXCEPT
{
    Target_Type* target = static_cast<Target_Type*>(type::apply_offset(const_cast<instance*>(this)->m_data_container.m_data_address, m_data_container.m_type, type::get<Target_Type>()));

    if (!target)
        return (static_cast<Target_Type*>(type::apply_offset(const_cast<instance*>(this)->m_data_container.m_data_address_wrapped_type, m_data_container.m_wrapped_type, type::get<Target_Type>())));

    return target;
}

/////////////////////////////////////////////////////////////////////////////////////////

RTTR_INLINE bool instance::is_valid() const RTTR_NOEXCEPT { return (m_data_container.m_data_address != nullptr); }

/////////////////////////////////////////////////////////////////////////////////////////

RTTR_INLINE instance::operator bool() const RTTR_NOEXCEPT { return (m_data_container.m_data_address != nullptr); }

/////////////////////////////////////////////////////////////////////////////////////////

RTTR_INLINE type instance::get_type() const RTTR_NOEXCEPT { return m_data_container.m_type; }

/////////////////////////////////////////////////////////////////////////////////////////

 RTTR_INLINE instance instance::get_wrapped_instance() const RTTR_NOEXCEPT
 {
     instance obj;
     if (m_data_container.m_data_address != m_data_container.m_data_address_wrapped_type)
     {
        obj.m_data_container.m_data_address = m_data_container.m_data_address_wrapped_type;
        obj.m_data_container.m_type = m_data_container.m_wrapped_type;
     }
     return obj;
 }

 /////////////////////////////////////////////////////////////////////////////////////////

RTTR_INLINE type instance::get_derived_type() const RTTR_NOEXCEPT
{
    return type::get_derived_type(const_cast<instance*>(this)->m_data_container.m_data_address, m_data_container.m_type);
}

/////////////////////////////////////////////////////////////////////////////////////////

} // end namespace rttr

#endif // RTTR_INSTANCE_IMPL_H_
