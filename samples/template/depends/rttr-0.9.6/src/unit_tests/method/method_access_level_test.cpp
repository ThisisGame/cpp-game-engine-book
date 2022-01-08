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

struct method_access_level_test
{
    void method_1() {}
    void method_2() {}
    void method_3() {}

    void default_method(int value = 23) {}
};

/////////////////////////////////////////////////////////////////////////////////////////

RTTR_REGISTRATION
{
    registration::class_<method_access_level_test>("method_access_level_test")
        .method("method_1", &method_access_level_test::method_1) // default policy should be "public_access"
        .method("method_2", &method_access_level_test::method_1, registration::private_access)
        .method("method_3", &method_access_level_test::method_2, registration::protected_access)
        .method("method_4", &method_access_level_test::method_3, registration::public_access)
        // method with metadata
        .method("method_5", &method_access_level_test::method_1) // default with custom data, should be "public_access"
        (
            metadata("23", 43)
        )
        .method("method_6", &method_access_level_test::method_2, registration::private_access)
        (
            metadata("23", 43)
        )
        .method("method_7", &method_access_level_test::method_3, registration::protected_access)
        (
            metadata("23", 43)
        )
        .method("method_8", &method_access_level_test::method_3, registration::public_access)
        (
            metadata("23", 43)
        )
        .method("default_method", &method_access_level_test::default_method, registration::public_access)
        (
            default_arguments(23)
        )
        ;
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("method - access_levels test", "[method]")
{
    type t = type::get_by_name("method_access_level_test");
    REQUIRE(t.is_valid() == true);

    // has to be checked, because get_access_level() default return value is public_access
    CHECK(t.get_method("method_1").is_valid() == true);
    CHECK(t.get_method("method_1").get_access_level() == access_levels::public_access);

    CHECK(t.get_method("method_2").get_access_level() == access_levels::private_access);
    CHECK(t.get_method("method_3").get_access_level() == access_levels::protected_access);
    CHECK(t.get_method("method_4").get_access_level() == access_levels::public_access);

    CHECK(t.get_method("method_5").get_access_level() == access_levels::public_access);
    CHECK(t.get_method("method_6").get_access_level() == access_levels::private_access);
    CHECK(t.get_method("method_7").get_access_level() == access_levels::protected_access);
    CHECK(t.get_method("method_8").get_access_level() == access_levels::public_access);
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("method - NEGATIVE - access_levels test", "[method]")
{
    type t = type::get_by_name("method_access_level_test");
    REQUIRE(t.is_valid() == true);

    REQUIRE(t.get_method("").is_valid() == false);
    CHECK(t.get_method("").get_access_level() == access_levels::public_access);

    REQUIRE(t.get_method("default_method").is_valid() == true);
    CHECK(t.get_method("default_method").get_access_level() == access_levels::public_access);
}

/////////////////////////////////////////////////////////////////////////////////////////
