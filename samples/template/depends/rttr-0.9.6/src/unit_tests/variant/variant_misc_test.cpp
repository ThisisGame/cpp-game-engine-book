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

#include <catch/catch.hpp>
#include <iostream>
#include <rttr/type>

using namespace rttr;
using namespace std;

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("variant - misc", "[variant]")
{
    SECTION("empty type")
    {
        variant var = 12;

        CHECK(var.is_valid() == true);
        var.clear();

        CHECK(var.is_valid() == false);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("variant - swap", "[variant]")
{
    SECTION("self")
    {
        variant a = 1;
        a.swap(a);

        CHECK(a.is_valid() == true);
    }

    SECTION("both valid")
    {
        variant a = 12;
        variant b = std::string("text");
        a.swap(b);

        CHECK(a.is_type<std::string>() == true);
        CHECK(b.is_type<int>() == true);
    }

    SECTION("left valid")
    {
        variant a = 12;
        variant b;
        a.swap(b);

        CHECK(a.is_valid() == false);
        CHECK(b.is_type<int>() == true);
    }

    SECTION("right valid")
    {
        variant a;
        variant b = 12;
        a.swap(b);

        CHECK(a.is_type<int>() == true);
        CHECK(b.is_valid() == false);
    }

    SECTION("both invalid")
    {
        variant a, b;
        a.swap(b);

        CHECK(a.is_valid() == false);
        CHECK(b.is_valid() == false);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("variant - get_value()", "[variant]")
{
    SECTION("check get_value() non-const version")
    {
        std::string text = "hello world";
        variant var = text;

        auto& value = var.get_value<std::string>();
        value = "world";
        using value_t = detail::remove_reference_t<decltype(value)>;

        static_assert(!std::is_const<value_t>::value, "Provide non-const getter!");

        CHECK(var.get_value<std::string>() == "world");
    }

    SECTION("check get_value() const version")
    {
        std::string text = "hello world";
        const variant var = text;

        auto& value = var.get_value<std::string>();

        using value_t = detail::remove_reference_t<decltype(value)>;
        static_assert(std::is_const<value_t>::value, "Provide non-const getter!");
        CHECK(value == text);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("variant - get_wrapped_value", "[variant]")
{
    int foo = 12;
    variant var = std::ref(foo);
    CHECK(var.get_type().is_wrapper() == true);
    CHECK(var.get_type() == type::get<std::reference_wrapper<int>>());
    REQUIRE(var.get_type().get_wrapped_type() == type::get<int>());
    REQUIRE(var.extract_wrapped_value().is_valid() == true);
    CHECK(var.extract_wrapped_value().get_value<int>() == 12);

    int* bar = &foo;
    var = std::ref(bar);
    CHECK(var.get_type().is_wrapper() == true);
    CHECK(var.get_type() == type::get<std::reference_wrapper<int*>>());
    REQUIRE(var.get_type().get_wrapped_type() == type::get<int*>());
    REQUIRE(var.extract_wrapped_value().is_valid() == true);
    CHECK(*var.extract_wrapped_value().get_value<int*>() == foo);

    int** bar2 = &bar;
    var = std::cref(bar2);
    CHECK(var.get_type().is_wrapper() == true);
    CHECK(var.get_type() == type::get<std::reference_wrapper<int** const>>());
    REQUIRE(var.get_type().get_wrapped_type() == type::get<int** const>());
    REQUIRE(var.extract_wrapped_value().is_valid() == true);
    CHECK(**var.extract_wrapped_value().get_value<int** const>() == foo);


    auto ptr = detail::make_unique<int>(24);
    var = std::ref(ptr);
    CHECK(var.get_type().is_wrapper() == true);
    REQUIRE(var.get_type().get_wrapped_type() == type::get<std::unique_ptr<int>>());
    CHECK(*var.get_wrapped_value<std::unique_ptr<int>>().get() == 24);
    CHECK(var.extract_wrapped_value().is_valid() == false);
}

/////////////////////////////////////////////////////////////////////////////////////////
