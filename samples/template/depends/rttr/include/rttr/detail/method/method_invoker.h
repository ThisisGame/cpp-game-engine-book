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

#ifndef RTTR_METHOD_INVOKER_H_
#define RTTR_METHOD_INVOKER_H_

#include "rttr/detail/misc/function_traits.h"
#include "rttr/detail/misc/utility.h"

namespace rttr
{
namespace detail
{

/////////////////////////////////////////////////////////////////////////////////////////

template<typename F, typename Policy, typename Method_Type, typename IndexSequence>
struct method_invoker;

template<typename F, std::size_t... ArgCount>
struct method_invoker<F, default_invoke, void_member_func, index_sequence<ArgCount...>>
{
    template<typename... TArgs>
    RTTR_INLINE static variant invoke(const F& func_ptr, const instance& obj, const TArgs&...args)
    {
        using class_t = typename function_traits<F>::class_type;
        class_t* ptr = obj.try_convert<class_t>();
        if (ptr && check_all_true(args.template is_type< param_types_t<F, ArgCount> >()...))
        {
            (ptr->*func_ptr)(args.template get_value< param_types_t<F, ArgCount> >()...);
            return void_variant_type{};
        }
        else
            return variant();
    }
};

/////////////////////////////////////////////////////////////////////////////////////////

template<typename F, std::size_t... ArgCount>
struct method_invoker<F, default_invoke, void_func, index_sequence<ArgCount...>>
{
    template<typename... TArgs>
    RTTR_INLINE static variant invoke(const F& func, const instance& obj, const TArgs&...args)
    {
        if (check_all_true(args.template is_type< param_types_t<F, ArgCount> >()...))
        {
            func(args.template get_value< param_types_t<F, ArgCount> >()...);
            return void_variant_type{};
        }
        else
            return variant();
    }
};

/////////////////////////////////////////////////////////////////////////////////////////

template<typename F, std::size_t... ArgCount>
struct method_invoker<F, default_invoke, return_member_func, index_sequence<ArgCount...>>
{
    template<typename... TArgs>
    RTTR_INLINE static variant invoke(const F& func_ptr, const instance& obj, const TArgs&...args)
    {
        using class_t = typename function_traits<F>::class_type;
        class_t* ptr = obj.try_convert<class_t>();
        if (ptr && check_all_true(args.template is_type< param_types_t<F, ArgCount> >()...))
            return (ptr->*func_ptr)(args.template get_value< param_types_t<F, ArgCount> >()...);
        else
            return variant();
    }
};

/////////////////////////////////////////////////////////////////////////////////////////

template<typename F, std::size_t... ArgCount>
struct method_invoker<F, default_invoke, return_func, index_sequence<ArgCount...>>
{
    template<typename... TArgs>
    RTTR_INLINE static variant invoke(const F& func, const instance& obj, const TArgs&...args)
    {
        if (check_all_true(args.template is_type< param_types_t<F, ArgCount> >()...))
            return func(args.template get_value< param_types_t<F, ArgCount> >()...);
        else
            return variant();
    }
};

/////////////////////////////////////////////////////////////////////////////////////////

template<typename F, std::size_t... ArgCount>
struct method_invoker<F, discard_return, return_member_func, index_sequence<ArgCount...>>
{
    template<typename... TArgs>
    RTTR_INLINE static variant invoke(const F& func_ptr, const instance& obj, const TArgs&...args)
    {
        using class_t = typename function_traits<F>::class_type;
        class_t* ptr = obj.try_convert<class_t>();
        if (ptr && check_all_true(args.template is_type< param_types_t<F, ArgCount> >()...))
        {
            (ptr->*func_ptr)(args.template get_value< param_types_t<F, ArgCount> >()...);
            return void_variant_type{};
        }
        else
            return variant();
    }
};

/////////////////////////////////////////////////////////////////////////////////////////

template<typename F, std::size_t... ArgCount>
struct method_invoker<F, discard_return, return_func, index_sequence<ArgCount...>>
{
    template<typename... TArgs>
    RTTR_INLINE static variant invoke(const F& func, const instance& obj, const TArgs&...args)
    {
        if (check_all_true(args.template is_type< param_types_t<F, ArgCount> >()...))
        {
            func(args.template get_value< param_types_t<F, ArgCount> >()...);
            return void_variant_type{};
        }
        else
            return variant();
    }
};

/////////////////////////////////////////////////////////////////////////////////////////

template<typename F, std::size_t... ArgCount>
struct method_invoker<F, return_as_ptr, return_member_func, index_sequence<ArgCount...>>
{
    template<typename... TArgs>
    RTTR_INLINE static variant invoke(const F& func_ptr, const instance& obj, const TArgs&...args)
    {
        using class_t = typename function_traits<F>::class_type;
        class_t* ptr = obj.try_convert<class_t>();
        if (ptr && check_all_true(args.template is_type< param_types_t<F, ArgCount> >()...))
        {
            return &(ptr->*func_ptr)(args.template get_value< param_types_t<F, ArgCount> >()...);
        }
        else
            return variant();
    }
};

/////////////////////////////////////////////////////////////////////////////////////////

template<typename F, std::size_t... ArgCount>
struct method_invoker<F, return_as_ptr, return_func, index_sequence<ArgCount...>>
{
    template<typename... TArgs>
    RTTR_INLINE static variant invoke(const F& func, const instance& obj, const TArgs&...args)
    {
        if (check_all_true(args.template is_type< param_types_t<F, ArgCount> >()...))
        {
            return &func(args.template get_value< param_types_t<F, ArgCount> >()...);
        }
        else
            return variant();
    }
};

/////////////////////////////////////////////////////////////////////////////////////////

} // end namespace detail
} // end namespace rttr

#endif // RTTR_METHOD_INVOKER_H_
