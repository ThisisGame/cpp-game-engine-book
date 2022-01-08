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

#ifndef RTTR_SEQUENTIAL_CONTAINER_TYPE_TRAITS_H_
#define RTTR_SEQUENTIAL_CONTAINER_TYPE_TRAITS_H_

#include "rttr/detail/base/core_prerequisites.h"

#include "rttr/detail/misc/misc_type_traits.h"

#include <type_traits>
#include <vector>
#include <tuple>

namespace rttr
{
class type;

namespace detail
{

/////////////////////////////////////////////////////////////////////////////////////

template <typename... T>
struct concat_sequiantials_types;


template <template <typename ...> class List, typename ...Types, typename T>
struct concat_sequiantials_types<List<Types...>, T, std::true_type>
{
    using type = List<Types...>;
};

template <template <typename... > class List, typename... Types, typename T>
struct concat_sequiantials_types<List<Types...>, T, std::false_type>
{
    using sub_type  = typename sequential_container_mapper<T>::value_t;
    using type      = typename concat_sequiantials_types< List<Types..., T>, sub_type, typename std::is_same<invalid_type, sub_type>::type >::type;
};

//////////////////////////////////////////////////////////////////////////////////////

template<typename T>
struct sequential_rank_type_list
{
    using sub_type  = typename sequential_container_mapper<T>::value_t;
    using types     = typename concat_sequiantials_types< std::tuple<>, T, typename std::is_same<invalid_type, sub_type>::type >::type;
};

//////////////////////////////////////////////////////////////////////////////////////

template<typename T, size_t N>
struct sequential_container_ranke_type
{
    using type = typename std::tuple_element<N, typename sequential_rank_type_list<T>::types>::type;
};

/////////////////////////////////////////////////////////////////////////////////////
// rank<T>::value
//
// rank<int[2][10][4]>::value => 3
// rank<std::vector<std::vector<int>>>::value => 2
template<typename T>
using rank_count = typename count_types< typename sequential_rank_type_list<T>::types >::type;

//////////////////////////////////////////////////////////////////////////////////////
template<typename T, std::size_t N>
struct get_ranke_type
{
    static ::rttr::type get_type(std::size_t index)
    {
        if (N == index)
            return type::get<typename sequential_container_ranke_type<T, N>::type>();
        else
            return get_ranke_type<T, N - 1>::get_type(index);
    }
};

template<typename T>
struct get_ranke_type<T, 0>
{
    static ::rttr::type get_type(std::size_t index)
    {
        return type::get<typename sequential_container_ranke_type<T, 0>::type>();
    }
};

/////////////////////////////////////////////////////////////////////////////////////

} // end namespace detail
} // end namespace rttr

#endif // RTTR_SEQUENTIAL_CONTAINER_TYPE_TRAITS_H_
