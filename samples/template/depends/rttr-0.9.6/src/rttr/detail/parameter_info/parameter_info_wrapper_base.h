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

#ifndef RTTR_PARAMETER_INFO_WRAPPER_BASE_H_
#define RTTR_PARAMETER_INFO_WRAPPER_BASE_H_

#include "rttr/detail/base/core_prerequisites.h"
#include "rttr/type.h"

namespace rttr
{
namespace detail
{

class RTTR_API parameter_info_wrapper_base
{
    public:
        parameter_info_wrapper_base() RTTR_NOEXCEPT;
        virtual ~parameter_info_wrapper_base();

        virtual string_view get_name() const RTTR_NOEXCEPT = 0;
        virtual type get_type() const RTTR_NOEXCEPT = 0;
        virtual bool has_default_value() const RTTR_NOEXCEPT = 0;
        virtual variant get_default_value() const = 0;
        virtual uint32_t get_index() const RTTR_NOEXCEPT = 0;
};

} // end namespace detail
} // end namespace rttr

#endif // RTTR_PARAMETER_INFO_WRAPPER_BASE_H_
