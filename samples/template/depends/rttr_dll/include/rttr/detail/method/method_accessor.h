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

#ifndef RTTR_METHOD_ACCESSOR_H_
#define RTTR_METHOD_ACCESSOR_H_

#include "rttr/detail/misc/function_traits.h"
#include "rttr/detail/misc/utility.h"
#include "rttr/detail/method/method_invoker.h"

namespace rttr
{
namespace detail
{

/////////////////////////////////////////////////////////////////////////////////////////

template<typename F, typename IndexSequence, typename B>
struct method_accessor_impl;

template<typename F, std::size_t... ArgCount>
struct method_accessor_impl<F, index_sequence<ArgCount...>, std::true_type>
{
    static std::vector<bool> get_is_reference() RTTR_NOEXCEPT
    {
        return { std::is_reference<typename param_types<F, ArgCount>::type>::value... };
    }

    static std::vector<bool> get_is_const() RTTR_NOEXCEPT
    {
        return { std::is_const<typename std::remove_reference<typename param_types<F, ArgCount>::type>::type>::value... };
    }
};

/////////////////////////////////////////////////////////////////////////////////////////

template<typename F, std::size_t... ArgCount>
struct method_accessor_impl<F, index_sequence<ArgCount...>, std::false_type>
{
    static std::vector<bool> get_is_reference() RTTR_NOEXCEPT
    {
        return std::vector<bool>();
    }

    static std::vector<bool> get_is_const() RTTR_NOEXCEPT
    {
        return std::vector<bool>();
    }
};

/////////////////////////////////////////////////////////////////////////////////////////

template<typename MethodType>
struct method_accessor_helper_is_static
{
    static bool is_static() RTTR_NOEXCEPT { return true; }
};

template<>
struct method_accessor_helper_is_static<return_member_func>
{
    static bool is_static() RTTR_NOEXCEPT { return false; }
};

template<>
struct method_accessor_helper_is_static<void_member_func>
{
    static bool is_static() RTTR_NOEXCEPT { return false; }
};

/////////////////////////////////////////////////////////////////////////////////////////

template<typename F, typename Policy>
struct method_accessor_helper_return_type
{
    static type get_return_type() RTTR_NOEXCEPT { return type::get<typename function_traits<F>::return_type>(); }
};

template<typename F>
struct method_accessor_helper_return_type<F, return_as_ptr>
{
    using return_type = typename function_traits<F>::return_type;
    static type get_return_type() RTTR_NOEXCEPT { return type::get<typename std::remove_reference<return_type>::type*>(); }
};

template<typename F>
struct method_accessor_helper_return_type<F, discard_return>
{
    static type get_return_type() RTTR_NOEXCEPT { return type::get<void>(); }
};

/////////////////////////////////////////////////////////////////////////////////////////
// this is a workaround because MSVC2013 cannot handle sizeof...(T) with enable_if
template<typename F, typename...TArgs>
struct does_argument_count_match : std::integral_constant<bool, (sizeof...(TArgs) == function_traits<F>::arg_count)>
{
};

/////////////////////////////////////////////////////////////////////////////////////////

template<typename F, typename Policy>
struct method_accessor
{
    static RTTR_CONSTEXPR_OR_CONST std::size_t arg_count = function_traits<F>::arg_count;
    using method_type = typename detail::method_type<F>::type;
    using arg_index_sequence = make_index_sequence< arg_count >;
    using invoker_class = method_invoker<F, Policy, method_type, arg_index_sequence>;

    static bool is_static() RTTR_NOEXCEPT
    {
        using method_type = typename detail::method_type<F>::type;
        return method_accessor_helper_is_static<method_type>::is_static();
    }

    /////////////////////////////////////////////////////////////////////////////////////

    static type get_return_type() RTTR_NOEXCEPT
    {
        return method_accessor_helper_return_type<F, Policy>::get_return_type();
    }

    /////////////////////////////////////////////////////////////////////////////////////

    static std::vector<bool> get_is_reference() RTTR_NOEXCEPT
    {
        using has_arguments         = typename std::integral_constant<bool, arg_count != 0>::type;
        return method_accessor_impl<F, make_index_sequence<arg_count>, has_arguments>::get_is_reference();
    }

    /////////////////////////////////////////////////////////////////////////////////////

    static std::vector<bool> get_is_const() RTTR_NOEXCEPT
    {
        using has_arguments         = typename std::integral_constant<bool, arg_count != 0>::type;
        return method_accessor_impl<F, make_index_sequence<arg_count>, has_arguments>::get_is_const();
    }

    /////////////////////////////////////////////////////////////////////////////////////

    template<typename... TArgs>
    static RTTR_INLINE
    enable_if_t< does_argument_count_match<F, TArgs...>::value, variant>
    invoke(const F& func_ptr, const instance& obj, const TArgs&...args)
    {
        return invoker_class::invoke(func_ptr, obj, args...);
    }

    /////////////////////////////////////////////////////////////////////////////////////

    template<typename... TArgs>
    static RTTR_INLINE
    enable_if_t< !does_argument_count_match<F, TArgs...>::value, variant>
    invoke(const F& func_ptr, const instance& obj, const TArgs&...args)
    {
        return variant();
    }

    /////////////////////////////////////////////////////////////////////////////////////

    template<std::size_t... Arg_Idx>
    static RTTR_INLINE variant invoke_variadic_impl(const F& func_ptr, const instance& obj, index_sequence<Arg_Idx...>, const std::vector<argument>& arg_list)
    {
        return invoker_class::invoke(func_ptr, obj, arg_list[Arg_Idx]...);
    }

    /////////////////////////////////////////////////////////////////////////////////////

    static RTTR_INLINE variant invoke_variadic(const F& func_ptr, const instance& obj, const std::vector<argument>& arg_list)
    {
        if (arg_list.size() == arg_count)
            return invoke_variadic_impl(func_ptr, obj, make_index_sequence<arg_count>(), arg_list);
        else
            return variant();
    }
};

/////////////////////////////////////////////////////////////////////////////////////////

} // end namespace detail
} // end namespace rttr

#endif // RTTR_METHOD_ACCESSOR_H_
