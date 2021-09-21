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

#ifndef RTTR_REGISTRATION_MANAGER_H_
#define RTTR_REGISTRATION_MANAGER_H_

#include "rttr/detail/base/core_prerequisites.h"

#include "rttr/detail/type/type_register.h"
#include "rttr/detail/property/property_wrapper_base.h"
#include "rttr/detail/method/method_wrapper_base.h"
#include "rttr/detail/constructor/constructor_wrapper_base.h"
#include "rttr/detail/destructor/destructor_wrapper_base.h"
#include "rttr/detail/enumeration/enumeration_wrapper_base.h"

#include <vector>
#include <memory>

namespace rttr
{
namespace detail
{

/*!
 * This class saves the registration of all possible items per module (*.DLL, *.so, ...)
 * and will undo the registration when the instance is destroyed.
 */
class RTTR_LOCAL registration_manager
{
    public:
        registration_manager()
        {
            type_register::register_reg_manager(this);
        }
        ~registration_manager()
        {
            unregister();
        }

        type_data* add_item(std::unique_ptr<type_data> obj)
        {
            auto reg_type = type_register::register_type(obj.get());
            const auto was_type_stored = (reg_type == obj.get());
            if (was_type_stored)
                m_type_data_list.push_back(std::move(obj)); // so we have to unregister it later

            return reg_type;
        }

        void add_item(std::unique_ptr<constructor_wrapper_base> ctor)
        {
            if (type_register::register_constructor(ctor.get()))
                m_constructors.push_back(std::move(ctor));
        }

        void add_item(std::unique_ptr<destructor_wrapper_base> dtor)
        {
            if (type_register::register_destructor(dtor.get()))
                m_destructors.push_back(std::move(dtor));
        }

        void add_item(std::unique_ptr<property_wrapper_base> prop)
        {
            if (type_register::register_property(prop.get()))
                m_properties.push_back(std::move(prop));
        }

        void add_item(std::unique_ptr<method_wrapper_base> meth)
        {
            if (type_register::register_method(meth.get()))
                m_methods.push_back(std::move(meth));
        }

        void add_item(std::unique_ptr<enumeration_wrapper_base> enum_)
        {
            if (type_register::register_enumeration(enum_.get()))
                m_enumerations.push_back(std::move(enum_));
        }

        void add_global_item(std::unique_ptr<property_wrapper_base> prop)
        {
            if (type_register::register_global_property(prop.get()))
                m_global_properties.push_back(std::move(prop));
        }

        void add_global_item(std::unique_ptr<method_wrapper_base> meth)
        {
            if (type_register::register_global_method(meth.get()))
                m_global_methods.push_back(std::move(meth));
        }

        void add_global_item(std::unique_ptr<enumeration_wrapper_base> enum_)
        {
            if (type_register::register_enumeration(enum_.get()))
                m_enumerations.push_back(std::move(enum_));
        }

        void add_item(std::unique_ptr<type_converter_base> conv)
        {
            if (type_register::register_converter(conv.get()))
                m_type_converters.push_back(std::move(conv));
        }

        void add_equal_cmp(std::unique_ptr<type_comparator_base> cmp)
        {
            if (type_register::register_equal_comparator(cmp.get()))
                m_type_equal_cmps.push_back(std::move(cmp));
        }

        void add_less_than_cmp(std::unique_ptr<type_comparator_base> cmp)
        {
            if (type_register::register_less_than_comparator(cmp.get()))
                m_type_less_than_cmps.push_back(std::move(cmp));
        }

        void set_disable_unregister()
        {
            m_should_unregister = false;
        }

        void unregister()
        {
            if (!m_should_unregister)
                return;

            for (auto& prop : m_global_properties)
                type_register::unregister_global_property(prop.get());
            for (auto& meth : m_global_methods)
                type_register::unregister_global_method(meth.get());
            for (auto& enum_ : m_enumerations)
                type_register::unregister_enumeration(enum_.get());

            for (auto& item : m_type_converters)
                type_register::unregister_converter(item.get());
            for (auto& item : m_type_equal_cmps)
                type_register::unregister_equal_comparator(item.get());
            for (auto& item : m_type_less_than_cmps)
                type_register::unregister_less_than_comparator(item.get());

            for (auto& type : m_type_data_list)
                type_register::unregister_type(type.get());

            type_register::unregister_reg_manager(this);

            m_type_data_list.clear();
            m_constructors.clear();
            m_destructors.clear();
            m_properties.clear();
            m_global_properties.clear();
            m_methods.clear();
            m_global_methods.clear();
            m_enumerations.clear();
            m_type_converters.clear();
            m_type_equal_cmps.clear();
            m_type_less_than_cmps.clear();

            m_should_unregister = false;
        }


        // no copy, no assign
        registration_manager(const registration_manager&) = delete;
        registration_manager& operator=(const registration_manager&) = delete;

    private:
        bool                                                    m_should_unregister = true;
        std::vector<std::unique_ptr<type_data>>                 m_type_data_list;

        std::vector<std::unique_ptr<constructor_wrapper_base>>  m_constructors;
        std::vector<std::unique_ptr<destructor_wrapper_base>>   m_destructors;

        std::vector<std::unique_ptr<property_wrapper_base>>     m_properties;
        std::vector<std::unique_ptr<property_wrapper_base>>     m_global_properties;

        std::vector<std::unique_ptr<method_wrapper_base>>       m_methods;
        std::vector<std::unique_ptr<method_wrapper_base>>       m_global_methods;

        std::vector<std::unique_ptr<enumeration_wrapper_base>>  m_enumerations;

        std::vector<std::unique_ptr<type_converter_base>>       m_type_converters;
        std::vector<std::unique_ptr<type_comparator_base>>      m_type_equal_cmps;
        std::vector<std::unique_ptr<type_comparator_base>>      m_type_less_than_cmps;
};

/////////////////////////////////////////////////////////////////////////////////////////


RTTR_LOCAL RTTR_INLINE registration_manager& get_registration_manager() RTTR_NOEXCEPT
{
    static registration_manager obj;
    return obj;
}

/////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
using is_global_item = std::integral_constant<bool, std::is_same<T, invalid_type>::value>;

template<typename T, typename Item>
RTTR_LOCAL RTTR_FORCE_INLINE enable_if_t<is_global_item<T>::value, void>
store_item(Item item)
{
    auto& obj = get_registration_manager();
    obj.add_global_item(std::move(item));
}

template<typename T, typename Item>
RTTR_LOCAL RTTR_FORCE_INLINE enable_if_t<!is_global_item<T>::value, void>
store_item(Item item)
{
    auto& obj = get_registration_manager();
    obj.add_item(std::move(item));
}

/////////////////////////////////////////////////////////////////////////////////////////

} // end namespace detail
} // end namespace rttr

#endif // RTTR_REGISTRATION_MANAGER_H_
