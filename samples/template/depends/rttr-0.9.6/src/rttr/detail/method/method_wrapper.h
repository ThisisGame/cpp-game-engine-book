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

#ifndef RTTR_METHOD_WRAPPER_H_
#define RTTR_METHOD_WRAPPER_H_

#include "rttr/detail/base/core_prerequisites.h"
#include "rttr/detail/method/method_wrapper_base.h"
#include "rttr/detail/misc/function_traits.h"
#include "rttr/detail/type/accessor_type.h"
#include "rttr/detail/method/method_accessor.h"
#include "rttr/detail/default_arguments/default_arguments.h"
#include "rttr/detail/default_arguments/invoke_with_defaults.h"
#include "rttr/detail/parameter_info/parameter_infos.h"
#include "rttr/argument.h"
#include "rttr/instance.h"
#include "rttr/variant.h"
#include "rttr/array_range.h"

#include <functional>
#include <string>
#include <array>

namespace rttr
{
namespace detail
{

template<typename F, access_levels Acc_Level, typename Policy, typename Default_Args, typename Parameter_Infos, std::size_t Metadata_Count>
class method_wrapper;

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

template<typename F, access_levels Acc_Level, typename Policy, typename... Param_Args, std::size_t Metadata_Count>
class method_wrapper<F, Acc_Level, Policy, default_args<>, parameter_infos<Param_Args...>, Metadata_Count> : public method_wrapper_base, public metadata_handler<Metadata_Count>
{
    public:
        method_wrapper(string_view name,
                       type declaring_type,
                       F func_acc,
                       std::array<metadata, Metadata_Count> metadata_list,
                       parameter_infos<Param_Args...> param_infos) RTTR_NOEXCEPT
        :   method_wrapper_base(name, declaring_type),
            metadata_handler<Metadata_Count>(std::move(metadata_list)),
            m_func_acc(func_acc),
            m_param_infos(std::move(param_infos)),
            m_param_info_list(create_paramter_info_array(m_param_infos))
        {
            init();
        }

        bool is_valid()                                     const RTTR_NOEXCEPT { return true;                                            }
        bool is_static()                                    const RTTR_NOEXCEPT { return method_accessor<F, Policy>::is_static();         }
        type get_return_type()                              const RTTR_NOEXCEPT { return method_accessor<F, Policy>::get_return_type();   }
        std::vector<bool> get_is_reference()                const RTTR_NOEXCEPT { return method_accessor<F, Policy>::get_is_reference();  }
        std::vector<bool> get_is_const()                    const RTTR_NOEXCEPT { return method_accessor<F, Policy>::get_is_const();      }
        access_levels get_access_level()                    const RTTR_NOEXCEPT { return Acc_Level; }
        array_range<parameter_info> get_parameter_infos()   const RTTR_NOEXCEPT { return array_range<parameter_info>(const_cast<decltype(m_param_info_list)&>(m_param_info_list).data(),
                                                                                                       m_param_info_list.size()); }
        variant get_metadata(const variant& key)            const { return metadata_handler<Metadata_Count>::get_metadata(key); }

        variant invoke(instance& object) const
        {
           return method_accessor<F, Policy>::invoke(m_func_acc, object);
        }
        variant invoke(instance& object, argument& arg1) const
        {
            return method_accessor<F, Policy>::invoke(m_func_acc, object, arg1);
        }
        variant invoke(instance& object, argument& arg1, argument& arg2) const
        {
            return method_accessor<F, Policy>::invoke(m_func_acc, object, arg1, arg2);
        }
        variant invoke(instance& object, argument& arg1, argument& arg2, argument& arg3) const
        {
            return method_accessor<F, Policy>::invoke(m_func_acc, object, arg1, arg2, arg3);
        }
        variant invoke(instance& object, argument& arg1, argument& arg2, argument& arg3, argument& arg4) const
        {
            return method_accessor<F, Policy>::invoke(m_func_acc, object, arg1, arg2, arg3, arg4);
        }
        variant invoke(instance& object, argument& arg1, argument& arg2, argument& arg3, argument& arg4, argument& arg5) const
        {
            return method_accessor<F, Policy>::invoke(m_func_acc, object, arg1, arg2, arg3, arg4, arg5);
        }
        variant invoke(instance& object, argument& arg1, argument& arg2, argument& arg3, argument& arg4, argument& arg5, argument& arg6) const
        {
            return method_accessor<F, Policy>::invoke(m_func_acc, object, arg1, arg2, arg3, arg4, arg5, arg6);
        }

        variant invoke_variadic(const instance& object, std::vector<argument>& args) const
        {
            return method_accessor<F, Policy>::invoke_variadic(m_func_acc, object, args);
        }

    private:
        F  m_func_acc;
        parameter_infos<Param_Args...> m_param_infos;
        std::array<parameter_info, sizeof...(Param_Args)> m_param_info_list;
};

/////////////////////////////////////////////////////////////////////////////////////////

template<typename F, access_levels Acc_Level, typename Policy, typename...Default_Args, typename...Param_Args, std::size_t Metadata_Count>
class method_wrapper<F, Acc_Level, Policy, default_args<Default_Args...>, parameter_infos<Param_Args...>, Metadata_Count> : public method_wrapper_base, public metadata_handler<Metadata_Count>
{
    using method_type = typename detail::method_type<F>::type;
    using arg_index_sequence = make_index_sequence<function_traits<F>::arg_count>;
    using invoker_class = method_invoker<F, Policy, method_type, arg_index_sequence>;
    using invoke_with_defaults = invoke_defaults_helper<invoker_class, F>;

    public:
        method_wrapper(string_view name,
                       type declaring_type,
                       F func_acc,
                       std::array<metadata, Metadata_Count> metadata_list,
                       default_args<Default_Args...> default_args,
                       parameter_infos<Param_Args...> param_infos) RTTR_NOEXCEPT
        :   method_wrapper_base(name, declaring_type),
            metadata_handler<Metadata_Count>(std::move(metadata_list)),
            m_func_acc(func_acc),
            m_def_args(std::move(default_args)),
            m_param_infos(std::move(param_infos)),
            m_param_info_list(create_paramter_info_array(m_param_infos))
        {
            store_default_args_in_param_infos(m_param_infos, m_def_args);
            init();
        }

        bool is_valid()                                     const RTTR_NOEXCEPT { return true;                                            }
        bool is_static()                                    const RTTR_NOEXCEPT { return method_accessor<F, Policy>::is_static();         }
        type get_return_type()                              const RTTR_NOEXCEPT { return method_accessor<F, Policy>::get_return_type();   }
        std::vector<bool> get_is_reference()                const RTTR_NOEXCEPT { return method_accessor<F, Policy>::get_is_reference();  }
        std::vector<bool> get_is_const()                    const RTTR_NOEXCEPT { return method_accessor<F, Policy>::get_is_const();      }
        access_levels get_access_level()                    const RTTR_NOEXCEPT { return Acc_Level; }
        array_range<parameter_info> get_parameter_infos()   const RTTR_NOEXCEPT { return array_range<parameter_info>(const_cast<decltype(m_param_info_list)&>(m_param_info_list).data(),
                                                                                                       m_param_info_list.size()); }
        variant get_metadata(const variant& key)            const { return metadata_handler<Metadata_Count>::get_metadata(key); }

        variant invoke(instance& object) const
        {
            return invoke_with_defaults::invoke(m_func_acc, object, m_def_args.m_args);
        }
        variant invoke(instance& object, argument& arg1) const
        {
            return invoke_with_defaults::invoke(m_func_acc, object, m_def_args.m_args, arg1);
        }
        variant invoke(instance& object, argument& arg1, argument& arg2) const
        {
            return invoke_with_defaults::invoke(m_func_acc, object, m_def_args.m_args, arg1, arg2);
        }
        variant invoke(instance& object, argument& arg1, argument& arg2, argument& arg3) const
        {
            return invoke_with_defaults::invoke(m_func_acc, object, m_def_args.m_args, arg1, arg2, arg3);
        }
        variant invoke(instance& object, argument& arg1, argument& arg2, argument& arg3, argument& arg4) const
        {
            return invoke_with_defaults::invoke(m_func_acc, object, m_def_args.m_args, arg1, arg2, arg3, arg4);
        }
        variant invoke(instance& object, argument& arg1, argument& arg2, argument& arg3, argument& arg4, argument& arg5) const
        {
            return invoke_with_defaults::invoke(m_func_acc, object, m_def_args.m_args, arg1, arg2, arg3, arg4, arg5);
        }
        variant invoke(instance& object, argument& arg1, argument& arg2, argument& arg3, argument& arg4, argument& arg5, argument& arg6) const
        {
            return invoke_with_defaults::invoke(m_func_acc, object, m_def_args.m_args, arg1, arg2, arg3, arg4, arg5, arg6);
        }

        variant invoke_variadic(const instance& object, std::vector<argument>& args) const
        {
            if (args.size() <= function_traits<F>::arg_count)
                return invoke_variadic_helper<invoke_with_defaults, arg_index_sequence>::invoke(args, m_func_acc, object, m_def_args.m_args);
            else
                return variant();
        }

    private:
        F                               m_func_acc;
        default_args<Default_Args...>   m_def_args;
        parameter_infos<Param_Args...>  m_param_infos;
        std::array<parameter_info, sizeof...(Param_Args)> m_param_info_list;
};

/////////////////////////////////////////////////////////////////////////////////////////

template<typename F, access_levels Acc_Level, typename Policy, std::size_t Metadata_Count>
class method_wrapper<F, Acc_Level, Policy, default_args<>, parameter_infos<>, Metadata_Count> : public method_wrapper_base, public metadata_handler<Metadata_Count>
{
    public:
        method_wrapper(string_view name,
                       type declaring_type,
                       F func_acc,
                       std::array<metadata, Metadata_Count> metadata_list,
                       parameter_infos<> param_infos) RTTR_NOEXCEPT
        :   method_wrapper_base(name, declaring_type),
            metadata_handler<Metadata_Count>(std::move(metadata_list)),
            m_func_acc(func_acc)
        {
            init();
        }

        bool is_valid()                                     const RTTR_NOEXCEPT { return true;                                            }
        bool is_static()                                    const RTTR_NOEXCEPT { return method_accessor<F, Policy>::is_static();         }
        type get_return_type()                              const RTTR_NOEXCEPT { return method_accessor<F, Policy>::get_return_type();   }
        std::vector<bool> get_is_reference()                const RTTR_NOEXCEPT { return method_accessor<F, Policy>::get_is_reference();  }
        std::vector<bool> get_is_const()                    const RTTR_NOEXCEPT { return method_accessor<F, Policy>::get_is_const();      }
        access_levels get_access_level()                    const RTTR_NOEXCEPT { return Acc_Level;                                       }
        array_range<parameter_info> get_parameter_infos()   const RTTR_NOEXCEPT { return array_range<parameter_info>();                   }
        variant get_metadata(const variant& key)            const { return metadata_handler<Metadata_Count>::get_metadata(key); }

        variant invoke(instance& object) const
        {
           return method_accessor<F, Policy>::invoke(m_func_acc, object);
        }
        variant invoke(instance& object, argument& arg1) const
        {
            return method_accessor<F, Policy>::invoke(m_func_acc, object, arg1);
        }
        variant invoke(instance& object, argument& arg1, argument& arg2) const
        {
            return method_accessor<F, Policy>::invoke(m_func_acc, object, arg1, arg2);
        }
        variant invoke(instance& object, argument& arg1, argument& arg2, argument& arg3) const
        {
            return method_accessor<F, Policy>::invoke(m_func_acc, object, arg1, arg2, arg3);
        }
        variant invoke(instance& object, argument& arg1, argument& arg2, argument& arg3, argument& arg4) const
        {
            return method_accessor<F, Policy>::invoke(m_func_acc, object, arg1, arg2, arg3, arg4);
        }
        variant invoke(instance& object, argument& arg1, argument& arg2, argument& arg3, argument& arg4, argument& arg5) const
        {
            return method_accessor<F, Policy>::invoke(m_func_acc, object, arg1, arg2, arg3, arg4, arg5);
        }
        variant invoke(instance& object, argument& arg1, argument& arg2, argument& arg3, argument& arg4, argument& arg5, argument& arg6) const
        {
            return method_accessor<F, Policy>::invoke(m_func_acc, object, arg1, arg2, arg3, arg4, arg5, arg6);
        }

        variant invoke_variadic(const instance& object, std::vector<argument>& args) const
        {
            return method_accessor<F, Policy>::invoke_variadic(m_func_acc, object, args);
        }

    private:
        F  m_func_acc;
};

/////////////////////////////////////////////////////////////////////////////////////////

template<typename F, access_levels Acc_Level, typename Policy, typename...Default_Args, std::size_t Metadata_Count>
class method_wrapper<F, Acc_Level, Policy, default_args<Default_Args...>, parameter_infos<>, Metadata_Count> : public method_wrapper_base, public metadata_handler<Metadata_Count>
{
    using method_type = typename detail::method_type<F>::type;
    using arg_index_sequence = make_index_sequence<function_traits<F>::arg_count>;
    using invoker_class = method_invoker<F, Policy, method_type, arg_index_sequence>;
    using invoke_with_defaults = invoke_defaults_helper<invoker_class, F>;

    public:
        method_wrapper(string_view name,
                       type declaring_type,
                       F func_acc,
                       std::array<metadata, Metadata_Count> metadata_list,
                       default_args<Default_Args...> default_args,
                       parameter_infos<> param_infos)
        :   method_wrapper_base(name, declaring_type),
            metadata_handler<Metadata_Count>(std::move(metadata_list)),
            m_func_acc(func_acc),
            m_def_args(std::move(default_args))
        {
            init();
        }

        bool is_valid()                                     const RTTR_NOEXCEPT { return true;                                            }
        bool is_static()                                    const RTTR_NOEXCEPT { return method_accessor<F, Policy>::is_static();         }
        type get_return_type()                              const RTTR_NOEXCEPT { return method_accessor<F, Policy>::get_return_type();   }
        std::vector<bool> get_is_reference()                const RTTR_NOEXCEPT { return method_accessor<F, Policy>::get_is_reference();  }
        std::vector<bool> get_is_const()                    const RTTR_NOEXCEPT { return method_accessor<F, Policy>::get_is_const();      }
        access_levels get_access_level()                    const RTTR_NOEXCEPT { return Acc_Level;                                       }
        array_range<parameter_info> get_parameter_infos()   const RTTR_NOEXCEPT { return array_range<parameter_info>();                   }
        variant get_metadata(const variant& key)            const { return metadata_handler<Metadata_Count>::get_metadata(key); }

        variant invoke(instance& object) const
        {
            return invoke_with_defaults::invoke(m_func_acc, object, m_def_args.m_args);
        }
        variant invoke(instance& object, argument& arg1) const
        {
            return invoke_with_defaults::invoke(m_func_acc, object, m_def_args.m_args, arg1);
        }
        variant invoke(instance& object, argument& arg1, argument& arg2) const
        {
            return invoke_with_defaults::invoke(m_func_acc, object, m_def_args.m_args, arg1, arg2);
        }
        variant invoke(instance& object, argument& arg1, argument& arg2, argument& arg3) const
        {
            return invoke_with_defaults::invoke(m_func_acc, object, m_def_args.m_args, arg1, arg2, arg3);
        }
        variant invoke(instance& object, argument& arg1, argument& arg2, argument& arg3, argument& arg4) const
        {
            return invoke_with_defaults::invoke(m_func_acc, object, m_def_args.m_args, arg1, arg2, arg3, arg4);
        }
        variant invoke(instance& object, argument& arg1, argument& arg2, argument& arg3, argument& arg4, argument& arg5) const
        {
            return invoke_with_defaults::invoke(m_func_acc, object, m_def_args.m_args, arg1, arg2, arg3, arg4, arg5);
        }
        variant invoke(instance& object, argument& arg1, argument& arg2, argument& arg3, argument& arg4, argument& arg5, argument& arg6) const
        {
            return invoke_with_defaults::invoke(m_func_acc, object, m_def_args.m_args, arg1, arg2, arg3, arg4, arg5, arg6);
        }

        variant invoke_variadic(const instance& object, std::vector<argument>& args) const
        {
            if (args.size() <= function_traits<F>::arg_count)
                return invoke_variadic_helper<invoke_with_defaults, arg_index_sequence>::invoke(args, m_func_acc, object, m_def_args.m_args);
            else
                return variant();
        }

    private:
        F                               m_func_acc;
        default_args<Default_Args...>   m_def_args;
};

/////////////////////////////////////////////////////////////////////////////////////////

} // end namespace detail
} // end namespace rttr

#endif // RTTR_METHOD_WRAPPER_H_
