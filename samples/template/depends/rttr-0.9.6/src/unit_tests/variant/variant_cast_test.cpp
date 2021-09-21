/************************************************************************************
*                                                                                   *
*   Copyright (c) 2014, 2015 - 2017 Axel Menzel <info@rttr.org>                     *
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


/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("variant - variant_cast<T>(variant*)", "[variant]")
{
    SECTION("empty cast")
    {
        variant var;

        auto value = variant_cast<int>(&var);

        CHECK(value == nullptr);
    }

    SECTION("valid cast")
    {
        variant var = 12;

        auto value = variant_cast<int>(&var);

        CHECK(value != nullptr);

        static_assert(std::is_same<decltype(value), int*>::value, "Must be integer pointer type!");

        REQUIRE(value != nullptr);
        CHECK(*value == 12);
    }

    SECTION("valid cast - pointer")
    {
        int obj = 12;
        variant var = &obj;

        auto value = variant_cast<int*>(&var);
        REQUIRE(value != nullptr);
        CHECK(**value == 12);
    }

    SECTION("invalid cast")
    {
        variant var = 12;

        auto value = variant_cast<bool>(&var);
        CHECK(value == nullptr); // nullptr, because not of type "int"
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("variant - variant_cast<T>(const variant*)", "[variant]")
{
    SECTION("empty cast")
    {
        const variant var;

        auto value = variant_cast<int>(&var);

        CHECK(value == nullptr);
    }

    SECTION("valid cast")
    {
        const variant var = 12;

        auto value = variant_cast<int>(&var);

        CHECK(value != nullptr);

        static_assert(std::is_same<decltype(value), const int*>::value, "Must be integer pointer type!");

        REQUIRE(value != nullptr);
        CHECK(*value == 12);
    }

    SECTION("invalid cast")
    {
        const variant var = 12;

        auto value = variant_cast<bool>(&var);
        CHECK(value == nullptr); // nullptr, because not of type "int"
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("variant - variant_cast<T>(variant&)", "[variant]")
{
    SECTION("by value")
    {
        variant var = 12;

        REQUIRE(var.get_type() == type::get<int>());

        auto value = variant_cast<int>(var);
        CHECK(value == 12);
        using value_t = decltype(value);
        static_assert(!std::is_reference<value_t>::value, "Must be value type!");
    }

    SECTION("by reference")
    {
        variant var = 12;

        REQUIRE(var.get_type() == type::get<int>());

        auto& value = variant_cast<int&>(var);
        CHECK(value == 12);

        using value_t = decltype(value);
        static_assert(std::is_reference<value_t>::value, "Must be reference type!");
    }

    SECTION("move from")
    {
        std::string text = "cat";
        variant var = text;

        REQUIRE(var.get_type() == type::get<std::string>());

        std::string s;
        s = std::move(variant_cast<std::string&>(var));
        CHECK(s == text);
        CHECK(variant_cast<std::string&>(var).empty() == true);

        std::string text2 = "hello";
        variant_cast<std::string&>(var) = std::move(text2);
        CHECK(variant_cast<std::string&>(var) == "hello");
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("variant - variant_cast<T>(const variant&)", "[variant]")
{
    SECTION("by value")
    {
        const variant var = 12;

        CHECK(var.get_type() == type::get<int>());

        auto value = variant_cast<const int>(var);
        CHECK(value == 12);
        using value_t = decltype(value);
        static_assert(!std::is_reference<value_t>::value, "Must be value type!");
    }

    SECTION("by reference")
    {
        std::string text = "cat";
        const variant var = text;

        CHECK(var.get_type() == type::get<std::string>());

        auto& value = variant_cast<const std::string&>(var);
        CHECK(value == text);

        using value_t = decltype(value);
        static_assert(std::is_reference<value_t>::value, "Must be reference type!");
        static_assert(std::is_const<detail::remove_reference_t<value_t>>::value, "Must be const-reference type!");
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

static int move_count = 0;

struct moveable_class
  {
    moveable_class(moveable_class&&)
    {
      ++move_count;
    }
    moveable_class() {}
    moveable_class(const moveable_class&) {}
    int value = 12;
};

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("variant - variant_cast<T>(variant&&)", "[variant]")
{
    SECTION("move test - string")
    {
        std::string text = "cat";
        variant var = text;

        CHECK(var.get_type() == type::get<std::string>());

        auto value = variant_cast<std::string>(std::move(var));
        CHECK(value == text);
    }

    SECTION("move test")
    {
        moveable_class obj;
        variant var = std::move(obj);
        CHECK(move_count == 1);
        moveable_class m2 = variant_cast<moveable_class>(std::move(var));
        CHECK(move_count == 2);
        CHECK(m2.value == 12); // to avoid not used variable warning

        moveable_class&& m3 = variant_cast<moveable_class&&>(std::move(var));
        CHECK(move_count == 2); // will not move
        CHECK(m3.value == 12); // to avoid not used variable warning
    }
}

/////////////////////////////////////////////////////////////////////////////////////////
