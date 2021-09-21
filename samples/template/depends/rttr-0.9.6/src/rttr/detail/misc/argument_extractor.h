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

#ifndef RTTR_ARGUMENT_EXTRACTOR_H_
#define RTTR_ARGUMENT_EXTRACTOR_H_

#include "rttr/detail/base/core_prerequisites.h"
#include "rttr/detail/misc/misc_type_traits.h"

#include <memory>
#include <type_traits>
#include <array>

namespace rttr
{
namespace detail
{

/*!
 * The class \ref argument_extractor will forwards all arguments of type \p T
 * via a variadic function call to a std::array.
 * All other types will be ignored.
 */
template<typename T, typename... OrigArgs>
struct argument_extractor
{
    using container_size = count_type<T, type_list<OrigArgs...>>;
    using array_type = std::array<T, container_size::value>;

    static array_type extract_to_array(OrigArgs&&... arg)
    {
        array_type result;
        extract_types_recursively(result, std::forward<OrigArgs>(arg)...);
        return result;
    }

    static std::vector<T> extract_to_vector(OrigArgs&&... arg)
    {
        std::vector<T> result;
        result.reserve(container_size::value);
        extract_types_recursively(result, std::forward<OrigArgs>(arg)...);
        return result;
    }

private:
    template<typename container_type>
    static void extract_types_recursively(container_type& container) { }

    /////////////////////////////////////////////////////////////////////////////////////

    template<typename container_type, typename U, typename... Args>
    static
    enable_if_t< !std::is_same<T, raw_type_t<U>>::value, void >
    extract_types_recursively(container_type& container, U&& value, Args &&... tail)
    {
        extract_types_recursively(container, std::forward< Args >(tail)...);
    }

    /////////////////////////////////////////////////////////////////////////////////////

    template<typename container_type, typename U, typename... Args>
    static
    enable_if_t< std::is_same<T, raw_type_t<U>>::value, void >
    extract_types_recursively(container_type& container, U&& value, Args &&... tail)
    {
        RTTR_STATIC_CONSTEXPR auto index = count_type<T, type_list<OrigArgs...>>::value - count_type<T, type_list<Args...>>::value - 1;
        container[index] = std::forward<U>(value);
        extract_types_recursively(container, std::forward< Args >(tail)...);
    }

    /////////////////////////////////////////////////////////////////////////////////////

    template<typename U, typename... Args>
    static
    enable_if_t< std::is_same<T, raw_type_t<U>>::value, void >
    extract_types_recursively(std::vector<T>& container, U&& value, Args &&... tail)
    {
        container.emplace_back(std::forward<U>(value));
        extract_types_recursively(container, std::forward< Args >(tail)...);
    }

};

/////////////////////////////////////////////////////////////////////////////////////

/*!
 * Forwards all objects of type \p T to an std::vector<T>.
 *
 * Use it in a context of a variadic function call.
 * See following code sample:
 *
 * \code{.cpp}
 *
 * template<typename...Args>
 * void get_all_int_values(Args&&... args)
 * {
 *   std::vector<int> my_values = get_as_vector<int>(std::forward<Args>(args)...);
 * }
 *
 * \endcode
 */
template<typename T, typename...Args>
std::vector<T> forward_to_vector(Args&&... args)
{
    return argument_extractor<T, Args...>::extract_to_vector(std::forward<Args>(args)...);
}

/*!
 * Forwards all objects of type \p T to an std::array<T, N>.
 *
 * Use it in a context of a variadic function call.
 * See following code sample:
 *
 * \code{.cpp}
 *
 * template<typename...Args>
 * void get_all_int_values(Args&&... args)
 * {
 *   auto my_values = forward_to_array<int>(std::forward<Args>(args)...);
 * }
 *
 * \endcode
 */
template<typename T, typename...Args>
std::array<T, count_type<T, type_list<Args...>>::value> forward_to_array(Args&&... args)
{
    return argument_extractor<T, Args...>::extract_to_array(std::forward<Args>(args)...);
}


} // end namespace detail
} // end namespace rttr

#endif // RTTR_ARGUMENT_EXTRACTOR_H_
