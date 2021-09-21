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

#ifndef RTTR_TYPE_REGISTER_P_H_
#define RTTR_TYPE_REGISTER_P_H_

#include "rttr/detail/base/core_prerequisites.h"
#include "rttr/detail/misc/flat_multimap.h"
#include "rttr/detail/misc/flat_map.h"
#include "rttr/enumeration.h"
#include "rttr/variant.h"
#include "rttr/detail/metadata/metadata.h"

#include "rttr/string_view.h"

#include <memory>
#include <string>
#include <vector>
#include <mutex>

namespace rttr
{
class type;
class property;
class method;

namespace detail
{

class constructor_wrapper_base;
class destructor_wrapper_base;
class property_wrapper_base;
class method_wrapper_base;
class enumeration_wrapper_base;

struct type_data;

/*!
 * This class contains all logic to register properties, methods etc.. for a specific type.
 * It is not part of the rttr API
 */
class RTTR_LOCAL type_register_private
{
public:

    /////////////////////////////////////////////////////////////////////////////////////
    void register_reg_manager(registration_manager* manager);
    void unregister_reg_manager(registration_manager* manager);

    /////////////////////////////////////////////////////////////////////////////////////

    type_data* register_type(type_data* info) RTTR_NOEXCEPT;
    void unregister_type(type_data* info) RTTR_NOEXCEPT;

    bool register_constructor(const constructor_wrapper_base* ctor);
    bool register_destructor(const destructor_wrapper_base* dtor);

    bool register_property(const property_wrapper_base* prop);
    bool register_global_property(const property_wrapper_base* prop);
    bool unregister_global_property(const property_wrapper_base* prop);

    bool register_method(const method_wrapper_base* meth);
    bool register_global_method(const method_wrapper_base* meth);
    bool unregister_global_method(const method_wrapper_base* meth);

    void register_custom_name(type& t, string_view custom_name);

    /////////////////////////////////////////////////////////////////////////////////////
    flat_multimap<string_view, ::rttr::property>& get_global_property_storage();
    flat_multimap<string_view, ::rttr::method>& get_global_method_storage();
    /////////////////////////////////////////////////////////////////////////////////////

    /////////////////////////////////////////////////////////////////////////////////////
    std::vector<::rttr::method>& get_global_methods();
    std::vector<::rttr::property>& get_global_properties();

    /////////////////////////////////////////////////////////////////////////////////////

    std::vector<type_data*>& get_type_data_storage();
    std::vector<type>& get_type_storage();
    flat_map<string_view, type>& get_orig_name_to_id();
    flat_map<std::string, type, hash>& get_custom_name_to_id();

    /////////////////////////////////////////////////////////////////////////////////////

    bool register_converter(const type_converter_base* converter);
    bool unregister_converter(const type_converter_base* converter);

    bool register_equal_comparator(const type_comparator_base* comparator);
    bool unregister_equal_comparator(const type_comparator_base* converter);

    bool register_less_than_comparator(const type_comparator_base* comparator);
    bool unregister_less_than_comparator(const type_comparator_base* converter);

    /////////////////////////////////////////////////////////////////////////////////////

    const type_converter_base* get_converter(const type& source_type, const type& target_type);
    const type_comparator_base* get_equal_comparator(const type& t);
    const type_comparator_base* get_less_than_comparator(const type& t);

    /////////////////////////////////////////////////////////////////////////////////////
    static variant get_metadata(const type& t, const variant& key);
    static variant get_metadata(const variant& key, const std::vector<metadata>& data);
    /////////////////////////////////////////////////////////////////////////////////////

    static type_register_private& get_instance();

private:
    type_register_private();
    ~type_register_private();

    template<typename T, typename Data_Type = conditional_t<std::is_pointer<T>::value, T, std::unique_ptr<T>>>
    struct data_container
    {
        data_container(type::type_id id) : m_id(id) {}
        data_container(type::type_id id, Data_Type data) : m_id(id), m_data(std::move(data)) {}
        data_container(data_container<T, Data_Type>&& other) : m_id(other.m_id), m_data(std::move(other.m_data)) {}
        data_container<T, Data_Type>& operator = (data_container<T, Data_Type>&& other)
        {
            m_id = other.m_id;
            m_data = std::move(other.m_data);
            return *this;
        }

        struct order_by_id
        {
            RTTR_INLINE bool operator () ( const data_container<T>& _left, const data_container<T>& _right )  const
            {
                return _left.m_id < _right.m_id;
            }
            RTTR_INLINE bool operator () ( const type::type_id& _left, const data_container<T>& _right ) const
            {
                return _left < _right.m_id;
            }
            RTTR_INLINE bool operator () ( const data_container<T>& _left, const type::type_id& _right ) const
            {
                return _left.m_id < _right;
            }

            RTTR_INLINE bool operator () ( const data_container<T>& _left, const Data_Type& _right ) const
            {
                return _left.m_data < _right;
            }
        };

        type::type_id   m_id;
        Data_Type       m_data;
    };

    static bool register_comparator_impl(const type& t, const type_comparator_base* comparator,
                                         std::vector<data_container<const type_comparator_base*>>& comparator_list);
    static const type_comparator_base* get_type_comparator_impl(const type& t,
                                                                const std::vector<data_container<const type_comparator_base*>>& comparator_list);

    static ::rttr::property get_type_property(const type& t, string_view name);
    static ::rttr::method get_type_method(const type& t, string_view name,
                                          const std::vector<type>& type_list);

    template<typename T>
    static void update_class_list(const type& t, T item_ptr);

    static std::string derive_name(const type& t);
    //! Returns true, when the name was already registered
    type_data* register_name_if_neccessary(type_data* info);
    static void register_base_class_info(type_data* info);
    /*!
     * \brief This will create the derived name of a template instance, with all the custom names of a template parameter.
     * e.g.: `std::reference_wrapper<class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> > >` =>
     *       `std::reference_wrapper<class std::string>`
     *
     */
    static std::string derive_template_instance_name(type_data* info);

    /*!
     * Updates the custom name for the given type \p t with \p new_name
     */
    void update_custom_name(std::string new_name, const type& t);

    /*! A helper class to register the registration managers.
     * This class is needed in order to avoid that the registration_manager instance's
     * are trying to deregister its content, although the RTTR library is already unloaded.
     * So every registration manager class holds a flag whether it should deregister itself or not.
     */
    std::set<registration_manager*>                             m_registration_manager_list;

    flat_map<std::string, type, hash>                           m_custom_name_to_id;
    flat_map<string_view, type>                                 m_orig_name_to_id;
    std::vector<type>                                           m_type_list;
    std::vector<type_data*>                                     m_type_data_storage;

    flat_multimap<string_view, ::rttr::property>                m_global_property_stroage;
    flat_multimap<string_view, ::rttr::method>                  m_global_method_stroage;
    std::vector<::rttr::property>                               m_global_properties;
    std::vector<::rttr::method>                                 m_global_methods;

    std::vector<data_container<const type_converter_base*>>     m_type_converter_list;
    std::vector<data_container<const type_comparator_base*>>    m_type_equal_cmp_list;
    std::vector<data_container<const type_comparator_base*>>    m_type_less_than_cmp_list;

    std::mutex                                                  m_mutex;
};

} // end namespace detail
} // end namespace rttr

#endif // RTTR_TYPE_REGISTER_P_H_
