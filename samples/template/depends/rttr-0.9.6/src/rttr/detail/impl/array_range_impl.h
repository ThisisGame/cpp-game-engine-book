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

#ifndef RTTR_ARRAY_RANGE_IMPL_H_
#define RTTR_ARRAY_RANGE_IMPL_H_

#include "rttr/detail/base/core_prerequisites.h"
#include "rttr/property.h"
#include "rttr/type.h"
#include <memory>

namespace rttr
{

/////////////////////////////////////////////////////////////////////////////////////////

template<typename T, typename Predicate>
RTTR_INLINE array_range<T, Predicate>::array_range(const T* begin, size_type size, const Predicate& pred)
:   m_begin(begin),
    m_end(begin + size),
    m_pred(pred)
{
}

/////////////////////////////////////////////////////////////////////////////////////////

template<typename T, typename Predicate>
RTTR_INLINE array_range<T, Predicate>::array_range()
:   m_begin(nullptr),
    m_end(nullptr)
{
}

/////////////////////////////////////////////////////////////////////////////////////////

template<typename T, typename Predicate>
RTTR_INLINE typename array_range<T, Predicate>::const_iterator array_range<T, Predicate>::begin()
{
    if (empty_())
    {
        return {m_end, this};
    }
    else
    {
        const_iterator itr(m_begin, this);
        if (m_pred(*itr))
            return itr;

        next(itr);

        return itr;
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

template<typename T, typename Predicate>
RTTR_INLINE typename array_range<T, Predicate>::const_iterator array_range<T, Predicate>::end()
{
    return {m_end, this};
}

/////////////////////////////////////////////////////////////////////////////////////////

template<typename T, typename Predicate>
RTTR_INLINE typename array_range<T, Predicate>::const_iterator array_range<T, Predicate>::begin() const
{
    if (empty_())
    {
        return {m_end, this};
    }
    else
    {
        const_iterator itr(m_begin, this);
        if (m_pred(*itr))
            return itr;

        next(itr);

        return itr;
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

template<typename T, typename Predicate>
RTTR_INLINE typename array_range<T, Predicate>::const_iterator array_range<T, Predicate>::end() const
{
    return {m_end, this};
}

/////////////////////////////////////////////////////////////////////////////////////////

template<typename T, typename Predicate>
RTTR_INLINE typename array_range<T, Predicate>::const_iterator array_range<T, Predicate>::cbegin() const
{
    if (empty_())
    {
        return {m_end, this};
    }
    else
    {
        const_iterator itr(m_begin, this);
        if (m_pred(*itr))
            return itr;

        next(itr);

        return itr;
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

template<typename T, typename Predicate>
RTTR_INLINE typename array_range<T, Predicate>::const_iterator array_range<T, Predicate>::cend() const
{
    return {m_end, this};
}

/////////////////////////////////////////////////////////////////////////////////////////

template<typename T, typename Predicate>
RTTR_INLINE typename array_range<T, Predicate>::const_reverse_iterator array_range<T, Predicate>::rbegin()
{
    if (empty_())
    {
        return {m_end, this};
    }
    else
    {
        const_reverse_iterator itr(m_end, this);
        if (m_pred(*itr))
            return itr;

        prev(itr);

        return itr;
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

template<typename T, typename Predicate>
RTTR_INLINE typename array_range<T, Predicate>::const_reverse_iterator array_range<T, Predicate>::rend()
{
    return const_reverse_iterator{m_begin, this};
}

/////////////////////////////////////////////////////////////////////////////////////////

template<typename T, typename Predicate>
RTTR_INLINE typename array_range<T, Predicate>::const_reverse_iterator array_range<T, Predicate>::rbegin() const
{
    if (empty_())
    {
        return {m_end, this};
    }
    else
    {
        const_reverse_iterator itr(m_end, this);
        if (m_pred(*itr))
            return itr;

        prev(itr);

        return itr;
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

template<typename T, typename Predicate>
RTTR_INLINE typename array_range<T, Predicate>::const_reverse_iterator array_range<T, Predicate>::rend() const
{
    return const_reverse_iterator{m_begin, this};
}

/////////////////////////////////////////////////////////////////////////////////////////

template<typename T, typename Predicate>
RTTR_INLINE typename array_range<T, Predicate>::const_reverse_iterator array_range<T, Predicate>::crbegin() const
{
    if (empty_())
    {
        return {m_end, this};
    }
    else
    {
        const_reverse_iterator itr(m_end, this);
        if (m_pred(*itr))
            return itr;

        prev(itr);

        return itr;
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

template<typename T, typename Predicate>
RTTR_INLINE typename array_range<T, Predicate>::const_reverse_iterator array_range<T, Predicate>::crend() const
{
    return const_reverse_iterator{m_begin, this};
}

/////////////////////////////////////////////////////////////////////////////////////////

template<typename T, typename Predicate>
RTTR_INLINE size_t array_range<T, Predicate>::size() const
{
    std::size_t result = 0;
    const_iterator itr{m_begin, this};
    while(itr != cend())
    {
        if (m_pred(*itr.m_ptr))
            ++result;

        ++itr.m_ptr;
    }

    return result;
}

/////////////////////////////////////////////////////////////////////////////////////////

template<typename T, typename Predicate>
RTTR_INLINE bool array_range<T, Predicate>::empty() const
{
    if (m_begin == m_end)
        return true;

    const_iterator itr{m_begin, this};
    if (m_pred(*itr))
        return false;

    ++itr.m_ptr;
    while(itr.m_ptr != m_end && !m_pred(*itr.m_ptr))
    {
        ++itr.m_ptr;
    }

    return (itr == cend());
}

/////////////////////////////////////////////////////////////////////////////////////////

template<typename T, typename Predicate>
RTTR_INLINE bool array_range<T, Predicate>::empty_() const
{
    return (m_begin == m_end);
}

/////////////////////////////////////////////////////////////////////////////////////////

template<typename T, typename Predicate>
template<typename DataType>
RTTR_INLINE void array_range<T, Predicate>::next(array_iterator<DataType>& itr) const
{
    ++itr.m_ptr;
    while(itr.m_ptr != m_end && !m_pred(*itr.m_ptr))
    {
        ++itr.m_ptr;
    }

}

/////////////////////////////////////////////////////////////////////////////////////////

template<typename T, typename Predicate>
template<typename DataType>
RTTR_INLINE void array_range<T, Predicate>::prev(array_reverse_iterator<DataType>& itr) const
{
    --itr.m_ptr;
    while(itr.m_ptr != (m_begin - 1) && !m_pred(*itr.m_ptr))
    {
        --itr.m_ptr;
    }
}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

template<typename T, typename Predicate>
template<typename DataType>
RTTR_INLINE
array_range<T, Predicate>::array_iterator_base<DataType>::array_iterator_base()
:   m_ptr(nullptr)
{
}

/////////////////////////////////////////////////////////////////////////////////////////

template<typename T, typename Predicate>
template<typename DataType>
RTTR_INLINE
array_range<T, Predicate>::array_iterator_base<DataType>::array_iterator_base(typename array_iterator_base<DataType>::pointer ptr,
                                                                              const array_range<T, Predicate>* const range)
:   m_ptr(ptr),
    m_range(range)
{
}

/////////////////////////////////////////////////////////////////////////////////////////

template<typename T, typename Predicate>
template<typename DataType>
RTTR_INLINE bool array_range<T, Predicate>::array_iterator_base<DataType>::operator==(const self_type& rhs) const
{
    return (m_ptr == rhs.m_ptr);
}

/////////////////////////////////////////////////////////////////////////////////////////

template<typename T, typename Predicate>
template<typename DataType>
RTTR_INLINE bool array_range<T, Predicate>::array_iterator_base<DataType>::operator!=(const self_type& rhs) const
{
    return (m_ptr != rhs.m_ptr);
}

/////////////////////////////////////////////////////////////////////////////////////////

template<typename T, typename Predicate>
template<typename DataType>
RTTR_INLINE typename array_range<T, Predicate>::template array_iterator_base<DataType>::self_type&
array_range<T, Predicate>::array_iterator_base<DataType>::operator=(const self_type& other)
{
    m_ptr = other.m_ptr;
    return *this;
}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

template<typename T, typename Predicate>
template<typename DataType>
RTTR_INLINE array_range<T, Predicate>::array_iterator<DataType>::array_iterator()
{
}

/////////////////////////////////////////////////////////////////////////////////////////

template<typename T, typename Predicate>
template<typename DataType>
RTTR_INLINE array_range<T, Predicate>::array_iterator<DataType>::array_iterator(const array_iterator<DataType>& other)
:   array_iterator_base<DataType>(other.m_ptr, other.m_range)
{
}

/////////////////////////////////////////////////////////////////////////////////////////

template<typename T, typename Predicate>
template<typename DataType>
RTTR_INLINE array_range<T, Predicate>::array_iterator<DataType>::array_iterator(typename array_iterator_base<DataType>::pointer ptr,
                                                                                const array_range<T, Predicate>* const range)
:   array_iterator_base<DataType>(ptr, range)
{
}

/////////////////////////////////////////////////////////////////////////////////////////

template<typename T, typename Predicate>
template<typename DataType>
RTTR_INLINE typename array_range<T, Predicate>::template array_iterator<DataType>::reference
array_range<T, Predicate>::array_iterator<DataType>::operator*() const
{
    return *this->m_ptr;
}

/////////////////////////////////////////////////////////////////////////////////////////

template<typename T, typename Predicate>
template<typename DataType>
RTTR_INLINE typename array_range<T, Predicate>::template array_iterator<DataType>::pointer
array_range<T, Predicate>::array_iterator<DataType>::operator->()
{
    return this->m_ptr;
}

/////////////////////////////////////////////////////////////////////////////////////////

template<typename T, typename Predicate>
template<typename DataType>
RTTR_INLINE typename array_range<T, Predicate>::template array_iterator<DataType>::self_type&
array_range<T, Predicate>::array_iterator<DataType>::operator++()
{
    this->m_range->next(*this);
    return *this;
}

/////////////////////////////////////////////////////////////////////////////////////////

template<typename T, typename Predicate>
template<typename DataType>
RTTR_INLINE typename array_range<T, Predicate>::template array_iterator<DataType>::self_type
array_range<T, Predicate>::array_iterator<DataType>::operator++(int index)
{
    auto old_itr = *this;
    this->m_range->next(*this);
    return old_itr;
}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

template<typename T, typename Predicate>
template<typename DataType>
RTTR_INLINE array_range<T, Predicate>::array_reverse_iterator<DataType>::array_reverse_iterator()
{
}

/////////////////////////////////////////////////////////////////////////////////////////

template<typename T, typename Predicate>
template<typename DataType>
RTTR_INLINE array_range<T, Predicate>::array_reverse_iterator<DataType>::array_reverse_iterator(const array_reverse_iterator<DataType>& other)
:   array_iterator_base<DataType>(other.m_ptr, other.m_range)
{
}

/////////////////////////////////////////////////////////////////////////////////////////

template<typename T, typename Predicate>
template<typename DataType>
RTTR_INLINE array_range<T, Predicate>::array_reverse_iterator<DataType>::array_reverse_iterator(typename array_iterator_base<DataType>::pointer ptr,
                                                                                                const array_range<T, Predicate>* const range)
:   array_iterator_base<DataType>(ptr, range)
{
}

/////////////////////////////////////////////////////////////////////////////////////////

template<typename T, typename Predicate>
template<typename DataType>
RTTR_INLINE typename array_range<T, Predicate>::template array_reverse_iterator<DataType>::reference
array_range<T, Predicate>::array_reverse_iterator<DataType>::operator*() const
{
    auto tmp = this->m_ptr;
    return (*--tmp);
}

/////////////////////////////////////////////////////////////////////////////////////////

template<typename T, typename Predicate>
template<typename DataType>
RTTR_INLINE typename array_range<T, Predicate>::template array_reverse_iterator<DataType>::pointer
array_range<T, Predicate>::array_reverse_iterator<DataType>::operator->()
{
    auto tmp = this->m_ptr;
    return --tmp;
}

////////////////////////////////////////////////////////////////////////////////////////////

template<typename T, typename Predicate>
template<typename DataType>
RTTR_INLINE typename array_range<T, Predicate>::template array_reverse_iterator<DataType>::self_type&
array_range<T, Predicate>::array_reverse_iterator<DataType>::operator++()
{
    this->m_range->prev(*this);
    return *this;
}

/////////////////////////////////////////////////////////////////////////////////////////

template<typename T, typename Predicate>
template<typename DataType>
RTTR_INLINE typename array_range<T, Predicate>::template array_reverse_iterator<DataType>::self_type
array_range<T, Predicate>::array_reverse_iterator<DataType>::operator++(int index)
{
    auto old_itr = *this;
    this->m_range->prev(*this);
    return old_itr;
}

/////////////////////////////////////////////////////////////////////////////////////////

namespace detail
{

template<typename T>
struct default_predicate
{
    RTTR_FORCE_INLINE default_predicate() {}
    RTTR_FORCE_INLINE default_predicate(std::function<bool(const T&)> func) : m_func(std::move(func)) {}
    RTTR_FORCE_INLINE bool operator()(const T& obj) const { return (m_func ? m_func(obj) : true); }
    std::function<bool(const T&)> m_func;
};


/////////////////////////////////////////////////////////////////////////////////////////

} // end namespace detail

} // end namespace rttr


#endif // RTTR_ITEM_RANGE_IMPL_H_
