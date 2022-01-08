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

#ifndef RTTR_TYPE_NAME_H_
#define RTTR_TYPE_NAME_H_

#include "rttr/detail/base/core_prerequisites.h"

#include "rttr/string_view.h"

/////////////////////////////////////////////////////////////////////////////////

#define RTTR_REGISTRATION_FUNC_EXTRACT_VARIABLES(begin_skip, end_skip)      \
namespace rttr                                                              \
{                                                                           \
namespace detail                                                            \
{                                                                           \
    RTTR_STATIC_CONSTEXPR std::size_t skip_size_at_begin = begin_skip;      \
    RTTR_STATIC_CONSTEXPR std::size_t skip_size_at_end   = end_skip;        \
}                                                                           \
}

/////////////////////////////////////////////////////////////////////////////////

#if RTTR_COMPILER == RTTR_COMPILER_MSVC
    // sizeof("const char *__cdecl rttr::detail::f<"), sizeof(">(void)")
#ifdef RTTR_NO_CXX11_NOEXCEPT
    RTTR_REGISTRATION_FUNC_EXTRACT_VARIABLES(36, 7)
#else
    RTTR_REGISTRATION_FUNC_EXTRACT_VARIABLES(36, 16)  // with " noexcept"
#endif

#elif RTTR_COMPILER == RTTR_COMPILER_GNUC
    // sizeof("const char* rttr::detail::f() [with T = "), sizeof("]")
    RTTR_REGISTRATION_FUNC_EXTRACT_VARIABLES(40, 1)
#elif RTTR_COMPILER == RTTR_COMPILER_CLANG || RTTR_COMPILER == RTTR_COMPILER_APPLECLANG
    // sizeof("const char* rttr::detail::f() [T = "), sizeof("]")
    RTTR_REGISTRATION_FUNC_EXTRACT_VARIABLES(35, 1)
#else
#   error "This compiler does not supported extracting a function signature via preprocessor!"
#endif

/////////////////////////////////////////////////////////////////////////////////

namespace rttr
{
namespace detail
{

/////////////////////////////////////////////////////////////////////////////////

RTTR_LOCAL RTTR_INLINE const char* extract_type_signature(const char* signature) RTTR_NOEXCEPT
{
//    static_assert(N > skip_size_at_begin + skip_size_at_end, "RTTR is misconfigured for your compiler.")
    return &signature[rttr::detail::skip_size_at_begin];
}

/////////////////////////////////////////////////////////////////////////////////

template<typename T>
RTTR_LOCAL RTTR_INLINE const char* f() RTTR_NOEXCEPT
{
    return extract_type_signature(
    #if RTTR_COMPILER == RTTR_COMPILER_MSVC
                                                            __FUNCSIG__
    #elif RTTR_COMPILER == RTTR_COMPILER_GNUC
                                                            __PRETTY_FUNCTION__
    #elif RTTR_COMPILER == RTTR_COMPILER_CLANG || RTTR_COMPILER == RTTR_COMPILER_APPLECLANG
                                                            __PRETTY_FUNCTION__
    #else
        #error "Don't know how the extract type signatur for this compiler! Abort! Abort!"
    #endif
                                   );
}

/////////////////////////////////////////////////////////////////////////////////

RTTR_LOCAL RTTR_INLINE std::size_t get_size(const char* s) RTTR_NOEXCEPT
{
    return ( std::char_traits<char>::length(s) - rttr::detail::skip_size_at_end);
}

/////////////////////////////////////////////////////////////////////////////////

template<typename T>
RTTR_LOCAL RTTR_INLINE string_view get_type_name() RTTR_NOEXCEPT
{
    return string_view(f<T>(), get_size(f<T>()));
}

/////////////////////////////////////////////////////////////////////////////////

} // end namespace detail
} // end namespace rttr

#endif // RTTR_TYPE_NAME_H_
