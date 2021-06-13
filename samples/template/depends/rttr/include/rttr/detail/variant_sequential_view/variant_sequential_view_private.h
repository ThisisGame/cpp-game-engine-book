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

#ifndef RTTR_VARIANT_SEQUENTIAL_VIEW_PRIVATE_H_
#define RTTR_VARIANT_SEQUENTIAL_VIEW_PRIVATE_H_

#include "rttr/variant.h"
#include "rttr/argument.h"
#include "rttr/instance.h"
#include "rttr/sequential_mapper.h"

namespace rttr
{

namespace detail
{

/////////////////////////////////////////////////////////////////////////////////////////

class RTTR_LOCAL variant_sequential_view_private
{
    public:
        variant_sequential_view_private() RTTR_NOEXCEPT
        :   m_type(get_invalid_type()),
            m_value_type(get_invalid_type()),
            m_container(nullptr),
            m_get_is_empty_func(sequential_container_empty::is_empty),
            m_get_size_func(sequential_container_empty::get_size),
            m_set_size_func(sequential_container_empty::set_size),
            m_is_dynamic_func(sequential_container_empty::is_dynamic),
            m_get_rank_func(sequential_container_empty::get_rank),
            m_get_rank_type_func(sequential_container_empty::get_rank_type),
            m_begin_func(sequential_container_empty::begin),
            m_end_func(sequential_container_empty::begin),
            m_equal_func(sequential_container_empty::equal),
            m_create_func(sequential_container_empty::create),
            m_delete_func(sequential_container_empty::destroy),
            m_get_data_func(sequential_container_empty::get_data),
            m_advance_func(sequential_container_empty::advance),
            m_erase_func(sequential_container_empty::erase),
            m_clear_func(sequential_container_empty::clear),
            m_insert_func(sequential_container_empty::insert),
            m_set_value_func(sequential_container_empty::set_value),
            m_get_value_func(sequential_container_empty::get_value)
        {
        }

        template<typename T, typename RawType = raw_type_t<T>, typename ConstType = remove_pointer_t<T>>
        variant_sequential_view_private(const T& container) RTTR_NOEXCEPT
            :   m_type(type::get<RawType>()),
            m_value_type(type::get<typename sequential_container_mapper<RawType>::value_t>()),
            m_container(as_void_ptr(container)),
            m_get_is_empty_func(sequential_container_mapper_wrapper<RawType, ConstType>::is_empty),
            m_get_size_func(sequential_container_mapper_wrapper<RawType, ConstType>::get_size),
            m_set_size_func(sequential_container_mapper_wrapper<RawType, ConstType>::set_size),
            m_is_dynamic_func(sequential_container_mapper_wrapper<RawType, ConstType>::is_dynamic),
            m_get_rank_func(sequential_container_mapper_wrapper<RawType, ConstType>::get_rank),
            m_get_rank_type_func(sequential_container_mapper_wrapper<RawType, ConstType>::get_rank_type),
            m_begin_func(sequential_container_mapper_wrapper<RawType, ConstType>::begin),
            m_end_func(sequential_container_mapper_wrapper<RawType, ConstType>::end),
            m_equal_func(sequential_container_mapper_wrapper<RawType, ConstType>::equal),
            m_create_func(sequential_container_mapper_wrapper<RawType, ConstType>::create),
            m_delete_func(sequential_container_mapper_wrapper<RawType, ConstType>::destroy),
            m_get_data_func(sequential_container_mapper_wrapper<RawType, ConstType>::get_data),
            m_advance_func(sequential_container_mapper_wrapper<RawType, ConstType>::advance),
            m_erase_func(sequential_container_mapper_wrapper<RawType, ConstType>::erase),
            m_clear_func(sequential_container_mapper_wrapper<RawType, ConstType>::clear),
            m_insert_func(sequential_container_mapper_wrapper<RawType, ConstType>::insert),
            m_set_value_func(sequential_container_mapper_wrapper<RawType, ConstType>::set_value),
            m_get_value_func(sequential_container_mapper_wrapper<RawType, ConstType>::get_value)
        {
        }

        RTTR_INLINE variant_sequential_view_private(const variant_sequential_view_private& other) = default;

        RTTR_INLINE ~variant_sequential_view_private()
        {

        }

        RTTR_INLINE type get_type() const RTTR_NOEXCEPT
        {
            return m_type;
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

        RTTR_INLINE bool is_dynamic() const RTTR_NOEXCEPT
        {
            return m_is_dynamic_func();
        }

        RTTR_INLINE std::size_t get_rank() const RTTR_NOEXCEPT
        {
            return m_get_rank_func();
        }

        RTTR_INLINE type get_rank_type(std::size_t index) const RTTR_NOEXCEPT
        {
            return m_get_rank_type_func(index);
        }

        RTTR_INLINE std::size_t get_size() const RTTR_NOEXCEPT
        {
            return m_get_size_func(m_container);
        }

        RTTR_INLINE bool set_size(std::size_t size) const RTTR_NOEXCEPT
        {
            return m_set_size_func(m_container, size);
        }

        RTTR_INLINE bool equal(const iterator_data& lhs_itr, const iterator_data& rhs_itr) const RTTR_NOEXCEPT
        {
            return m_equal_func(lhs_itr, rhs_itr);
        }

        RTTR_INLINE const variant get_data(const iterator_data& itr) const
        {
            return m_get_data_func(itr);
        }


        RTTR_INLINE void advance(iterator_data& itr, std::ptrdiff_t index) const
        {
            m_advance_func(itr, index);
        }

        RTTR_INLINE void clear()
        {
            m_clear_func(m_container);
        }

        RTTR_INLINE void erase(const iterator_data& itr_pos, iterator_data& itr)
        {
            m_erase_func(m_container, itr_pos, itr);
        }

        RTTR_INLINE void insert(const iterator_data& itr_pos, argument& value, iterator_data& itr)
        {
            m_insert_func(m_container, value, itr_pos, itr);
        }

        RTTR_INLINE bool set_value(std::size_t index, argument& arg)
        {
            return m_set_value_func(m_container, index, arg);
        }

        RTTR_INLINE variant get_value(std::size_t index) const
        {
            return m_get_value_func(m_container, index);
        }

    private:
        static bool equal_cmp_dummy_func(const iterator_data& lhs_itr, const iterator_data& rhs_itr) RTTR_NOEXCEPT;
        using equality_func     = decltype(&equal_cmp_dummy_func); // workaround because of 'noexcept' can only appear on function declaration

        using get_is_empty_func = bool(*)(void* container);
        using get_size_func     = std::size_t(*)(void* container);
        using set_size_func     = bool(*)(void* container, std::size_t size);
        using get_rank_func     = std::size_t(*)(void);
        using get_rank_type_func= type(*)(std::size_t index);
        using begin_func        = void(*)(void* container, iterator_data& itr);
        using end_func          = void(*)(void* container, iterator_data& itr);
        using advance_func      = void(*)(iterator_data& itr, std::ptrdiff_t index);
        using create_func       = void(*)(iterator_data& itr_tgt, const iterator_data& itr_src);
        using delete_func       = void(*)(iterator_data& itr);
        using get_data_func     = variant (*)(const iterator_data& itr);
        using clear_func        = void(*)(void* container);
        using is_dynamic_func   = bool(*)();
        using erase_func        = void(*)(void* container, const iterator_data& itr_pos, iterator_data& itr);
        using insert_func       = void(*)(void* container, argument& value, const detail::iterator_data& itr_pos, detail::iterator_data& itr);
        using set_value_func    = bool(*)(void* container, std::size_t index, argument& arg);
        using get_value_func    = variant(*)(void* container, std::size_t index);

        type                    m_type;
        type                    m_value_type;
        void*                   m_container;
        get_is_empty_func       m_get_is_empty_func;
        get_size_func           m_get_size_func;
        set_size_func           m_set_size_func;
        is_dynamic_func         m_is_dynamic_func;
        get_rank_func           m_get_rank_func;
        get_rank_type_func      m_get_rank_type_func;
        begin_func              m_begin_func;
        end_func                m_end_func;
        equality_func           m_equal_func;
        create_func             m_create_func;
        delete_func             m_delete_func;
        get_data_func           m_get_data_func;
        advance_func            m_advance_func;
        erase_func              m_erase_func;
        clear_func              m_clear_func;
        insert_func             m_insert_func;
        set_value_func          m_set_value_func;
        get_value_func          m_get_value_func;
};

} // end namespace detail
} // end namespace rttr

#endif // RTTR_VARIANT_SEQUENTIAL_VIEW_PRIVATE_H_
