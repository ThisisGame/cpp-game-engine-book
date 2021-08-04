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

#ifndef RTTR_PARAMETER_INFO_WRAPPER_H_
#define RTTR_PARAMETER_INFO_WRAPPER_H_

#include "rttr/detail/base/core_prerequisites.h"

#include "rttr/detail/parameter_info/parameter_info_wrapper_base.h"
#include "rttr/parameter_info.h"

namespace rttr
{
namespace detail
{
struct has_param_name {};
struct no_param_name {};

template<typename Param_Type, std::size_t Param_Index, typename Has_Name, typename Default_Type>
class parameter_info_wrapper;

/////////////////////////////////////////////////////////////////////////////////////////

template<typename Param_Type, std::size_t Param_Index, typename Default_Type>
class parameter_info_wrapper<Param_Type, Param_Index, has_param_name, Default_Type>: public parameter_info_wrapper_base
{
    public:
        parameter_info_wrapper(string_view name) RTTR_NOEXCEPT
        :   m_default_value(nullptr), m_name(name)
        {
        }

        string_view get_name() const        RTTR_NOEXCEPT   { return m_name; }
        type get_type() const               RTTR_NOEXCEPT   { return type::get<Param_Type>(); }
        bool has_default_value() const      RTTR_NOEXCEPT   { return true; }
        variant get_default_value() const                   { return variant(*m_default_value); }
        void set_default_value(const Default_Type* default_value) RTTR_NOEXCEPT { m_default_value = default_value; }
        uint32_t get_index() const          RTTR_NOEXCEPT   { return static_cast<uint32_t>(Param_Index); }
    private:
        const Default_Type* m_default_value;
        string_view         m_name;
};

/////////////////////////////////////////////////////////////////////////////////////////

template<typename Param_Type, std::size_t Param_Index, typename Default_Type>
class parameter_info_wrapper<Param_Type, Param_Index, no_param_name, Default_Type>: public parameter_info_wrapper_base
{
    public:
        parameter_info_wrapper() RTTR_NOEXCEPT
        :   m_default_value(nullptr)
        {
        }

        string_view get_name() const        RTTR_NOEXCEPT   { return string_view(); }
        type get_type() const               RTTR_NOEXCEPT   { return type::get<Param_Type>(); }
        bool has_default_value() const      RTTR_NOEXCEPT   { return true; }
        variant get_default_value() const                   { return variant(*m_default_value); }
        void set_default_value(const Default_Type* default_value) RTTR_NOEXCEPT { m_default_value = default_value; }
        uint32_t get_index() const         RTTR_NOEXCEPT    { return static_cast<uint32_t>(Param_Index); }
    private:
        const Default_Type* m_default_value;
};

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
// Specialization which has NO default value
template<typename Param_Type, std::size_t Param_Index>
class parameter_info_wrapper<Param_Type, Param_Index, has_param_name, void> : public parameter_info_wrapper_base
{
    public:
        parameter_info_wrapper(string_view name) RTTR_NOEXCEPT
        :   m_name(name)
        {
        }

        string_view get_name() const        RTTR_NOEXCEPT   { return m_name; }
        type get_type() const               RTTR_NOEXCEPT   { return type::get<Param_Type>(); }
        bool has_default_value() const      RTTR_NOEXCEPT   { return false; }
        variant get_default_value() const                   { return variant(); }
        uint32_t get_index() const          RTTR_NOEXCEPT   { return static_cast<uint32_t>(Param_Index); }
    private:
        string_view m_name;
};

/////////////////////////////////////////////////////////////////////////////////////////
// Specialization which has NO default value
template<typename Param_Type, std::size_t Param_Index>
class parameter_info_wrapper<Param_Type, Param_Index, no_param_name, void> : public parameter_info_wrapper_base
{
    public:
        parameter_info_wrapper() RTTR_NOEXCEPT {}

        string_view get_name() const        RTTR_NOEXCEPT   { return string_view(); }
        type get_type() const               RTTR_NOEXCEPT   { return type::get<Param_Type>(); }
        bool has_default_value() const      RTTR_NOEXCEPT   { return false; }
        variant get_default_value() const                   { return variant(); }
        uint32_t get_index() const          RTTR_NOEXCEPT   { return static_cast<uint32_t>(Param_Index); }
};

/////////////////////////////////////////////////////////////////////////////////////////

} // end namespace detail
} // end namespace rttr

#endif // RTTR_PARAMETER_INFO_WRAPPER_H_
