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

#ifndef RTTR_DESTRUCTOR_WRAPPER_H_
#define RTTR_DESTRUCTOR_WRAPPER_H_

#include "rttr/detail/base/core_prerequisites.h"
#include "rttr/detail/destructor/destructor_wrapper_base.h"
#include "rttr/variant.h"

namespace rttr
{
namespace detail
{

template<typename ClassType>
class destructor_wrapper : public destructor_wrapper_base
{
    public:
        type get_declaring_type() const RTTR_NOEXCEPT { return type::get<ClassType>(); }

        type get_destructed_type() const RTTR_NOEXCEPT { return type::get<ClassType*>(); }

        bool is_valid() const RTTR_NOEXCEPT { return true; }

        bool invoke(variant& obj) const RTTR_NOEXCEPT
        {
            if (obj.is_type<ClassType*>())
            {
                // although an exception can occur when calling 'delete',
                // nobody should throw an exception in destructor, so we don't catch the exception
                // and just terminate the application
                delete obj.get_value<ClassType*>();
                obj = variant();
                return true;
            }
            else
            {
                return false;
            }
        }
};

} // end namespace detail
} // end namespace rttr

#endif // RTTR_DESTRUCTOR_WRAPPER_H_
