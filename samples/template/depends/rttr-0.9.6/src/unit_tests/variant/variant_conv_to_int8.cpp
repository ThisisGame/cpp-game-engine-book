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

TEST_CASE("variant::to_int8() - from empty", "[variant]")
{
    variant var;
    bool ok = false;
    CHECK(var.to_int8(&ok) == 0);
    CHECK(ok == false);
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("variant::to_int8() - from bool", "[variant]")
{
    variant var = true;
    REQUIRE(var.is_valid() == true);
    REQUIRE(var.can_convert<int8_t>() == true);

    // true case
    bool ok = false;
    CHECK(var.to_int8(&ok) == 1);
    CHECK(ok == true);

    CHECK(var.convert<int8_t>(&ok) == 1);
    CHECK(ok == true);
    REQUIRE(var.convert(type::get<int8_t>()) == true);
    CHECK(var.get_value<int8_t>() == 1);

    // false case
    var = false;
    CHECK(var.to_int8(&ok) == 0);
    CHECK(ok == true);

    CHECK(var.convert<int8_t>(&ok) == 0);
    CHECK(ok == true);
    REQUIRE(var.convert(type::get<int8_t>()) == true);
    CHECK(var.get_value<int8_t>() == 0);
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("variant::to_int8() - from char", "[variant]")
{
    SECTION("valid conversion")
    {
        variant var = char('A');
        REQUIRE(var.can_convert<int8_t>() == true);
        bool ok = false;
        CHECK(var.to_int8(&ok) == 65);
        CHECK(ok == true);

        REQUIRE(var.convert(type::get<int8_t>()) == true);
        CHECK(var.get_value<int8_t>() == 65);
    }

RTTR_BEGIN_DISABLE_CONDITIONAL_EXPR_WARNING

    if (std::numeric_limits<char>::is_signed)
    {
        SECTION("valid conversion negative")
        {
            variant var = char(-60);
            bool ok = false;
            CHECK(var.to_int8(&ok) == -60);
            CHECK(ok == true);
            CHECK(var.convert(type::get<int8_t>()) == true);
        }
    }

RTTR_END_DISABLE_CONDITIONAL_EXPR_WARNING
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("variant::to_int8() - from std::string", "[variant]")
{
    SECTION("valid conversion positive")
    {
        variant var = std::string("23");
        REQUIRE(var.can_convert<int8_t>() == true);
        bool ok = false;
        CHECK(var.to_int8(&ok) == 23);
        CHECK(ok == true);

        REQUIRE(var.convert(type::get<int8_t>()) == true);
        CHECK(var.get_value<int8_t>() == 23);
    }

    SECTION("valid conversion negative")
    {
        variant var = std::string("-12");
        bool ok = false;
        CHECK(var.to_int8(&ok) == -12);
        CHECK(ok == true);
        CHECK(var.convert(type::get<int8_t>()) == true);
    }

    SECTION("too big")
    {
        variant var = std::string("512");
        bool ok = false;
        CHECK(var.to_int8(&ok) == 0);
        CHECK(ok == false);
        CHECK(var.convert(type::get<int8_t>()) == false);
    }

    SECTION("too small")
    {
        variant var = std::string("-512");
        bool ok = false;
        CHECK(var.to_int8(&ok) == 0);
        CHECK(ok == false);
        CHECK(var.convert(type::get<int8_t>()) == false);
    }

    SECTION("invalid conversion")
    {
        variant var = std::string("text 34 and text");
        bool ok = false;
        CHECK(var.to_int8(&ok) == 0);
        CHECK(ok == false);
        CHECK(var.convert(type::get<int8_t>()) == false);

        var = std::string("34 and text");
        ok = false;
        CHECK(var.to_int8(&ok) == 0);
        CHECK(ok == false);
        CHECK(var.convert(type::get<int8_t>()) == false);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("variant::to_int8() - from int", "[variant]")
{
    SECTION("valid conversion positive")
    {
        variant var = 50;
        REQUIRE(var.can_convert<int8_t>() == true);
        bool ok = false;
        CHECK(var.to_int8(&ok) == 50);
        CHECK(ok == true);

        REQUIRE(var.convert(type::get<int8_t>()) == true);
        CHECK(var.get_value<int8_t>() == 50);
    }

    SECTION("valid conversion negative")
    {
        variant var = -60;
        bool ok = false;
        CHECK(var.to_int8(&ok) == -60);
        CHECK(ok == true);
        CHECK(var.convert(type::get<int8_t>()) == true);
    }

    SECTION("too big")
    {
        variant var = 512;
        bool ok = false;
        CHECK(var.to_int8(&ok) == 0);
        CHECK(ok == false);
        CHECK(var.convert(type::get<int8_t>()) == false);
    }

    SECTION("too small")
    {
        variant var = -512;
        bool ok = false;
        CHECK(var.to_int8(&ok) == 0);
        CHECK(ok == false);
        CHECK(var.convert(type::get<int8_t>()) == false);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("variant::to_int8() - from float", "[variant]")
{
    SECTION("valid conversion positive")
    {
        variant var = 1.5f;
        REQUIRE(var.can_convert<int8_t>() == true);
        bool ok = false;
        CHECK(var.to_int8(&ok) == 1);
        CHECK(ok == true);

        REQUIRE(var.convert(type::get<int8_t>()) == true);
        CHECK(var.get_value<int8_t>() == 1);
    }

    SECTION("valid conversion negative")
    {
        variant var = -21.9f;
        bool ok = false;
        CHECK(var.to_int8(&ok) == -21);
        CHECK(ok == true);
        CHECK(var.convert(type::get<int8_t>()) == true);
    }

    SECTION("too big")
    {
        variant var = 512.2f;
        bool ok = false;
        CHECK(var.to_int8(&ok) == 0);
        CHECK(ok == false);
        CHECK(var.convert(type::get<int8_t>()) == false);
    }

    SECTION("too small")
    {
        variant var = -512.2f;
        bool ok = false;
        CHECK(var.to_int8(&ok) == 0);
        CHECK(ok == false);
        CHECK(var.convert(type::get<int8_t>()) == false);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("variant::to_int8() - from double", "[variant]")
{
    SECTION("valid conversion positive")
    {
        variant var = 1.5;
        REQUIRE(var.can_convert<int8_t>() == true);
        bool ok = false;
        CHECK(var.to_int8(&ok) == 1);

        CHECK(ok == true);
        REQUIRE(var.convert(type::get<int8_t>()) == true);
        CHECK(var.get_value<int8_t>() == 1);
    }

    SECTION("valid conversion negative")
    {
        variant var = -21.9;
        bool ok = false;
        CHECK(var.to_int8(&ok) == -21);
        CHECK(ok == true);
        CHECK(var.convert(type::get<int8_t>()) == true);
    }

    SECTION("too big")
    {
        variant var = 512.2;
        bool ok = false;
        CHECK(var.to_int8(&ok) == 0);
        CHECK(ok == false);
        CHECK(var.convert(type::get<int8_t>()) == false);
    }

    SECTION("too small")
    {
        variant var = -512.2;
        bool ok = false;
        CHECK(var.to_int8(&ok) == 0);
        CHECK(ok == false);
        CHECK(var.convert(type::get<int8_t>()) == false);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("variant::to_int8() - from int8_t", "[variant]")
{
    SECTION("valid conversion positive")
    {
        variant var = int8_t(50);
        REQUIRE(var.can_convert<int8_t>() == true);
        bool ok = false;
        CHECK(var.to_int8(&ok) == 50);
        CHECK(ok == true);

        REQUIRE(var.convert(type::get<int8_t>()) == true);
        CHECK(var.get_value<int8_t>() == 50);
    }

    SECTION("valid conversion negative")
    {
        variant var = int8_t(-60);
        bool ok = false;
        CHECK(var.to_int8(&ok) == int8_t(-60));
        CHECK(ok == true);
        CHECK(var.convert(type::get<int8_t>()) == true);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("variant::to_int8() - from int16_t", "[variant]")
{
    SECTION("valid conversion positive")
    {
        variant var = int16_t(50);
        REQUIRE(var.can_convert<int8_t>() == true);
        bool ok = false;
        CHECK(var.to_int8(&ok) == int16_t(50));
        CHECK(ok == true);

        REQUIRE(var.convert(type::get<int8_t>()) == true);
        CHECK(var.get_value<int8_t>() == int8_t(50));
    }

    SECTION("valid conversion negative")
    {
        variant var = int16_t(-60);
        bool ok = false;
        CHECK(var.to_int8(&ok) == int8_t(-60));
        CHECK(ok == true);
        CHECK(var.convert(type::get<int8_t>()) == true);
    }

    SECTION("too big")
    {
        variant var = int16_t(512);
        bool ok = false;
        CHECK(var.to_int8(&ok) == 0);
        CHECK(ok == false);
        CHECK(var.convert(type::get<int8_t>()) == false);
    }

    SECTION("too small")
    {
        variant var = int16_t(-512);
        bool ok = false;
        CHECK(var.to_int8(&ok) == 0);
        CHECK(ok == false);
        CHECK(var.convert(type::get<int8_t>()) == false);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("variant::to_int8() - from int32_t", "[variant]")
{
    SECTION("valid conversion positive")
    {
        variant var = int32_t(50);
        REQUIRE(var.can_convert<int8_t>() == true);
        bool ok = false;
        CHECK(var.to_int8(&ok) == int8_t(50));
        CHECK(ok == true);

        REQUIRE(var.convert(type::get<int8_t>()) == true);
        CHECK(var.get_value<int8_t>() == int8_t(50));
    }

    SECTION("valid conversion negative")
    {
        variant var = int32_t(-60);
        bool ok = false;
        CHECK(var.to_int8(&ok) == int32_t(-60));
        CHECK(ok == true);
        CHECK(var.convert(type::get<int8_t>()) == true);
    }

    SECTION("too big")
    {
        variant var = int32_t(512);
        bool ok = false;
        CHECK(var.to_int8(&ok) == 0);
        CHECK(ok == false);
        CHECK(var.convert(type::get<int8_t>()) == false);
    }

    SECTION("too small")
    {
        variant var = int32_t(-512);
        bool ok = false;
        CHECK(var.to_int8(&ok) == 0);
        CHECK(ok == false);
        CHECK(var.convert(type::get<int8_t>()) == false);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("variant::to_int8() - from int64_t", "[variant]")
{
    SECTION("valid conversion positive")
    {
        variant var = int64_t(50);
        REQUIRE(var.can_convert<int8_t>() == true);
        bool ok = false;
        CHECK(var.to_int8(&ok) == int64_t(50));
        CHECK(ok == true);

        REQUIRE(var.convert(type::get<int8_t>()) == true);
        CHECK(var.get_value<int8_t>() == int8_t(50));
    }

    SECTION("valid conversion negative")
    {
        variant var = int64_t(-60);
        bool ok = false;
        CHECK(var.to_int8(&ok) == int8_t(-60));
        CHECK(ok == true);
        CHECK(var.convert(type::get<int8_t>()) == true);
    }

    SECTION("too big")
    {
        variant var = int64_t(512);
        bool ok = false;
        CHECK(var.to_int8(&ok) == 0);
        CHECK(ok == false);
        CHECK(var.convert(type::get<int8_t>()) == false);
    }

    SECTION("too small")
    {
        variant var = int64_t(-512);
        bool ok = false;
        CHECK(var.to_int8(&ok) == 0);
        CHECK(ok == false);
        CHECK(var.convert(type::get<int8_t>()) == false);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("variant::to_int8() - from uint8_t", "[variant]")
{
    SECTION("valid conversion positive")
    {
        variant var = uint8_t(50);
        REQUIRE(var.can_convert<uint8_t>() == true);
        bool ok = false;
        CHECK(var.to_int8(&ok) == int8_t(50));
        CHECK(ok == true);

        REQUIRE(var.convert(type::get<int8_t>()) == true);
        CHECK(var.get_value<int8_t>() == int8_t(50));
    }

    SECTION("too big")
    {
        variant var = uint8_t(200);
        bool ok = false;
        CHECK(var.to_int8(&ok) == 0);
        CHECK(ok == false);
        CHECK(var.convert(type::get<int8_t>()) == false);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("variant::to_int8() - from uint16_t", "[variant]")
{
    SECTION("valid conversion positive")
    {
        variant var = uint16_t(50);
        REQUIRE(var.can_convert<uint16_t>() == true);
        bool ok = false;
        CHECK(var.to_int8(&ok) == int8_t(50));
        CHECK(ok == true);

        REQUIRE(var.convert(type::get<int8_t>()) == true);
        CHECK(var.get_value<int8_t>() == int8_t(50));
    }

    SECTION("too big")
    {
        variant var = uint16_t(1000);
        bool ok = false;
        CHECK(var.to_int8(&ok) == 0);
        CHECK(ok == false);
        CHECK(var.convert(type::get<int8_t>()) == false);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("variant::to_int8() - from uint32_t", "[variant]")
{
    SECTION("valid conversion positive")
    {
        variant var = uint32_t(50);
        REQUIRE(var.can_convert<uint32_t>() == true);
        bool ok = false;
        CHECK(var.to_int8(&ok) == 50);
        CHECK(ok == true);

        REQUIRE(var.convert(type::get<int8_t>()) == true);
        CHECK(var.get_value<int8_t>() == int8_t(50));
    }

    SECTION("too big")
    {
        variant var = uint32_t(1000);
        bool ok = false;
        CHECK(var.to_int8(&ok) == 0);
        CHECK(ok == false);
        CHECK(var.convert(type::get<int8_t>()) == false);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("variant::to_int8() - from uint64_t", "[variant]")
{
    SECTION("valid conversion positive")
    {
        variant var = uint64_t(50);
        REQUIRE(var.can_convert<uint64_t>() == true);
        bool ok = false;
        CHECK(var.to_int8(&ok) == 50);
        CHECK(ok == true);

        REQUIRE(var.convert(type::get<int8_t>()) == true);
        CHECK(var.get_value<int8_t>() == int8_t(50));
    }

    SECTION("too big")
    {
        variant var = uint64_t(1000);
        bool ok = false;
        CHECK(var.to_int8(&ok) == 0);
        CHECK(ok == false);
        REQUIRE(var.convert(type::get<int8_t>()) == false);
        CHECK(var.get_value<uint64_t>() == uint64_t(1000));
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("variant::to_int8() - from enum", "[variant]")
{
    SECTION("valid conversion positive")
    {
        variant var = enum_int8_t::VALUE_1;
        REQUIRE(var.can_convert<int8_t>() == true);
        bool ok = false;
        CHECK(var.to_int8(&ok) == 100);
        CHECK(ok == true);

        REQUIRE(var.convert(type::get<int8_t>()) == true);
        CHECK(var.get_value<int8_t>() == 100);
    }

    SECTION("valid conversion negative")
    {
        variant var = enum_int8_t::VALUE_NEG;
        bool ok = false;
        CHECK(var.to_int8(&ok) == -100);
        CHECK(ok == true);
        CHECK(var.convert(type::get<int8_t>()) == true);
    }

    SECTION("too big")
    {
        variant var = enum_int16_t::VALUE_1;
        bool ok = false;
        CHECK(var.to_int8(&ok) == 0);
        CHECK(ok == false);
        CHECK(var.convert(type::get<int8_t>()) == false);
    }

    SECTION("too small")
    {
        variant var = enum_int16_t::VALUE_NEG;
        bool ok = false;
        CHECK(var.to_int8(&ok) == 0);
        CHECK(ok == false);
        CHECK(var.convert(type::get<int8_t>()) == false);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////
