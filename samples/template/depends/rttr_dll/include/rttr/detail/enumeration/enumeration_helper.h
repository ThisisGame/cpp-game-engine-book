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

#ifndef RTTR_ENUMERATION_HELPER_H_
#define RTTR_ENUMERATION_HELPER_H_

#include "rttr/detail/base/core_prerequisites.h"
#include "rttr/string_view.h"


namespace rttr
{
class argument;
class variant;

namespace detail
{

/////////////////////////////////////////////////////////////////////////////////////////

/*!
 * \brief Returns the corresponding name of the given enumeration value \p enum_value.
 *        Otherwise an empty string is returned.
 */
RTTR_API string_view get_enumeration_name(const argument& enum_value);

/////////////////////////////////////////////////////////////////////////////////////////

/*!
 * \brief Converts the given string \p from to its corresponding enumeration value.
 *        The result is stored inside \p to.
 *
 * \remark The parameter \p to should contain a variant with \ref type object of the enumeration.
 *         The result is stored then inside this variant.
 */
RTTR_API bool to_enumeration(string_view from, argument& to);

/////////////////////////////////////////////////////////////////////////////////////////

/*!
 * \brief Converts the given variant \p var (containing an arithmetic type)
 *        to its corresponding enumeration value.
 *
 * \remark The parameter \p to should contain a variant with \ref type object of the enumeration.
 *         The result is stored then inside this variant.
 */
RTTR_API bool to_enumeration(const variant& from, argument& to);

/////////////////////////////////////////////////////////////////////////////////////////

/*!
 * \brief Returns `true`, when the given argument \p arg contains a variant with a \ref type
 *        object which represents an enumeration.  Otherwise `false`.
 */
RTTR_API bool is_variant_with_enum(const argument& arg);

/////////////////////////////////////////////////////////////////////////////////////////

} // end namespace detail
} // end namespace rttr

#endif // RTTR_ENUMERATION_HELPER_H_
