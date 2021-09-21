
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

#ifndef RTTR_STRING_VIEW_IMPL_H_
#define RTTR_STRING_VIEW_IMPL_H_

#include "rttr/detail/misc/utility.h"

namespace rttr
{

/////////////////////////////////////////////////////////////////////////////////////////

template<typename CharT, typename Traits>
RTTR_CONSTEXPR basic_string_view<CharT, Traits>::basic_string_view() RTTR_NOEXCEPT
:   m_data(nullptr), m_size(0)
{
}

/////////////////////////////////////////////////////////////////////////////////////////

template<typename CharT, typename Traits>
RTTR_CONSTEXPR basic_string_view<CharT, Traits>::basic_string_view(const basic_string_view& other) RTTR_NOEXCEPT
:   m_data(other.m_data), m_size(other.m_size)
{
}

/////////////////////////////////////////////////////////////////////////////////////////

template<typename CharT, typename Traits>
RTTR_CONSTEXPR basic_string_view<CharT, Traits>::basic_string_view(const CharT* str)
:   m_data(str), m_size(str ? traits_type::length(str) : 0)
{
}

/////////////////////////////////////////////////////////////////////////////////////////

template<typename CharT, typename Traits>
RTTR_CONSTEXPR basic_string_view<CharT, Traits>::basic_string_view(const CharT* str, size_type len)
:   m_data(str), m_size(len)
{
}

/////////////////////////////////////////////////////////////////////////////////////////

template<typename CharT, typename Traits>
template<typename Allocator>
basic_string_view<CharT, Traits>::basic_string_view(const std::basic_string<CharT, Traits, Allocator>& str) RTTR_NOEXCEPT
:   m_data(str.data()), m_size(str.length())
{
}

/////////////////////////////////////////////////////////////////////////////////////////

template<typename CharT, typename Traits>
basic_string_view<CharT, Traits>&
basic_string_view<CharT, Traits>::operator=(const basic_string_view<CharT, Traits>& other) RTTR_NOEXCEPT
{
    m_data = other.m_data;
    m_size = other.m_size;
    return *this;
}

/////////////////////////////////////////////////////////////////////////////////////////

template<typename CharT, typename Traits>
RTTR_CXX14_CONSTEXPR void basic_string_view<CharT, Traits>::swap(basic_string_view<CharT, Traits>& s) RTTR_NOEXCEPT
{
    std::swap(m_data, s.m_data);
    std::swap(m_size, s.m_size);
}

/////////////////////////////////////////////////////////////////////////////////////////

template<typename CharT, typename Traits>
RTTR_CONSTEXPR typename basic_string_view<CharT, Traits>::const_iterator
basic_string_view<CharT, Traits>::begin() const RTTR_NOEXCEPT
{
    return m_data;
}

/////////////////////////////////////////////////////////////////////////////////////////

template<typename CharT, typename Traits>
RTTR_CONSTEXPR typename basic_string_view<CharT, Traits>::const_iterator
basic_string_view<CharT, Traits>::cbegin() const RTTR_NOEXCEPT
{
    return m_data;
}

/////////////////////////////////////////////////////////////////////////////////////////

template<typename CharT, typename Traits>
RTTR_CONSTEXPR typename basic_string_view<CharT, Traits>::const_iterator
basic_string_view<CharT, Traits>::end() const RTTR_NOEXCEPT
{
    return (m_data + m_size);
}

/////////////////////////////////////////////////////////////////////////////////////////

template<typename CharT, typename Traits>
RTTR_CONSTEXPR typename basic_string_view<CharT, Traits>::const_iterator
basic_string_view<CharT, Traits>::cend() const RTTR_NOEXCEPT
{
    return (m_data + m_size);
}

/////////////////////////////////////////////////////////////////////////////////////////

template<typename CharT, typename Traits>
typename basic_string_view<CharT, Traits>::const_reverse_iterator
basic_string_view<CharT, Traits>::rbegin() const RTTR_NOEXCEPT
{
    return const_reverse_iterator(end());
}

/////////////////////////////////////////////////////////////////////////////////////////

template<typename CharT, typename Traits>
typename basic_string_view<CharT, Traits>::const_reverse_iterator
basic_string_view<CharT, Traits>::crbegin() const RTTR_NOEXCEPT
{
    return const_reverse_iterator(end());
}

/////////////////////////////////////////////////////////////////////////////////////////

template<typename CharT, typename Traits>
typename basic_string_view<CharT, Traits>::const_reverse_iterator
basic_string_view<CharT, Traits>::rend() const RTTR_NOEXCEPT
{
    return const_reverse_iterator(begin());
}

/////////////////////////////////////////////////////////////////////////////////////////

template<typename CharT, typename Traits>
typename basic_string_view<CharT, Traits>::const_reverse_iterator
basic_string_view<CharT, Traits>::crend() const RTTR_NOEXCEPT
{
    return const_reverse_iterator(begin());
}

/////////////////////////////////////////////////////////////////////////////////////////

template<typename CharT, typename Traits>
RTTR_CONSTEXPR typename basic_string_view<CharT, Traits>::size_type
basic_string_view<CharT, Traits>::size() const RTTR_NOEXCEPT
{
    return m_size;
}

/////////////////////////////////////////////////////////////////////////////////////////

template<typename CharT, typename Traits>
RTTR_CONSTEXPR typename basic_string_view<CharT, Traits>::size_type
basic_string_view<CharT, Traits>::length() const RTTR_NOEXCEPT
{
    return m_size;
}

/////////////////////////////////////////////////////////////////////////////////////////

template<typename CharT, typename Traits>
RTTR_CONSTEXPR typename basic_string_view<CharT, Traits>::size_type
basic_string_view<CharT, Traits>::max_size() const RTTR_NOEXCEPT
{
    return m_size;
}

/////////////////////////////////////////////////////////////////////////////////////////

template<typename CharT, typename Traits>
RTTR_CONSTEXPR bool basic_string_view<CharT, Traits>::empty() const RTTR_NOEXCEPT
{
    return (m_size == 0);
}

/////////////////////////////////////////////////////////////////////////////////////////

template<typename CharT, typename Traits>
RTTR_CONSTEXPR typename basic_string_view<CharT, Traits>::const_reference
basic_string_view<CharT, Traits>::operator[](size_type pos) const RTTR_NOEXCEPT
{
    return m_data[pos];
}

/////////////////////////////////////////////////////////////////////////////////////////

template<typename CharT, typename Traits>
RTTR_CONSTEXPR typename basic_string_view<CharT, Traits>::const_reference
basic_string_view<CharT, Traits>::front() const
{
    return m_data[0];
}

/////////////////////////////////////////////////////////////////////////////////////////

template<typename CharT, typename Traits>
RTTR_CONSTEXPR typename basic_string_view<CharT, Traits>::const_reference
basic_string_view<CharT, Traits>::back() const
{
    return m_data[m_size - 1];
}

/////////////////////////////////////////////////////////////////////////////////////////

template<typename CharT, typename Traits>
RTTR_CONSTEXPR typename basic_string_view<CharT, Traits>::const_pointer
basic_string_view<CharT, Traits>::data() const RTTR_NOEXCEPT
{
    return m_data;
}

/////////////////////////////////////////////////////////////////////////////////////////

template<typename CharT, typename Traits>
RTTR_CXX14_CONSTEXPR void basic_string_view<CharT, Traits>::remove_prefix(size_type n) RTTR_NOEXCEPT
{
    if (n > m_size)
        n = m_size;
    m_data += n;
    m_size -= n;
}

/////////////////////////////////////////////////////////////////////////////////////////

template<typename CharT, typename Traits>
RTTR_CXX14_CONSTEXPR void basic_string_view<CharT, Traits>::remove_suffix(size_type n) RTTR_NOEXCEPT
{
    m_size -= (n > m_size ? m_size : n);
}

/////////////////////////////////////////////////////////////////////////////////////////

template<typename CharT, typename Traits>
template<typename Allocator>
basic_string_view<CharT, Traits>::operator std::basic_string<CharT, Traits, Allocator>() const
{
    return std::basic_string<CharT, Traits, Allocator>(begin(), end());
}

/////////////////////////////////////////////////////////////////////////////////////////

template<typename CharT, typename Traits>
template<typename Allocator>
std::basic_string<CharT, Traits> basic_string_view<CharT, Traits>::to_string(const Allocator& a) const
{
    return std::basic_string<CharT, Traits, Allocator>(begin(), end(), a);
}

/////////////////////////////////////////////////////////////////////////////////////////

template<typename CharT, typename Traits>
RTTR_CXX14_CONSTEXPR int basic_string_view<CharT, Traits>::compare(basic_string_view v) const RTTR_NOEXCEPT
{
    const int cmp = traits_type::compare(m_data, v.m_data, (std::min)(m_size, v.m_size));
    return (cmp != 0 ? cmp : (m_size == v.m_size ? 0 : m_size < v.m_size ? -1 : 1));
}

/////////////////////////////////////////////////////////////////////////////////////////

template<typename CharT, typename Traits>
RTTR_CXX14_CONSTEXPR int basic_string_view<CharT, Traits>::compare(const CharT* c) const RTTR_NOEXCEPT
{
    return compare(basic_string_view(c));
}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

template<typename CharT, typename Traits>
RTTR_INLINE RTTR_CXX14_CONSTEXPR bool operator==(basic_string_view<CharT, Traits> lhs,
                                                 basic_string_view<CharT, Traits> rhs) RTTR_NOEXCEPT
{
    if (lhs.size () == rhs.size ())
        return (lhs.compare(rhs) == 0);
    else
        return false;
}

/////////////////////////////////////////////////////////////////////////////////////////

template<typename CharT, typename Traits>
RTTR_INLINE RTTR_CXX14_CONSTEXPR bool operator==(const char* lhs,
                                                 basic_string_view<CharT, Traits> rhs) RTTR_NOEXCEPT
{
    return (basic_string_view<CharT, Traits>(lhs) == rhs);
}

/////////////////////////////////////////////////////////////////////////////////////////

template<typename CharT, typename Traits>
RTTR_INLINE RTTR_CXX14_CONSTEXPR bool operator==(basic_string_view<CharT, Traits> lhs,
                                                 const char* rhs) RTTR_NOEXCEPT
{
    return (lhs == basic_string_view<CharT, Traits>(rhs));
}

/////////////////////////////////////////////////////////////////////////////////////////

template<typename CharT, typename Traits>
RTTR_INLINE RTTR_CXX14_CONSTEXPR bool operator==(const std::basic_string<CharT, Traits>& lhs,
                                                 basic_string_view<CharT, Traits> rhs) RTTR_NOEXCEPT
{
    return (basic_string_view<CharT, Traits>(lhs) == rhs);
}

/////////////////////////////////////////////////////////////////////////////////////////

template<typename CharT, typename Traits>
RTTR_INLINE RTTR_CXX14_CONSTEXPR bool operator==(basic_string_view<CharT, Traits> lhs,
                                                 const std::basic_string<CharT, Traits>& rhs) RTTR_NOEXCEPT
{
    return (lhs == basic_string_view<CharT, Traits>(rhs));
}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

template<typename CharT, typename Traits>
RTTR_INLINE RTTR_CXX14_CONSTEXPR bool operator!=(basic_string_view<CharT, Traits> lhs,
                                                 basic_string_view<CharT, Traits> rhs) RTTR_NOEXCEPT
{
    if (lhs.size () != rhs.size ())
        return true;
    else
        return (lhs.compare(rhs) != 0);
}

/////////////////////////////////////////////////////////////////////////////////////////

template<typename CharT, typename Traits>
RTTR_INLINE RTTR_CXX14_CONSTEXPR bool operator!=(const char* lhs,
                                                 basic_string_view<CharT, Traits> rhs) RTTR_NOEXCEPT
{
    return (basic_string_view<CharT, Traits>(lhs) != rhs);
}

/////////////////////////////////////////////////////////////////////////////////////////

template<typename CharT, typename Traits>
RTTR_INLINE RTTR_CXX14_CONSTEXPR bool operator!=(basic_string_view<CharT, Traits> lhs,
                                                 const char* rhs) RTTR_NOEXCEPT
{
    return (lhs != basic_string_view<CharT, Traits>(rhs));
}

/////////////////////////////////////////////////////////////////////////////////////////

template<typename CharT, typename Traits>
RTTR_INLINE RTTR_CXX14_CONSTEXPR bool operator!=(const std::basic_string<CharT, Traits>& lhs,
                                                 basic_string_view<CharT, Traits> rhs) RTTR_NOEXCEPT
{
    return (basic_string_view<CharT, Traits>(lhs) != rhs);
}

/////////////////////////////////////////////////////////////////////////////////////////

template<typename CharT, typename Traits>
RTTR_INLINE RTTR_CXX14_CONSTEXPR bool operator!=(basic_string_view<CharT, Traits> lhs,
                                                 const std::basic_string<CharT, Traits>& rhs) RTTR_NOEXCEPT
{
    return (lhs != basic_string_view<CharT, Traits>(rhs));
}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

template<typename CharT, typename Traits>
RTTR_INLINE RTTR_CXX14_CONSTEXPR bool operator<=(basic_string_view<CharT, Traits> lhs,
                                                 basic_string_view<CharT, Traits> rhs) RTTR_NOEXCEPT
{
    return (lhs.compare(rhs) <= 0);
}

/////////////////////////////////////////////////////////////////////////////////////////

template<typename CharT, typename Traits>
RTTR_INLINE RTTR_CXX14_CONSTEXPR bool operator<=(const char* lhs,
                                                 basic_string_view<CharT, Traits> rhs) RTTR_NOEXCEPT
{
    return (basic_string_view<CharT, Traits>(lhs) <= rhs);
}

/////////////////////////////////////////////////////////////////////////////////////////

template<typename CharT, typename Traits>
RTTR_INLINE RTTR_CXX14_CONSTEXPR bool operator<=(basic_string_view<CharT, Traits> lhs,
                                                 const char* rhs) RTTR_NOEXCEPT
{
    return (lhs <= basic_string_view<CharT, Traits>(rhs));
}

/////////////////////////////////////////////////////////////////////////////////////////

template<typename CharT, typename Traits>
RTTR_INLINE RTTR_CXX14_CONSTEXPR bool operator<=(const std::basic_string<CharT, Traits>& lhs,
                                                 basic_string_view<CharT, Traits> rhs) RTTR_NOEXCEPT
{
    return (basic_string_view<CharT, Traits>(lhs) <= rhs);
}

/////////////////////////////////////////////////////////////////////////////////////////

template<typename CharT, typename Traits>
RTTR_INLINE RTTR_CXX14_CONSTEXPR bool operator<=(basic_string_view<CharT, Traits> lhs,
                                                 const std::basic_string<CharT, Traits>& rhs) RTTR_NOEXCEPT
{
    return (lhs <= basic_string_view<CharT, Traits>(rhs));
}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

template<typename CharT, typename Traits>
RTTR_INLINE RTTR_CXX14_CONSTEXPR bool operator>=(basic_string_view<CharT, Traits> lhs,
                                                 basic_string_view<CharT, Traits> rhs) RTTR_NOEXCEPT
{
    return (lhs.compare(rhs) >= 0);
}

/////////////////////////////////////////////////////////////////////////////////////////

template<typename CharT, typename Traits>
RTTR_INLINE RTTR_CXX14_CONSTEXPR bool operator>=(const char* lhs,
                                                 basic_string_view<CharT, Traits> rhs) RTTR_NOEXCEPT
{
    return (basic_string_view<CharT, Traits>(lhs) >= rhs);
}

/////////////////////////////////////////////////////////////////////////////////////////

template<typename CharT, typename Traits>
RTTR_INLINE RTTR_CXX14_CONSTEXPR bool operator>=(basic_string_view<CharT, Traits> lhs,
                                                 const char* rhs) RTTR_NOEXCEPT
{
    return (lhs >= basic_string_view<CharT, Traits>(rhs));
}

/////////////////////////////////////////////////////////////////////////////////////////

template<typename CharT, typename Traits>
RTTR_INLINE RTTR_CXX14_CONSTEXPR bool operator>=(const std::basic_string<CharT, Traits>& lhs,
                                                 basic_string_view<CharT, Traits> rhs) RTTR_NOEXCEPT
{
    return (basic_string_view<CharT, Traits>(lhs) >= rhs);
}

/////////////////////////////////////////////////////////////////////////////////////////

template<typename CharT, typename Traits>
RTTR_INLINE RTTR_CXX14_CONSTEXPR bool operator>=(basic_string_view<CharT, Traits> lhs,
                                                 const std::basic_string<CharT, Traits>& rhs) RTTR_NOEXCEPT
{
    return (lhs >= basic_string_view<CharT, Traits>(rhs));
}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

template<typename CharT, typename Traits>
RTTR_INLINE RTTR_CXX14_CONSTEXPR bool operator>(basic_string_view<CharT, Traits> lhs,
                                                basic_string_view<CharT, Traits> rhs) RTTR_NOEXCEPT
{
    return (lhs.compare(rhs) > 0);
}

/////////////////////////////////////////////////////////////////////////////////////////

template<typename CharT, typename Traits>
RTTR_INLINE RTTR_CXX14_CONSTEXPR bool operator>(const char* lhs,
                                                basic_string_view<CharT, Traits> rhs) RTTR_NOEXCEPT
{
    return (basic_string_view<CharT, Traits>(lhs) > rhs);
}

/////////////////////////////////////////////////////////////////////////////////////////

template<typename CharT, typename Traits>
RTTR_INLINE RTTR_CXX14_CONSTEXPR bool operator>(basic_string_view<CharT, Traits> lhs,
                                                const char* rhs) RTTR_NOEXCEPT
{
    return (lhs > basic_string_view<CharT, Traits>(rhs));
}

/////////////////////////////////////////////////////////////////////////////////////////

template<typename CharT, typename Traits>
RTTR_INLINE RTTR_CXX14_CONSTEXPR bool operator>(const std::basic_string<CharT, Traits>& lhs,
                                                basic_string_view<CharT, Traits> rhs) RTTR_NOEXCEPT
{
    return (basic_string_view<CharT, Traits>(lhs) > rhs);
}

/////////////////////////////////////////////////////////////////////////////////////////

template<typename CharT, typename Traits>
RTTR_INLINE RTTR_CXX14_CONSTEXPR bool operator>(basic_string_view<CharT, Traits> lhs,
                                                const std::basic_string<CharT, Traits>& rhs) RTTR_NOEXCEPT
{
    return (lhs > basic_string_view<CharT, Traits>(rhs));
}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

template<typename CharT, typename Traits>
RTTR_INLINE RTTR_CXX14_CONSTEXPR bool operator<(basic_string_view<CharT, Traits> lhs,
                                                basic_string_view<CharT, Traits> rhs) RTTR_NOEXCEPT
{
    return (lhs.compare(rhs) < 0);
}

/////////////////////////////////////////////////////////////////////////////////////////

template<typename CharT, typename Traits>
RTTR_INLINE RTTR_CXX14_CONSTEXPR bool operator<(const char* lhs,
                                                basic_string_view<CharT, Traits> rhs) RTTR_NOEXCEPT
{
    return (basic_string_view<CharT, Traits>(lhs) < rhs);
}

/////////////////////////////////////////////////////////////////////////////////////////

template<typename CharT, typename Traits>
RTTR_INLINE RTTR_CXX14_CONSTEXPR bool operator<(basic_string_view<CharT, Traits> lhs,
                                                const char* rhs) RTTR_NOEXCEPT
{
    return (lhs < basic_string_view<CharT, Traits>(rhs));
}

/////////////////////////////////////////////////////////////////////////////////////////

template<typename CharT, typename Traits>
RTTR_INLINE RTTR_CXX14_CONSTEXPR bool operator<(const std::basic_string<CharT, Traits>& lhs,
                                                basic_string_view<CharT, Traits> rhs) RTTR_NOEXCEPT
{
    return (basic_string_view<CharT, Traits>(lhs) < rhs);
}

/////////////////////////////////////////////////////////////////////////////////////////

template<typename CharT, typename Traits>
RTTR_INLINE RTTR_CXX14_CONSTEXPR bool operator<(basic_string_view<CharT, Traits> lhs,
                                                const std::basic_string<CharT, Traits>& rhs) RTTR_NOEXCEPT
{
    return (lhs < basic_string_view<CharT, Traits>(rhs));
}

/////////////////////////////////////////////////////////////////////////////////////////

template<typename CharT, typename Traits>
RTTR_INLINE std::basic_string<CharT, Traits> operator+(basic_string_view<CharT, Traits> lhs,
                                                       const std::basic_string<CharT, Traits>& rhs)
{
    return (lhs.to_string() + rhs);
}

template<typename CharT, typename Traits>
RTTR_INLINE std::basic_string<CharT, Traits> operator+(const std::basic_string<CharT, Traits>& lhs,
                                                       basic_string_view<CharT, Traits> rhs)
{
    return (lhs + rhs.to_string());
}

template<typename CharT, typename Traits>
RTTR_INLINE std::basic_string<CharT, Traits> operator+(basic_string_view<CharT, Traits> lhs,
                                                       std::basic_string<CharT, Traits>&& rhs)
{
    return std::move(rhs.insert(0, lhs.data(), lhs.size()));
}

template<typename CharT, typename Traits>
RTTR_INLINE std::basic_string<CharT, Traits> operator+(std::basic_string<CharT, Traits>&& lhs,
                                                       basic_string_view<CharT, Traits> rhs)
{
    return std::move(lhs.append(rhs.data(), rhs.size()));
}

/////////////////////////////////////////////////////////////////////////////////////////

namespace detail
{

template<typename CharT, typename Traits>
inline void insert_fill_chars(std::basic_ostream<CharT, Traits>& os, std::size_t n)
{
    enum { chunk_size = 8 };
    CharT fill_chars[chunk_size];
    std::fill_n(fill_chars, static_cast< std::size_t >(chunk_size), os.fill());
    for (; n >= chunk_size && os.good(); n -= chunk_size)
        os.write(fill_chars, static_cast< std::size_t >(chunk_size));
    if (n > 0 && os.good())
        os.write(fill_chars, n);
}

/////////////////////////////////////////////////////////////////////////////////////////

template<typename CharT, typename Traits>
void insert_aligned(std::basic_ostream<CharT, Traits>& os, const basic_string_view<CharT, Traits>& str)
{
    const std::size_t size = str.size();
    const std::size_t alignment_size = static_cast< std::size_t >(os.width()) - size;
    const bool align_left = (os.flags() & std::basic_ostream<CharT, Traits>::adjustfield) == std::basic_ostream<CharT, Traits>::left;
    if (!align_left)
    {
        detail::insert_fill_chars(os, alignment_size);
        if (os.good())
            os.write(str.data(), size);
    }
    else
    {
        os.write(str.data(), size);
        if (os.good())
            detail::insert_fill_chars(os, alignment_size);
    }
}

} // end namespace detail

/////////////////////////////////////////////////////////////////////////////////////////

template <typename CharT, typename Traits>
std::basic_ostream<CharT, Traits>& operator<<(std::basic_ostream<CharT, Traits>& os,
                                              basic_string_view<CharT, Traits> str)
{
    if (os.good())
    {
        const std::size_t size = str.size();
        const std::size_t w = static_cast< std::size_t >(os.width());
        if (w <= size)
            os.write(str.data(), size);
        else
            detail::insert_aligned(os, str);
        os.width(0);
    }

    return os;
}

/////////////////////////////////////////////////////////////////////////////////////////

namespace detail
{

template <>
struct hash<string_view>
{
public:
    size_t operator()(const string_view& text) const
    {
        return generate_hash(text.data(), text.length());
    }
};

} // end namespace detail
} // end namespace rttr

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

namespace std
{
    template<typename CharT, typename Traits>
    struct hash<rttr::basic_string_view<CharT, Traits>>
    {
    public:
        size_t operator()(const rttr::basic_string_view<CharT, Traits>& value) const
        {
            return rttr::detail::generate_hash(value.data(), value.size());
        }
    };
} // end namespace std

/////////////////////////////////////////////////////////////////////////////////////////

#endif // RTTR_STRING_VIEW_IMPL_H_
