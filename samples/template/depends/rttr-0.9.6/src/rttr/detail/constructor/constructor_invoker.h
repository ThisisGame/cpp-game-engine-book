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

#ifndef RTTR_CONSTRUCTOR_INVOKER_H_
#define RTTR_CONSTRUCTOR_INVOKER_H_

#include "rttr/detail/misc/function_traits.h"
#include "rttr/detail/misc/utility.h"
#include "rttr/detail/policies/ctor_policies.h"

namespace rttr
{
namespace detail
{

struct ctor_type { };

struct ctor_func_type { };

/////////////////////////////////////////////////////////////////////////////////////////

template<typename Ctor_Type, typename Policy, typename Accessor, typename Arg_Indexer>
struct constructor_invoker;

/////////////////////////////////////////////////////////////////////////////////////////

template<typename Class_Type, typename...Ctor_Args, std::size_t... Arg_Count>
struct constructor_invoker<ctor_type, as_raw_pointer, type_list<Class_Type, Ctor_Args...>, index_sequence<Arg_Count...>>
{
    using return_type = add_pointer_t<Class_Type>;

    template<typename... TArgs>
    static RTTR_INLINE variant invoke(TArgs&&...args)
    {
        if (check_all_true(args. template is_type<Ctor_Args>()...))
            return variant(new Class_Type(args. template get_value<Ctor_Args>()...));
        else
            return variant();
    }
};

/////////////////////////////////////////////////////////////////////////////////////////

template<typename Class_Type, typename...Ctor_Args, std::size_t... Arg_Count>
struct constructor_invoker<ctor_type, as_object, type_list<Class_Type, Ctor_Args...>, index_sequence<Arg_Count...>>
{
    using return_type = Class_Type;

    template<typename... TArgs>
    static RTTR_INLINE variant invoke(TArgs&&...args)
    {
        if (check_all_true(args. template is_type<Ctor_Args>()...))
            return variant(Class_Type(args. template get_value<Ctor_Args>()...));
        else
            return variant();
    }
};

/////////////////////////////////////////////////////////////////////////////////////////

template<typename Class_Type, typename...Ctor_Args, std::size_t... Arg_Count>
struct constructor_invoker<ctor_type, as_std_shared_ptr, type_list<Class_Type, Ctor_Args...>, index_sequence<Arg_Count...>>
{
    using return_type = std::shared_ptr<Class_Type>;

    template<typename... TArgs>
    static RTTR_INLINE variant invoke(TArgs&&...args)
    {
        // we cannot use std::make_shared<T> here because, otherwise we cannot instantiate, constructors which are declared as protected or private
        if (check_all_true(args. template is_type<Ctor_Args>()...))
            return variant(std::shared_ptr<Class_Type>(new Class_Type(args. template get_value<Ctor_Args>()...)));
        else
            return variant();
    }
};

/////////////////////////////////////////////////////////////////////////////////////////

} // end namespace detail
} // end namespace rttr

#endif // RTTR_CONSTRUCTOR_INVOKER_H_
