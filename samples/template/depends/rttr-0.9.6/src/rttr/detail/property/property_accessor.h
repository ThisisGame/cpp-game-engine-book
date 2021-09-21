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

#ifndef RTTR_PROPERTY_ACCESSOR_H_
#define RTTR_PROPERTY_ACCESSOR_H_

namespace rttr
{
namespace detail
{

/////////////////////////////////////////////////////////////////////////////////////////
// We have to use this property_accessor helper struct, because of a bug in MSVC compiler
// "ambiguous call to overloaded function when using raw arrays"

template<typename T>
struct property_accessor
{
    static bool set_value(T& prop, const T& arg)
    {
        prop = arg;
        return true;
    }
};

/////////////////////////////////////////////////////////////////////////////////////////

template<typename T, std::size_t N>
struct property_accessor<T[N]>
{
    static bool set_value(T (& target)[N], const T (& src)[N])
    {
#if RTTR_COMPILER == RTTR_COMPILER_MSVC  && RTTR_COMP_VER <= 1800
        using array_t = remove_const_t<remove_reference_t<decltype(src)>>;
        copy_array(const_cast<array_t&>(src), target);
#else
        copy_array(src, target);
#endif
        return true;
    }
};

/////////////////////////////////////////////////////////////////////////////////////////

template<typename T, std::size_t N>
struct property_accessor<T(*)[N]>
{

    static bool set_value(T (*target)[N], const T (*src)[N])
    {
        copy_array(src, target);
        return true;
    }
};

/////////////////////////////////////////////////////////////////////////////////////////

} // end namespace detail
} // end namespace rttr

#endif // RTTR_PROPERTY_ACCESSOR_H_
