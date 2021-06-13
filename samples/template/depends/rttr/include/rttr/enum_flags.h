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

#ifndef RTTR_ENUM_FLAGS_H_
#define RTTR_ENUM_FLAGS_H_

#include "rttr/detail/base/core_prerequisites.h"
#include "rttr/detail/misc/std_type_traits.h"

#include <type_traits>
#include <cstdint>

namespace rttr
{

namespace detail
{
class enum_flag;
}

/*!
 * The \ref enum_flags class template is used to store OR-combinations of enum values in a type-safe way.
 *
 * The values are stored internally inside an integer (unsigned or signed, depending on the underlying type of the enum).
 * Using values from other enums or raw integer (except 0) with this class will result in a compile time error.
 *
 * In order to use this class with your own enum, use RTTR_DECLARE_FLAGS() and RTTR_DECLARE_ENUM_FLAGS_OPERATORS().
 *
 * Typical Usage
 * -------------
 *
 * \code{.cpp}
 *
 * enum class my_option
 * {
 *     ALL_DISABLED    = 0,
 *     OPTION_1        = 1,
 *     OPTION_2        = 2,
 *     OPTION_3        = 4,
 *     OPTION_4        = 8
 * };
 *
 * RTTR_DECLARE_FLAGS(my_options, my_option)
 * RTTR_DECLARE_ENUM_FLAGS_OPERATORS(my_options)
 *
 * void my_func(my_options flags)
 * {
 *     if (flags.test_flag(my_option::OPTION_1)
 *     {
 *         // ...
 *     }
 *
 *     if (flags.test_flag(my_option::OPTION_2)
 *     {
 *         // ...
 *     }
* }
 *
 * \endcode
 */
template<typename Enum>
class enum_flags
{
    static_assert(sizeof(Enum) <= sizeof(int32_t), "Cannot store enums value with the given type."
                                                   "Please use an enum which fits into an 'int32_t'." );
    public:
        using type = Enum;
        using enum_type = detail::conditional_t<std::is_signed<typename std::underlying_type<Enum>::type>::value,
                                                int32_t,
                                                uint32_t>;
        using zero = enum_type*;

        /*!
         * Creates an enum_flags object with no flags set.
         */
        RTTR_CONSTEXPR RTTR_INLINE enum_flags(zero = nullptr) RTTR_NOEXCEPT;

        /*!
         * Creates a enum_flags object with the given flag \p flag.
         */
        RTTR_CONSTEXPR RTTR_INLINE enum_flags(Enum flag) RTTR_NOEXCEPT;

        /*!
         * Creates an enum_flags object initialized with the given integer value \p value.
         *
         * \remark enum_flag is a wrapper class around an integer to avoid creation
         *          of enum_flags object from enum values.
         */
        RTTR_CONSTEXPR RTTR_INLINE enum_flags(detail::enum_flag v) RTTR_NOEXCEPT;

        /////////////////////////////////////////////////////////////////////////////////

        /*!
         * \brief Performs a bitwise `AND` operation with \p mask
         *        and store the result in this object.
         *
         * \return A reference to this object.
         */
        RTTR_CXX14_CONSTEXPR RTTR_INLINE enum_flags& operator&=(int mask) RTTR_NOEXCEPT;

        /*!
         * \brief Performs a bitwise `AND` operation with \p mask
         *        and store the result in this object.
         *
         * \return A reference to this object.
         */
        RTTR_CXX14_CONSTEXPR RTTR_INLINE enum_flags& operator&=(uint32_t mask) RTTR_NOEXCEPT;

        /*!
         * \brief Performs a bitwise `AND` operation with \p mask
         *        and store the result in this object.
         *
         * \return A reference to this object.
         */
        RTTR_CXX14_CONSTEXPR RTTR_INLINE enum_flags& operator&=(Enum mask) RTTR_NOEXCEPT;

        /*!
         * \brief Performs a bitwise `OR` operation with \p f
         *        and store the result in this object.
         *
         * \return A reference to this object.
         */
        RTTR_CXX14_CONSTEXPR RTTR_INLINE enum_flags& operator|=(enum_flags f) RTTR_NOEXCEPT;

        /*!
         * \brief Performs a bitwise `OR` operation with \p f
         *        and store the result in this object.
         *
         * \return A reference to this object.
         */
        RTTR_CXX14_CONSTEXPR RTTR_INLINE enum_flags& operator|=(Enum f) RTTR_NOEXCEPT;

        /*!
         * \brief Performs a bitwise `XOR` operation with \p f
         *        and store the result in this object.
         *
         * \return A reference to this object.
         */
        RTTR_CXX14_CONSTEXPR RTTR_INLINE enum_flags& operator^=(enum_flags f) RTTR_NOEXCEPT;

        /*!
         * \brief Performs a bitwise `XOR` operation with \p f
         *        and store the result in this object.
         *
         * \return A reference to this object.
         */
        RTTR_CXX14_CONSTEXPR RTTR_INLINE enum_flags& operator^=(Enum f) RTTR_NOEXCEPT;

        /*!
         * \brief Performs a bitwise `XOR` operation with \p f
         *        and store the result in this object.
         *
         * \return A reference to this object.
         */
        RTTR_CONSTEXPR RTTR_INLINE operator enum_type() const RTTR_NOEXCEPT;

        /////////////////////////////////////////////////////////////////////////////////

        /*!
         * \brief Performs a bitwise `OR` operation on this object and \p f
         *        and return the result as new enum_flags object.
         *
         * \return A enum_flags object containing the result of the bitwise `OR` operation on this object and \p f.
         */
        RTTR_CONSTEXPR RTTR_INLINE enum_flags operator|(Enum f) const RTTR_NOEXCEPT;


        /*!
         * \brief Performs a bitwise `OR` operation on this object and \p f
         *        and return the result as new enum_flags object.
         *
         * \return A enum_flags object containing the result of the bitwise `OR` operation on this object and \p f.
         */
        RTTR_CONSTEXPR RTTR_INLINE enum_flags operator|(enum_flags f) const RTTR_NOEXCEPT;

        /*!
         * \brief Performs a bitwise `XOR` operation on this object and \p f
         *        and return the result as new enum_flags object.
         *
         * \return A enum_flags object containing the result of the bitwise `XOR` operation on this object and \p f.
         */
        RTTR_CONSTEXPR RTTR_INLINE enum_flags operator^(Enum f) const RTTR_NOEXCEPT;

        /*!
         * \brief Performs a bitwise `XOR` operation on this object and \p f
         *        and return the result as new enum_flags object.
         *
         * \return A enum_flags object containing the result of the bitwise `XOR` operation on this object and \p f.
         */
        RTTR_CONSTEXPR RTTR_INLINE enum_flags operator^(enum_flags f) const RTTR_NOEXCEPT;

        /*!
         * \brief Performs a bitwise `AND` operation on this object and \p f
         *        and return the result as new enum_flags object.
         *
         * \return A enum_flags object containing the result of the bitwise `AND` operation on this object and \p f.
         */
        RTTR_CONSTEXPR RTTR_INLINE enum_flags operator&(Enum f) const RTTR_NOEXCEPT;


        /*!
         * \brief Performs a bitwise `AND` operation on this object and \p mask
         *        and return the result as new enum_flags object.
         *
         * \return A enum_flags object containing the result of the bitwise `AND` operation on this object and \p mask.
         */
        RTTR_CONSTEXPR RTTR_INLINE enum_flags operator&(int mask) const RTTR_NOEXCEPT;

        /*!
         * \brief Performs a bitwise `AND` operation on this object and \p mask
         *        and return the result as new enum_flags object.
         *
         * \return A enum_flags object containing the result of the bitwise `AND` operation on this object and \p mask.
         */
        RTTR_CONSTEXPR RTTR_INLINE enum_flags operator&(uint32_t mask) const RTTR_NOEXCEPT;

        /*!
         * \brief Performs a bitwise negation of the current object
         *        and return the result as new enum_flags object.
         *
         * \return A enum_flags object that contains the bitwise negation of this object.
         */
        RTTR_CONSTEXPR RTTR_INLINE enum_flags operator~() const RTTR_NOEXCEPT;

        /*!
         * \brief This will test whether a flag was set or not.
         *
         * \return `true`, when no flag is set, otherwise `false`.
         */
        RTTR_CONSTEXPR RTTR_INLINE bool operator!() const RTTR_NOEXCEPT;

        /*!
         * \brief This will test whether the given flag \p flag was set.
         *
         * \return `true`, when the flag is set, otherwise `false`.
         */
        RTTR_CONSTEXPR RTTR_INLINE bool test_flag(Enum flag) const RTTR_NOEXCEPT;

    private:
        enum_type m_value;
};

namespace detail
{

/*!
 * Simple helper class to avoid the creation of `OR` combination with enum values and raw integers.
 *
 */
class invalid_enum_flag
{
    public:
        RTTR_CONSTEXPR RTTR_INLINE explicit invalid_enum_flag(int v){}
};

} // end namespace detail

#ifdef DOXYGEN

/*!
 * This macro expands to:
 * \code{.cpp}
 *    using Flags = rttr::enum_flags<Enum>;
 * \endcode
 *
 * \p Flags is the name of the alias for `rttr::enum_flags<Enum>`.
 *
 * \p Enum is the name of the existing enum.
 */
#define RTTR_DECLARE_FLAGS(Flags, Enum)

/*!
 * This macro declares the a global operator `|` for enums of type \ref enum_flags<T>
 *
 * \see RTTR_DECLARE_FLAGS()
 */
#define RTTR_DECLARE_ENUM_FLAGS_OPERATORS(Flags)

#else

#define RTTR_DECLARE_FLAGS(Flags, Enum) \
using Flags = rttr::enum_flags<Enum>;

#define RTTR_DECLARE_ENUM_FLAGS_OPERATORS(Flags)                                                                                        \
RTTR_CONSTEXPR RTTR_INLINE rttr::enum_flags<Flags::type> operator|(Flags::type lhs, Flags::type rhs) RTTR_NOEXCEPT                      \
{                                                                                                                                       \
    return (rttr::enum_flags<Flags::type>(lhs) | rhs);                                                                                  \
}                                                                                                                                       \
RTTR_CONSTEXPR RTTR_INLINE rttr::enum_flags<Flags::type> operator|(Flags::type lhs, rttr::enum_flags<Flags::type> rhs) RTTR_NOEXCEPT    \
{                                                                                                                                       \
    return (rhs | lhs);                                                                                                                 \
}                                                                                                                                       \
RTTR_CONSTEXPR RTTR_INLINE rttr::detail::invalid_enum_flag operator|(Flags::type lhs, int rhs)                                          \
{                                                                                                                                       \
    return rttr::detail::invalid_enum_flag(int(lhs) | rhs);                                                                             \
}

#endif

} // end namespace rttr

#include "rttr/detail/impl/enum_flags_impl.h"

#endif // RTTR_ENUM_FLAGS_H_
