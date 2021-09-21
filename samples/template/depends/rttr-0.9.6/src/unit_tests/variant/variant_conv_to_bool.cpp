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

#include "unit_tests/variant/test_enums.h"

#include <catch/catch.hpp>
#include <rttr/type>

using namespace rttr;

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("variant::to_bool() - from empty", "[variant]")
{
    variant var;
    CHECK(var.to_bool() == false);
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("variant::to_bool() - from bool", "[variant]")
{
    variant var = true;
    REQUIRE(var.is_valid() == true);
    REQUIRE(var.can_convert<bool>() == true);

    // true case
    CHECK(var.to_bool() == true);

    CHECK(var.convert<bool>() == true);
    REQUIRE(var.convert(type::get<bool>()) == true);
    CHECK(var.get_value<bool>() == true);

    // false case
    var = false;
    CHECK(var.to_bool() == false);
    CHECK(var.convert<bool>() == false);
    REQUIRE(var.convert(type::get<bool>()) == true);
    CHECK(var.get_value<bool>() == false);
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("variant::to_bool() - from char", "[variant]")
{
    SECTION("valid conversion")
    {
        // true
        variant var = char('A');
        REQUIRE(var.can_convert<bool>() == true);
        CHECK(var.to_bool() == true);

        REQUIRE(var.convert(type::get<bool>()) == true);
        CHECK(var.get_value<bool>() == true);

        // false
        var = char('\0');
        CHECK(var.to_bool() == false);

        REQUIRE(var.convert(type::get<bool>()) == true);
        CHECK(var.get_value<bool>() == false);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("variant::to_bool() - from std::string", "[variant]")
{
    variant var = std::string("true");
    REQUIRE(var.is_valid() == true);
    REQUIRE(var.can_convert<bool>() == true);

    CHECK(var.to_bool() == true);

    var = std::string("fdsfsdf");
    CHECK(var.to_bool() == true);

    var = std::string("false");
    CHECK(var.to_bool() == false);
    var = std::string("false   ");
    CHECK(var.to_bool() == false);
    var = std::string("   false   ");
    CHECK(var.to_bool() == false);
    var = std::string("   FALSE   ");
    CHECK(var.to_bool() == false);
    var = std::string(" \n  FALSE\n");
    CHECK(var.to_bool() == false);
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("variant::to_bool() - from int", "[variant]")
{
    variant var = 1;
    REQUIRE(var.is_valid() == true);
    REQUIRE(var.can_convert<bool>() == true);

    CHECK(var.to_bool() == true);
    CHECK(var.convert<bool>() == true);
    REQUIRE(var.convert(type::get<bool>()) == true);
    CHECK(var.get_value<bool>() == true);

    var = 0;
    CHECK(var.to_bool() == false);
    CHECK(var.convert<bool>() == false);
    REQUIRE(var.convert(type::get<bool>()) == true);
    CHECK(var.get_value<bool>() == false);
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("variant::to_bool() - from float", "[variant]")
{

    variant var = 1.0f;
    REQUIRE(var.is_valid() == true);
    REQUIRE(var.can_convert<bool>() == true);

    CHECK(var.to_bool() == true);
    CHECK(var.convert<bool>() == true);
    REQUIRE(var.convert(type::get<bool>()) == true);
    CHECK(var.get_value<bool>() == true);

    var = 0.0f;
    CHECK(var.to_bool() == false);

    var = 1.17149435e-38f;
    CHECK(var.to_bool() == false);
    var = 1.17149435e-37f;
    CHECK(var.to_bool() == true);
    CHECK(var.convert<bool>() == true);
    REQUIRE(var.convert(type::get<bool>()) == true);
    CHECK(var.get_value<bool>() == true);
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("variant::to_bool() - from double", "[variant]")
{
    variant var = 1.0;
    REQUIRE(var.is_valid() == true);
    REQUIRE(var.can_convert<bool>() == true);

    CHECK(var.to_bool() == true);

    var = 0.0;
    CHECK(var.to_bool() == false);

    var = 0.5;
    CHECK(var.to_bool() == true);
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("variant::to_bool() - from int8_t", "[variant]")
{
    SECTION("positive")
    {
        variant var = int8_t(23);
        REQUIRE(var.can_convert<bool>() == true);

        CHECK(var.to_bool() == true);
        CHECK(var.convert<bool>() == true);
        REQUIRE(var.convert(type::get<bool>()) == true);
        CHECK(var.get_value<bool>() == true);
    }

    SECTION("negative")
    {
        variant var = int8_t(-23);
        CHECK(var.to_bool() == true);
        CHECK(var.convert<bool>() == true);
        REQUIRE(var.convert(type::get<bool>()) == true);
        CHECK(var.get_value<bool>() == true);
    }

    SECTION("from zero")
    {
        variant var = int8_t(0);
        CHECK(var.to_bool() == false);
        CHECK(var.convert<bool>() == false);
        REQUIRE(var.convert(type::get<bool>()) == true);
        CHECK(var.get_value<bool>() == false);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("variant::to_bool() - from int16_t", "[variant]")
{
    SECTION("positive")
    {
        variant var = int16_t(23);
        REQUIRE(var.can_convert<bool>() == true);

        CHECK(var.to_bool() == true);
        CHECK(var.convert<bool>() == true);
        REQUIRE(var.convert(type::get<bool>()) == true);
        CHECK(var.get_value<bool>() == true);
    }

    SECTION("negative")
    {
        variant var = int16_t(-23);
        CHECK(var.to_bool() == true);
        CHECK(var.convert<bool>() == true);
        REQUIRE(var.convert(type::get<bool>()) == true);
        CHECK(var.get_value<bool>() == true);
    }

    SECTION("from zero")
    {
        variant var = int16_t(0);
        CHECK(var.to_bool() == false);
        CHECK(var.convert<bool>() == false);
        REQUIRE(var.convert(type::get<bool>()) == true);
        CHECK(var.get_value<bool>() == false);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("variant::to_bool() - from int32_t", "[variant]")
{
    SECTION("positive")
    {
        variant var = int32_t(23);
        REQUIRE(var.can_convert<bool>() == true);

        CHECK(var.to_bool() == true);
        CHECK(var.convert<bool>() == true);
        REQUIRE(var.convert(type::get<bool>()) == true);
        CHECK(var.get_value<bool>() == true);
    }

    SECTION("negative")
    {
        variant var = int32_t(-23);
        CHECK(var.to_bool() == true);
        CHECK(var.convert<bool>() == true);
        REQUIRE(var.convert(type::get<bool>()) == true);
        CHECK(var.get_value<bool>() == true);
    }

    SECTION("from zero")
    {
        variant var = int32_t(0);
        CHECK(var.to_bool() == false);
        CHECK(var.convert<bool>() == false);
        REQUIRE(var.convert(type::get<bool>()) == true);
        CHECK(var.get_value<bool>() == false);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("variant::to_bool() - from int64_t", "[variant]")
{
    SECTION("positive")
    {
        variant var = int64_t(23);
        REQUIRE(var.can_convert<bool>() == true);

        CHECK(var.to_bool() == true);
        CHECK(var.convert<bool>() == true);
        REQUIRE(var.convert(type::get<bool>()) == true);
        CHECK(var.get_value<bool>() == true);
    }

    SECTION("negative")
    {
        variant var = int64_t(-23);
        CHECK(var.to_bool() == true);
        CHECK(var.convert<bool>() == true);
        REQUIRE(var.convert(type::get<bool>()) == true);
        CHECK(var.get_value<bool>() == true);
    }

    SECTION("from zero")
    {
        variant var = int64_t(0);
        CHECK(var.to_bool() == false);
        CHECK(var.convert<bool>() == false);
        REQUIRE(var.convert(type::get<bool>()) == true);
        CHECK(var.get_value<bool>() == false);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("variant::to_bool() - from uint8_t", "[variant]")
{
    SECTION("positive")
    {
        variant var = uint8_t(23);
        REQUIRE(var.can_convert<bool>() == true);

        CHECK(var.to_bool() == true);
        CHECK(var.convert<bool>() == true);
        REQUIRE(var.convert(type::get<bool>()) == true);
        CHECK(var.get_value<bool>() == true);
    }

    SECTION("from zero")
    {
        variant var = uint8_t(0);
        CHECK(var.to_bool() == false);
        CHECK(var.convert<bool>() == false);
        REQUIRE(var.convert(type::get<bool>()) == true);
        CHECK(var.get_value<bool>() == false);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("variant::to_bool() - from uint16_t", "[variant]")
{
    SECTION("positive")
    {
        variant var = uint16_t(23);
        REQUIRE(var.can_convert<bool>() == true);

        CHECK(var.to_bool() == true);
        CHECK(var.convert<bool>() == true);
        REQUIRE(var.convert(type::get<bool>()) == true);
        CHECK(var.get_value<bool>() == true);
    }

    SECTION("from zero")
    {
        variant var = uint16_t(0);
        CHECK(var.to_bool() == false);
        CHECK(var.convert<bool>() == false);
        REQUIRE(var.convert(type::get<bool>()) == true);
        CHECK(var.get_value<bool>() == false);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("variant::to_bool() - from uint32_t", "[variant]")
{
    SECTION("positive")
    {
        variant var = uint32_t(23);
        REQUIRE(var.can_convert<bool>() == true);

        CHECK(var.to_bool() == true);
        CHECK(var.convert<bool>() == true);
        REQUIRE(var.convert(type::get<bool>()) == true);
        CHECK(var.get_value<bool>() == true);
    }

    SECTION("from zero")
    {
        variant var = uint32_t(0);
        CHECK(var.to_bool() == false);
        CHECK(var.convert<bool>() == false);
        REQUIRE(var.convert(type::get<bool>()) == true);
        CHECK(var.get_value<bool>() == false);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("variant::to_bool() - from uint64_t", "[variant]")
{
    SECTION("positive")
    {
        variant var = uint64_t(23);
        REQUIRE(var.can_convert<bool>() == true);

        CHECK(var.to_bool() == true);
        CHECK(var.convert<bool>() == true);
        REQUIRE(var.convert(type::get<bool>()) == true);
        CHECK(var.get_value<bool>() == true);
    }

    SECTION("from zero")
    {
        variant var = uint64_t(0);
        CHECK(var.to_bool() == false);
        CHECK(var.convert<bool>() == false);
        REQUIRE(var.convert(type::get<bool>()) == true);
        CHECK(var.get_value<bool>() == false);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("variant::to_bool() - from enum", "[variant]")
{
    SECTION("valid conversion")
    {
        variant var = enum_bool::ON;
        REQUIRE(var.can_convert<bool>() == true);
        CHECK(var.to_bool() == true);

        REQUIRE(var.convert(type::get<bool>()) == true);
        CHECK(var.get_value<bool>() == true);

        // test with non-bool enum
        var = variant_enum_test::VALUE_0;
        CHECK(var.to_bool() == false);
        CHECK(var.convert(type::get<bool>()) == true);

        var = variant_enum_test::VALUE_1;
        CHECK(var.to_bool() == true);
        CHECK(var.convert(type::get<bool>()) == true);
    }

    SECTION("invalid conversion")
    {
        variant var = enum_int8_t::VALUE_4;
        CHECK(var.to_bool() == false);
        CHECK(var.convert(type::get<bool>()) == false);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////
