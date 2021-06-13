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

#ifndef RTTR_REGISTRATION_FRIEND_H_
#define RTTR_REGISTRATION_FRIEND_H_

#include "rttr/detail/base/core_prerequisites.h"

namespace rttr
{
namespace detail
{
template<typename Ctor_Type, typename Policy, typename Accessor, typename Arg_Indexer>
struct constructor_invoker;
}
}

static void rttr_auto_register_reflection_function_();

#ifdef DOXYGEN

/*!
 * When you have a class and the method or property is declared in private scope,
 * then you can still register this item when you insert the macro: \ref RTTR_REGISTRATION_FRIEND inside the class.
 *
 * See following example code:
 * \code{.cpp}
 * #include <rttr/registration_friend> // important!
 * class foo
 * {
 * private:
 *     int value;
 *
 *     RTTR_REGISTRATION_FRIEND
 * };
 *
 * RTTR_REGISTRATION
 * {
 *     rttr::registration::class_<foo>("foo")
 *                         .property("value", &foo:value); // no compile error, because we use 'RTTR_REGISTRATION_FRIEND' inside 'foo'
 * }
 * \endcode
 */
#define RTTR_REGISTRATION_FRIEND
#else
#define RTTR_REGISTRATION_FRIEND friend void ::rttr_auto_register_reflection_function_();                               \
                                 template<typename Ctor_Type, typename Policy, typename Accessor, typename Arg_Indexer> \
                                 friend struct rttr::detail::constructor_invoker;
#endif

#endif // RTTR_REGISTRATION_FRIEND_H_
