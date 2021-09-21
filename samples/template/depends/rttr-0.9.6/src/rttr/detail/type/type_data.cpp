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

#include "rttr/type.h"
#include "rttr/detail/type/type_data.h"

namespace rttr
{
namespace detail
{

/////////////////////////////////////////////////////////////////////////////////////////

static class_data& get_invalid_type_class_data() RTTR_NOEXCEPT
{
    static std::unique_ptr<class_data> info = ::rttr::detail::make_unique<class_data>(nullptr, std::vector<type>());
    return (*info.get());
}


/////////////////////////////////////////////////////////////////////////////////////////

static type_data& get_invalid_type_data_impl() RTTR_NOEXCEPT
{
    static type_data instance{ nullptr, nullptr,
                               nullptr,
                               std::string(""), string_view(),
                               0, 0,
                               &create_invalid_variant_policy::create_variant,
                               &base_classes<void>::get_types,
                               nullptr,
                               nullptr,
                               get_create_wrapper_func<void>(),
                               &get_invalid_type_class_data,
                               false,
                               type_trait_value{0}};

    instance.raw_type_data  = &instance;
    instance.wrapped_type   = &instance;
    instance.array_raw_type = &instance;

    return instance;
}

/////////////////////////////////////////////////////////////////////////////////////////

type_data* get_invalid_type_data() RTTR_NOEXCEPT
{
    static auto instance = &get_invalid_type_data_impl();
    return instance;
}

/////////////////////////////////////////////////////////////////////////////////////////

} // end namespace detail
} // end namespace rttr


