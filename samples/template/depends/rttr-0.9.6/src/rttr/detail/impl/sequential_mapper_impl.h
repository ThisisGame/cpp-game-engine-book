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

#ifndef RTTR_SEQUENTIAL_MAPPER_IMPL_H_
#define RTTR_SEQUENTIAL_MAPPER_IMPL_H_

#include "rttr/detail/base/core_prerequisites.h"
#include "rttr/detail/misc/iterator_wrapper.h"
#include "rttr/detail/misc/sequential_container_type_traits.h"

#include "rttr/variant.h"
#include <type_traits>

#include <vector>
#include <list>
#include <deque>
#include <array>
#include <initializer_list>

namespace rttr
{
namespace detail
{


/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

template<typename T, typename ConstType, typename Tp = conditional_t<std::is_const<ConstType>::value,
                                                                     typename sequential_container_mapper<T>::const_itr_t,
                                                                     typename sequential_container_mapper<T>::itr_t>>
struct sequential_container_mapper_wrapper : iterator_wrapper_base<Tp>
{
    using base_class    = sequential_container_mapper<T>;
    using value_t       = typename base_class::value_t;
    using itr_t         = typename base_class::itr_t;
    using const_itr_t   = typename base_class::const_itr_t;
    using itr_wrapper   = iterator_wrapper_base<Tp>;

    static ConstType& get_container(void* container)
    {
        return *reinterpret_cast<ConstType*>(container);
    }

    template<typename..., typename ReturnType = decltype(base_class::get_data(std::declval<itr_t>())),
             enable_if_t<std::is_reference<ReturnType>::value && !std::is_array<remove_reference_t<ReturnType>>::value, int> = 0>
    static variant get_data(const iterator_data& itr)
    {
        auto& it = itr_wrapper::get_iterator(itr);
        return variant(std::ref(base_class::get_data(it)));
    }

    template<typename..., typename ReturnType = decltype(base_class::get_data(std::declval<itr_t>())),
             enable_if_t<std::is_reference<ReturnType>::value && std::is_array<remove_reference_t<ReturnType>>::value, int> = 0>
    static variant get_data(const iterator_data& itr)
    {
        auto& it = itr_wrapper::get_iterator(itr);
#if RTTR_COMPILER == RTTR_COMPILER_MSVC && RTTR_COMP_VER <= 1800
        // the compiler has a bug:
        // int foo[2];
        // auto bar = std::ref(foo);
        // does not compile..., so we return a ptr here, instead of a std::reference_wrapper
        return variant(&base_class::get_data(it));
#else
        return variant(std::ref(base_class::get_data(it)));
#endif
    }

    template<typename..., typename ReturnType = decltype(base_class::get_data(std::declval<itr_t>())),
             enable_if_t<!std::is_reference<ReturnType>::value, int> = 0>
    static variant get_data(const iterator_data& itr)
    {
        auto& it = itr_wrapper::get_iterator(itr);
        return variant(base_class::get_data(it));
    }

    static void begin(void* container, iterator_data& itr)
    {
        itr_wrapper::create(itr, base_class::begin(get_container(container)));
    }

    static void end(void* container, iterator_data& itr)
    {
        itr_wrapper::create(itr, base_class::end(get_container(container)));
    }

    static bool is_empty(void* container)
    {
        return base_class::is_empty(get_container(container));
    }

    static bool is_dynamic()
    {
        return base_class::is_dynamic();
    }

    static std::size_t get_rank()
    {
        return rank_count<T>::value;
    }

    static ::rttr::type get_rank_type(std::size_t index)
    {
        if (index <= rank_count<T>::value)
            return get_ranke_type<T, rank_count<T>::value>::get_type(index);
        else
            return get_invalid_type();
    }

    static std::size_t get_size(void* container)
    {
        return base_class::get_size(get_container(container));
    }

    /////////////////////////////////////////////////////////////////////////

    template<typename..., typename C = ConstType, enable_if_t<!std::is_const<C>::value, int> = 0>
    static bool set_size(void* container, std::size_t size)
    {
        return base_class::set_size(get_container(container), size);
    }

    template<typename..., typename C = ConstType, enable_if_t<std::is_const<C>::value, int> = 0>
    static bool set_size(void* container, std::size_t size)
    {
        // cannot set size for a const container...
        return false;
    }

    /////////////////////////////////////////////////////////////////////////

    template<typename..., typename C = ConstType, enable_if_t<!std::is_const<C>::value, int> = 0>
    static void clear(void* container)
    {
        base_class::clear(get_container(container));
    }

    template<typename..., typename C = ConstType, enable_if_t<std::is_const<C>::value, int> = 0>
    static void clear(void* container)
    {
        // cannot clear a const container...
    }

    /////////////////////////////////////////////////////////////////////////

    template<typename..., typename C = ConstType, enable_if_t<!std::is_const<C>::value, int> = 0>
    static void erase(void* container, const iterator_data& itr_pos, iterator_data& itr)
    {
        const auto ret = base_class::erase(get_container(container), itr_wrapper::get_iterator(itr_pos));
        itr_wrapper::create(itr, ret);
    }

    template<typename..., typename C = ConstType, enable_if_t<std::is_const<C>::value, int> = 0>
    static void erase(void* container, const iterator_data& itr_pos, iterator_data& itr)
    {
        itr_wrapper::create(itr, base_class::end(get_container(container)));
        return;
    }

    /////////////////////////////////////////////////////////////////////////

    template<typename..., typename C = ConstType, enable_if_t<!std::is_const<C>::value, int> = 0>
    static void insert(void* container, argument& value, const iterator_data& itr_pos, iterator_data& itr)
    {
        if (value.get_type() == ::rttr::type::get<value_t>())
        {
            auto ret = base_class::insert(get_container(container), value.get_value<value_t>(), itr_wrapper::get_iterator(itr_pos));
            itr_wrapper::create(itr, ret);
        }
        else
        {
            end(container, itr);
        }
    }

    template<typename..., typename C = ConstType, enable_if_t<std::is_const<C>::value, int> = 0>
    static void insert(void* container, argument& value, const iterator_data& itr_pos, iterator_data& itr)
    {
        end(container, itr);
    }

    /////////////////////////////////////////////////////////////////////////
    // is_const<T> is used because of std::initializer_list, it can only return a constant value
    template<typename..., typename C = ConstType, typename ReturnType = decltype(base_class::get_value(std::declval<C&>(), 0)),
             enable_if_t<!std::is_const<C>::value &&
                         !std::is_array<remove_reference_t<ReturnType>>::value &&
                         !std::is_const<remove_reference_t<ReturnType>>::value, int> = 0>
    static bool set_value(void* container, std::size_t index, argument& value)
    {
        if (value.get_type() == ::rttr::type::get<value_t>())
        {
            base_class::get_value(get_container(container), index) = value.get_value<value_t>();
            return true;
        }
        else
        {
            return false;
        }
    }

    template<typename..., typename C = ConstType, typename ReturnType = decltype(base_class::get_value(std::declval<C&>(), 0)),
             enable_if_t<!std::is_const<C>::value &&
                         std::is_array<remove_reference_t<ReturnType>>::value &&
                         !std::is_const<remove_reference_t<ReturnType>>::value, int> = 0>
    static bool set_value(void* container, std::size_t index, argument& value)
    {
        if (value.get_type() == ::rttr::type::get<value_t>())
        {
            copy_array(value.get_value<value_t>(), base_class::get_value(get_container(container), index));
            return true;
        }
        else
        {
            return false;
        }
    }

    template<typename..., typename C = ConstType, typename ReturnType = decltype(base_class::get_value(std::declval<C&>(), 0)),
             enable_if_t<std::is_const<C>::value ||
                         std::is_const<remove_reference_t<ReturnType>>::value, int> = 0>
    static bool set_value(void* container, std::size_t index, argument& value)
    {
        base_class::get_value(get_container(container), index);
        return false;
    }

    /////////////////////////////////////////////////////////////////////////

    template<typename..., typename C = ConstType, typename ReturnType = decltype(base_class::get_value(std::declval<C&>(), 0)),
             enable_if_t<std::is_reference<ReturnType>::value && !std::is_array<remove_reference_t<ReturnType>>::value, int> = 0>
    static variant get_value(void* container, std::size_t index)
    {
        return variant(std::ref(base_class::get_value(get_container(container), index)));
    }

    template<typename..., typename C = ConstType, typename ReturnType = decltype(base_class::get_value(std::declval<C&>(), 0)),
             enable_if_t<std::is_reference<ReturnType>::value && std::is_array<remove_reference_t<ReturnType>>::value, int> = 0>
    static variant get_value(void* container, std::size_t index)
    {

#if RTTR_COMPILER == RTTR_COMPILER_MSVC && RTTR_COMP_VER <= 1800
        // the compiler has a bug:
        // int foo[2];
        // auto bar = std::ref(foo);
        // does not compile..., so we return a ptr here, instead of a std::reference_wrapper
        return variant(&base_class::get_value(get_container(container), index));
#else
        return variant(std::ref(base_class::get_value(get_container(container), index)));
#endif
    }

    template<typename..., typename C = ConstType, typename ReturnType = decltype(base_class::get_value(std::declval<C&>(), 0)),
             enable_if_t<!std::is_reference<ReturnType>::value, int> = 0>
    static variant get_value(void* container, std::size_t index)
    {
        return variant(static_cast<value_t>(base_class::get_value(get_container(container), index)));
    }

};

//////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
struct sequential_container_base_dynamic
{
    using container_t   = T;
    using value_t       = typename T::value_type;
    using itr_t         = typename T::iterator;
    using const_itr_t   = typename T::const_iterator;

    static bool is_dynamic()
    {
        return true;
    }

    static value_t& get_data(const itr_t& itr)
    {
        return *itr;
    }

    static const value_t& get_data(const const_itr_t& itr)
    {
        return *itr;
    }

    /////////////////////////////////////////////////////////////////////////////////////

    static itr_t begin(container_t& container)
    {
        return container.begin();
    }

    static const_itr_t begin(const container_t& container)
    {
        return container.begin();
    }

    /////////////////////////////////////////////////////////////////////////////////////

    static itr_t end(container_t& container)
    {
        return container.end();
    }

    static const_itr_t end(const container_t& container)
    {
        return container.end();
    }

    /////////////////////////////////////////////////////////////////////////////////////

    static void clear(container_t& container)
    {
        container.clear();
    }

    static bool is_empty(const container_t& container)
    {
        return container.empty();
    }

    static std::size_t get_size(const container_t& container)
    {
        return container.size();
    }

    static bool set_size(container_t& container, std::size_t size)
    {
        container.resize(size);
        return true;
    }

    static itr_t erase(container_t& container, const itr_t& itr)
    {
        return container.erase(itr);
    }

    static itr_t erase(container_t& container, const const_itr_t& itr)
    {
        return container.erase(itr);
    }

    static itr_t insert(container_t& container, const value_t& value, const itr_t& itr_pos)
    {
        return container.insert(itr_pos, value);
    }

    static itr_t insert(container_t& container, const value_t& value, const const_itr_t& itr_pos)
    {
        return container.insert(itr_pos, value);
    }
};

//////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
struct sequential_container_base_dynamic_direct_access : sequential_container_base_dynamic<T>
{
    using container_t   = T;
    using value_t       = typename T::value_type;

    static value_t& get_value(container_t& container, std::size_t index)
    {
        return container[index];
    }

    static const value_t& get_value(const container_t& container, std::size_t index)
    {
        return container[index];
    }
};

//////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
struct sequential_container_base_dynamic_itr_access : sequential_container_base_dynamic<T>
{
    using container_t   = T;
    using value_t       = typename T::value_type;

    static value_t& get_value(container_t& container, std::size_t index)
    {
        auto it = container.begin();
        std::advance(it, index);
        return *it;
    }

    static const value_t& get_value(const container_t& container, std::size_t index)
    {
        auto it = container.begin();
        std::advance(it, index);
        return *it;
    }
};

//////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
struct sequential_container_base_static
{
    using container_t   = T;
    using value_t       = typename T::value_type;
    using itr_t         = typename T::iterator;
    using const_itr_t   = typename T::const_iterator;

    static bool is_dynamic()
    {
        return false;
    }

    static value_t& get_data(const itr_t& itr)
    {
        return *itr;
    }

    static const value_t& get_data(const const_itr_t& itr)
    {
        return *itr;
    }

    /////////////////////////////////////////////////////////////////////////////////////

    static itr_t begin(container_t& container)
    {
        return container.begin();
    }

    static const_itr_t begin(const container_t& container)
    {
        return container.begin();
    }

    /////////////////////////////////////////////////////////////////////////////////////

    static itr_t end(container_t& container)
    {
        return container.end();
    }

    static const_itr_t end(const container_t& container)
    {
        return container.end();
    }

    /////////////////////////////////////////////////////////////////////////////////////

    static void clear(container_t& container)
    {
    }

    static bool is_empty(const container_t& container)
    {
        return container.empty();
    }

    static std::size_t get_size(const container_t& container)
    {
        return container.size();
    }

    static bool set_size(container_t& container, std::size_t size)
    {
        return false;
    }

    static itr_t erase(container_t& container, const itr_t& itr)
    {
        return end(container);
    }

    static itr_t erase(container_t& container, const const_itr_t& itr)
    {
        return end(container);
    }

    static itr_t insert(container_t& container, const value_t& value, const itr_t& itr_pos)
    {
        return end(container);
    }

    static itr_t insert(container_t& container, const value_t& value, const const_itr_t& itr_pos)
    {
        return end(container);
    }

    static value_t& get_value(container_t& container, std::size_t index)
    {
        return container[index];
    }

    static const value_t& get_value(const container_t& container, std::size_t index)
    {
        return container[index];
    }
};

} // end namespace detail

//////////////////////////////////////////////////////////////////////////////////////////
// direct specialization

template<typename T, std::size_t N>
struct sequential_container_mapper<T[N]>
{
    using container_t   = T[N];
    using value_t       = ::rttr::detail::remove_pointer_t<typename std::decay<T[N]>::type>;
    using itr_t         = typename std::decay<T[N]>::type;
    using const_itr_t   = typename std::decay<::rttr::detail::add_const_t<T[N]>>::type;

    static bool is_dynamic()
    {
        return false;
    }

    static value_t& get_data(itr_t& itr)
    {
        return *itr;
    }

    static const value_t& get_data(const const_itr_t& itr)
    {
        return *itr;
    }

    /////////////////////////////////////////////////////////////////////////////////////

    static itr_t begin(container_t& container)
    {
        return &container[0];
    }

    static const_itr_t begin(const container_t& container)
    {
        return &container[0];
    }

    /////////////////////////////////////////////////////////////////////////////////////

    static itr_t end(container_t& container)
    {
        return &container[N];
    }

    static const_itr_t end(const container_t& container)
    {
        return &container[N];
    }

    /////////////////////////////////////////////////////////////////////////////////////

    static void clear(container_t& container)
    {
    }

    static bool is_empty(const container_t& container)
    {
        return false;
    }

    static std::size_t get_size(const container_t& container)
    {
        return N;
    }

    static bool set_size(container_t& container, std::size_t size)
    {
        return false;
    }

    static itr_t erase(container_t& container, const itr_t& itr)
    {
        return end(container);
    }

    static itr_t insert(container_t& container, const value_t& value, const itr_t& itr_pos)
    {
        return end(container);
    }

    static value_t& get_value(container_t& container, std::size_t index)
    {
        return container[index];
    }

    static const value_t& get_value(const container_t& container, std::size_t index)
    {
        return container[index];
    }
};

//////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
struct sequential_container_mapper<std::initializer_list<T>>
{
    using container_t   = std::initializer_list<T>;
    using value_t       = typename std::initializer_list<T>::value_type;
    using itr_t         = typename std::initializer_list<T>::iterator;
    using const_itr_t   = typename std::initializer_list<T>::const_iterator;

    static bool is_dynamic()
    {
        return false;
    }

    static value_t& get_data(itr_t& itr)
    {
        return *itr;
    }

    static const value_t& get_data(const const_itr_t& itr)
    {
        return *itr;
    }

    /////////////////////////////////////////////////////////////////////////////////////

    static itr_t begin(container_t& container)
    {
        return container.begin();
    }

    static const_itr_t begin(const container_t& container)
    {
        return container.end();
    }

    /////////////////////////////////////////////////////////////////////////////////////

    static itr_t end(container_t& container)
    {
        return container.end();
    }

    static const_itr_t end(const container_t& container)
    {
        return container.end();
    }

    /////////////////////////////////////////////////////////////////////////////////////

    static void clear(container_t& container)
    {
    }

    static bool is_empty(const container_t& container)
    {
        return false;
    }

    static std::size_t get_size(const container_t& container)
    {
        return container.size();
    }

    static bool set_size(container_t& container, std::size_t size)
    {
        return false;
    }

    static itr_t erase(container_t& container, const itr_t& itr)
    {
        return end(container);
    }

    static itr_t insert(container_t& container, const value_t& value, const itr_t& itr_pos)
    {
        return end(container);
    }

    static const value_t& get_value(container_t& container, std::size_t index)
    {
        auto it = container.begin();
        std::advance(it, index);
        return *it;
    }

    static const value_t& get_value(const container_t& container, std::size_t index)
    {
        auto it = container.begin();
        std::advance(it, index);
        return *it;
    }
};

//////////////////////////////////////////////////////////////////////////////////////
// specialization for std::vector<bool>, because vec[index] returns a `std::vector<bool>::reference` not a `bool&`

template<>
struct sequential_container_mapper<std::vector<bool>>
{
    using container_t   = std::vector<bool>;
    using value_t       = std::vector<bool>::value_type;
    using itr_t         = std::vector<bool>::iterator;
    using const_itr_t   = std::vector<bool>::const_iterator;

    static bool is_dynamic()
    {
        return true;
    }

    static value_t get_data(const itr_t& itr)
    {
        return *itr;
    }

    static value_t get_data(const const_itr_t& itr)
    {
        return *itr;
    }

    /////////////////////////////////////////////////////////////////////////////////////

    static itr_t begin(container_t& container)
    {
        return container.begin();
    }

    static const_itr_t begin(const container_t& container)
    {
        return container.begin();
    }

    /////////////////////////////////////////////////////////////////////////////////////

    static itr_t end(container_t& container)
    {
        return container.end();
    }

    static const_itr_t end(const container_t& container)
    {
        return container.end();
    }

    /////////////////////////////////////////////////////////////////////////////////////

    static void clear(container_t& container)
    {
        container.clear();
    }

    static bool is_empty(const container_t& container)
    {
        return container.empty();
    }

    static std::size_t get_size(const container_t& container)
    {
        return container.size();
    }

    static bool set_size(container_t& container, std::size_t size)
    {
        container.resize(size);
        return true;
    }

    static itr_t erase(container_t& container, const itr_t& itr)
    {
        return container.erase(itr);
    }

    static itr_t erase(container_t& container, const const_itr_t& itr)
    {
// to prevent following gcc bug: 'no matching function for call to `std::vector<bool>::erase(const const_itr_t&) return container.erase(itr);`
// vec.erase(vec.cbegin()); fails for unkown reason with this old version
#if (RTTR_COMPILER == RTTR_COMPILER_GNUC && RTTR_COMP_VER < 490)
        auto itr_non_const = container.begin();
        std::advance (itr_non_const, std::distance<const_itr_t>(itr_non_const, itr));
        return container.erase(itr_non_const);
#else
        return container.erase(itr);
#endif
    }

    static itr_t insert(container_t& container, const value_t& value, const itr_t& itr_pos)
    {
        return container.insert(itr_pos, value);
    }

    static itr_t insert(container_t& container, const value_t& value, const const_itr_t& itr_pos)
    {
// to prevent following gcc bug: 'no matching function for call to `std::vector<bool>::insert(const const_itr_t&, bool) return container.insert(itr, bool);`
// vec.erase(vec.cbegin()); fails for unkown reason with this old version
#if (RTTR_COMPILER == RTTR_COMPILER_GNUC && RTTR_COMP_VER < 490)
        auto itr_non_const = container.begin();
        std::advance (itr_non_const, std::distance<const_itr_t>(itr_non_const, itr_pos));
        return container.insert(itr_non_const, value);
#else
        return container.insert(itr_pos, value);
#endif
    }

    static std::vector<bool>::reference get_value(container_t& container, std::size_t index)
    {
        return container[index];
    }

    static std::vector<bool>::const_reference get_value(const container_t& container, std::size_t index)
    {
        return container[index];
    }
};

//////////////////////////////////////////////////////////////////////////////////////

template<typename T>
struct sequential_container_mapper<std::vector<T>> : detail::sequential_container_base_dynamic_direct_access<std::vector<T>> {};
template<typename T>
struct sequential_container_mapper<std::list<T>> : detail::sequential_container_base_dynamic_itr_access<std::list<T>> {};
template<typename T>
struct sequential_container_mapper<std::deque<T>> : detail::sequential_container_base_dynamic_direct_access<std::deque<T>> {};

template<typename T, std::size_t N>
struct sequential_container_mapper<std::array<T, N>> : detail::sequential_container_base_static<std::array<T, N>> {};


//////////////////////////////////////////////////////////////////////////////////////

namespace detail
{

struct sequential_container_empty
{
    static void create(iterator_data& itr_tgt, const iterator_data& src)
    {
    }

    static void advance(iterator_data& itr, std::ptrdiff_t idx)
    {
    }

    static void destroy(iterator_data& itr)
    {
    }

    static bool equal(const iterator_data& lhs_itr, const iterator_data& rhs_itr) RTTR_NOEXCEPT
    {
        return true;
    }

    static variant get_data(const iterator_data& itr)
    {
        return variant();
    }

    static void begin(void* container, iterator_data& itr)
    {
    }

    static bool is_empty(void* container)
    {
        return true;
    }

    static bool is_dynamic()
    {
        return false;
    }

    static std::size_t get_rank()
    {
        return 0;
    }

    static type get_rank_type(std::size_t index)
    {
        return get_invalid_type();
    }

    static std::size_t get_size(void* container)
    {
        return 0;
    }

    static bool set_size(void* container, std::size_t size)
    {
        return false;
    }

    static void erase(void* container, const iterator_data& itr_pos, iterator_data& itr)
    {
    }

    static void clear(void* container)
    {
    }

    static void insert(void* container, argument& value, const iterator_data& itr, iterator_data& pos)
    {
    }

    static bool set_value(void* container, std::size_t index, argument& value)
    {
        return false;
    }

    static variant get_value(void* container, std::size_t index)
    {
        return variant();
    }
};

//////////////////////////////////////////////////////////////////////////////////////

} // end namespace detail
} // end namespace rttr

#endif // RTTR_SEQUENTIAL_MAPPER_IMPL_H_
