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

#ifndef RTTR_ACCESS_LEVELS_H_
#define RTTR_ACCESS_LEVELS_H_

#include "rttr/detail/base/core_prerequisites.h"

namespace rttr
{

/*!
 * The \ref access_levels enum represents the three access modifiers, which can be used
 * in classes to encapsulate members access.
 *
 * In contrast to the three static type values in the \ref registration::public_access "registration" class,
 * represents this enum the return value to retrieve the access level at runtime.
 *
 * \see \ref method::get_access_level(), \ref property::get_access_level(), \ref constructor::get_access_level()
 */
enum class access_levels
{
    /*!
     * Declares that this member was registered with `public` access.
     */
    public_access,

    /*!
     * Declares that this member was registered with `protected` access.
     */
    protected_access,

    /*!
     * Declares that this member was registered with `private` access.
     */
    private_access
};

} // end namespace rttr

#endif // RTTR_ACCESS_LEVELS_H_
