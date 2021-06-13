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

#ifndef RTTR_PARAMETER_INFOS_H_
#define RTTR_PARAMETER_INFOS_H_

#include "rttr/detail/base/core_prerequisites.h"
#include "rttr/detail/parameter_info/parameter_info_wrapper.h"
#include "rttr/detail/default_arguments/default_arguments.h"
#include "rttr/parameter_info.h"

namespace rttr
{
namespace detail
{

/////////////////////////////////////////////////////////////////////////////////////////

template<typename...T>
struct parameter_infos
{
    parameter_infos() {}
    parameter_infos(T&&... args) : m_param_infos(std::forward<T>(args)...) {}

    std::tuple<T...> m_param_infos;

    RTTR_STATIC_CONSTEXPR std::size_t count = sizeof...(T);
};

template<>
struct parameter_infos<>
{
    parameter_infos() {}

    std::tuple<> m_param_infos;

    RTTR_STATIC_CONSTEXPR std::size_t count = 0;
};

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

template<typename...Args>
using has_param_names = std::integral_constant<bool, !std::is_same<null_type, find_if_t<is_parameter_names, Args...>>::value>;

template<typename...TArgs>
using count_param_names = count_if<is_parameter_names, raw_type_t<TArgs>... >;


template<typename Args, typename Enable = void>
struct param_names_count_impl;

template<typename...Args>
struct param_names_count_impl<type_list<Args...>, enable_if_t<has_param_names<Args...>::value>>
{
    RTTR_STATIC_CONSTEXPR std::size_t count = find_if_t<is_parameter_names, Args...>::count;
};

template<typename...Args>
struct param_names_count_impl<type_list<Args...>, enable_if_t<!has_param_names<Args...>::value>>
{
    RTTR_STATIC_CONSTEXPR std::size_t count = 0;
};

template<typename...Args>
using param_names_count = std::integral_constant<std::size_t, param_names_count_impl<type_list<Args...>>::count>;

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
static RTTR_INLINE
parameter_info create_param_info(const T& data)
{
    return parameter_info(&data);
}

template<std::size_t... Indices, typename...T>
static RTTR_INLINE std::array<parameter_info, sizeof...(T)>
create_paramter_info_array_impl(index_sequence<Indices...>, const parameter_infos<T...>& param_infos)
 {
    return { {create_param_info(std::get<Indices>(param_infos.m_param_infos))...} };
 };

// MSVC 2015 cannot handle sizeof...(),
// I retrieve a fatal error C1001: An internal error has occurred in the compiler.
// (compiler file 'msc1.cpp', line 1421)
// MSVC 2013 can handle it...
template<typename...T, std::size_t Size = sizeof...(T)>
static RTTR_INLINE std::array<parameter_info, Size>
create_paramter_info_array(const parameter_infos<T...>& param_infos)
 {
     return create_paramter_info_array_impl(make_index_sequence<Size>(), param_infos);
 };

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

template<typename F, typename Has_Name, typename Default_Type_List, typename Indices>
struct param_info_creater_func_impl;

template<typename F, typename Has_Name, typename Default_Type_List, std::size_t... Arg_Count>
struct param_info_creater_func_impl<F, Has_Name, Default_Type_List, index_sequence<Arg_Count...>>
{
    using type = parameter_infos< parameter_info_wrapper<param_types_t<F, Arg_Count>, Arg_Count, Has_Name,
                                                         type_list_element_t<Arg_Count, Default_Type_List> >...>;
};

/////////////////////////////////////////////////////////////////////////////////////////

template<typename Ctor_Args_List, typename Has_Name, typename Default_Type_List, typename Indices>
struct param_info_creater_ctor_impl;

template<typename Ctor_Args_List, typename Has_Name, typename Default_Type_List, std::size_t... Arg_Count>
struct param_info_creater_ctor_impl<Ctor_Args_List, Has_Name, Default_Type_List, index_sequence<Arg_Count...>>
{
    using type = parameter_infos< parameter_info_wrapper<type_list_element_t<Arg_Count, Ctor_Args_List>, Arg_Count, Has_Name,
                                                         type_list_element_t<Arg_Count, Default_Type_List> >...>;
};

/////////////////////////////////////////////////////////////////////////////////////////

template<typename T, typename Acc_Type, typename Has_Name, typename Default_Type_List, typename Enable = void>
struct param_info_creater;

// for functions
template<typename F, typename Acc_Type, typename Has_Name, typename...Def_Args>
struct param_info_creater<type_list<F>, Acc_Type, Has_Name, type_list<Def_Args...>, enable_if_t< std::is_same<Acc_Type, function_type>::value >>
{
    using new_default_list = push_front_n_t<void, type_list<Def_Args...>, function_traits<F>::arg_count - sizeof...(Def_Args)>;
    using idx_seq = make_index_sequence< function_traits<F>::arg_count>;
    using type = typename param_info_creater_func_impl<F, Has_Name, new_default_list, idx_seq>::type;
};

// ctor with one argument
template<typename T, typename Has_Name, typename...Def_Args>
struct param_info_creater<type_list<T>, constructor_type, Has_Name, type_list<Def_Args...>, enable_if_t< is_one_argument<T>::value >>
{
    static_assert(sizeof...(Def_Args) < 2, "Invalid 'Def_Args' size.");
    using new_default_list = push_front_n_t<void, type_list<Def_Args...>, 1 - sizeof...(Def_Args)>;
    using idx_seq = make_index_sequence<1>;
    using type = typename param_info_creater_ctor_impl<type_list<T>, Has_Name, new_default_list, idx_seq>::type;
};

// ctor with zero or more then one argument
template<typename Has_Name, typename...Ctor_Args, typename...Def_Args>
struct param_info_creater<type_list<Ctor_Args...>, constructor_type, Has_Name, type_list<Def_Args...>, enable_if_t< !is_one_argument<Ctor_Args...>::value >>
{
    static_assert(sizeof...(Ctor_Args) >= sizeof...(Def_Args), "Invalid 'Def_Args' size.");
    using new_default_list = push_front_n_t<void, type_list<Def_Args...>, sizeof...(Ctor_Args) - sizeof...(Def_Args)>;
    using idx_seq = make_index_sequence< sizeof...(Ctor_Args) >;
    using type = typename param_info_creater_ctor_impl<type_list<Ctor_Args...>, Has_Name, new_default_list, idx_seq>::type;
};


template<typename Acc_Args, typename Acc_Type, typename Has_Name, typename Def_Args>
using param_info_creater_t = typename param_info_creater<Acc_Args, Acc_Type, Has_Name, Def_Args>::type;

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////


template<typename Acc_Args, typename Acc_Type, typename Def_List, typename Param_Names, std::size_t... Indices>
static RTTR_INLINE
param_info_creater_t<Acc_Args, Acc_Type, has_param_name, Def_List>
create_param_infos_and_store_names(index_sequence<Indices...>, Param_Names& names)
{
    return param_info_creater_t<Acc_Args, Acc_Type, has_param_name, Def_List>{string_view(std::get<Indices>(names.m_param_names))...};
}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

template<typename Acc_Args, typename Acc_Type, typename...TArgs, typename T_Def = as_type_list_t<typename find_default_args_t<get_default_args_t<TArgs...>, Acc_Args, Acc_Type>::default_types_func>>
static RTTR_INLINE
enable_if_t< !has_param_names<TArgs...>::value && has_default_types<Acc_Args, type_list<TArgs...>, Acc_Type>::value, param_info_creater_t<Acc_Args, Acc_Type, no_param_name, T_Def>>
create_param_infos(TArgs&&... args)
{
    return param_info_creater_t<Acc_Args, Acc_Type, no_param_name, T_Def>();
}

/////////////////////////////////////////////////////////////////////////////////////////

template<typename Acc_Args, typename Acc_Type, typename...TArgs>
static RTTR_INLINE
enable_if_t< !has_param_names<TArgs...>::value && !has_default_types<Acc_Args, type_list<TArgs...>, Acc_Type>::value, param_info_creater_t<Acc_Args, Acc_Type, no_param_name, type_list<> >>
create_param_infos(TArgs&&... args)
{
    return param_info_creater_t<Acc_Args, Acc_Type, no_param_name, type_list<> >();
}

/////////////////////////////////////////////////////////////////////////////////////////

template<typename Acc_Args, typename Acc_Type, typename...TArgs, typename T_Def = as_type_list_t<typename find_default_args_t<get_default_args_t<TArgs...>, Acc_Args, Acc_Type>::default_types_func>>
static RTTR_INLINE
enable_if_t< has_param_names<TArgs...>::value && has_default_types<Acc_Args, type_list<TArgs...>, Acc_Type>::value, param_info_creater_t<Acc_Args, Acc_Type, has_param_name, T_Def>>
create_param_infos(TArgs&&... args)
{
    using Param_Type = find_if_t<is_parameter_names, TArgs...>;
    auto result = forward_to_array<Param_Type>(std::forward<TArgs>(args)...);
    return create_param_infos_and_store_names<Acc_Args, Acc_Type, T_Def>(make_index_sequence<Param_Type::count>(), result[0]);
}

/////////////////////////////////////////////////////////////////////////////////////////

template<typename Acc_Args, typename Acc_Type, typename...TArgs>
static RTTR_INLINE
enable_if_t< has_param_names<TArgs...>::value && !has_default_types<Acc_Args, type_list<TArgs...>, Acc_Type>::value, param_info_creater_t<Acc_Args, Acc_Type, has_param_name, type_list<> >>
create_param_infos(TArgs&&... args)
{
    using Param_Type = find_if_t<is_parameter_names, TArgs...>;
    auto result = forward_to_array<Param_Type>(std::forward<TArgs>(args)...);
    return create_param_infos_and_store_names<Acc_Args, Acc_Type, type_list<>>(make_index_sequence<Param_Type::count>(), result[0]);
}


/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

template<typename Param_Arg, typename Default_Args>
static RTTR_INLINE void store_default_value(Param_Arg& param_info, const Default_Args& def_arg)
{
    param_info.set_default_value(&def_arg);
}

/////////////////////////////////////////////////////////////////////////////////////////

template<typename Indices_Defaults, typename Indices_Params>
struct store_default_args_helper;

template<std::size_t D_Index, std::size_t...D_Indices, std::size_t P_Index, std::size_t...P_Indices>
struct store_default_args_helper<index_sequence<D_Index, D_Indices...>, index_sequence<P_Index, P_Indices...>>
{
    template<typename...Default_Args, typename...Param_Args>
    static RTTR_INLINE
    void perform(parameter_infos<Param_Args...>& param_infos, const default_args<Default_Args...>& def_args)
    {
        store_default_value(std::get<P_Index>(param_infos.m_param_infos), std::get<D_Index>(def_args.m_args));

        store_default_args_helper<index_sequence<D_Indices...>, index_sequence<P_Indices...>>::perform(param_infos, def_args);
    }
};

template<>
struct store_default_args_helper<index_sequence<>, index_sequence<>>
{
    template<typename...Default_Args, typename...Param_Args>
    static RTTR_INLINE
    void perform(parameter_infos<Param_Args...>& param_infos, const default_args<Default_Args...>& def_args)
    {
    }
};

/////////////////////////////////////////////////////////////////////////////////////////

template<typename...Default_Args, typename...Param_Args>
static RTTR_INLINE void store_default_args_in_param_infos(parameter_infos<Param_Args...>& param_infos, const default_args<Default_Args...>& def_args)
{
    static_assert((sizeof...(Default_Args) <= sizeof...(Param_Args)), "Invalid size of default arguments!");

    using param_idx_seq = typename erase_sequence_till<make_index_sequence<sizeof...(Param_Args)>, sizeof...(Param_Args) - sizeof...(Default_Args)>::type;
    store_default_args_helper<make_index_sequence<sizeof...(Default_Args)>, param_idx_seq>::perform(param_infos, def_args);
}

/////////////////////////////////////////////////////////////////////////////////////////

} // end namespace detail
} // end namespace rttr

#endif // RTTR_PARAMETER_INFOS_H_
