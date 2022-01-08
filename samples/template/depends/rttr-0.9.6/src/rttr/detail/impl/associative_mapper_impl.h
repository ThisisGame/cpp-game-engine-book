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

#ifndef RTTR_ASSOCIATIVE_MAPPER_IMPL_H_
#define RTTR_ASSOCIATIVE_MAPPER_IMPL_H_

#include "rttr/detail/base/core_prerequisites.h"
#include "rttr/detail/misc/iterator_wrapper.h"
#include "rttr/argument.h"
#include "rttr/variant.h"

#include <type_traits>
#include <set>
#include <map>
#include <unordered_map>
#include <unordered_set>

namespace rttr
{
namespace detail
{


/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

template<typename T, typename ConstType, typename Tp = conditional_t<std::is_const<ConstType>::value,
                                                                     typename associative_container_mapper<T>::const_itr_t,
                                                                     typename associative_container_mapper<T>::itr_t>>
struct associative_container_mapper_wrapper : iterator_wrapper_base<Tp>
{
    using base_class    = associative_container_mapper<T>;
    using key_t         = typename base_class::key_t;
    using value_t       = typename base_class::value_t;
    using itr_t         = typename base_class::itr_t;
    using const_itr_t   = typename base_class::const_itr_t;
    using itr_wrapper   = iterator_wrapper_base<Tp>;

    static ConstType& get_container(void* container)
    {
        return *reinterpret_cast<ConstType*>(container);
    }

    static variant get_key(const iterator_data& itr)
    {
        auto& it = itr_wrapper::get_iterator(itr);
        return variant(std::ref(base_class::get_key(it)));
    }

    template<typename..., typename V = value_t, enable_if_t<!std::is_void<V>::value, int> = 0>
    static variant
    get_value(const iterator_data& itr)
    {
        auto& it = itr_wrapper::get_iterator(itr);
        return variant(std::ref(base_class::get_value(it)));
    }

    template<typename..., typename V = value_t, enable_if_t<std::is_void<V>::value, int> = 0>
    static variant
    get_value(const iterator_data& itr)
    {
        return variant();
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

    static std::size_t get_size(void* container)
    {
        return base_class::get_size(get_container(container));
    }

    static void find(void* container, iterator_data& itr, argument& key)
    {
        if (key.get_type() == ::rttr::type::get<key_t>())
            itr_wrapper::create(itr, base_class::find(get_container(container), key.get_value<key_t>()));
        else
            end(container, itr);
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

    static void equal_range(void* container, argument& key,
                            iterator_data& itr_begin, iterator_data& itr_end)
    {
        if (key.get_type() == ::rttr::type::get<key_t>())
        {
            auto ret = base_class::equal_range(get_container(container), key.get_value<key_t>());
            itr_wrapper::create(itr_begin, ret.first);
            itr_wrapper::create(itr_end, ret.second);
        }
        else
        {
            end(container, itr_begin);
            end(container, itr_end);
        }
    }

    /////////////////////////////////////////////////////////////////////////

    template<typename..., typename C = ConstType, enable_if_t<!std::is_const<C>::value, int> = 0>
    static std::size_t erase(void* container, argument& key)
    {
        if (key.get_type() == ::rttr::type::get<key_t>())
        {
            return base_class::erase(get_container(container), key.get_value<key_t>());
        }
        else
        {
            return 0;
        }
    }

    template<typename..., typename C = ConstType, enable_if_t<std::is_const<C>::value, int> = 0>
    static std::size_t erase(void* container, argument& key)
    {
        return 0;
    }

    /////////////////////////////////////////////////////////////////////////

    template<typename..., typename V = value_t, enable_if_t<std::is_void<V>::value && !std::is_const<ConstType>::value, int> = 0>
    static bool insert_key(void* container, argument& key, iterator_data& itr)
    {
        if (key.get_type() == ::rttr::type::get<key_t>())
        {
            auto ret = base_class::insert_key(get_container(container), key.get_value<key_t>());
            itr_wrapper::create(itr, ret.first);
            return ret.second;
        }
        else
        {
            end(container, itr);
            return false;
        }
    }

    template<typename..., typename V = value_t, enable_if_t<!std::is_void<V>::value || std::is_const<ConstType>::value, int> = 0>
    static bool insert_key(void* container, argument& key, iterator_data& itr)
    {
        end(container, itr);
        return false;
    }

    /////////////////////////////////////////////////////////////////////////

    template<typename..., typename V = value_t, enable_if_t<!std::is_void<V>::value && !std::is_const<ConstType>::value, int> = 0>
    static bool insert_key_value(void* container, argument& key, argument& value, iterator_data& itr)
    {
        if (key.get_type() == ::rttr::type::get<key_t>() &&
            value.get_type() == ::rttr::type::get<value_t>())
        {
            auto ret = base_class::insert_key_value(get_container(container), key.get_value<key_t>(), value.get_value<value_t>());
            itr_wrapper::create(itr, ret.first);
            return ret.second;
        }
        else
        {
            end(container, itr);
            return false;
        }
    }

    template<typename..., typename V = value_t, enable_if_t<std::is_void<V>::value || std::is_const<ConstType>::value, int> = 0>
    static bool insert_key_value(void* container, argument& key, argument& value, iterator_data& itr)
    {
        end(container, itr);
        return false;
    }

};

//////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
struct associative_container_base
{
    using container_t   = T;
    using key_t         = typename T::key_type;
    using value_t       = typename associative_container_value_t<T>::type;
    using itr_t         = typename T::iterator;
    using const_itr_t   = typename T::const_iterator;

    static const key_t& get_key(const const_itr_t& itr)
    {
        return itr->first;
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

    static itr_t find(container_t& container, const key_t& key)
    {
        return container.find(key);
    }

    static const_itr_t find(const container_t& container, const key_t& key)
    {
        return container.find(key);
    }

    /////////////////////////////////////////////////////////////////////////////////////

    static std::pair<itr_t, itr_t> equal_range(container_t& container, const key_t& key)
    {
        return container.equal_range(key);
    }

    static std::pair<const_itr_t, const_itr_t> equal_range(const container_t& container, const key_t& key)
    {
        return container.equal_range(key);
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

    static std::size_t erase(container_t& container, const key_t& key)
    {
        return container.erase(key);
    }

    static std::pair<itr_t, bool> insert_key(container_t& container, const key_t& key)
    {
        return {container.end(), false};
    }
};


//////////////////////////////////////////////////////////////////////////////////////

template<typename T>
struct associative_container_map_base : associative_container_base<T>
{
    using container_t   = typename associative_container_base<T>::container_t;
    using key_t         = typename associative_container_base<T>::key_t;
    using value_t       = typename associative_container_base<T>::value_t;
    using itr_t         = typename associative_container_base<T>::itr_t;
    using const_itr_t   = typename associative_container_base<T>::const_itr_t;

    static value_t& get_value(itr_t& itr)
    {
        return itr->second;
    }

    static const value_t& get_value(const const_itr_t& itr)
    {
        return itr->second;
    }

    static std::pair<itr_t, bool> insert_key_value(container_t& container, const key_t& key, const value_t& value)
    {
        return container.insert(std::make_pair(key, value));
    }
};

//////////////////////////////////////////////////////////////////////////////////////

template<typename T>
struct associative_container_key_base : associative_container_base<T>
{
    using container_t   = typename associative_container_base<T>::container_t;
    using key_t         = typename T::key_type;
    using value_t       = void;
    using itr_t         = typename T::iterator;
    using const_itr_t   = typename T::const_iterator;

    static const key_t& get_key(const const_itr_t& itr)
    {
        return *itr;
    }

    static std::pair<itr_t, bool> insert_key(container_t& container, const key_t& key)
    {
        return container.insert(key);
    }
};

//////////////////////////////////////////////////////////////////////////////////////

template<typename T>
struct associative_container_base_multi : associative_container_base<T>
{
    using container_t   = typename associative_container_base<T>::container_t;
    using key_t         = typename T::key_type;
    using value_t       = typename T::mapped_type;
    using itr_t         = typename T::iterator;
    using const_itr_t   = typename T::const_iterator;

    static value_t& get_value(itr_t& itr)
    {
        return itr->second;
    }

    static const value_t& get_value(const const_itr_t& itr)
    {
        return itr->second;
    }

    static std::pair<itr_t, bool> insert_key_value(container_t& container, const key_t& key, const value_t& value)
    {
        return {container.insert(std::make_pair(key, value)), true};
    }
};

//////////////////////////////////////////////////////////////////////////////////////

template<typename T>
struct associative_container_key_base_multi : associative_container_key_base<T>
{
    using container_t   = typename associative_container_key_base<T>::container_t;
    using key_t         = typename T::key_type;
    using itr_t         = typename T::iterator;

    static std::pair<itr_t, bool> insert_key(container_t& container, const key_t& key)
    {
        return {container.insert(key), true};
    }
};

//////////////////////////////////////////////////////////////////////////////////////

} // end namespace detail

//////////////////////////////////////////////////////////////////////////////////////

template<typename K>
struct associative_container_mapper<std::set<K>> : detail::associative_container_key_base<std::set<K>> {};

//////////////////////////////////////////////////////////////////////////////////////

template<typename K>
struct associative_container_mapper<std::multiset<K>> : detail::associative_container_key_base_multi<std::multiset<K>> {};

//////////////////////////////////////////////////////////////////////////////////////

template<typename K, typename T>
struct associative_container_mapper<std::map<K, T>> : detail::associative_container_map_base<std::map<K, T>> { };

//////////////////////////////////////////////////////////////////////////////////////

template<typename K, typename T>
struct associative_container_mapper<std::multimap<K, T>> : detail::associative_container_base_multi<std::multimap<K, T>> {};

//////////////////////////////////////////////////////////////////////////////////////

template<typename K>
struct associative_container_mapper<std::unordered_set<K>> : detail::associative_container_key_base<std::unordered_set<K>> {};

//////////////////////////////////////////////////////////////////////////////////////

template<typename K, typename T>
struct associative_container_mapper<std::unordered_map<K, T>> : detail::associative_container_map_base<std::unordered_map<K, T>> {};

//////////////////////////////////////////////////////////////////////////////////////

template<typename K>
struct associative_container_mapper<std::unordered_multiset<K>> : detail::associative_container_key_base_multi<std::unordered_multiset<K>> {};

//////////////////////////////////////////////////////////////////////////////////////

template<typename K, typename T>
struct associative_container_mapper<std::unordered_multimap<K, T>> : detail::associative_container_base_multi<std::unordered_multimap<K, T>> {};

//////////////////////////////////////////////////////////////////////////////////////

namespace detail
{

struct associative_container_empty
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

    static variant get_key(const iterator_data& itr)
    {
        return variant();
    }

    static variant get_value(const iterator_data& itr)
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

    static std::size_t get_size(void* container)
    {
        return 0;
    }

    static void find(void* container, iterator_data& itr, argument& arg)
    {
    }

    static std::size_t erase(void* container, argument& arg)
    {
        return 0;
    }

    static void clear(void* container)
    {
    }

    static void equal_range(void* container, argument& key,
                            iterator_data& itr_begin, iterator_data& itr_end)
    {

    }

    static bool insert_key(void* container, argument& key, iterator_data& itr)
    {
        return false;
    }

    static bool insert_key_value(void* container, argument& key, argument& value, iterator_data& itr)
    {
        return false;
    }
};

} // end namespace detail
} // end namespace rttr

#endif // RTTR_ASSOCIATIVE_MAPPER_H_
