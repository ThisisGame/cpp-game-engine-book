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

#include <rttr/type>
#include <catch/catch.hpp>

using namespace rttr;

struct instance_base
{
    RTTR_ENABLE()
};

struct instance_derived : instance_base
{
    RTTR_ENABLE(instance_base)
};


////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("instance - empty", "[instance]")
{
    instance obj;
    CHECK(obj.is_valid() == false);
    CHECK(obj.get_type().is_valid() == false);

    CHECK(obj.get_derived_type().is_valid() == false);
    CHECK(obj.get_wrapped_instance().is_valid() == false);
}

////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("instance - valid", "[instance]")
{
    std::string text = "test";
    instance obj = text;
    CHECK(obj.is_valid() == true);
    CHECK(obj.get_type() == type::get<std::string>());

    CHECK(obj.get_derived_type() == type::get<std::string>());
    CHECK(obj.get_wrapped_instance().is_valid() == false);
}

////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("instance - derived", "[instance]")
{
    instance_derived d;
    instance_base& base = d;
    instance obj = base;
    CHECK(obj.is_valid() == true);
    CHECK(obj.get_type() == type::get<instance_base>());

    CHECK(obj.get_derived_type() == type::get<instance_derived>());
    CHECK(obj.get_wrapped_instance().is_valid() == false);
}

////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("instance - wrapped derived type", "[instance]")
{
    std::shared_ptr<instance_base> b = std::make_shared<instance_derived>();
    instance obj = b;
    CHECK(obj.is_valid() == true);
    CHECK(obj.get_type() == type::get<std::shared_ptr<instance_base>>());

    CHECK(obj.get_derived_type() == type::get<std::shared_ptr<instance_base>>());
    CHECK(obj.get_wrapped_instance().is_valid() == true);
    CHECK(obj.get_wrapped_instance().get_type() == type::get<instance_base*>());
    CHECK(obj.get_wrapped_instance().get_derived_type() == type::get<instance_derived>());
}

/////////////////////////////////////////////////////////////////////////////////////////

