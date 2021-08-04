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

#ifndef RTTR_VARIANT_ASSOCIATIVE_CREATOR_H_
#define RTTR_VARIANT_ASSOCIATIVE_CREATOR_H_

#include "rttr/detail/base/core_prerequisites.h"
#include "rttr/detail/misc/misc_type_traits.h"
#include "rttr/detail/misc/std_type_traits.h"
#include "rttr/associative_mapper.h"
#include "rttr/wrapper_mapper.h"

#include <memory>

namespace rttr
{
namespace detail
{

class variant_associative_view_private;

template<typename T>
using can_create_associative_view = std::integral_constant<bool, is_associative_container<raw_type_t<wrapper_address_return_type_t<T>>>::value>;

/////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
enable_if_t<can_create_associative_view<T>::value, variant_associative_view_private>
create_variant_associative_view(T&& value);

template<typename T>
enable_if_t<!can_create_associative_view<T>::value, variant_associative_view_private>
create_variant_associative_view(T&& value);

/////////////////////////////////////////////////////////////////////////////////////////

} // end namespace detail
} // end namespace rttr

#include "rttr/detail/variant_associative_view/variant_associative_view_creator_impl.h"

#endif // RTTR_VARIANT_ASSOCIATIVE_CREATOR_H_
