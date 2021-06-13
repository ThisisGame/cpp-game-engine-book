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

#ifndef RTTR_NUMBER_CONVERSION_H_
#define RTTR_NUMBER_CONVERSION_H_

#include "rttr/detail/base/core_prerequisites.h"
#include <limits>

namespace rttr
{
namespace detail
{

/////////////////////////////////////////////////////////////////////////////////////////

/*!
 * \brief Identity function
 *
 */
template<typename F, typename T>
typename std::enable_if<std::is_same<F, T>::value, bool>::type
convert_to(const F& from, T& to)
{
    to = from;
    return true;
}

template<typename F, typename T>
using is_integer = std::integral_constant<bool, !std::is_same<F, T>::value &&
                                                std::is_integral<F>::value && std::is_integral<T>::value>;

/////////////////////////////////////////////////////////////////////////////////////////

template<typename F, typename T>
typename std::enable_if<is_integer<F, T>::value &&
                        std::numeric_limits<F>::is_signed &&
                        !std::numeric_limits<T>::is_signed,
                        bool>::type
convert_to(const F& from, T& to)
{
    if (from < 0)
        return false; // value too small

    if (static_cast<typename std::make_unsigned<F>::type>(from) > std::numeric_limits<T>::max())
        return false; // value too large

    to = static_cast<T>(from);
    return true;
}

/////////////////////////////////////////////////////////////////////////////////////////

template<typename F, typename T>
typename std::enable_if<is_integer<F, T>::value &&
                        !std::numeric_limits<F>::is_signed &&
                        std::numeric_limits<T>::is_signed,
                        bool>::type
convert_to(const F& from, T& to)
{
    if (from > static_cast<typename std::make_unsigned<T>::type>(std::numeric_limits<T>::max()))
        return false; // value too large

    to = static_cast<T>(from);
    return true;
}

/////////////////////////////////////////////////////////////////////////////////////////

template<typename F, typename T>
typename std::enable_if<is_integer<F, T>::value &&
                        std::numeric_limits<F>::is_signed &&
                        std::numeric_limits<T>::is_signed,
                        bool>::type
convert_to(const F& from, T& to)
{
    if (from > std::numeric_limits<T>::max())
        return false; // value too large
    else if (from < std::numeric_limits<T>::min())
        return false; // value too small

    to = static_cast<T>(from);
    return true;
}

/////////////////////////////////////////////////////////////////////////////////////////

template<typename F, typename T>
typename std::enable_if<is_integer<F, T>::value &&
                        !std::numeric_limits<F>::is_signed &&
                        !std::numeric_limits<T>::is_signed,
                        bool>::type
convert_to(const F& from, T& to)
{
    if (from > std::numeric_limits<T>::max())
        return false; // value too large

    to = static_cast<T>(from);
    return true;
}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
// floating point conversion

template<typename F, typename T>
typename std::enable_if<std::is_floating_point<F>::value &&
                        std::is_integral<T>::value && std::numeric_limits<T>::is_signed,
                        bool>::type
convert_to(const F& from, T& to)
{
    if (from > std::numeric_limits<T>::max())
        return false; // value too large
    else if (from < -std::numeric_limits<T>::max())
        return false; // value to small

    to = static_cast<T>(from);
    return true;
}

/////////////////////////////////////////////////////////////////////////////////////////

template<typename F, typename T>
typename std::enable_if<std::is_floating_point<F>::value &&
                        std::is_integral<T>::value && !std::numeric_limits<T>::is_signed,
                        bool>::type
convert_to(const F& from, T& to)
{
    if (from < 0 || from > std::numeric_limits<T>::max())
        return false; // value too large

    to = static_cast<T>(from);
    return true;
}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
// string conversion

template<typename F, typename T>
typename std::enable_if<std::is_same<T, std::string>::value,
                        bool>::type
convert_to(const F& from, T& to)
{
    bool ok = false;
    to = to_string(from, &ok);
    return ok;
}

/////////////////////////////////////////////////////////////////////////////////////////

} // end namespace detail
} // end namespace rttr

#endif // RTTR_NUMBER_CONVERSION_H_
