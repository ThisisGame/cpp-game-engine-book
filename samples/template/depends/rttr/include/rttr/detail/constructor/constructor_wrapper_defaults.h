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

#ifndef RTTR_CONSTRUCTOR_WRAPPER_DEFAULTS_H_
#define RTTR_CONSTRUCTOR_WRAPPER_DEFAULTS_H_

#include "rttr/detail/base/core_prerequisites.h"
#include "rttr/detail/constructor/constructor_wrapper_base.h"
#include "rttr/detail/type/accessor_type.h"
#include "rttr/argument.h"
#include "rttr/detail/misc/utility.h"
#include "rttr/detail/misc/function_traits.h"
#include "rttr/variant.h"
#include "rttr/policy.h"
#include "rttr/detail/method/method_accessor.h"
#include "rttr/detail/constructor/constructor_invoker.h"
#include "rttr/detail/default_arguments/default_arguments.h"
#include "rttr/detail/default_arguments/invoke_with_defaults.h"
#include "rttr/detail/parameter_info/parameter_infos.h"

#include <vector>
#include <utility>
#include <type_traits>

namespace rttr
{
namespace detail
{

template<typename ClassType, typename Constructor_Type, access_levels Acc_Level, typename Policy,
         std::size_t Metadata_Count, typename Default_Args, typename Parameter_Infos, typename... Args>
class constructor_wrapper;

/////////////////////////////////////////////////////////////////////////////////////////

template<typename Class_Type, access_levels Acc_Level, typename Policy, std::size_t Metadata_Count, typename... Def_Args, typename...Param_Args, typename... Ctor_Args>
class constructor_wrapper<Class_Type, class_ctor, Acc_Level, Policy, Metadata_Count, default_args<Def_Args...>, parameter_infos<Param_Args...>, Ctor_Args...>
:   public constructor_wrapper_base, public metadata_handler<Metadata_Count>
{
        using invoker_class = constructor_invoker<ctor_type, Policy, type_list<Class_Type, Ctor_Args...>, index_sequence_for<Ctor_Args...>>;
        using instanciated_type = typename invoker_class::return_type;
        using invoke_with_defaults = invoke_defaults_helper<invoker_class, type_list<Ctor_Args...>>;

    public:
        constructor_wrapper(std::array<metadata, Metadata_Count> metadata_list,
                            default_args<Def_Args...> default_args,
                            parameter_infos<Param_Args...> param_infos) RTTR_NOEXCEPT
        :   metadata_handler<Metadata_Count>(std::move(metadata_list)),
            m_def_args(std::move(default_args)), m_param_infos(std::move(param_infos)),
            m_param_info_list(create_paramter_info_array(m_param_infos))
        {
            store_default_args_in_param_infos(m_param_infos, m_def_args);
            init();
        }

        bool is_valid()                  const RTTR_NOEXCEPT { return true; }
        type get_instantiated_type()     const RTTR_NOEXCEPT { return type::get<instanciated_type>(); }
        type get_declaring_type()        const RTTR_NOEXCEPT { return type::get<typename raw_type<Class_Type>::type>(); }
        access_levels get_access_level() const RTTR_NOEXCEPT { return Acc_Level; }

        RTTR_INLINE std::vector<bool> get_is_reference_impl(std::true_type) const RTTR_NOEXCEPT { return {std::is_reference<Ctor_Args>::value...}; }
        RTTR_INLINE std::vector<bool> get_is_reference_impl(std::false_type) const RTTR_NOEXCEPT { return {}; }

        RTTR_INLINE std::vector<bool> get_is_const_impl(std::true_type) const RTTR_NOEXCEPT { return {std::is_const<typename std::remove_reference<Ctor_Args>::type>::value...}; }
        RTTR_INLINE std::vector<bool> get_is_const_impl(std::false_type) const RTTR_NOEXCEPT { return {}; }

        std::vector<bool> get_is_reference() const RTTR_NOEXCEPT { return get_is_reference_impl(std::integral_constant<bool, sizeof...(Ctor_Args) != 0>()); }
        std::vector<bool> get_is_const() const RTTR_NOEXCEPT { return get_is_const_impl(std::integral_constant<bool, sizeof...(Ctor_Args) != 0>()); }

        array_range<parameter_info> get_parameter_infos() const RTTR_NOEXCEPT { return array_range<parameter_info>(m_param_info_list.data(),
                                                                                                                   m_param_info_list.size()); }
        variant get_metadata(const variant& key) const { return metadata_handler<Metadata_Count>::get_metadata(key); }

        variant invoke() const
        {
            return invoke_with_defaults::invoke(m_def_args.m_args);
        }

        variant invoke(argument& arg1) const
        {
            return invoke_with_defaults::invoke(m_def_args.m_args, arg1);
        }
        variant invoke(argument& arg1, argument& arg2) const
        {
            return invoke_with_defaults::invoke(m_def_args.m_args, arg1, arg2);
        }
        variant invoke(argument& arg1, argument& arg2, argument& arg3) const
        {
            return invoke_with_defaults::invoke(m_def_args.m_args, arg1, arg2, arg3);
        }
        variant invoke(argument& arg1, argument& arg2, argument& arg3, argument& arg4) const
        {
            return invoke_with_defaults::invoke(m_def_args.m_args, arg1, arg2, arg3, arg4);
        }
        variant invoke(argument& arg1, argument& arg2, argument& arg3, argument& arg4, argument& arg5) const
        {
            return invoke_with_defaults::invoke(m_def_args.m_args, arg1, arg2, arg3, arg4, arg5);
        }
        variant invoke(argument& arg1, argument& arg2, argument& arg3, argument& arg4, argument& arg5, argument& arg6) const
        {
            return invoke_with_defaults::invoke(m_def_args.m_args, arg1, arg2, arg3, arg4, arg5, arg6);
        }

        variant invoke_variadic(std::vector<argument>& args) const
        {
            if (args.size() <= sizeof...(Ctor_Args))
                return invoke_variadic_helper<invoke_with_defaults, index_sequence_for<Ctor_Args...>>::invoke(args, m_def_args.m_args);
            else
                return variant();
        }

    private:
        default_args<Def_Args...> m_def_args;
        parameter_infos<Param_Args...> m_param_infos;
        std::array<parameter_info, sizeof...(Param_Args)> m_param_info_list;
};

/////////////////////////////////////////////////////////////////////////////////////////

template<typename ClassType, access_levels Acc_Level, typename Policy,
         std::size_t Metadata_Count, typename... Def_Args, typename...Param_Args, typename F>
class constructor_wrapper<ClassType, return_func, Acc_Level, Policy,
                          Metadata_Count, default_args<Def_Args...>, parameter_infos<Param_Args...>, F>
:   public constructor_wrapper_base, public metadata_handler<Metadata_Count>
{
    using instanciated_type = typename function_traits<F>::return_type;
    using method_type = typename detail::method_type<F>::type;
    using arg_index_sequence = make_index_sequence< function_traits<F>::arg_count >;
    using invoker_class = method_invoker<F, Policy, method_type, arg_index_sequence>;
    using invoke_with_defaults = invoke_defaults_helper<invoker_class, F>;

    public:
        constructor_wrapper(F creator_func,
                            std::array<metadata, Metadata_Count> metadata_list,
                            default_args<Def_Args...> default_args,
                            parameter_infos<Param_Args...> param_infos) RTTR_NOEXCEPT
        :   metadata_handler<Metadata_Count>(std::move(metadata_list)),
            m_creator_func(creator_func),
            m_def_args(std::move(default_args)), m_param_infos(std::move(param_infos)),
            m_param_info_list(create_paramter_info_array(m_param_infos))
        {
            store_default_args_in_param_infos(m_param_infos, m_def_args);
            init();
        }

        bool is_valid()                                     const RTTR_NOEXCEPT { return true; }
        access_levels get_access_level()                    const RTTR_NOEXCEPT { return Acc_Level; }
        type get_instantiated_type()                        const RTTR_NOEXCEPT { return type::get<instanciated_type>();                      }
        type get_declaring_type()                           const RTTR_NOEXCEPT { return type::get<typename raw_type<ClassType>::type>();     }
        std::vector<bool> get_is_reference()                const RTTR_NOEXCEPT { return method_accessor<F, Policy>::get_is_reference();      }
        std::vector<bool> get_is_const()                    const RTTR_NOEXCEPT { return method_accessor<F, Policy>::get_is_const();          }
        array_range<parameter_info> get_parameter_infos()   const RTTR_NOEXCEPT { return array_range<parameter_info>(m_param_info_list.data(),
                                                                                                       m_param_info_list.size()); }
        variant get_metadata(const variant& key)            const RTTR_NOEXCEPT { return metadata_handler<Metadata_Count>::get_metadata(key); }

        variant invoke() const
        {
           return invoke_with_defaults::invoke(m_creator_func, instance(), m_def_args.m_args);
        }
        variant invoke(argument& arg1) const
        {
            return invoke_with_defaults::invoke(m_creator_func, instance(), m_def_args.m_args, arg1);
        }
        variant invoke(argument& arg1, argument& arg2) const
        {
            return invoke_with_defaults::invoke(m_creator_func, instance(), m_def_args.m_args, arg1, arg2);
        }
        variant invoke(argument& arg1, argument& arg2, argument& arg3) const
        {
            return invoke_with_defaults::invoke(m_creator_func, instance(), m_def_args.m_args, arg1, arg2, arg3);
        }
        variant invoke(argument& arg1, argument& arg2, argument& arg3, argument& arg4) const
        {
            return invoke_with_defaults::invoke(m_creator_func, instance(), m_def_args.m_args, arg1, arg2, arg3, arg4);
        }
        variant invoke(argument& arg1, argument& arg2, argument& arg3, argument& arg4, argument& arg5) const
        {
            return invoke_with_defaults::invoke(m_creator_func, instance(), m_def_args.m_args, arg1, arg2, arg3, arg4, arg5);
        }
        variant invoke(argument& arg1, argument& arg2, argument& arg3, argument& arg4, argument& arg5, argument& arg6) const
        {
            return invoke_with_defaults::invoke(m_creator_func, instance(), m_def_args.m_args, arg1, arg2, arg3, arg4, arg5, arg6);
        }
        variant invoke_variadic(std::vector<argument>& args) const
        {
            if (args.size() <= function_traits<F>::arg_count)
                return invoke_variadic_helper<invoke_with_defaults, arg_index_sequence>::invoke(args, m_creator_func, instance(), m_def_args.m_args);
            else
                return variant();
        }

    private:
         F  m_creator_func;
         default_args<Def_Args...> m_def_args;
         parameter_infos<Param_Args...> m_param_infos;
         std::array<parameter_info, sizeof...(Param_Args)> m_param_info_list;
};

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

template<typename Class_Type, access_levels Acc_Level, typename Policy, std::size_t Metadata_Count, typename... Def_Args, typename... Ctor_Args>
class constructor_wrapper<Class_Type, class_ctor, Acc_Level, Policy, Metadata_Count, default_args<Def_Args...>, parameter_infos<>, Ctor_Args...>
:   public constructor_wrapper_base, public metadata_handler<Metadata_Count>
{
        using invoker_class = constructor_invoker<ctor_type, Policy, type_list<Class_Type, Ctor_Args...>, index_sequence_for<Ctor_Args...>>;
        using instanciated_type = typename invoker_class::return_type;
        using invoke_with_defaults = invoke_defaults_helper<invoker_class, type_list<Ctor_Args...>>;

    public:
        constructor_wrapper(std::array<metadata, Metadata_Count> metadata_list,
                            default_args<Def_Args...> default_args,
                            parameter_infos<> param_infos) RTTR_NOEXCEPT
        :   metadata_handler<Metadata_Count>(std::move(metadata_list)),
            m_def_args(std::move(default_args))
        {
            init();
        }

        bool is_valid()                  const RTTR_NOEXCEPT { return true; }
        type get_instantiated_type()     const RTTR_NOEXCEPT { return type::get<instanciated_type>(); }
        type get_declaring_type()        const RTTR_NOEXCEPT { return type::get<typename raw_type<Class_Type>::type>(); }
        access_levels get_access_level() const RTTR_NOEXCEPT { return Acc_Level; }

        RTTR_INLINE std::vector<bool> get_is_reference_impl(std::true_type) const RTTR_NOEXCEPT { return {std::is_reference<Ctor_Args>::value...}; }
        RTTR_INLINE std::vector<bool> get_is_reference_impl(std::false_type) const RTTR_NOEXCEPT { return {}; }

        RTTR_INLINE std::vector<bool> get_is_const_impl(std::true_type) const RTTR_NOEXCEPT { return {std::is_const<typename std::remove_reference<Ctor_Args>::type>::value...}; }
        RTTR_INLINE std::vector<bool> get_is_const_impl(std::false_type) const RTTR_NOEXCEPT { return {}; }

        std::vector<bool> get_is_reference() const RTTR_NOEXCEPT { return get_is_reference_impl(std::integral_constant<bool, sizeof...(Ctor_Args) != 0>()); }
        std::vector<bool> get_is_const() const RTTR_NOEXCEPT { return get_is_const_impl(std::integral_constant<bool, sizeof...(Ctor_Args) != 0>()); }

        array_range<parameter_info> get_parameter_infos()   const RTTR_NOEXCEPT { return array_range<parameter_info>(); }
        variant get_metadata(const variant& key)            const { return metadata_handler<Metadata_Count>::get_metadata(key); }

        variant invoke() const
        {
            return invoke_with_defaults::invoke(m_def_args.m_args);
        }

        variant invoke(argument& arg1) const
        {
            return invoke_with_defaults::invoke(m_def_args.m_args, arg1);
        }
        variant invoke(argument& arg1, argument& arg2) const
        {
            return invoke_with_defaults::invoke(m_def_args.m_args, arg1, arg2);
        }
        variant invoke(argument& arg1, argument& arg2, argument& arg3) const
        {
            return invoke_with_defaults::invoke(m_def_args.m_args, arg1, arg2, arg3);
        }
        variant invoke(argument& arg1, argument& arg2, argument& arg3, argument& arg4) const
        {
            return invoke_with_defaults::invoke(m_def_args.m_args, arg1, arg2, arg3, arg4);
        }
        variant invoke(argument& arg1, argument& arg2, argument& arg3, argument& arg4, argument& arg5) const
        {
            return invoke_with_defaults::invoke(m_def_args.m_args, arg1, arg2, arg3, arg4, arg5);
        }
        variant invoke(argument& arg1, argument& arg2, argument& arg3, argument& arg4, argument& arg5, argument& arg6) const
        {
            return invoke_with_defaults::invoke(m_def_args.m_args, arg1, arg2, arg3, arg4, arg5, arg6);
        }

        variant invoke_variadic(std::vector<argument>& args) const
        {
            if (args.size() <= sizeof...(Ctor_Args))
                return invoke_variadic_helper<invoke_with_defaults, index_sequence_for<Ctor_Args...>>::invoke(args, m_def_args.m_args);
            else
                return variant();
        }

    private:
        default_args<Def_Args...> m_def_args;
};

/////////////////////////////////////////////////////////////////////////////////////////

template<typename ClassType, access_levels Acc_Level, typename Policy,
         std::size_t Metadata_Count, typename... Def_Args, typename F>
class constructor_wrapper<ClassType, return_func, Acc_Level, Policy,
                          Metadata_Count, default_args<Def_Args...>, parameter_infos<>, F>
:   public constructor_wrapper_base, public metadata_handler<Metadata_Count>
{
    using instanciated_type = typename function_traits<F>::return_type;
    using method_type = typename detail::method_type<F>::type;
    using arg_index_sequence = make_index_sequence< function_traits<F>::arg_count >;
    using invoker_class = method_invoker<F, Policy, method_type, arg_index_sequence>;
    using invoke_with_defaults = invoke_defaults_helper<invoker_class, F>;

    public:
        constructor_wrapper(F creator_func,
                            std::array<metadata, Metadata_Count> metadata_list,
                            default_args<Def_Args...> default_args,
                            parameter_infos<> param_infos) RTTR_NOEXCEPT
        :   metadata_handler<Metadata_Count>(std::move(metadata_list)),
            m_creator_func(creator_func),
            m_def_args(std::move(default_args))
        {
            init();
        }

        bool is_valid()                                     const RTTR_NOEXCEPT { return true; }
        access_levels get_access_level()                    const RTTR_NOEXCEPT { return Acc_Level;                                           }
        type get_instantiated_type()                        const RTTR_NOEXCEPT { return type::get<instanciated_type>();                      }
        type get_declaring_type()                           const RTTR_NOEXCEPT { return type::get<typename raw_type<ClassType>::type>();     }
        std::vector<bool> get_is_reference()                const RTTR_NOEXCEPT { return method_accessor<F, Policy>::get_is_reference();      }
        std::vector<bool> get_is_const()                    const RTTR_NOEXCEPT { return method_accessor<F, Policy>::get_is_const();          }
        array_range<parameter_info> get_parameter_infos()   const RTTR_NOEXCEPT { return array_range<parameter_info>();                       }
        variant get_metadata(const variant& key)            const { return metadata_handler<Metadata_Count>::get_metadata(key); }

        variant invoke() const
        {
           return invoke_with_defaults::invoke(m_creator_func, instance(), m_def_args.m_args);
        }
        variant invoke(argument& arg1) const
        {
            return invoke_with_defaults::invoke(m_creator_func, instance(), m_def_args.m_args, arg1);
        }
        variant invoke(argument& arg1, argument& arg2) const
        {
            return invoke_with_defaults::invoke(m_creator_func, instance(), m_def_args.m_args, arg1, arg2);
        }
        variant invoke(argument& arg1, argument& arg2, argument& arg3) const
        {
            return invoke_with_defaults::invoke(m_creator_func, instance(), m_def_args.m_args, arg1, arg2, arg3);
        }
        variant invoke(argument& arg1, argument& arg2, argument& arg3, argument& arg4) const
        {
            return invoke_with_defaults::invoke(m_creator_func, instance(), m_def_args.m_args, arg1, arg2, arg3, arg4);
        }
        variant invoke(argument& arg1, argument& arg2, argument& arg3, argument& arg4, argument& arg5) const
        {
            return invoke_with_defaults::invoke(m_creator_func, instance(), m_def_args.m_args, arg1, arg2, arg3, arg4, arg5);
        }
        variant invoke(argument& arg1, argument& arg2, argument& arg3, argument& arg4, argument& arg5, argument& arg6) const
        {
            return invoke_with_defaults::invoke(m_creator_func, instance(), m_def_args.m_args, arg1, arg2, arg3, arg4, arg5, arg6);
        }
        variant invoke_variadic(std::vector<argument>& args) const
        {
            if (args.size() <= function_traits<F>::arg_count)
                return invoke_variadic_helper<invoke_with_defaults, arg_index_sequence>::invoke(args, m_creator_func, instance(), m_def_args.m_args);
            else
                return variant();
        }

    private:
         F  m_creator_func;
         default_args<Def_Args...> m_def_args;
};

/////////////////////////////////////////////////////////////////////////////////////////

} // end namespace detail
} // end namespace rttr

#endif // RTTR_CONSTRUCTOR_WRAPPER_DEFAULTS_H_
