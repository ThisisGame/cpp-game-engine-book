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

#ifndef RTTR_VARIANT_ASSOCIATIVE_VIEW_PRIVATE_H_
#define RTTR_VARIANT_ASSOCIATIVE_VIEW_PRIVATE_H_

#include "rttr/variant.h"
#include "rttr/argument.h"
#include "rttr/instance.h"
#include "rttr/associative_mapper.h"

namespace rttr
{

namespace detail
{

/////////////////////////////////////////////////////////////////////////////////////////

class RTTR_LOCAL variant_associative_view_private
{
    public:
        variant_associative_view_private() RTTR_NOEXCEPT
        :   m_type(get_invalid_type()),
            m_key_type(get_invalid_type()),
            m_value_type(get_invalid_type()),
            m_container(nullptr),
            m_get_is_empty_func(associative_container_empty::is_empty),
            m_get_size_func(associative_container_empty::get_size),
            m_begin_func(associative_container_empty::begin),
            m_end_func(associative_container_empty::begin),
            m_equal_func(associative_container_empty::equal),
            m_create_func(associative_container_empty::create),
            m_delete_func(associative_container_empty::destroy),
            m_get_key_func(associative_container_empty::get_key),
            m_get_value_func(associative_container_empty::get_value),
            m_advance_func(associative_container_empty::advance),
            m_find_func(associative_container_empty::find),
            m_erase_func(associative_container_empty::erase),
            m_clear_func(associative_container_empty::clear),
            m_equal_range_func(associative_container_empty::equal_range),
            m_insert_func_key(associative_container_empty::insert_key),
            m_insert_func_key_value(associative_container_empty::insert_key_value)
        {
        }

        template<typename T, typename RawType = raw_type_t<T>, typename ConstType = remove_pointer_t<T>>
        variant_associative_view_private(const T& container) RTTR_NOEXCEPT
        :   m_type(type::get<RawType>()),
            m_key_type(type::get<typename associative_container_mapper<RawType>::key_t>()),
            m_value_type(type::get<conditional_t<std::is_void<typename associative_container_mapper<RawType>::value_t>::value,
                                                invalid_type,
                                                typename associative_container_mapper<RawType>::value_t>>()),
            m_container(as_void_ptr(container)),
            m_get_is_empty_func(associative_container_mapper_wrapper<RawType, ConstType>::is_empty),
            m_get_size_func(associative_container_mapper_wrapper<RawType, ConstType>::get_size),
            m_begin_func(associative_container_mapper_wrapper<RawType, ConstType>::begin),
            m_end_func(associative_container_mapper_wrapper<RawType, ConstType>::end),
            m_equal_func(associative_container_mapper_wrapper<RawType, ConstType>::equal),
            m_create_func(associative_container_mapper_wrapper<RawType, ConstType>::create),
            m_delete_func(associative_container_mapper_wrapper<RawType, ConstType>::destroy),
            m_get_key_func(associative_container_mapper_wrapper<RawType, ConstType>::get_key),
            m_get_value_func(associative_container_mapper_wrapper<RawType, ConstType>::get_value),
            m_advance_func(associative_container_mapper_wrapper<RawType, ConstType>::advance),
            m_find_func(associative_container_mapper_wrapper<RawType, ConstType>::find),
            m_erase_func(associative_container_mapper_wrapper<RawType, ConstType>::erase),
            m_clear_func(associative_container_mapper_wrapper<RawType, ConstType>::clear),
            m_equal_range_func(associative_container_mapper_wrapper<RawType, ConstType>::equal_range),
            m_insert_func_key(associative_container_mapper_wrapper<RawType, ConstType>::insert_key),
            m_insert_func_key_value(associative_container_mapper_wrapper<RawType, ConstType>::insert_key_value)
        {
        }

        RTTR_INLINE variant_associative_view_private(const variant_associative_view_private& other) = default;

        RTTR_INLINE ~variant_associative_view_private()
        {

        }

        RTTR_INLINE type get_type() const RTTR_NOEXCEPT
        {
            return m_type;
        }

        RTTR_INLINE type get_key_type() const RTTR_NOEXCEPT
        {
            return m_key_type;
        }

        RTTR_INLINE type get_value_type() const RTTR_NOEXCEPT
        {
            return m_value_type;
        }

        RTTR_INLINE void copy(iterator_data& itr_tgt, const iterator_data& itr_src) const
        {
            m_create_func(itr_tgt, itr_src);
        }

        RTTR_INLINE void destroy(iterator_data& itr) const
        {
            m_delete_func(itr);
        }

        RTTR_INLINE void begin(iterator_data& itr) const
        {
            m_begin_func(m_container, itr);
        }

        RTTR_INLINE void end(iterator_data& itr) const
        {
            m_end_func(m_container, itr);
        }

        RTTR_INLINE bool is_empty() const RTTR_NOEXCEPT
        {
            return m_get_is_empty_func(m_container);
        }

        RTTR_INLINE std::size_t get_size() const RTTR_NOEXCEPT
        {
            return m_get_size_func(m_container);
        }

        RTTR_INLINE bool equal(const iterator_data& lhs_itr, const iterator_data& rhs_itr) const RTTR_NOEXCEPT
        {
            return m_equal_func(lhs_itr, rhs_itr);
        }

        RTTR_INLINE const variant get_key(const iterator_data& itr) const
        {
            return m_get_key_func(itr);
        }

        RTTR_INLINE const variant get_value(const iterator_data& itr) const
        {
            return m_get_value_func(itr);
        }

        RTTR_INLINE const std::pair<variant, variant> get_key_value(const iterator_data& itr) const
        {
            return {m_get_key_func(itr), m_get_value_func(itr)};
        }


        RTTR_INLINE void advance(iterator_data& itr, std::ptrdiff_t index) const
        {
            m_advance_func(itr, index);
        }

        RTTR_INLINE void find(iterator_data& itr, argument& key)
        {
            m_find_func(m_container, itr, key);
        }

        RTTR_INLINE void equal_range(argument& key,
                                     iterator_data& itr_begin, detail::iterator_data& itr_end)
        {
            m_equal_range_func(m_container, key, itr_begin, itr_end);
        }

        RTTR_INLINE void clear()
        {
            m_clear_func(m_container);
        }

        RTTR_INLINE std::size_t erase(argument& key)
        {
            return m_erase_func(m_container, key);
        }

        RTTR_INLINE bool insert(argument& key, iterator_data& itr)
        {
            return m_insert_func_key(m_container, key, itr);
        }

        RTTR_INLINE bool insert(argument& key, argument& value, iterator_data& itr)
        {
            return m_insert_func_key_value(m_container, key, value, itr);
        }

    private:
        static bool equal_cmp_dummy_func(const iterator_data& lhs_itr, const iterator_data& rhs_itr) RTTR_NOEXCEPT;
        using equality_func     = decltype(&equal_cmp_dummy_func); // workaround because of 'noexcept' can only appear on function declaration

        using get_is_empty_func = bool(*)(void* container);
        using get_size_func     = std::size_t(*)(void* container);
        using begin_func        = void(*)(void* container, iterator_data& itr);
        using end_func          = void(*)(void* container, iterator_data& itr);
        using advance_func      = void(*)(iterator_data& itr, std::ptrdiff_t index);
        using create_func       = void(*)(iterator_data& itr_tgt, const iterator_data& itr_src);
        using delete_func       = void(*)(iterator_data& itr);
        using get_key_func      = variant (*)(const iterator_data& itr);
        using get_value_func    = variant (*)(const iterator_data& itr);
        using clear_func        = void(*)(void* container);
        using erase_func        = std::size_t(*)(void* container, argument& key);
        using find_func         = void(*)(void* container, detail::iterator_data& itr, argument& key);
        using equal_range_func  = void(*)(void* container, argument& key,
                                          detail::iterator_data& itr_begin, detail::iterator_data& itr_end);
        using insert_func_key   = bool(*)(void* container, argument& key, detail::iterator_data& itr);
        using insert_func_key_value = bool(*)(void* container, argument& key, argument& value, detail::iterator_data& itr);

        type                    m_type;
        type                    m_key_type;
        type                    m_value_type;
        void*                   m_container;
        get_is_empty_func       m_get_is_empty_func;
        get_size_func           m_get_size_func;
        begin_func              m_begin_func;
        end_func                m_end_func;
        equality_func           m_equal_func;
        create_func             m_create_func;
        delete_func             m_delete_func;
        get_key_func            m_get_key_func;
        get_value_func          m_get_value_func;
        advance_func            m_advance_func;
        find_func               m_find_func;
        erase_func              m_erase_func;
        clear_func              m_clear_func;
        equal_range_func        m_equal_range_func;
        insert_func_key         m_insert_func_key;
        insert_func_key_value   m_insert_func_key_value;
};

} // end namespace detail
} // end namespace rttr

#endif // RTTR_VARIANT_ASSOCIATIVE_VIEW_PRIVATE_H_
