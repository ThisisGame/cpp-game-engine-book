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

#include <rttr/registration>
#include <catch/catch.hpp>

using namespace rttr;

struct ctor_test
{
    ctor_test(){}
    ctor_test(const ctor_test& other) {}
    ctor_test(int, double) {}

    static ctor_test create_object() { return ctor_test(); }
};

static ctor_test global_create_object() { return ctor_test(); }

RTTR_REGISTRATION
{
    registration::class_<ctor_test>("ctor_test")
        .constructor<>()
        (
            policy::ctor::as_object
        )
        .constructor<const ctor_test&>()
        (
            policy::ctor::as_object
        )
        .constructor<int, double>()
        (
            policy::ctor::as_object
        )
        .constructor(&ctor_test::create_object)
        .constructor(&global_create_object);
}

////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("constructor - retrieve", "[constructor]")
{
    type t = type::get<ctor_test>();
    REQUIRE(t.is_valid() == true);

    SECTION("retrieve default ctor")
    {
        constructor ctor = t.get_constructor();
        CHECK(ctor.is_valid() == true);
        CHECK(static_cast<bool>(ctor) == true);
    }

    SECTION("retrieve copy-ctor")
    {
        constructor ctor = t.get_constructor({type::get<ctor_test>()});
        CHECK(ctor.is_valid() == true);
        CHECK(static_cast<bool>(ctor) == true);
    }

    SECTION("retrieve custom ctor")
    {
        constructor ctor = t.get_constructor({type::get<int>(), type::get<double>()});
        CHECK(ctor.is_valid() == true);
        CHECK(static_cast<bool>(ctor) == true);
    }

    SECTION("retrieve all ctors")
    {
        auto range = t.get_constructors();
        std::vector<constructor> ctor_list(range.cbegin(), range.cend());
        REQUIRE(ctor_list.size() == 5);
        auto ctor_name = t.get_name();
        // check order
        CHECK(ctor_list[0].get_signature() == std::string(ctor_name) + "( )");
        CHECK(ctor_list[1].get_signature() == std::string(ctor_name) + "( ctor_test const & )");
        CHECK(ctor_list[2].get_signature() == std::string(ctor_name) + "( int, double )");
        CHECK(ctor_list[3].get_signature() == std::string(ctor_name) + "( )");
        CHECK(ctor_list[4].get_signature() == std::string(ctor_name) + "( )");
    }
}

/////////////////////////////////////////////////////////////////////////////////////////
