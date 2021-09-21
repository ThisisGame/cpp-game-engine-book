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

#ifndef RTTR_FILTER_ITEM_H_
#define RTTR_FILTER_ITEM_H_

#include "rttr/detail/base/core_prerequisites.h"
#include "rttr/enum_flags.h"

namespace rttr
{

/*!
 * The \ref filter_item enum is used to filter the search for certain members in \ref type "types".
 *
 * Combine the enum value together with the bitwise OR operator.
 *
 * \remark In order to get a return, you must specify **filter_item::instance_item** or **filter_item::static_item**
 *         along with an accessor **filter_item::public_access** or **filter_item::non_public_access**.
 *
 * \see \ref type::get_properties(filter_items) const
 */
enum class filter_item
{
    /*!
     * \brief Includes all members in the search, which needs an instance to be invoked.
     */
    instance_item = 1,

    /*!
     * \brief Includes all static members in the search.
     *        Static members doesn't need any instance for invoking.
     */
    static_item = 2,

    /*!
     * \brief Includes all public specified members in the search.
     *
     * \see access_levels
     */
    public_access = 4,

    /*!
     * \brief Includes all non public (*private* or *protected*) members in the search.
     *
     * \see access_levels
     */
    non_public_access = 8,

    /*!
     * \brief Returns all members declared for the current type;
     *        inherited members are not returned.
     */
    declared_only = 16
};

RTTR_DECLARE_FLAGS(filter_items, filter_item)
RTTR_DECLARE_ENUM_FLAGS_OPERATORS(filter_items)

} // end namespace rttr

#endif // RTTR_FILTER_ITEM_H_
