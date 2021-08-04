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

#ifndef RTTR_RTTR_ENABLE_H_
#define RTTR_RTTR_ENABLE_H_

#include <type_traits>

#include "rttr/type.h"
#include "rttr/detail/misc/misc_type_traits.h"

namespace rttr
{
namespace detail
{
#ifndef DOXYGEN
    template<typename... U> struct type_list;
#endif
} // end namespace detail
} // end namespace rttr

#ifdef DOXYGEN

/*!
 * \brief This macro is necessary in order to retrieve type information about the
 *        inheritance graph of a class. When there is no inheritance graph, the macro is **not** needed, e.g. for POD types.
 *
 * Put the macro inside every class, where you need the complete inheritance information about the class type.
 *
 * \code{.cpp}
 * struct Base
 * {
 *   RTTR_ENABLE()
 * };
 * \endcode
 *
 * Place the macro \ref RTTR_ENABLE() somewhere in the class, it doesn't matter if its under the public,
 * protected or private class accessor section.
 *
 * Into the derived class you put the same macro, but now as argument the name of the parent class.
 * Which is in this case `Base`.
 * \code{.cpp}
 *   struct Derived : Base
 *   {
 *     RTTR_ENABLE(Base)
 *   };
 * \endcode
 *
 *  When you use multiple inheritance you simply separate every class with a comma.
 * \code{.cpp}
 *   struct MultipleDerived : Base, Other
 *   {
 *     RTTR_ENABLE(Base, Other)
 *   };
 * \endcode
 *
 * \remark Without this macro, it will not be possible to use \ref rttr::rttr_cast "rttr_cast" or
 *         meta information in the type class, like: \ref rttr::type::get_base_classes() "get_base_classes()" or
 *         \ref rttr::type::get_derived_classes() "get_derived_classes()".
 */
#define RTTR_ENABLE(...)

#else

#define TYPE_LIST(...)      rttr::detail::type_list<__VA_ARGS__>

#define RTTR_ENABLE(...) \
public:\
    virtual RTTR_INLINE rttr::type get_type() const { return rttr::detail::get_type_from_instance(this); }  \
    virtual RTTR_INLINE void* get_ptr() { return reinterpret_cast<void*>(this); } \
    virtual RTTR_INLINE rttr::detail::derived_info get_derived_info() { return {reinterpret_cast<void*>(this), rttr::detail::get_type_from_instance(this)}; } \
    using base_class_list = TYPE_LIST(__VA_ARGS__); \
private:

#endif // DOXYGEN

#endif // RTTR_RTTR_ENABLE_H_
