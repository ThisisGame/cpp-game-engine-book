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

#ifndef RTTR_ENUMERATION_WRAPPER_BASE_H_
#define RTTR_ENUMERATION_WRAPPER_BASE_H_

#include "rttr/detail/base/core_prerequisites.h"
#include "rttr/detail/metadata/metadata_handler.h"
#include "rttr/variant.h"
#include "rttr/type.h"

#include <string>
#include <vector>
#include <initializer_list>

namespace rttr
{
class argument;

namespace detail
{

/*!
 * Abstract class for a method.
 *
 * This is the base class for all methods.
 * You can invoke the method.
 */
class RTTR_API enumeration_wrapper_base
{
    public:
        enumeration_wrapper_base() RTTR_NOEXCEPT;
        virtual ~enumeration_wrapper_base() RTTR_NOEXCEPT;

        type get_declaring_type() const RTTR_NOEXCEPT;

        virtual bool is_valid() const RTTR_NOEXCEPT;

        virtual type get_underlying_type() const RTTR_NOEXCEPT;

        virtual type get_type() const RTTR_NOEXCEPT;

        virtual array_range<string_view> get_names() const RTTR_NOEXCEPT;

        virtual array_range<variant> get_values() const RTTR_NOEXCEPT;

        virtual string_view value_to_name(argument& value) const;

        virtual variant name_to_value(string_view name) const;

        void set_declaring_type(type declaring_type) RTTR_NOEXCEPT;

        virtual variant get_metadata(const variant& key) const;
    private:
        type m_declaring_type;
};

} // end namespace detail
} // end namespace rttr

#endif // RTTR_ENUMERATION_WRAPPER_BASE_H_
