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

#ifndef RTTR_BIND_IMPL_H_
#define RTTR_BIND_IMPL_H_

#include "rttr/detail/base/core_prerequisites.h"
#include "rttr/detail/misc/argument_extractor.h"
#include "rttr/detail/constructor/constructor_wrapper.h"
#include "rttr/detail/destructor/destructor_wrapper.h"
#include "rttr/detail/enumeration/enumeration_wrapper.h"
#include "rttr/detail/enumeration/enum_data.h"
#include "rttr/detail/metadata/metadata.h"
#include "rttr/detail/method/method_wrapper.h"
#include "rttr/detail/parameter_info/parameter_infos.h"
#include "rttr/detail/property/property_wrapper.h"
#include "rttr/detail/type/accessor_type.h"
#include "rttr/detail/misc/misc_type_traits.h"
#include "rttr/detail/misc/utility.h"
#include "rttr/detail/type/type_register.h"
#include "rttr/detail/default_arguments/default_arguments.h"
#include "rttr/detail/registration/register_base_class_from_accessor.h"
#include "rttr/policy.h"
#include "rttr/type.h"
#include "rttr/detail/registration/registration_manager.h"

#include <functional>
#include <string>
#include <vector>

namespace rttr
{
namespace detail
{

/////////////////////////////////////////////////////////////////////////////////////////

template<typename... Args>
static RTTR_INLINE auto get_metadata(Args&&... arg) -> decltype(forward_to_array<metadata>(std::forward<Args>(arg)...))
{
    return forward_to_array<metadata>(std::forward<Args>(arg)...);
}

/////////////////////////////////////////////////////////////////////////////////////////

template<typename Enum_Type, typename... Args>
static RTTR_INLINE auto get_enum_values(Args&&... arg) -> decltype(forward_to_array<enum_data<Enum_Type>>(std::forward<Args>(arg)...))
{
    return forward_to_array<enum_data<Enum_Type>>(std::forward<Args>(arg)...);
}

/////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
using map_access_level_to_enum = conditional_t< std::is_same<T, detail::public_access>::value,
                                                std::integral_constant<access_levels, access_levels::public_access>,
                                                conditional_t< std::is_same<T, detail::protected_access>::value,
                                                               std::integral_constant<access_levels, access_levels::protected_access>,
                                                               std::integral_constant<access_levels, access_levels::private_access>
                                                             >
                                              >;


/////////////////////////////////////////////////////////////////////////////////////////

} // end namespace detail

/////////////////////////////////////////////////////////////////////////////////////////

template<typename Class_Type, typename acc_level, typename...Ctor_Args>
class registration::bind<detail::ctor, Class_Type, acc_level, Ctor_Args...> : public registration::class_<Class_Type>
{
    private:
        // this 'as_std_shared_ptr' policy has been selected, because:
        // * it has automatic memory management; should not leak anything
        // * we can copy the object inside the variant, without invoking the copy constructor (could be disabled by user)
        // REMARK: For the default constructor registration will always be code instantiated, when copying is disabled, we get a compile error
        using default_create_policy = detail::as_std_shared_ptr;

        // this additional wrapper function is needed, because of not triggered static_assert in func: 'operator()(Args&&... args)'
        // when we create the object directly inside the operator function, seems to be a bug in MSVC
        template<typename Policy, std::size_t Metadata_Count, typename...TArgs, typename...Param_Args>
        static RTTR_INLINE std::unique_ptr<detail::constructor_wrapper_base>
        create_constructor_wrapper(std::array<detail::metadata, Metadata_Count> metadata_list,
                                   detail::default_args<TArgs...> def_args,
                                   detail::parameter_infos<Param_Args...> param_infos)
        {
            using namespace detail;
            return detail::make_unique<constructor_wrapper<Class_Type, class_ctor,
                                                           detail::map_access_level_to_enum<acc_level>::value,
                                                           Policy,
                                                           Metadata_Count,
                                                           default_args<TArgs...>,
                                                           parameter_infos<Param_Args...>, Ctor_Args...>>(std::move(metadata_list),
                                                                                                          std::move(def_args),
                                                                                                          std::move(param_infos));
        }

        template<typename Policy, std::size_t Metadata_Count, typename...Param_Args>
        static RTTR_INLINE std::unique_ptr<detail::constructor_wrapper_base>
        create_constructor_wrapper(std::array<detail::metadata, Metadata_Count> metadata_list,
                                   detail::default_args<> def_args,
                                   detail::parameter_infos<Param_Args...> param_infos)
        {
            using namespace detail;
            return detail::make_unique<constructor_wrapper<Class_Type, class_ctor,
                                                           detail::map_access_level_to_enum<acc_level>::value,
                                                           Policy,
                                                           Metadata_Count,
                                                           default_args<>,
                                                           parameter_infos<Param_Args...>, Ctor_Args...>>(std::move(metadata_list),
                                                                                                          std::move(param_infos));
        }

    public:
        bind(const std::shared_ptr<detail::registration_executer>& reg_exec)
        :   registration::class_<Class_Type>(reg_exec), m_reg_exec(reg_exec)
        {
            m_reg_exec->add_registration_func(this);
        }
        ~bind()
        {
            using namespace detail;
            using param_info_t = decltype(create_param_infos<type_list<Ctor_Args...>, constructor_type>());
            if (!m_ctor.get())
                m_ctor = detail::make_unique<detail::constructor_wrapper<Class_Type, class_ctor,
                                                                         detail::map_access_level_to_enum<acc_level>::value, default_create_policy,
                                                                         0,
                                                                         detail::default_args<>,
                                                                         param_info_t,
                                                                         Ctor_Args...>>(std::array<detail::metadata, 0>(),
                                                                                        param_info_t());

            auto wrapper = detail::make_rref(std::move(m_ctor));
            auto reg_func = [wrapper]()
            {
                store_item<Class_Type>(std::move(wrapper.m_value));
                store_item<Class_Type>(detail::make_unique<destructor_wrapper<Class_Type>>());
            };
            m_reg_exec->add_registration_func(this, std::move(reg_func));
        }

        template<typename... Args>
        registration::class_<Class_Type> operator()(Args&&... args)
        {
            using namespace detail;

            using policy_types_found = typename find_types<constructor_policy_list, as_type_list_t<raw_type_t<Args>...>>::type;
            static_assert(!has_double_types<policy_types_found>::value, "There are multiple policies of the same type forwarded, that is not allowed!");

            using has_valid_default_types = has_default_types<type_list<Ctor_Args...>, type_list<Args...>, constructor_type>;
            static_assert( (has_default_args<Args...>::value && has_valid_default_types::value) || !has_default_args<Args...>::value,
                           "The provided default arguments, cannot be used with the given constructor. Please check the provided argument types."
                           "The given arguments must match the signature from the starting position to the right most argument.");

            static_assert((count_default_args<Args...>::value <= 1),
                          "Only one set of 'default_arguments' can be provided during registration of a constructor!");

            static_assert((count_param_names<Args...>::value <= 1),
                          "Only one set of 'parameter_names' can be provided during registration of a constructor!");

            static_assert(((!has_param_names<Args...>::value) ||
                           (param_names_count<Args...>::value == sizeof...(Ctor_Args))),
                          "The provided amount of names in 'parameter_names' does not match argument count of the constructor signature.");

            // when no policy was added, we need a default policy
            using policy_list = conditional_t< type_list_size<policy_types_found>::value == 0,
                                               default_create_policy,
                                               policy_types_found>;

            // at the moment we only supported one policy
            using first_prop_policy = typename std::tuple_element<0, as_std_tuple_t<policy_list>>::type;
            m_ctor = create_constructor_wrapper<first_prop_policy>(std::move(get_metadata(std::forward<Args>(args)...)),
                                                                   std::move(get_default_args<type_list<Ctor_Args...>, constructor_type>(std::forward<Args>(args)...)),
                                                                   std::move(create_param_infos<type_list<Ctor_Args...>, constructor_type>(std::forward<Args>(args)...)));
            return registration::class_<Class_Type>(m_reg_exec);
        }

    private:
        std::shared_ptr<detail::registration_executer> m_reg_exec;
        std::unique_ptr<detail::constructor_wrapper_base> m_ctor;
};

/////////////////////////////////////////////////////////////////////////////////////////

template<typename Class_Type, typename F, typename acc_level>
class registration::bind<detail::ctor_func, Class_Type, F, acc_level> : public registration::class_<Class_Type>
{
    private:
        template<std::size_t Metadata_Count, typename...TArgs, typename...Param_Args>
        static RTTR_INLINE std::unique_ptr<detail::constructor_wrapper_base>
        create_constructor_wrapper(F func,
                                   std::array<detail::metadata, Metadata_Count> metadata_list,
                                   detail::default_args<TArgs...> def_args,
                                   detail::parameter_infos<Param_Args...> param_infos)
        {
            using namespace detail;
            return detail::make_unique<constructor_wrapper<Class_Type, return_func,
                                                           detail::map_access_level_to_enum<acc_level>::value,
                                                           default_invoke,
                                                           Metadata_Count,
                                                           default_args<TArgs...>,
                                                           parameter_infos<Param_Args...>,
                                                           F>>(func,
                                                               std::move(metadata_list),
                                                               std::move(def_args),
                                                               std::move(param_infos));
        }

        template<std::size_t Metadata_Count, typename...Param_Args>
        static RTTR_INLINE std::unique_ptr<detail::constructor_wrapper_base>
        create_constructor_wrapper(F func,
                                   std::array<detail::metadata, Metadata_Count> metadata_list,
                                   detail::default_args<> def_args,
                                   detail::parameter_infos<Param_Args...> param_infos)
        {
            using namespace detail;
            return detail::make_unique<constructor_wrapper<Class_Type, return_func,
                                                           detail::map_access_level_to_enum<acc_level>::value,
                                                           default_invoke,
                                                           Metadata_Count,
                                                           default_args<>,
                                                           parameter_infos<Param_Args...>,
                                                           F>>(func,
                                                               std::move(metadata_list),
                                                               std::move(param_infos));
        }

        // this indirection is necessary to delay the instantiation of the created constructor wrapper
        // so we can make use of static_assert and check whether the given accessor is correct or not
        // REMARK: this seems to be a MSVC problem...
        template<typename Acc_Func>
        static RTTR_INLINE std::unique_ptr<detail::constructor_wrapper_base> create_default_constructor(Acc_Func func)
        {
            using namespace detail;
            using param_info_t = decltype(create_param_infos<type_list<Acc_Func>, function_type>());
            return detail::make_unique<constructor_wrapper<Class_Type, return_func,
                                       detail::map_access_level_to_enum<acc_level>::value,
                                       default_invoke,
                                       0,
                                       detail::default_args<>,
                                       param_info_t, F>>(func, std::array<detail::metadata, 0>(), param_info_t());
        }

        template<typename Acc_Func, typename... Args>
        static RTTR_INLINE std::unique_ptr<detail::constructor_wrapper_base> create_custom_constructor(Acc_Func func, Args&&...args)
        {
            using namespace detail;
            using has_valid_default_types = has_default_types<type_list<Acc_Func>, type_list<Args...>, function_type>;
            static_assert( (has_default_args<Args...>::value && has_valid_default_types::value) || !has_default_args<Args...>::value,
                           "The provided default arguments, cannot be used with the given constructor. Please check the provided argument types."
                           "The given arguments must match the signature from the starting position to the right most argument.");

            static_assert((count_default_args<Args...>::value <= 1),
                          "Only one set of 'default_arguments' can be provided during registration of a constructor!");

            static_assert((count_param_names<Args...>::value <= 1),
                          "Only one set of 'parameter_names' can be provided during registration of a constructor!");

            static_assert(((!has_param_names<Args...>::value) ||
                           (param_names_count<Args...>::value == function_traits<Acc_Func>::arg_count)),
                          "The provided amount of names in 'parameter_names' does not match argument count of the function signature.");

            auto ctor = create_constructor_wrapper(func,
                                                   std::move(get_metadata(std::forward<Args>(args)...)),
                                                   std::move(get_default_args<type_list<Acc_Func>, function_type>(std::forward<Args>(args)...)),
                                                   std::move(create_param_infos<type_list<F>, function_type>(std::forward<Args>(args)...)));
            return std::move(ctor);
        }
    public:
        bind(const std::shared_ptr<detail::registration_executer>& reg_exec, F func)
        :   registration::class_<Class_Type>(reg_exec), m_reg_exec(reg_exec), m_func(func)
        {
            m_reg_exec->add_registration_func(this);
        }

        ~bind()
        {
            using namespace detail;
            if (!m_ctor.get())
                m_ctor = create_default_constructor(m_func);

            auto wrapper = detail::make_rref(std::move(m_ctor));
            auto reg_func = [wrapper]()
            {
                store_item<Class_Type>(std::move(wrapper.m_value));
                store_item<Class_Type>(detail::make_unique<destructor_wrapper<Class_Type>>());
            };

            m_reg_exec->add_registration_func(this, reg_func);
        }

        template<typename... Args>
        registration::class_<Class_Type> operator()(Args&&... args)
        {
            m_ctor = create_custom_constructor(m_func, std::forward<Args>(args)...);
            return registration::class_<Class_Type>(m_reg_exec);
        }

    private:
        std::shared_ptr<detail::registration_executer> m_reg_exec;
        F m_func;
        std::unique_ptr<detail::constructor_wrapper_base> m_ctor;
};

/////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
using registration_derived_t = detail::conditional_t< std::is_same<T, void>::value,
                                                      registration,
                                                      registration::class_<T>
                                                     >;

/////////////////////////////////////////////////////////////////////////////////////////

template<typename Class_Type, typename A, typename acc_level>
class registration::bind<detail::prop, Class_Type, A, acc_level> : public registration_derived_t<Class_Type>
{
    private:
        using default_getter_policy = detail::return_as_copy;
        using default_setter_policy = detail::set_value;

        template<typename Acc>
        static RTTR_INLINE
        std::unique_ptr<detail::property_wrapper_base> create_default_property(string_view name, Acc acc)
        {
            using namespace detail;
            using acc_type = typename property_type<Acc>::type;
            return detail::make_unique<property_wrapper<acc_type,
                                                        A,
                                                        void,
                                                        detail::map_access_level_to_enum<acc_level>::value,
                                                        default_getter_policy, default_setter_policy,
                                                        0>>(name, type::get<Class_Type>(), acc, std::array<detail::metadata, 0>());
        }

        template<typename Acc, std::size_t Metadata_Count, typename... Args>
        static RTTR_INLINE
        std::unique_ptr<detail::property_wrapper_base> create_custom_property(string_view name,
                                                                              Acc acc,
                                                                              std::array<detail::metadata, Metadata_Count> metadata_list,
                                                                              Args&&...args)
        {
            using namespace detail;
            using policy_types_found = typename find_types<property_policy_list, as_type_list_t<raw_type_t<Args>...>>::type;
            static_assert(!has_double_types<policy_types_found>::value, "There are multiple policies of the same type forwarded, that is not allowed!");
            // when no policy was added, we need a default policy
            using policy_list = conditional_t< type_list_size<policy_types_found>::value == 0,
                                               default_getter_policy,
                                               policy_types_found>;


            // at the moment we only supported one policy
            using first_prop_policy = typename std::tuple_element<0, as_std_tuple_t<policy_list>>::type;
            using getter_policy     = typename get_getter_policy<first_prop_policy>::type;
            using setter_policy     = typename get_setter_policy<first_prop_policy>::type;
            using acc_type          = typename property_type<Acc>::type;

            auto prop = detail::make_unique<property_wrapper<acc_type,
                                                             Acc,
                                                             void,
                                                             detail::map_access_level_to_enum<acc_level>::value,
                                                             getter_policy, setter_policy,
                                                             Metadata_Count>>(name, type::get<Class_Type>(), acc, std::move(metadata_list));
            return std::move(prop);
        }

    public:
        bind(const std::shared_ptr<detail::registration_executer>& reg_exec, string_view name, A acc)
        :   registration_derived_t<Class_Type>(reg_exec), m_reg_exec(reg_exec), m_name(name), m_acc(acc)
        {
            detail::register_accessor_class_type_when_needed<Class_Type, A>();
            m_reg_exec->add_registration_func(this);
        }

        ~bind()
        {
            using namespace detail;
            if (!m_prop.get())
                m_prop = create_default_property(m_name, m_acc);

            auto wrapper = detail::make_rref(std::move(m_prop));
            auto reg_func = [wrapper]()
            {
                store_item<Class_Type>(std::move(wrapper.m_value));
            };
            m_reg_exec->add_registration_func(this, std::move(reg_func));
        }


        template<typename... Args>
        registration_derived_t<Class_Type> operator()(Args&&... args)
        {
            m_prop = create_custom_property(m_name, m_acc, std::move(get_metadata(std::forward<Args>(args)...)), std::forward<Args>(args)...);
            return registration_derived_t<Class_Type>(m_reg_exec);
        }

    private:
        std::shared_ptr<detail::registration_executer> m_reg_exec;
        string_view m_name;
        A           m_acc;
        std::unique_ptr<detail::property_wrapper_base> m_prop;

};

/////////////////////////////////////////////////////////////////////////////////////////

template<typename Class_Type, typename A1, typename A2, typename acc_level>
class registration::bind<detail::prop, Class_Type, A1, A2, acc_level> : public registration_derived_t<Class_Type>
{
    private:
        using default_getter_policy = detail::return_as_copy;
        using default_setter_policy = detail::set_value;

        template<typename Acc1, typename Acc2>
        static RTTR_INLINE
        std::unique_ptr<detail::property_wrapper_base> create_default_property(string_view name,
                                                                               Acc1 getter, Acc2 setter)
        {
            using namespace detail;
            using acc_type = typename property_type<A1>::type;
            return detail::make_unique<property_wrapper<acc_type,
                                                        Acc1, Acc2,
                                                        detail::map_access_level_to_enum<acc_level>::value,
                                                        default_getter_policy, default_setter_policy, 0>>(name, type::get<Class_Type>(),
                                                                                                          getter, setter, std::array<detail::metadata, 0>());
        }

        template<typename Acc1, typename Acc2, std::size_t Metadata_Count, typename... Args>
        static RTTR_INLINE
        std::unique_ptr<detail::property_wrapper_base> create_custom_property(string_view name,
                                                                              Acc1 getter, Acc2 setter,
                                                                              std::array<detail::metadata, Metadata_Count> metadata_list,
                                                                              Args&&...args)
        {
            using namespace detail;

            using policy_types_found = typename find_types<property_policy_list, as_type_list_t<raw_type_t<Args>...>>::type;
            static_assert(!has_double_types<policy_types_found>::value, "There are multiple policies of the same type forwarded, that is not allowed!");
            // when no policy was added, we need a default policy
            using policy_list = conditional_t< type_list_size<policy_types_found>::value == 0,
                                               default_getter_policy,
                                               policy_types_found>;
            // at the moment we only supported one policy
            using first_prop_policy = typename std::tuple_element<0, as_std_tuple_t<policy_list>>::type;
            using getter_policy     = typename get_getter_policy<first_prop_policy>::type;
            using setter_policy     = typename get_setter_policy<first_prop_policy>::type;
            using acc_type          = typename property_type<A1>::type;
            auto prop = detail::make_unique<property_wrapper<acc_type,
                                                             Acc1, Acc2,
                                                             detail::map_access_level_to_enum<acc_level>::value,
                                                             getter_policy, setter_policy,
                                                             Metadata_Count>>(name, type::get<Class_Type>(),
                                                                              getter, setter, std::move(metadata_list));
            return std::move(prop);
        }

    public:
        bind(const std::shared_ptr<detail::registration_executer>& reg_exec, string_view name, A1 getter, A2 setter)
        :   registration_derived_t<Class_Type>(reg_exec), m_reg_exec(reg_exec), m_name(name), m_getter(getter), m_setter(setter)
        {
            detail::register_accessor_class_type_when_needed<Class_Type, A1>();
            detail::register_accessor_class_type_when_needed<Class_Type, A2>();
            m_reg_exec->add_registration_func(this);
        }

        ~bind()
        {
            using namespace detail;
            if (!m_prop.get())
                m_prop = create_default_property(m_name, m_getter, m_setter);

            auto wrapper = detail::make_rref(std::move(m_prop));
            auto reg_func = [wrapper]()
            {
                store_item<Class_Type>(std::move(wrapper.m_value));
            };
            m_reg_exec->add_registration_func(this, std::move(reg_func));
        }

        template<typename... Args>
        registration_derived_t<Class_Type> operator()(Args&&... args)
        {
            m_prop = create_custom_property(m_name,
                                            m_getter, m_setter,
                                            std::move(get_metadata(std::forward<Args>(args)...)), std::forward<Args>(args)...);
            return registration_derived_t<Class_Type>(m_reg_exec);
        }

    private:
        std::shared_ptr<detail::registration_executer> m_reg_exec;
        string_view m_name;
        A1          m_getter;
        A2          m_setter;
        std::unique_ptr<detail::property_wrapper_base> m_prop;
};

/////////////////////////////////////////////////////////////////////////////////////////

template<typename Class_Type, typename A, typename acc_level>
class registration::bind<detail::prop_readonly, Class_Type, A, acc_level> : public registration_derived_t<Class_Type>
{
    private:
        using default_getter_policy = detail::return_as_copy;
        using default_setter_policy = detail::read_only;

        template<typename Acc>
        static RTTR_INLINE
        std::unique_ptr<detail::property_wrapper_base> create_default_property(string_view name, Acc acc)
        {
            using namespace detail;
            using acc_type = typename property_type<Acc>::type;
            return detail::make_unique<property_wrapper<acc_type, A, void,
                                                        detail::map_access_level_to_enum<acc_level>::value,
                                                        default_getter_policy, default_setter_policy, 0>>(name, type::get<Class_Type>(),
                                                                                                          acc, std::array<detail::metadata, 0>());
        }

        template<typename Acc, std::size_t Metadata_Count, typename... Args>
        static RTTR_INLINE
        std::unique_ptr<detail::property_wrapper_base>  create_custom_property(string_view name,
                                                                               Acc acc,
                                                                               std::array<detail::metadata,
                                                                               Metadata_Count> metadata_list,
                                                                               Args&&...args)
        {
            using namespace detail;
            using policy_types_found = typename find_types<property_policy_list, as_type_list_t<raw_type_t<Args>...>>::type;
            static_assert(!has_double_types<policy_types_found>::value, "There are multiple policies of the same type forwarded, that is not allowed!");
            // when no policy was added, we need a default policy
            using policy_list = conditional_t< type_list_size<policy_types_found>::value == 0,
                                               default_getter_policy,
                                               policy_types_found>;

            // at the moment we only supported one policy
            using first_prop_policy = typename std::tuple_element<0, as_std_tuple_t<policy_list>>::type;
            using getter_policy     = typename get_getter_policy<first_prop_policy>::type;
            using acc_type          = typename property_type<Acc>::type;

            auto prop = detail::make_unique<property_wrapper<acc_type, Acc, void,
                                                             detail::map_access_level_to_enum<acc_level>::value,
                                                             getter_policy, default_setter_policy, Metadata_Count>>(name, type::get<Class_Type>(),
                                                                                                                    acc, std::move(metadata_list));

            return std::move(prop);
        }

    public:
        bind(const std::shared_ptr<detail::registration_executer>& reg_exec, string_view name, A acc)
        :   registration_derived_t<Class_Type>(reg_exec), m_reg_exec(reg_exec), m_name(name), m_acc(acc)
        {
            detail::register_accessor_class_type_when_needed<Class_Type, A>();
            m_reg_exec->add_registration_func(this);
        }

        ~bind()
        {
            using namespace detail;
            if (!m_prop.get())
                m_prop = create_default_property(m_name, m_acc);

            auto wrapper = detail::make_rref(std::move(m_prop));
            auto reg_func = [wrapper]()
            {
                store_item<Class_Type>(std::move(wrapper.m_value));
            };
            m_reg_exec->add_registration_func(this, std::move(reg_func));
        }

        template<typename... Args>
        registration_derived_t<Class_Type> operator()(Args&&... args)
        {
            m_prop = create_custom_property(m_name, m_acc,
                                            std::move(get_metadata(std::forward<Args>(args)...)),
                                            std::forward<Args>(args)...);
            return registration_derived_t<Class_Type>(m_reg_exec);
        }
    private:
        std::shared_ptr<detail::registration_executer> m_reg_exec;
        string_view m_name;
        A           m_acc;
        std::unique_ptr<detail::property_wrapper_base> m_prop;
};

/////////////////////////////////////////////////////////////////////////////////////////

template<typename Class_Type, typename F, typename acc_level>
class registration::bind<detail::meth, Class_Type, F, acc_level> : public registration_derived_t<Class_Type>
{
    private:
        template<typename Acc_Func>
        static RTTR_INLINE std::unique_ptr<detail::method_wrapper_base> create_default_method(string_view name, Acc_Func func)
        {
            using namespace detail;
            using param_info_t =  decltype(create_param_infos<type_list<F>, function_type>());
            return detail::make_unique<method_wrapper<Acc_Func,
                                                      map_access_level_to_enum<acc_level>::value,
                                                      default_invoke,
                                                      default_args<>,
                                                      param_info_t,
                                                      0>>(name, type::get<Class_Type>(), func, std::array<detail::metadata, 0>(), param_info_t());
        }

        template<typename Acc_Func, typename... Args>
        static RTTR_INLINE std::unique_ptr<detail::method_wrapper_base> create_custom_method(string_view name, Acc_Func func, Args&&...args)
        {
            using namespace detail;

            using policy_types_found = typename find_types<method_policy_list, as_type_list_t<raw_type_t<Args>...>>::type;
            static_assert(!has_double_types<policy_types_found>::value, "There are multiple policies of the same type forwarded, that is not allowed!");

            using has_valid_default_types = has_default_types<type_list<Acc_Func>, type_list<Args...>, function_type>;
            static_assert( (has_default_args<Args...>::value && has_valid_default_types::value) || !has_default_args<Args...>::value,
                           "The provided default arguments, cannot be used with the given method. Please check the provided argument types."
                           "The given arguments must match the signature from the starting position to the right most argument.");

            static_assert((count_default_args<Args...>::value <= 1),
                          "Only one set of 'default_arguments' can be provided during registration of a method!");

            static_assert((count_param_names<Args...>::value <= 1),
                          "Only one set of 'parameter_names' can be provided during registration of a method!");

            static_assert(((!has_param_names<Args...>::value) ||
                           (param_names_count<Args...>::value == function_traits<Acc_Func>::arg_count)),
                          "The provided amount of names in 'parameter_names' does not match argument count of the function signature.");

            // when no policy was added, we need a default policy
            using policy_list = conditional_t< type_list_size<policy_types_found>::value == 0,
                                               default_invoke,
                                               policy_types_found>;
            using policy = typename std::tuple_element<0, as_std_tuple_t<policy_list>>::type;
            using metadata_count = count_type<::rttr::detail::metadata, type_list<Args...>>;
            auto meth = create_method_wrapper<policy,
                                              metadata_count::value>(name, func,
                                                                     std::move(get_metadata(std::forward<Args>(args)...)),
                                                                     std::move(get_default_args<type_list<Acc_Func>, function_type>(std::forward<Args>(args)...)),
                                                                     std::move(create_param_infos<type_list<F>, function_type>(std::forward<Args>(args)...)) );
            return std::move(meth);
        }

        template<typename Policy, std::size_t Metadata_Count, typename...TArgs, typename...Param_Args>
        static RTTR_INLINE std::unique_ptr<detail::method_wrapper_base>
        create_method_wrapper(string_view name, F func,
                              std::array<detail::metadata, Metadata_Count> metadata_list,
                              detail::default_args<TArgs...> def_args,
                              detail::parameter_infos<Param_Args...> param_infos)
        {
            return detail::make_unique<detail::method_wrapper<F,
                                                              detail::map_access_level_to_enum<acc_level>::value,
                                                              Policy,
                                                              detail::default_args<TArgs...>,
                                                              detail::parameter_infos<Param_Args...>,
                                                              Metadata_Count>>(name,
                                                                               type::get<Class_Type>(),
                                                                               func,
                                                                               std::move(metadata_list),
                                                                               std::move(def_args),
                                                                               std::move(param_infos));
        }

        template<typename Policy, std::size_t Metadata_Count, typename...Param_Args>
        static RTTR_INLINE std::unique_ptr<detail::method_wrapper_base>
        create_method_wrapper(string_view name, F func,
                              std::array<detail::metadata, Metadata_Count> metadata_list,
                              detail::default_args<> def_args,
                              detail::parameter_infos<Param_Args...> param_infos)
        {
            return detail::make_unique<detail::method_wrapper<F,
                                                              detail::map_access_level_to_enum<acc_level>::value,
                                                              Policy,
                                                              detail::default_args<>,
                                                              detail::parameter_infos<Param_Args...>,
                                                              Metadata_Count>>(name,
                                                                               type::get<Class_Type>(),
                                                                               func,
                                                                               std::move(metadata_list),
                                                                               std::move(param_infos));
        }

    public:
        bind(const std::shared_ptr<detail::registration_executer>& reg_exec, string_view name, F f)
        :   registration_derived_t<Class_Type>(reg_exec), m_reg_exec(reg_exec), m_name(name), m_func(f)
        {
            detail::register_accessor_class_type_when_needed<Class_Type, F>();
            m_reg_exec->add_registration_func(this);
        }

        ~bind()
        {
            using namespace detail;
            if (!m_meth.get())
                m_meth = create_default_method(m_name, m_func);


            auto wrapper = detail::make_rref(std::move(m_meth));
            auto reg_func = [wrapper]()
            {
                store_item<Class_Type>(std::move(wrapper.m_value));
            };
            m_reg_exec->add_registration_func(this, std::move(reg_func));
        }

    public:
        template<typename... Args>
        registration_derived_t<Class_Type> operator()(Args&&... args)
        {
            m_meth = create_custom_method(m_name, m_func, std::forward<Args>(args)...);
            return registration_derived_t<Class_Type>(m_reg_exec);
        }

    private:
        std::shared_ptr<detail::registration_executer> m_reg_exec;
        string_view m_name;
        F           m_func;
        std::unique_ptr<detail::method_wrapper_base> m_meth;
};

/////////////////////////////////////////////////////////////////////////////////////////

template<typename Class_Type, typename Enum_Type>
class registration::bind<detail::enum_, Class_Type, Enum_Type> : public registration_derived_t<Class_Type>
{
    private:
        template<typename E_Type>
        static RTTR_INLINE std::unique_ptr<detail::enumeration_wrapper_base> create_default_enum()
        {
            using namespace detail;
            return detail::make_unique<enumeration_wrapper<E_Type, 0, 0>>(get_enum_values<E_Type>(), std::array<detail::metadata, 0>());
        }

        template<typename E_Type, typename... Args>
        static RTTR_INLINE std::unique_ptr<detail::enumeration_wrapper_base> create_custom_enum(Args&&...args)
        {
            using namespace detail;

            static RTTR_CONSTEXPR_OR_CONST std::size_t enum_count = count_type<enum_data<Enum_Type>, type_list<raw_type_t<Args>...>>::value;
            static RTTR_CONSTEXPR_OR_CONST std::size_t global_enum_count = count_if<is_enum_data, raw_type_t<Args>...>::value;

            static_assert(enum_count == global_enum_count, "Invalid 'value' pair for enumeration type provided, please specify values only for enums of type 'Enum_Type'.");

            using metadata_count = count_type<::rttr::detail::metadata, type_list<Args...>>;

            auto enum_wrapper = detail::make_unique<enumeration_wrapper<E_Type,
                                                                        enum_count,
                                                                        metadata_count::value>>(get_enum_values<E_Type>(std::forward<Args>(args)...),
                                                                                                std::move(get_metadata(std::forward<Args>(args)...)));


            return std::move(enum_wrapper);
        }

    public:
        bind(const std::shared_ptr<detail::registration_executer>& reg_exec, string_view name)
        :   registration_derived_t<Class_Type>(reg_exec), m_reg_exec(reg_exec), m_declared_type(type::get<Class_Type>())
        {
            using namespace detail;

            m_reg_exec->add_registration_func(this);
            auto t = type::get<Enum_Type>();
            type_register::custom_name(t, name);
        }

        ~bind()
        {
            using namespace detail;
            if (!m_enum.get())
                m_enum = create_default_enum<Enum_Type>();

            m_enum->set_declaring_type(m_declared_type);
            // register the underlying type with the following call:
            m_enum->get_type();

            auto wrapper = detail::make_rref(std::move(m_enum));
            auto reg_func = [wrapper]()
            {
                store_item<Class_Type>(std::move(wrapper.m_value));
            };
            m_reg_exec->add_registration_func(this, std::move(reg_func));
        }

        template<typename... Args>
        registration_derived_t<Class_Type> operator()(Args&&... arg)
        {
            m_enum = create_custom_enum<Enum_Type>(std::forward<Args>(arg)...);
            return registration_derived_t<Class_Type>(m_reg_exec);
        }
    private:
        std::shared_ptr<detail::registration_executer> m_reg_exec;
        type m_declared_type;
        std::unique_ptr<detail::enumeration_wrapper_base> m_enum;
};

} // end namespace rttr


#endif // RTTR_BIND_REFLECTION_IMPL_H_
