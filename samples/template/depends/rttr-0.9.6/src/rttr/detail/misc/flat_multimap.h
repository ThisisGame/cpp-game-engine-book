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

#ifndef RTTR_FLAT_MULTIMAP_H_
#define RTTR_FLAT_MULTIMAP_H_

#include "rttr/detail/base/core_prerequisites.h"
#include "rttr/detail/misc/misc_type_traits.h"
#include "rttr/detail/misc/std_type_traits.h"

#include <vector>
#include <utility>
#include <functional>
#include <algorithm>

namespace rttr
{
namespace detail
{

/*!
 * \brief The \ref flat_multimap class implements a simple multimap based on std::vector instead of a binary tree.
 *
 */
template<typename Key, typename Value, template<class> class Hash = std::hash, typename Compare = std::equal_to<Key>>
class flat_multimap
{
    template<typename Hash_Type = std::size_t>
    struct key_data
    {
        Key         m_key;
        Hash_Type   m_hash_value;

        struct order
        {
            RTTR_INLINE bool operator () (const key_data& left, const key_data& right)  const
            {
                return (left.m_hash_value < right.m_hash_value);
            }
            RTTR_INLINE bool operator () (const Hash_Type& left, const key_data& right) const
            {
                return (left < right.m_hash_value);
            }
            RTTR_INLINE bool operator () (const key_data& left, const Hash_Type& right) const
            {
                return (left.m_hash_value < right);
            }
        };
    };

    public:
        using key_data_type = key_data<>;
        using value_type = std::pair<const Key, Value>;
        using iterator = typename std::vector<Value>::iterator;
        using const_iterator = typename std::vector<Value>::const_iterator;
        using const_iterator_key = typename std::vector<key_data_type>::const_iterator;
        using iterator_key = typename std::vector<key_data_type>::iterator;
        using hash_type = std::size_t;


        flat_multimap() {}

    private:
        using has_type = Hash<Key>;

    public:
        iterator end()
        {
            return m_value_list.end();
        }

        const_iterator end() const
        {
            return m_value_list.end();
        }

        const_iterator cend() const
        {
            return m_value_list.cend();
        }

        void insert(value_type&& value)
        {
            insert(std::move(value.first), std::move(value.second));
        }

        void insert(const Key&& key, Value&& value)
        {
            m_key_list.push_back(key_data_type{std::move(key), has_type()(key)});
            std::stable_sort(m_key_list.begin(), m_key_list.end(), typename key_data_type::order());

            auto found_key = find_key_const(key);
            if (found_key != m_key_list.cend())
            {
                auto itr_key = found_key;
                for (; itr_key != m_key_list.cend(); ++itr_key)
                {
                    if (Compare()(itr_key->m_key, key))
                        found_key = itr_key;
                    else
                        break;
                }

                const auto index = std::distance(m_key_list.cbegin(), found_key);
                m_value_list.insert(m_value_list.begin() + index, std::move(value));
            }
        }

        iterator find(const Key& key)
        {
            const auto itr = find_key_const(key);
            if (itr != m_key_list.end())
                return (m_value_list.begin() + std::distance(m_key_list.cbegin(), itr));
            else
                return (m_value_list.end());
        }

        const_iterator find(const Key& key) const
        {
            const auto itr = find_key_const(key);
            if (itr != m_key_list.end())
                return (m_value_list.cbegin() + std::distance(m_key_list.cbegin(), itr));
            else
                return (m_value_list.cend());
        }
// older versions of gcc stl, have no support for const_iterator in std::vector<T>::erase(const_iterator)
#if RTTR_COMPILER == RTTR_COMPILER_GNUC && RTTR_COMP_VER < 490
        bool erase(const Key& key)
        {
            iterator_key itr = find_key(key);
            if (itr != m_key_list.end())
            {
                auto value_itr = m_value_list.begin() + std::distance(m_key_list.begin(), itr);
                if (value_itr != m_value_list.end())
                {
                    m_key_list.erase(itr);
                    m_value_list.erase(value_itr);
                     return true;
                }
            }

            return false;
        }
#else
        bool erase(const Key& key)
        {
            const_iterator_key itr = find_key_const(key);
            if (itr != m_key_list.end())
            {
                auto value_itr = m_value_list.cbegin() + std::distance(m_key_list.cbegin(), itr);
                if (value_itr != m_value_list.cend())
                {
                    m_key_list.erase(itr);
                    m_value_list.erase(value_itr);
                    return true;
                }
            }

            return false;
        }
#endif

        const std::vector<Value>& value_data() const
        {
            return m_value_list;
        }

    private:

        const_iterator_key find_key_const(const Key& key) const
        {
            const auto hash_value = has_type()(key);
            auto itr = std::lower_bound(m_key_list.begin(), m_key_list.end(),
                                        hash_value,
                                        typename key_data_type::order());
            for (; itr != m_key_list.end(); ++itr)
            {
                auto& item = *itr;
                if (item.m_hash_value != hash_value)
                    break;

                if (Compare()(item.m_key, key))
                    return itr;
            }
            return m_key_list.end();
        }

        iterator_key find_key(const Key& key)
        {
            const auto hash_value = has_type()(key);
            auto itr = std::lower_bound(m_key_list.begin(), m_key_list.end(),
                                        hash_value,
                                        typename key_data_type::order());
            for (; itr != m_key_list.end(); ++itr)
            {
                auto& item = *itr;
                if (item.m_hash_value != hash_value)
                    break;

                if (Compare()(item.m_key, key))
                    return itr;
            }
            return m_key_list.end();
        }

    private:
        std::vector<key_data_type>  m_key_list;
        std::vector<Value>          m_value_list;
};

} // end namespace detail
} // end namespace rttr

#endif // RTTR_FLAT_MULTIMAP_H_
