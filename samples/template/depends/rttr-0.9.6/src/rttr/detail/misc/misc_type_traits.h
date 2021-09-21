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

#ifndef RTTR_MISC_TYPE_TRAITS_H_
#define RTTR_MISC_TYPE_TRAITS_H_

#include "rttr/detail/base/core_prerequisites.h"

#include "rttr/detail/misc/function_traits.h"

#include "rttr/detail/misc/std_type_traits.h"


#include <type_traits>
#include <memory>

namespace rttr
{
class type;

template<typename T>
struct associative_container_mapper;
template<typename T>
struct sequential_container_mapper;

namespace detail
{
    struct derived_info;
    struct invalid_type;

    /////////////////////////////////////////////////////////////////////////////////////////
    // This trait will removes cv-qualifiers, pointers and reference from type T.
    template<typename T, typename Enable = void>
    struct raw_type
    {
        using type = detail::remove_cv_t<T>;
    };

    template<typename T> struct raw_type<T, enable_if_t<std::is_pointer<T>::value && !detail::is_function_ptr<T>::value>>
    {
        using type = typename raw_type< detail::remove_pointer_t<T>>::type;
    };

    template<typename T> struct raw_type<T, enable_if_t<std::is_reference<T>::value> >
    {
        using type = typename raw_type< remove_reference_t<T> >::type;
    };

    template<typename T>
    using raw_type_t = typename raw_type<T>::type;

    /////////////////////////////////////////////////////////////////////////////////////////
    // this trait removes all pointers

    template<typename T, typename Enable = void>
    struct remove_pointers { using type = T; };
    template<typename T>
    struct remove_pointers<T, enable_if_t<std::is_pointer<T>::value>>
    {
        using type = typename remove_pointers<remove_pointer_t<T> >::type;
    };

    template<typename T>
    using remove_pointers_t = typename remove_pointers<T>::type;

    /////////////////////////////////////////////////////////////////////////////////////////
    // this trait removes all pointers except one

    template<typename T, typename Enable = void>
    struct remove_pointers_except_one { using type = T; };
    template<typename T>
    struct remove_pointers_except_one<T, enable_if_t<std::is_pointer<T>::value>>
    {
        using type = conditional_t< std::is_pointer< remove_pointer_t<T> >::value,
                                    typename remove_pointers_except_one<remove_pointer_t<T>>::type,
                                    T
                                  >;
    };

    template<typename T>
    using remove_pointers_except_one_t = typename remove_pointers_except_one<T>::type;

    /////////////////////////////////////////////////////////////////////////////////////////
    // this trait will remove the cv-qualifier, pointer types, reference type and also the array dimension

    template<typename T, typename Enable = void>
    struct raw_array_type { using type = raw_type_t<T>; };

    template<typename T>
    struct raw_array_type_impl;

    template<typename T, std::size_t N>
    struct raw_array_type_impl<T[N]> { using type = typename raw_array_type<T>::type; };

    template<typename T>
    struct raw_array_type<T, typename std::enable_if<std::is_array<T>::value>::type>
    {
         using type = typename raw_array_type_impl< remove_cv_t<T> >::type;
    };

    template<typename T>
    using raw_array_type_t = typename raw_array_type<T>::type;

    /////////////////////////////////////////////////////////////////////////////////////////

    template <bool... b> struct static_all_of;
    //specialization for type true, go continue recurse if the first argument is true
    template <bool... tail>
    struct static_all_of<true, tail...> : static_all_of<tail...> {};
    // end recursion if first argument is false
    template <bool... tail>
    struct static_all_of<false, tail...> : std::false_type {};

    // finish when no argument are left
    template <> struct static_all_of<> : std::true_type {};

    // use it like e.g.:
    // static_all_of<std::is_class<ClassType>::value...>::value

    /////////////////////////////////////////////////////////////////////////////////////////

    template <bool... b> struct static_any_of;

    template <bool... tail>
    struct static_any_of<true, tail...> : std::true_type {};

    template <bool... tail>
    struct static_any_of<false, tail...> : static_any_of<tail...> {};

    // finish when no argument are left
    template <> struct static_any_of<> : std::false_type {};

    /////////////////////////////////////////////////////////////////////////////////////////
    /*!
     * Determine if the given type \a T has the method
     * 'type get_type() const' declared.
     */
    template <typename T>
    class has_get_type_func_impl
    {
        typedef char YesType[1];
        typedef char NoType[2];

        template <typename U, rttr::type (U::*)() const>
        class check { };

        template <typename C>
        static YesType& f(check<C, &C::get_type>*);

        template <typename C>
        static NoType& f(...);

    public:
        static RTTR_CONSTEXPR_OR_CONST bool value = (sizeof(f<typename raw_type<T>::type>(0)) == sizeof(YesType));
    };

    /*!
     * If T has a member function 'type get_type() const;' then inherits from true_type, otherwise inherits from false_type.
     */
    template<class T, typename Enable = void>
    struct has_get_type_func : std::false_type
    {};

    template<class T>
    struct has_get_type_func<T, enable_if_t<has_get_type_func_impl<T>::value> > : std::true_type
    {};

    /////////////////////////////////////////////////////////////////////////////////

    /*!
     * Determine if the given type \a T has the method
     * 'type get_type() const' declared.
     */
    template <typename T>
    class has_get_ptr_func_impl
    {
        typedef char YesType[1];
        typedef char NoType[2];

        template <typename U, void* (U::*)()>
        class check { };

        template <typename C>
        static YesType& f(check<C, &C::get_ptr>*);

        template <typename C>
        static NoType& f(...);

    public:
        static RTTR_CONSTEXPR_OR_CONST bool value = (sizeof(f<typename raw_type<T>::type>(0)) == sizeof(YesType));
    };

    /*!
     * If T has a member function 'type get_type() const;' then inherits from true_type, otherwise inherits from false_type.
     */
    template<class T, typename Enable = void>
    struct has_get_ptr_func : std::false_type
    {};

    template<class T>
    struct has_get_ptr_func<T, enable_if_t<has_get_ptr_func_impl<T>::value> > : std::true_type
    {};

    /////////////////////////////////////////////////////////////////////////////////

    /*!
     * Determine if the given type \a T has the method
     * 'type get_type() const' declared.
     */
    template <typename T>
    class has_get_derived_info_func_impl
    {
        typedef char YesType[1];
        typedef char NoType[2];

        template <typename U, derived_info (U::*)()>
        class check { };

        template <typename C>
        static YesType& f(check<C, &C::get_derived_info>*);

        template <typename C>
        static NoType& f(...);

    public:
        static RTTR_CONSTEXPR_OR_CONST bool value = (sizeof(f<typename raw_type<T>::type>(0)) == sizeof(YesType));
    };

    /*!
     * If T has a member function 'type get_type() const;' then inherits from true_type, otherwise inherits from false_type.
     */
    template<class T, typename Enable = void>
    struct has_get_derived_info_func : std::false_type
    {};

    template<class T>
    struct has_get_derived_info_func<T, enable_if_t<has_get_derived_info_func_impl<T>::value> > : std::true_type
    {};

    /////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////

    template<typename T>
    struct get_ptr_impl
    {
        static RTTR_INLINE void* get(T& data)
        {
            return const_cast<void*>(reinterpret_cast<const void*>(&data));
        }
    };

    template<typename T>
    struct get_ptr_impl<T*>
    {
        static RTTR_INLINE void* get(T* data)
        {
            return get_ptr_impl<T>::get(*data);
        }
    };

    template<>
    struct get_ptr_impl<void*>
    {
        static RTTR_INLINE void* get(void* data)
        {
            return data;
        }
    };

    template<>
    struct get_ptr_impl<const void*>
    {
        static RTTR_INLINE void* get(const void* data)
        {
            return const_cast<void*>(data);
        }
    };

    template<typename T>
    static RTTR_INLINE void* get_void_ptr(T* data)
    {
        return get_ptr_impl<T*>::get(data);
    }

    template<typename T>
    static RTTR_INLINE void* get_void_ptr(T& data)
    {
        return get_ptr_impl<T>::get(data);
    }

    /////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////
    // A simple list for types
    // use it like this:
    // using my_list = type_list<int, bool>;

    template<typename... T>
    struct type_list {};

    template<typename...T>
    struct as_type_list
    {
        using type = type_list<T...>;
    };

    template<template<class...> class Type_List, typename...Ts>
    struct as_type_list<Type_List<Ts...>>
    {
        using type = type_list<Ts...>;
    };

    template<typename...T>
    using as_type_list_t = typename as_type_list<T...>::type;

    /////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////

    template< std::size_t I, typename T >
    struct type_list_element;

    template< std::size_t I, typename Head, typename... Tail >
    struct type_list_element<I, type_list<Head, Tail...>> : type_list_element<I - 1, type_list<Tail...>>
    {
    };

    template< typename Head, typename... Tail >
    struct type_list_element<0, type_list<Head, Tail...>>
    {
       using type = Head;
    };

    template<std::size_t I, typename List>
    using type_list_element_t = typename type_list_element<I, List>::type;

    /////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////
    // as_std_tuple
    // use it like this:
    // typename as_std_tuple<int, bool>::type => std::tuple<int, bool>

    template<typename...T>
    struct as_std_tuple
    {
        using type = std::tuple<T...>;
    };

    template<template<class...> class Type_List, typename...Ts>
    struct as_std_tuple<Type_List<Ts...>>
    {
        using type = std::tuple<Ts...>;
    };

    template<typename...T>
    using as_std_tuple_t = typename as_std_tuple<T...>::type;

    /////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////
    // Returns the size of the type_list
    // e.g.: type_list_size<int, bool>::value => 2
    // e.g.: type_list_size<>::value => 0
    template<typename T>
    struct type_list_size_impl;

    template<typename...Ts>
    struct type_list_size_impl<type_list<Ts...>> : std::integral_constant<std::size_t, sizeof...(Ts)>
    {
    };

    template<typename...T>
    using type_list_size = typename type_list_size_impl<as_type_list_t<T...>>::type;

    /////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////
    // pushed the given item \p T at the front of the type list

    template<typename T, typename List>
    struct push_front;

    template<typename T, template<class...> class Type_List, typename...Ts>
    struct push_front<T, Type_List<Ts...>>
    {
        using type = Type_List<T, Ts...>;
    };

    /////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////
    // pushed the given item \p T N times at the front of the type list

    template<typename T, typename List, std::size_t N>
    struct push_front_n
    {
        using type = typename push_front_n<T, typename push_front<T, List>::type, N - 1>::type;
    };

    template<typename T, typename List>
    struct push_front_n<T, List, 0>
    {
        using type = List;
    };

    template<typename T, typename List, std::size_t N>
    using push_front_n_t = typename push_front_n<T, List, N>::type;

    /////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////
    // checks if type \p T is given in the list of \p Types
    // e.g.:
    // contains<int, std::tuple<bool, double, int>>::value => true
    // contains<int, std::tuple<bool, double, void>>::value => false
    // or
    // contains<int, bool, double, void>::value => false

    template<typename T, typename... Types>
    struct contains : static_any_of<std::is_same<T, Types>::value...>
    {
    };

    template<typename T, template<class...> class TContainer, typename... Types>
    struct contains<T, TContainer<Types...>> : contains<T, Types...>
    {
    };

    /////////////////////////////////////////////////////////////////////////////////////

    template <typename T>
    struct has_is_valid_alias
    {
        typedef char YesType[1];
        typedef char NoType[2];

        template <typename U> static YesType& check(typename U::is_valid*);
        template <typename U> static NoType& check(...);


        static RTTR_CONSTEXPR_OR_CONST bool value = (sizeof(check<T>(0)) == sizeof(YesType));
    };

    template<typename T, typename Tp = remove_cv_t<remove_reference_t<T>>>
    using is_associative_container = std::integral_constant<bool, !has_is_valid_alias<associative_container_mapper<Tp>>::value>;

    template<typename T, typename Tp = remove_cv_t<remove_reference_t<T>>>
    using is_sequential_container = std::integral_constant<bool, !has_is_valid_alias<sequential_container_mapper<Tp>>::value>;

    /////////////////////////////////////////////////////////////////////////////////////

    template <typename T>
    struct associative_container_value_t
    {
        template <typename U> static typename U::mapped_type check(typename U::mapped_type*);
        template <typename U> static void check(...);

        using type = decltype(check<T>(nullptr));
    };

    /////////////////////////////////////////////////////////////////////////////////////

    template<typename T>
    using is_raw_array_type = std::is_array<raw_type_t<T>>;

    /////////////////////////////////////////////////////////////////////////////////////
    // pointer_count<T>::value Returns the number of pointers for a type
    // e.g. pointer_count<char**>::value => 2
    //      pointer_count<char*>::value  => 1
    //      pointer_count<char>::value   => 0
    template<typename T, typename Enable = void>
    struct pointer_count_impl
    {
        static RTTR_CONSTEXPR_OR_CONST std::size_t size = 0;
    };

    template<typename T>
    struct pointer_count_impl<T, enable_if_t<std::is_pointer<T>::value &&
                                             !is_function_ptr<T>::value &&
                                             !std::is_member_pointer<T>::value>>
    {
        static RTTR_CONSTEXPR_OR_CONST std::size_t size = pointer_count_impl<remove_pointer_t<T> >::size + 1;
    };

    template<typename T>
    using pointer_count = std::integral_constant<std::size_t, pointer_count_impl<T>::size>;

    /////////////////////////////////////////////////////////////////////////////////////
    // is_char_array<T>::value Returns true if the given type is a char array
    // e.g. is_char_array<char[10]>::value => true
    //      is_char_array<int[10]>::value => false
    //      is_char_array<char>::value => false
    template<typename T>
    using is_one_dim_char_array = std::integral_constant<bool, std::is_array<T>::value &&
                                                               std::is_same<char, raw_array_type_t<T>>::value &&
                                                               (std::rank<T>::value == 1)>;

    /////////////////////////////////////////////////////////////////////////////////////

    template <typename T, typename...Ts>
    struct max_sizeof_list_impl;

     template <typename T>
    struct max_sizeof_list_impl<T>
    {
        static RTTR_CONSTEXPR_OR_CONST size_t value = sizeof(T);
    };

    template<typename T1, typename T2, typename... U>
    struct max_sizeof_list_impl<T1, T2, U...>
    {
        static RTTR_CONSTEXPR_OR_CONST std::size_t value = max_sizeof_list_impl< conditional_t< sizeof(T1) >= sizeof(T2),
                                                                                              T1,
                                                                                              T2>,
                                                                                 U...>::value;
    };

    template<template<class...> class List, class... Ts>
    struct max_sizeof_list_impl<List<Ts...>>
    {
        static RTTR_CONSTEXPR_OR_CONST std::size_t value = max_sizeof_list_impl<Ts...>::value;
    };

    /////////////////////////////////////////////////////////////////////////////////////
    // Returns the maximum sizeof value from all given types
    // use it like this:
    // max_size_of_list<int, bool, double>::value => 8

    template<typename... Ts>
    using max_sizeof_list = std::integral_constant<std::size_t, max_sizeof_list_impl<Ts...>::value>;


    /////////////////////////////////////////////////////////////////////////////////////

    template <typename T, typename...Ts>
    struct max_alignof_list_impl;

     template <typename T>
    struct max_alignof_list_impl<T>
    {
        static RTTR_CONSTEXPR_OR_CONST size_t value = std::alignment_of<T>::value;
    };

    template<typename T1, typename T2, typename... U>
    struct max_alignof_list_impl<T1, T2, U...>
    {
        static RTTR_CONSTEXPR_OR_CONST std::size_t value = max_alignof_list_impl<
                                                                conditional_t<std::alignment_of<T1>::value >= std::alignment_of<T2>::value,
                                                                              T1, T2>,
                                                                U...>::value;
    };

    template<template<class...> class List, typename... Ts>
    struct max_alignof_list_impl<List<Ts...>>
    {
        static RTTR_CONSTEXPR_OR_CONST std::size_t value = max_alignof_list_impl<Ts...>::value;
    };

    /////////////////////////////////////////////////////////////////////////////////////
    // Returns the maximum sizeof value from all given types
    // use it like this:
    // max_alignof_list<int, bool, double>::value => 8

    template<typename... Ts>
    using max_alignof_list = std::integral_constant<std::size_t, max_alignof_list_impl<Ts...>::value>;

    /////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////

    template<typename T>
    using remove_cv_ref_t = typename detail::remove_cv<typename std::remove_reference<T>::type>::type;

    /*!
     * A slightly different decay than in the standard, the extent of arrays are not removed.
     * Applies lvalue-to-rvalue, function-to-pointer implicit conversions to the type T and removes cv-qualifiers.
     */
    template<typename T>
    struct decay_except_array
    {
        using Tp    = remove_reference_t<T>;
        using type  = conditional_t< std::is_function<Tp>::value,
                                     add_pointer_t<Tp>,
                                     remove_cv_t<Tp>
                                   >;
    };

    template<typename T>
    using decay_except_array_t = typename decay_except_array<T>::type;

    /////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////
    // checks whether the given type T has a less than operator
    template<typename T, typename = decltype(std::declval<T>() < std::declval<T>() )>
    std::true_type  supports_less_than_test(const T&);
    std::false_type supports_less_than_test(...);

    template<typename T>
    struct has_less_than_operator : std::integral_constant<bool, std::is_same<std::true_type,
                                                                             decltype(supports_less_than_test(std::declval<T>()))>::value> {};

    /////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////
    // checks whether the given type T has the equal operator
    template<typename T, typename = decltype(std::declval<T>() == std::declval<T>() )>
    std::true_type  supports_equal_test(const T&);
    std::false_type supports_equal_test(...);


    template<typename T>
    struct has_equal_operator : std::integral_constant<bool, std::is_same<std::true_type,
                                                                          decltype(supports_equal_test(std::declval<T>()))>::value> {};

    /////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////
    // count the number of type in a variadic template list
    // e.g. count_type<int, std::tuple<bool, int, bool, int, int, std::string>>::value => 3

    template <typename T, typename Type_List>
    struct count_type_impl;

    template<typename T>
    struct count_type_impl<T, type_list<>>
    {
        static RTTR_CONSTEXPR_OR_CONST std::size_t value = 0;
    };

    template<typename T, typename... Tail>
    struct count_type_impl<T, type_list<T, Tail...>>
    {
        static RTTR_CONSTEXPR_OR_CONST std::size_t value = count_type_impl<T, type_list<Tail...>>::value + 1;
    };

    template<typename T, typename U, typename... Tail>
    struct count_type_impl<T, type_list<U, Tail...>>
    {
        static RTTR_CONSTEXPR_OR_CONST std::size_t value = count_type_impl<T, type_list<Tail...>>::value;
    };

    template<typename T, typename Type_List>
    using count_type = std::integral_constant<std::size_t, count_type_impl<T, as_type_list_t<Type_List>>::value>;


    /////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////
    // find from a list of types
    // find_types<std::tuple<int, bool>, bool, float>::type => std::tuple<bool>

    template<typename Result_List, typename Types_To_Find, typename Types_To_Iterate>
    struct find_types_impl;

    template<typename...Rs, typename...Tf>
    struct find_types_impl<type_list<Rs...>, type_list<Tf...>, type_list<>>
    {
        using type = type_list<Rs...>;
    };


    template<typename...Rs, typename Types_To_Find, typename T, typename...Tail>
    struct find_types_impl<type_list<Rs...>, Types_To_Find, type_list<T, Tail...>>
    {
        using type = conditional_t< contains<T, Types_To_Find>::value,
                                    typename find_types_impl<type_list<Rs..., T>, Types_To_Find, type_list<Tail...>>::type,
                                    typename find_types_impl<type_list<Rs...>, Types_To_Find, type_list<Tail...>>::type
                                   >;
    };

    template<typename Types_To_Find, typename Types_To_Iterate>
    using find_types = find_types_impl<type_list<>, as_type_list_t<Types_To_Find>, as_type_list_t<Types_To_Iterate>>;

    /////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////
    // returns true when in the given list has a double item entry
    // has_double_types<std::tuple<int, bool, int>>::value => true
    // has_double_types<std::tuple<int, bool, double>>::value => false

    template<typename O, typename T>
    struct has_double_types_impl;

    template<typename...Orig_Types>
    struct has_double_types_impl<type_list<Orig_Types...>, type_list<>> : std::false_type
    {
    };

    template<typename...Orig_Types, typename T, typename... Tail>
    struct has_double_types_impl<type_list<Orig_Types...>, type_list<T, Tail...>>
    :   conditional_t< count_type<T, type_list<Orig_Types...>>::value >= 2,
                       std::true_type,
                       typename has_double_types_impl<type_list<Orig_Types...>, type_list<Tail...>>::type
                     >
    {
    };

    template <typename... T>
    using has_double_types = typename has_double_types_impl<as_type_list_t<T...>, as_type_list_t<T...>>::type;

    /////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////
    // Returns the number of elements in the type list \p T which satisfy the condition \p Condition.
    // e.g. count_if<is_enum_type, type_list<int, enum_data<E>, std::string>>::value => 1

    template<template<class> class Condition, typename T, typename Enable = void>
    struct count_if_impl;

    template<template<class> class Condition>
    struct count_if_impl<Condition, type_list<>>
    {
        static RTTR_CONSTEXPR_OR_CONST std::size_t value = 0;
    };

    template<template<class> class Condition, typename T, typename...TArgs>
    struct count_if_impl<Condition, type_list<T, TArgs...>, enable_if_t<Condition<T>::value>>
    {
        static RTTR_CONSTEXPR_OR_CONST std::size_t value = count_if_impl<Condition, type_list<TArgs...>>::value + 1;
    };

    template<template<class> class Condition, typename T, typename...TArgs>
    struct count_if_impl<Condition, type_list<T, TArgs...>, enable_if_t<!Condition<T>::value>>
    {
        static RTTR_CONSTEXPR_OR_CONST std::size_t value = count_if_impl<Condition, type_list<TArgs...>>::value;
    };

    template<template<class> class Condition, typename...TArgs>
    using count_if = std::integral_constant<std::size_t, count_if_impl<Condition, type_list< raw_type_t<TArgs>... > >::value>;

    /////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////

    template<typename T>
    struct is_string_literal : std::false_type {};

    template<std::size_t N>
    struct is_string_literal<char[N]> : std::true_type {};

    template<>
    struct is_string_literal<const char*> : std::true_type {};

    /////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////

    struct null_type {};

    /////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////

    template<template<class> class Condition, typename T>
    struct find_if_impl;

    template<template<class> class Condition, typename T, typename...TArgs>
    struct find_if_impl<Condition, type_list<T, TArgs...>>
    {
        using type = conditional_t< Condition<T>::value,
                                    T,
                                    typename find_if_impl<Condition, type_list<TArgs...>>::type
                                   >;
    };

    template<template<class> class Condition>
    struct find_if_impl<Condition, type_list<>>
    {
        using type = null_type;
    };

    //! This type trait will return the first type which matches the template type `default_args<T...>`
    template<template<class> class Condition, typename...TArgs>
    using find_if_t = typename find_if_impl<Condition, type_list< TArgs... > >::type;

    /////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////

    //! Workaround because msvc2013 cannot handle '<sizeof...(T)' with std::enable_if<T>.
    template<typename...TArgs>
    struct is_not_one_argument : std::integral_constant<bool, (sizeof...(TArgs) != 1)> {};

    template<typename...TArgs>
    struct is_one_argument : std::integral_constant<bool, (sizeof...(TArgs) == 1)> {};

    template<typename...TArgs>
    struct is_empty_args : std::integral_constant<bool, (sizeof...(TArgs) == 0)> {};

    /////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////

    template<typename T>
    using is_nullptr_t = std::is_same<T, std::nullptr_t>;

    /////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////
    // This trait will compare two list of types, they are the same (std::true_type)
    // when the contain the same types. There is an exception pointer types
    // and std::nullptr_t are treated the same
    // e.g.:
    // is_same_nullptr<std::tuple<int, bool*>, std::tuple<int, nullptr>>::value => std::true_type
    // is_same_nullptr<std::tuple<int, bool>, std::tuple<int, nullptr>>::value => std::false_type
    template<typename List1, typename List2, typename Enable = void>
    struct is_same_nullptr_impl;

    template<template <class...> class List1, typename T, typename... T1,
             template <class...> class List2, typename U, typename... U1>
    struct is_same_nullptr_impl< List1<T, T1...>, List2<U, U1...>, void >
    {
        using type = conditional_t<std::is_same<T, U>::value,
                                   typename is_same_nullptr_impl<List1<T1...>, List2<U1...>>::type,
                                   conditional_t< std::is_pointer<T>::value && is_nullptr_t<U>::value,
                                                  typename is_same_nullptr_impl<List1<T1...>, List2<U1...>>::type,
                                                  conditional_t< std::is_pointer<U>::value && is_nullptr_t<T>::value,
                                                                 typename is_same_nullptr_impl<List1<T1...>, List2<U1...>>::type,
                                                                 std::false_type
                                                                >
                                                 >
                                  >;
    };

    template<template <class...> class List1, typename ...T,
             template <class...> class List2, typename ...U>
    struct is_same_nullptr_impl< List1<T...>, List2<U...>, enable_if_t< is_empty_args<T...>::value && is_empty_args<U...>::value> >
    {
        using type = std::true_type;
    };

    template<template <class...> class List1, typename ...T,
             template <class...> class List2, typename ...U>
    struct is_same_nullptr_impl< List1<T...>, List2<U...>, enable_if_t< (!is_empty_args<T...>::value && is_empty_args<U...>::value) ||
                                                                        (is_empty_args<T...>::value && !is_empty_args<U...>::value)> >
    {
        using type = std::false_type;
    };


    template<typename T1, typename T2>
    using is_same_nullptr = typename is_same_nullptr_impl<T1, T2>::type;
    /////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////
    // workaround, because MSVC 2013 can't handle deleted copy-ctors very well; specialize for further types here
    template<typename T>
    struct is_copy_constructible : std::is_copy_constructible<T>{};

    template<typename T>
    struct is_copy_constructible<std::unique_ptr<T>> : std::false_type {};

    /////////////////////////////////////////////////////////////////////////////////////////

    template<typename T>
    struct property_traits;

    template<typename A, typename C>
    struct property_traits<A(C::*)>
    {
        using class_type = C;
    };

    /////////////////////////////////////////////////////////////////////////////////////////

     template <typename... T>
     struct count_types : std::integral_constant<std::size_t, 0>::type
     {
     };

     template <template <typename... > class List, typename... Types>
     struct count_types<List<Types...>> : std::integral_constant<std::size_t, sizeof...(Types) - 1>::type
     {
     };

} // end namespace detail
} // end namespace rttr

#endif // RTTR_MISC_TYPE_TRAITS_H_
