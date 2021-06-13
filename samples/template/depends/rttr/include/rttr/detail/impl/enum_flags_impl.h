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

#ifndef RTTR_ENUM_FLAGS_IMPL_H_
#define RTTR_ENUM_FLAGS_IMPL_H_


namespace rttr
{
namespace detail
{
/*!
 * The \ref enum_flag class is a wrapper around integer value.
 *
 */
class enum_flag
{
public:
    RTTR_CONSTEXPR RTTR_INLINE enum_flag(int value) RTTR_NOEXCEPT : m_value(value) {}
    RTTR_CONSTEXPR RTTR_INLINE operator int32_t() const RTTR_NOEXCEPT { return m_value; }

    private:
        int32_t m_value;
};
} // end namespace detail

/////////////////////////////////////////////////////////////////////////////////////////

template<typename Enum>
RTTR_CONSTEXPR RTTR_INLINE enum_flags<Enum>::enum_flags(zero) RTTR_NOEXCEPT
:   m_value(0)
{

}

/////////////////////////////////////////////////////////////////////////////////////////

template<typename Enum>
RTTR_CONSTEXPR RTTR_INLINE enum_flags<Enum>::enum_flags(Enum enum_value) RTTR_NOEXCEPT
:   m_value(static_cast<enum_type>(enum_value))
{

}

/////////////////////////////////////////////////////////////////////////////////////////

template<typename Enum>
RTTR_CONSTEXPR RTTR_INLINE enum_flags<Enum>::enum_flags(detail::enum_flag f) RTTR_NOEXCEPT
:   m_value(f)
{

}

/////////////////////////////////////////////////////////////////////////////////////////

template<typename Enum>
RTTR_CXX14_CONSTEXPR RTTR_INLINE enum_flags<Enum>& enum_flags<Enum>::operator&=(int mask) RTTR_NOEXCEPT
{
    m_value &= mask;
    return *this;
}

/////////////////////////////////////////////////////////////////////////////////////////

template<typename Enum>
RTTR_CXX14_CONSTEXPR RTTR_INLINE enum_flags<Enum>& enum_flags<Enum>::operator&=(uint32_t mask) RTTR_NOEXCEPT
{
    m_value &= mask;
    return *this;
}

/////////////////////////////////////////////////////////////////////////////////////////

template<typename Enum>
RTTR_CXX14_CONSTEXPR RTTR_INLINE enum_flags<Enum>& enum_flags<Enum>::operator&=(Enum mask) RTTR_NOEXCEPT
{
    m_value &= static_cast<enum_type>(mask);
    return *this;
}

/////////////////////////////////////////////////////////////////////////////////////////

template<typename Enum>
RTTR_CXX14_CONSTEXPR RTTR_INLINE enum_flags<Enum>& enum_flags<Enum>::operator|=(enum_flags f) RTTR_NOEXCEPT
{
    m_value |= f.m_value;
    return *this;
}

/////////////////////////////////////////////////////////////////////////////////////////

template<typename Enum>
RTTR_CXX14_CONSTEXPR RTTR_INLINE enum_flags<Enum>& enum_flags<Enum>::operator|=(Enum f) RTTR_NOEXCEPT
{
    m_value |= static_cast<enum_type>(f);
    return *this;
}

/////////////////////////////////////////////////////////////////////////////////////////

template<typename Enum>
RTTR_CXX14_CONSTEXPR RTTR_INLINE enum_flags<Enum>& enum_flags<Enum>::operator^=(enum_flags f) RTTR_NOEXCEPT
{
    m_value ^= f.m_value;
    return *this;
}

/////////////////////////////////////////////////////////////////////////////////////////

template<typename Enum>
RTTR_CXX14_CONSTEXPR RTTR_INLINE enum_flags<Enum>& enum_flags<Enum>::operator^=(Enum f) RTTR_NOEXCEPT
{
    m_value ^= static_cast<enum_type>(f);
    return *this;
}

/////////////////////////////////////////////////////////////////////////////////////////

template<typename Enum>
RTTR_CONSTEXPR RTTR_INLINE enum_flags<Enum>::operator enum_type() const RTTR_NOEXCEPT
{
    return m_value;
}

/////////////////////////////////////////////////////////////////////////////////////////

template<typename Enum>
RTTR_CONSTEXPR RTTR_INLINE enum_flags<Enum> enum_flags<Enum>::operator|(Enum f) const RTTR_NOEXCEPT
{
    return enum_flags(m_value | static_cast<enum_type>(f));
}

/////////////////////////////////////////////////////////////////////////////////////////

template<typename Enum>
RTTR_CONSTEXPR RTTR_INLINE enum_flags<Enum> enum_flags<Enum>::operator|(enum_flags f) const RTTR_NOEXCEPT
{
    return enum_flags(m_value | f.m_value);
}

/////////////////////////////////////////////////////////////////////////////////////////

template<typename Enum>
RTTR_CONSTEXPR RTTR_INLINE enum_flags<Enum> enum_flags<Enum>::operator^(Enum f) const RTTR_NOEXCEPT
{
    return enum_flags(m_value ^ static_cast<enum_type>(f));
}

/////////////////////////////////////////////////////////////////////////////////////////

template<typename Enum>
RTTR_CONSTEXPR RTTR_INLINE enum_flags<Enum> enum_flags<Enum>::operator^(enum_flags f) const RTTR_NOEXCEPT
{
    return enum_flags(m_value ^ f.m_value);
}

/////////////////////////////////////////////////////////////////////////////////////////

template<typename Enum>
RTTR_CONSTEXPR RTTR_INLINE enum_flags<Enum> enum_flags<Enum>::operator&(Enum f) const RTTR_NOEXCEPT
{
    return enum_flags(m_value & static_cast<enum_type>(f));
}

/////////////////////////////////////////////////////////////////////////////////////////

template<typename Enum>
RTTR_CONSTEXPR RTTR_INLINE enum_flags<Enum> enum_flags<Enum>::operator&(int mask) const RTTR_NOEXCEPT
{
    return enum_flags(m_value & mask);
}

/////////////////////////////////////////////////////////////////////////////////////////

template<typename Enum>
RTTR_CONSTEXPR RTTR_INLINE enum_flags<Enum> enum_flags<Enum>::operator&(uint32_t mask) const RTTR_NOEXCEPT
{
    return enum_flags(m_value & mask);
}

/////////////////////////////////////////////////////////////////////////////////////////

template<typename Enum>
RTTR_CONSTEXPR RTTR_INLINE enum_flags<Enum> enum_flags<Enum>::operator~() const RTTR_NOEXCEPT
{
    return enum_flags(~m_value);
}

/////////////////////////////////////////////////////////////////////////////////////////

template<typename Enum>
RTTR_CONSTEXPR RTTR_INLINE bool enum_flags<Enum>::operator!() const RTTR_NOEXCEPT
{
    return (!m_value);
}

/////////////////////////////////////////////////////////////////////////////////////////

template<typename Enum>
RTTR_CONSTEXPR RTTR_INLINE bool enum_flags<Enum>::test_flag(Enum flag) const RTTR_NOEXCEPT
{
    return ((m_value & static_cast<enum_type>(flag)) == static_cast<enum_type>(flag) &&
            (static_cast<enum_type>(flag) != 0 || m_value == static_cast<enum_type>(flag)) );
}

/////////////////////////////////////////////////////////////////////////////////////////

} // end namespace rttr

#endif // RTTR_ENUM_FLAGS_IMPL_H_
