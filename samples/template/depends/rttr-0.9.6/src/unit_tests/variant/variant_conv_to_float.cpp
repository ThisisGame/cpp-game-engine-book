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

TEST_CASE("variant::to_float() - from empty", "[variant]")
{
    variant var;
    bool ok = false;
    CHECK(var.to_float(&ok) == 0.0f);
    CHECK(ok == false);
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("variant::to_float() - from bool", "[variant]")
{
    variant var = true;
    REQUIRE(var.is_valid() == true);
    REQUIRE(var.can_convert<float>() == true);

    // true case
    bool ok = false;
    CHECK(var.to_float(&ok) == 1.0f);
    CHECK(ok == true);

    CHECK(var.convert<float>(&ok) == 1.0f);
    CHECK(ok == true);
    CHECK(var.convert(type::get<float>()) == true);
    CHECK(var.get_value<float>() == 1.0f);

    // false case
    var = false;
    CHECK(var.to_float(&ok) == 0.0f);
    CHECK(ok == true);

    CHECK(var.convert<float>(&ok) == 0.0f);
    CHECK(ok == true);
    CHECK(var.convert(type::get<float>()) == true);
    CHECK(var.get_value<float>() == 0.0f);
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("variant::to_float() - from char", "[variant]")
{
    SECTION("valid conversion")
    {
        variant var = char('A');
        REQUIRE(var.can_convert<float>() == true);
        bool ok = false;
        CHECK(var.to_float(&ok) == 65.0);
        CHECK(ok == true);

        CHECK(var.convert(type::get<float>()) == true);
        CHECK(var.get_value<float>() == 65.0);
    }

RTTR_BEGIN_DISABLE_CONDITIONAL_EXPR_WARNING

    if (std::numeric_limits<char>::is_signed)
    {
        SECTION("valid conversion negative")
        {
            variant var = char(-60);
            bool ok = false;
            CHECK(var.to_float(&ok) == -60.0);
            CHECK(ok == true);
            CHECK(var.convert(type::get<float>()) == true);
        }
    }

RTTR_END_DISABLE_CONDITIONAL_EXPR_WARNING

}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("variant::to_float() - from std::string", "[variant]")
{
    SECTION("valid conversion positive")
    {
        variant var = std::string("5000000000");
        REQUIRE(var.can_convert<float>() == true);
        bool ok = false;
        CHECK(var.to_float(&ok) == 5000000000.0f);
        CHECK(ok == true);

        CHECK(var.convert(type::get<float>()) == true);
        CHECK(var.get_value<float>() == 5000000000.0f);
    }

    SECTION("valid conversion negative")
    {
        variant var = std::string("-5000000000");
        bool ok = false;
        CHECK(var.to_float(&ok) == -5000000000.0f);
        CHECK(ok == true);
        CHECK(var.convert(type::get<float>()) == true);
    }

    SECTION("too big")
    {
        variant var = std::string("3.40282e+39f");
        bool ok = false;
        CHECK(var.to_float(&ok) == 0.0);
        CHECK(ok == false);
        CHECK(var.convert(type::get<float>()) == false);
    }

    SECTION("too small")
    {
        variant var = std::string("-3.40282e+39f");
        bool ok = false;
        CHECK(var.to_float(&ok) == 0.0);
        CHECK(ok == false);
        CHECK(var.convert(type::get<float>()) == false);
    }

    SECTION("invalid conversion")
    {
        variant var = std::string("text 34 and text");
        bool ok = false;
        CHECK(var.to_float(&ok) == 0.0);
        CHECK(ok == false);
        CHECK(var.convert(type::get<float>()) == false);

        var = std::string("34 and text");
        ok = false;
        CHECK(var.to_float(&ok) == 0.0);
        CHECK(ok == false);
        CHECK(var.convert(type::get<float>()) == false);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("variant::to_float() - from int", "[variant]")
{
    SECTION("valid conversion positive")
    {
        variant var = 2147483640;
        REQUIRE(var.can_convert<float>() == true);
        bool ok = false;
        CHECK(var.to_float(&ok) == 2147483640.0f);

        CHECK(ok == true);
        CHECK(var.convert(type::get<float>()) == true);
        CHECK(var.get_value<float>() == 2147483640.0f);
    }

    SECTION("valid conversion negative")
    {
        variant var = -2147483640;
        bool ok = false;
        CHECK(var.to_float(&ok) == -2147483640.0f);
        CHECK(ok == true);
        CHECK(var.convert(type::get<float>()) == true);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("variant::to_float() - from float", "[variant]")
{
    SECTION("valid conversion positive")
    {
        variant var = 214748.9f;
        REQUIRE(var.can_convert<float>() == true);
        bool ok = false;
        CHECK(var.to_float(&ok) == Approx(214748.9));
        CHECK(ok == true);

        CHECK(var.convert(type::get<float>()) == true);
        CHECK(var.get_value<float>() == Approx(214748.9));
    }

    SECTION("valid conversion negative")
    {
        variant var = -214748.9f;
        bool ok = false;
        CHECK(var.to_float(&ok) == Approx(-214748.9));
        CHECK(ok == true);
        CHECK(var.convert(type::get<float>()) == true);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("variant::to_float() - from double", "[variant]")
{
    SECTION("valid conversion positive")
    {
        variant var = 5000000000.9;
        REQUIRE(var.can_convert<float>() == true);
        bool ok = false;
        CHECK(var.to_float(&ok) == Approx(5000000000.9));
        CHECK(ok == true);

        CHECK(var.convert(type::get<float>()) == true);
        CHECK(var.get_value<float>() == Approx(5000000000.9));
    }

    SECTION("valid conversion negative")
    {
        variant var = -5000000000.9;
        bool ok = false;
        CHECK(var.to_float(&ok) == Approx(-5000000000.9));
        CHECK(ok == true);
        CHECK(var.convert(type::get<float>()) == true);
    }

    SECTION("too big")
    {
        variant var = 3.40282e+39;
        bool ok = false;
        CHECK(var.to_float(&ok) == 0.0f);
        CHECK(ok == false);
        CHECK(var.convert(type::get<float>()) == false);
    }

    SECTION("too small")
    {
        variant var = -3.40282e+39;
        bool ok = false;
        CHECK(var.to_float(&ok) == 0.0f);
        CHECK(ok == false);
        CHECK(var.convert(type::get<float>()) == false);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("variant::to_float() - from int8_t", "[variant]")
{
    SECTION("valid conversion positive")
    {
        variant var = int8_t(50);
        REQUIRE(var.can_convert<float>() == true);
        bool ok = false;
        CHECK(var.to_float(&ok) == 50.0f);
        CHECK(ok == true);

        CHECK(var.convert(type::get<float>()) == true);
        CHECK(var.get_value<float>() == 50.0f);
    }

    SECTION("valid conversion negative")
    {
        variant var = int8_t(-60);
        bool ok = false;
        CHECK(var.to_float(&ok) == -60.0f);
        CHECK(ok == true);
        CHECK(var.convert(type::get<float>()) == true);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("variant::to_float() - from int16_t", "[variant]")
{
    SECTION("valid conversion positive")
    {
        variant var = int16_t(32760);
        REQUIRE(var.can_convert<float>() == true);
        bool ok = false;
        CHECK(var.to_float(&ok) == 32760.0f);
        CHECK(ok == true);

        CHECK(var.convert(type::get<float>()) == true);
        CHECK(var.get_value<float>() == 32760.0f);
    }

    SECTION("valid conversion negative")
    {
        variant var = int16_t(-32760);
        bool ok = false;
        CHECK(var.to_float(&ok) == -32760.0);
        CHECK(ok == true);
        CHECK(var.convert(type::get<float>()) == true);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("variant::to_float() - from int32_t", "[variant]")
{
    SECTION("valid conversion positive")
    {
        variant var = int32_t(2147483640);
        REQUIRE(var.can_convert<float>() == true);
        bool ok = false;
        CHECK(var.to_float(&ok) == 2147483640.0f);
        CHECK(ok == true);

        CHECK(var.convert(type::get<float>()) == true);
        CHECK(var.get_value<float>() == 2147483640.0f);
    }

    SECTION("valid conversion negative")
    {
        variant var = int32_t(-2147483640);
        bool ok = false;
        CHECK(var.to_float(&ok) == -2147483640.0f);
        CHECK(ok == true);
        CHECK(var.convert(type::get<float>()) == true);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("variant::to_float() - from int64_t", "[variant]")
{
    SECTION("valid conversion positive")
    {
        variant var = int64_t(5000000000L);
        REQUIRE(var.can_convert<float>() == true);
        bool ok = false;
        CHECK(var.to_float(&ok) == 5000000000.0f);
        CHECK(ok == true);

        CHECK(var.convert(type::get<float>()) == true);
        CHECK(var.get_value<float>() == 5000000000.0f);
    }

    SECTION("valid conversion negative")
    {
        variant var = int64_t(-5000000000L);
        bool ok = false;
        CHECK(var.to_float(&ok) == -5000000000.0f);
        CHECK(ok == true);
        CHECK(var.convert(type::get<float>()) == true);
        CHECK(var.get_value<float>() == -5000000000.0f);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("variant::to_float() - from uint8_t", "[variant]")
{
    SECTION("valid conversion positive")
    {
        variant var = uint8_t(50);
        REQUIRE(var.can_convert<uint8_t>() == true);
        bool ok = false;
        CHECK(var.to_float(&ok) == 50.0f);
        CHECK(ok == true);

        CHECK(var.convert(type::get<float>()) == true);
        CHECK(var.get_value<float>() == 50.0f);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("variant::to_float() - from uint16_t", "[variant]")
{
    SECTION("valid conversion positive")
    {
        variant var = uint16_t(32760);
        REQUIRE(var.can_convert<uint16_t>() == true);
        bool ok = false;
        CHECK(var.to_float(&ok) == 32760.0f);
        CHECK(ok == true);

        CHECK(var.convert(type::get<float>()) == true);
        CHECK(var.get_value<float>() == 32760.0f);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("variant::to_float() - from uint32_t", "[variant]")
{
    SECTION("valid conversion positive")
    {
        variant var = uint32_t(32760);
        REQUIRE(var.can_convert<uint32_t>() == true);
        bool ok = false;
        CHECK(var.to_float(&ok) == 32760.0f);
        CHECK(ok == true);

        CHECK(var.convert(type::get<float>()) == true);
        CHECK(var.get_value<float>() == 32760.0f);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("variant::to_float() - from uint64_t", "[variant]")
{
    SECTION("valid conversion positive")
    {
        variant var = uint64_t(2147483640);
        REQUIRE(var.can_convert<uint64_t>() == true);
        bool ok = false;
        CHECK(var.to_float(&ok) == 2147483640.0f);
        CHECK(ok == true);

        CHECK(var.convert(type::get<float>()) == true);
        CHECK(var.get_value<float>() == 2147483640.0f);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("variant::to_float() - from enum", "[variant]")
{
    SECTION("valid conversion positive")
    {
        variant var = enum_int32_t::VALUE_1;
        REQUIRE(var.can_convert<float>() == true);
        bool ok = false;
        CHECK(var.to_float(&ok) == 2147483630.0f);
        CHECK(ok == true);

        CHECK(var.convert(type::get<float>()) == true);
        CHECK(var.get_value<float>() == 2147483630.0f);
    }

    SECTION("valid conversion negative")
    {
        variant var = enum_int32_t::VALUE_NEG;
        bool ok = false;
        CHECK(var.to_float(&ok) == -2147483630.0f);
        CHECK(ok == true);
        CHECK(var.convert(type::get<float>()) == true);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////
