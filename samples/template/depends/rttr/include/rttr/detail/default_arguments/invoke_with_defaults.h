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

#ifndef RTTR_INVOKE_WITH_DEFAULTS_H_
#define RTTR_INVOKE_WITH_DEFAULTS_H_

#include "rttr/detail/base/core_prerequisites.h"
#include "rttr/detail/misc/misc_type_traits.h"
#include "rttr/detail/misc/argument_extractor.h"
#include "rttr/detail/misc/function_traits.h"

#include <type_traits>

namespace rttr
{
namespace detail
{

template<typename Ctor_Args, typename List1, typename List2>
struct are_args_and_defaults_in_valid_range;

template<typename... Ctor_Args, typename... TArgs, typename... Default_Args>
struct are_args_and_defaults_in_valid_range<type_list<Ctor_Args...>, type_list<TArgs...>, type_list<Default_Args...>>
:   std::integral_constant<bool, (sizeof...(TArgs) <= sizeof...(Ctor_Args)) && ( sizeof...(TArgs) + sizeof...(Default_Args) >= sizeof...(Ctor_Args)) >
{
};

template<typename F, typename...TArgs, typename...Default_Args>
struct are_args_and_defaults_in_valid_range<F, type_list<TArgs...>, type_list<Default_Args...>>
:   are_args_and_defaults_in_valid_range<as_type_list_t<typename function_traits<F>::arg_types>, type_list<TArgs...>, type_list<Default_Args...>>
{
};

/////////////////////////////////////////////////////////////////////////////////////////

/*!
 * A helper class to invoke functions objects with default arguments,
 * via static method `Invoker_Class::invoke`
 */
template<typename Invoker_Class, typename F>
struct invoke_defaults_helper
{
private:
    template<std::size_t... Def_Idx, typename... Def_Types, typename... TArgs>
    static RTTR_INLINE variant
    invoke_with_defaults_helper(const F& func_ptr, const instance& obj, index_sequence<Def_Idx...>,
                                const std::tuple<Def_Types...>& def_args, const TArgs&...args)
    {
        return Invoker_Class::invoke(func_ptr, obj, args..., argument(std::get<Def_Idx>(def_args))...);
    }


public:
    template<typename... Def_Types, typename... TArgs>
    static RTTR_INLINE
    enable_if_t< are_args_and_defaults_in_valid_range<F, type_list<TArgs...>, type_list<Def_Types...>>::value, variant>
    invoke(const F& func_ptr, const instance& obj, const std::tuple<Def_Types...>& def_args, const TArgs&...args)
    {
        static RTTR_CONSTEXPR_OR_CONST std::size_t arg_count = function_traits<F>::arg_count;
        // here we calculate the integer sequence for retrieving the data from the tuple,
        // this depends on the number of arguments provided by the caller
        static RTTR_CONSTEXPR_OR_CONST std::size_t start_index = sizeof...(TArgs) + sizeof...(Def_Types) - arg_count;
        using idx_seq = typename erase_sequence_till<index_sequence_for<Def_Types...>, start_index>::type;
        return invoke_with_defaults_helper(func_ptr, obj, idx_seq(), def_args, args...);
    }

    /////////////////////////////////////////////////////////////////////////////////////

    template<typename... Def_Types, typename... TArgs>
    static RTTR_INLINE
    enable_if_t< !are_args_and_defaults_in_valid_range<F, type_list<TArgs...>, type_list<Def_Types...>>::value, variant>
    invoke(const F& func_ptr, const instance& obj, const std::tuple<Def_Types...>& def_args, const TArgs&...args)
    {
        return variant();
    }
};

/////////////////////////////////////////////////////////////////////////////////////////

/*!
 * A helper class to invoke a constructor signature with default arguments,
 * via static method `Invoker_Class::invoke`
 */
template<typename Invoker_Class, typename... Ctor_Args>
struct invoke_defaults_helper<Invoker_Class, type_list<Ctor_Args...>>
{
private:
    template<std::size_t... Def_Idx, typename... Def_Types, typename... TArgs>
    static RTTR_INLINE variant
    invoke_with_defaults_extract(index_sequence<Def_Idx...>, const std::tuple<Def_Types...>& def_args, const TArgs&...args)
    {
        return Invoker_Class::invoke(args..., argument(std::get<Def_Idx>(def_args))...);
    }

public:
    template<typename... Def_Types, typename... TArgs>
    static RTTR_INLINE
    enable_if_t< are_args_and_defaults_in_valid_range<type_list<Ctor_Args...>, type_list<TArgs...>, type_list<Def_Types...>>::value, variant>
    invoke(const std::tuple<Def_Types...>& def_args, const TArgs&...args)
    {
        static RTTR_CONSTEXPR_OR_CONST std::size_t arg_count = sizeof...(Ctor_Args);
        // here we calculate the integer sequence for retrieving the data from the tuple,
        // this depends on the number of arguments provided by the caller
        static RTTR_CONSTEXPR_OR_CONST std::size_t start_index = sizeof...(TArgs) + sizeof...(Def_Types) - arg_count;
        using idx_seq = typename erase_sequence_till<index_sequence_for<Def_Types...>, start_index>::type;
        return invoke_with_defaults_extract(idx_seq(), def_args, args...);
    }

    /////////////////////////////////////////////////////////////////////////////////////

    template<typename... Def_Types, typename... TArgs>
    static RTTR_INLINE
    enable_if_t< !are_args_and_defaults_in_valid_range<type_list<Ctor_Args...>, type_list<TArgs...>, type_list<Def_Types...>>::value, variant>
    invoke(const std::tuple<Def_Types...>& def_args, const TArgs&...args)
    {
        return variant();
    }
};

/////////////////////////////////////////////////////////////////////////////////////////

template<typename Invoker_Class, typename Arg_Indexer>
struct invoke_variadic_helper;

/*!
 * Generic class to call the static method 'Invoker_Class::invoke',
 * with the correct argument list 'size()'; this can only be decided at runtime.
 */
template<typename Invoker_Class, std::size_t... Arg_Idx>
struct invoke_variadic_helper<Invoker_Class, index_sequence<Arg_Idx...>>
{
    template<typename... Args>
    static RTTR_INLINE variant invoke(std::vector<argument>& arg_list, Args&&...args)
    {
        static RTTR_CONSTEXPR_OR_CONST std::size_t Arg_Count = sizeof...(Arg_Idx);
        if (arg_list.size() == Arg_Count)
            return Invoker_Class::invoke(args..., arg_list[Arg_Idx]...);
        else
            return invoke_variadic_helper<Invoker_Class, make_index_sequence<Arg_Count - 1>>::invoke(arg_list, args...);
    }
};

template<typename Invoker_Class>
struct invoke_variadic_helper<Invoker_Class, index_sequence<>>
{
    template<typename...Args>
    static RTTR_INLINE variant invoke(std::vector<argument>& arg_list, Args&&...args)
    {
        return Invoker_Class::invoke(args...);
    }
};

/////////////////////////////////////////////////////////////////////////////////////////

} // end namespace detail
} // end namespace rttr

#endif // RTTR_DEFAULT_ARGUMENTS_H_
