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

TEST_CASE("variant::to_uint64() - from empty", "[variant]")
{
    variant var;
    bool ok = false;
    CHECK(var.to_uint64(&ok) == 0);
    CHECK(ok == false);
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("variant::to_uint64() - from bool", "[variant]")
{
    variant var = true;
    REQUIRE(var.is_valid() == true);
    REQUIRE(var.can_convert<uint64_t>() == true);

    // true case
    bool ok = false;
    CHECK(var.to_uint64(&ok) == 1);
    CHECK(ok == true);

    CHECK(var.convert<uint64_t>(&ok) == 1);
    CHECK(ok == true);
    REQUIRE(var.convert(type::get<uint64_t>()) == true);
    CHECK(var.get_value<uint64_t>() == 1);

    // false case
    var = false;
    CHECK(var.to_uint64(&ok) == 0);
    CHECK(ok == true);

    CHECK(var.convert<uint64_t>(&ok) == 0);
    CHECK(ok == true);
    REQUIRE(var.convert(type::get<uint64_t>()) == true);
    CHECK(var.get_value<uint64_t>() == 0);
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("variant::to_uint64() - from char", "[variant]")
{
    SECTION("valid conversion")
    {
        variant var = char('A');
        REQUIRE(var.can_convert<uint64_t>() == true);
        bool ok = false;
        CHECK(var.to_uint64(&ok) == 65);
        CHECK(ok == true);

        REQUIRE(var.convert(type::get<uint64_t>()) == true);
        CHECK(var.get_value<uint64_t>() == 65);
    }

RTTR_BEGIN_DISABLE_CONDITIONAL_EXPR_WARNING

    if (std::numeric_limits<char>::is_signed)
    {
        SECTION("invalid conversion negative")
        {
            variant var = char(-60);
            bool ok = false;
            CHECK(var.to_uint64(&ok) == 0);
            CHECK(ok == false);
            CHECK(var.convert(type::get<uint64_t>()) == false);
        }
    }

RTTR_END_DISABLE_CONDITIONAL_EXPR_WARNING

}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("variant::to_uint64() - from std::string", "[variant]")
{
    SECTION("valid conversion positive")
    {
        variant var = std::string("17446744073709551615");
        REQUIRE(var.can_convert<uint64_t>() == true);
        bool ok = false;

        CHECK(var.to_uint64(&ok) == 17446744073709551615UL);
        CHECK(ok == true);

        REQUIRE(var.convert(type::get<uint64_t>()) == true);
        CHECK(var.get_value<uint64_t>() == 17446744073709551615UL);
    }

    SECTION("invalid conversion negative")
    {
        variant var = std::string("-2147483640");
        bool ok = false;
        CHECK(var.to_uint64(&ok) == 0);
        CHECK(ok == false);
        CHECK(var.convert(type::get<uint64_t>()) == false);
    }

    SECTION("too big")
    {
        variant var = std::string("20446744073709551615");
        bool ok = false;
        CHECK(var.to_uint64(&ok) == 0);
        CHECK(ok == false);
        CHECK(var.convert(type::get<uint64_t>()) == false);
    }

    SECTION("invalid conversion")
    {
        variant var = std::string("text 34 and text");
        bool ok = false;
        CHECK(var.to_uint64(&ok) == 0);
        CHECK(ok == false);
        CHECK(var.convert(type::get<uint64_t>()) == false);

        var = std::string("34 and text");
        ok = false;
        CHECK(var.to_uint64(&ok) == 0);
        CHECK(ok == false);
        CHECK(var.convert(type::get<uint64_t>()) == false);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("variant::to_uint64() - from int", "[variant]")
{
    SECTION("valid conversion positive")
    {
        variant var = 50;
        REQUIRE(var.can_convert<uint64_t>() == true);
        bool ok = false;
        CHECK(var.to_uint64(&ok) == 50);
        CHECK(ok == true);

        REQUIRE(var.convert(type::get<uint64_t>()) == true);
        CHECK(var.get_value<uint64_t>() == 50);
    }

    SECTION("invalid conversion negative")
    {
        variant var = -60;
        bool ok = false;
        CHECK(var.to_uint64(&ok) == 0);
        CHECK(ok == false);
        CHECK(var.convert(type::get<uint64_t>()) == false);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("variant::to_uint64() - from float", "[variant]")
{
    SECTION("valid conversion positive")
    {
        variant var = 214748.9f;
        REQUIRE(var.can_convert<uint64_t>() == true);
        bool ok = false;
        CHECK(var.to_uint64(&ok) == 214748);
        CHECK(ok == true);

        REQUIRE(var.convert(type::get<uint64_t>()) == true);
        CHECK(var.get_value<uint64_t>() == 214748);
    }

    SECTION("invalid conversion negative")
    {
        variant var = -21.9f;
        bool ok = false;
        CHECK(var.to_uint64(&ok) == 0);
        CHECK(ok == false);
        CHECK(var.convert(type::get<uint64_t>()) == false);
    }

    SECTION("too big")
    {
        variant var = 3.40282e+37f;
        bool ok = false;
        CHECK(var.to_uint64(&ok) == 0);
        CHECK(ok == false);
        CHECK(var.convert(type::get<uint64_t>()) == false);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("variant::to_uint64() - from double", "[variant]")
{
    SECTION("valid conversion positive")
    {
        variant var = 174407329551615.9;
        REQUIRE(var.can_convert<uint64_t>() == true);
        bool ok = false;
        CHECK(var.to_uint64(&ok) == 174407329551615u);

        CHECK(ok == true);
        REQUIRE(var.convert(type::get<uint64_t>()) == true);
        CHECK(var.get_value<uint64_t>() == 174407329551615u);
    }

    SECTION("invalid conversion negative")
    {
        variant var = -21.9;
        bool ok = false;
        CHECK(var.to_uint64(&ok) == 0);
        CHECK(ok == false);
        CHECK(var.convert(type::get<uint64_t>()) == false);
    }

    SECTION("too big")
    {
        variant var = 19446744073709551615.2;
        bool ok = false;
        CHECK(var.to_uint64(&ok) == 0);
        CHECK(ok == false);
        REQUIRE(var.convert(type::get<uint64_t>()) == false);
        CHECK(var.get_value<double>() == 19446744073709551615.2);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("variant::to_uint64() - from int8_t", "[variant]")
{
    SECTION("valid conversion positive")
    {
        variant var = int8_t(50);
        REQUIRE(var.can_convert<uint64_t>() == true);
        bool ok = false;
        CHECK(var.to_uint64(&ok) == uint64_t(50));
        CHECK(ok == true);

        REQUIRE(var.convert(type::get<uint64_t>()) == true);
        CHECK(var.get_value<uint64_t>() == 50);
    }

    SECTION("invalid conversion negative")
    {
        variant var = int8_t(-60);
        bool ok = false;
        CHECK(var.to_uint64(&ok) == 0);
        CHECK(ok == false);
        CHECK(var.convert(type::get<uint64_t>()) == false);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("variant::to_uint64() - from int16_t", "[variant]")
{
    SECTION("valid conversion positive")
    {
        variant var = int16_t(50);
        REQUIRE(var.can_convert<uint64_t>() == true);
        bool ok = false;
        CHECK(var.to_uint64(&ok) == uint64_t(50));
        CHECK(ok == true);

        REQUIRE(var.convert(type::get<uint64_t>()) == true);
        CHECK(var.get_value<uint64_t>() == uint64_t(50));
    }

    SECTION("invalid conversion negative")
    {
        variant var = int16_t(-60);
        bool ok = false;
        CHECK(var.to_uint64(&ok) == 0);
        CHECK(ok == false);
        CHECK(var.convert(type::get<uint64_t>()) == false);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("variant::to_uint64() - from int32_t", "[variant]")
{
    SECTION("valid conversion positive")
    {
        variant var = int32_t(50);
        REQUIRE(var.can_convert<uint64_t>() == true);
        bool ok = false;
        CHECK(var.to_uint64(&ok) == uint64_t(50));
        CHECK(ok == true);

        REQUIRE(var.convert(type::get<uint64_t>()) == true);
        CHECK(var.get_value<uint64_t>() == uint64_t(50));
    }

    SECTION("invalid conversion negative")
    {
        variant var = int32_t(-60);
        bool ok = false;
        CHECK(var.to_uint64(&ok) == 0);
        CHECK(ok == false);
        CHECK(var.convert(type::get<uint64_t>()) == false);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("variant::to_uint64() - from int64_t", "[variant]")
{
    SECTION("valid conversion positive")
    {
        variant var = int64_t(50);
        REQUIRE(var.can_convert<uint64_t>() == true);
        bool ok = false;
        CHECK(var.to_uint64(&ok) == uint64_t(50));
        CHECK(ok == true);

        REQUIRE(var.convert(type::get<uint64_t>()) == true);
        CHECK(var.get_value<uint64_t>() == uint64_t(50));
    }

    SECTION("invalid conversion negative")
    {
        variant var = int64_t(-60);
        bool ok = false;
        CHECK(var.to_uint64(&ok) == 0);
        CHECK(ok == false);
        CHECK(var.convert(type::get<uint64_t>()) == false);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("variant::to_uint64() - from uint8_t", "[variant]")
{
    SECTION("valid conversion positive")
    {
        variant var = uint8_t(50);
        REQUIRE(var.can_convert<uint64_t>() == true);
        bool ok = false;
        CHECK(var.to_uint64(&ok) == 50);
        CHECK(ok == true);

        REQUIRE(var.convert(type::get<uint64_t>()) == true);
        CHECK(var.get_value<uint64_t>() == 50);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("variant::to_uint64() - from uint16_t", "[variant]")
{
    SECTION("valid conversion positive")
    {
        variant var = uint16_t(50);
        REQUIRE(var.can_convert<uint64_t>() == true);
        bool ok = false;
        CHECK(var.to_uint64(&ok) == 50);
        CHECK(ok == true);

        REQUIRE(var.convert(type::get<uint64_t>()) == true);
        CHECK(var.get_value<uint64_t>() == 50);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("variant::to_uint64() - from uint32_t", "[variant]")
{
    SECTION("valid conversion positive")
    {
        variant var = uint32_t(50);
        REQUIRE(var.can_convert<uint64_t>() == true);
        bool ok = false;
        CHECK(var.to_uint64(&ok) == 50);
        CHECK(ok == true);

        REQUIRE(var.convert(type::get<uint64_t>()) == true);
        CHECK(var.get_value<uint64_t>() == 50);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("variant::to_uint64() - from uint64_t", "[variant]")
{
    SECTION("valid conversion positive")
    {
        variant var = uint64_t(3147483640);
        REQUIRE(var.can_convert<uint64_t>() == true);
        bool ok = false;
        CHECK(var.to_uint64(&ok) == 3147483640u);
        CHECK(ok == true);

        REQUIRE(var.convert(type::get<uint64_t>()) == true);
        CHECK(var.get_value<uint64_t>() == 3147483640u);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("variant::to_uint64() - from enum", "[variant]")
{
    SECTION("valid conversion positive")
    {
        variant var = enum_uint64_t::VALUE_1;
        REQUIRE(var.can_convert<uint64_t>() == true);
        bool ok = false;
        CHECK(var.to_uint64(&ok) == 17446744073709551615U);
        CHECK(ok == true);

        REQUIRE(var.convert(type::get<uint64_t>()) == true);
        CHECK(var.get_value<uint64_t>() == 17446744073709551615U);
    }

    SECTION("too small")
    {
        variant var = enum_int8_t::VALUE_NEG;
        bool ok = false;
        CHECK(var.to_uint64(&ok) == 0);
        CHECK(ok == false);
        CHECK(var.convert(type::get<uint64_t>()) == false);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////
