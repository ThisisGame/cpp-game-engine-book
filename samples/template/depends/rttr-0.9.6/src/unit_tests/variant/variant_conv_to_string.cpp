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
#include <rttr/registration>

using namespace rttr;

RTTR_REGISTRATION
{
    registration::enumeration<enum_int64_t>("enum_int64_t")
    (
        value("VALUE_1", enum_int64_t::VALUE_1),
        value("VALUE_2", enum_int64_t::VALUE_2),
        value("VALUE_3", enum_int64_t::VALUE_3),
        value("VALUE_4", enum_int64_t::VALUE_4),
        value("VALUE_X", enum_int64_t::VALUE_NEG)
    )
    ;
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("variant::to_string() - from empty", "[variant]")
{
    variant var;
    bool ok = false;
    CHECK(var.to_string(&ok) == "");
    CHECK(ok == false);
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("variant::to_string() - from bool", "[variant]")
{
    variant var = true;
    REQUIRE(var.is_valid() == true);
    REQUIRE(var.can_convert<std::string>() == true);

    // true case
    bool ok = false;
    CHECK(var.to_string(&ok) == "true");
    CHECK(ok == true);

    CHECK(var.convert<std::string>(&ok) == "true");
    CHECK(ok == true);
    REQUIRE(var.convert(type::get<std::string>()) == true);
    CHECK(var.get_value<std::string>() == "true");

    // false case
    var = false;
    CHECK(var.to_string(&ok) == "false");
    CHECK(ok == true);

    CHECK(var.convert<std::string>(&ok) == "false");
    CHECK(ok == true);
    REQUIRE(var.convert(type::get<std::string>()) == true);
    CHECK(var.get_value<std::string>() == "false");
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("variant::to_string() - from char", "[variant]")
{
    SECTION("valid conversion")
    {
        variant var = char('A');
        REQUIRE(var.can_convert<std::string>() == true);
        bool ok = false;
        CHECK(var.to_string(&ok) == "A");
        CHECK(ok == true);

        REQUIRE(var.convert(type::get<std::string>()) == true);
        CHECK(var.get_value<std::string>() == "A");
    }

    SECTION("valid conversion negative")
    {
        variant var = char(-60);
        bool ok = false;
        CHECK(var.to_string(&ok) == std::string(1, char(-60)));
        CHECK(ok == true);
        CHECK(var.convert(type::get<std::string>()) == true);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("variant::to_string() - from std::string", "[variant]")
{
    variant var = std::string("23");
    REQUIRE(var.can_convert<std::string>() == true);
    bool ok = false;
    CHECK(var.to_string(&ok) == "23");
    CHECK(ok == true);

    var = std::string("-12");
    CHECK(var.to_string() == "-12");
    REQUIRE(var.convert(type::get<std::string>()) == true);
    CHECK(var.get_value<std::string>() == "-12");
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("variant::to_string() - from int", "[variant]")
{
    SECTION("conversion positive")
    {
        variant var = 2147483640;
        REQUIRE(var.can_convert<std::string>() == true);
        bool ok = false;
        CHECK(var.to_string(&ok) == "2147483640");

        CHECK(ok == true);
        REQUIRE(var.convert(type::get<std::string>()) == true);
        CHECK(var.get_value<std::string>() == "2147483640");
    }

    SECTION("conversion negative")
    {
        variant var = -2147483640;
        bool ok = false;
        CHECK(var.to_string(&ok) == "-2147483640");
        CHECK(ok == true);
        CHECK(var.convert(type::get<std::string>()) == true);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("variant::to_string() - from float", "[variant]")
{
    SECTION("conversion positive")
    {
        variant var = 214748.9f;
        REQUIRE(var.can_convert<std::string>() == true);
        bool ok = false;
        CHECK(var.to_string(&ok) == "214749");
        CHECK(ok == true);

        REQUIRE(var.convert(type::get<std::string>()) == true);
        CHECK(var.get_value<std::string>() == "214749");
    }

    SECTION("conversion negative")
    {
        variant var = -214748.9f;
        bool ok = false;
        CHECK(var.to_string(&ok) == "-214749");
        CHECK(ok == true);
        CHECK(var.convert(type::get<std::string>()) == true);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("variant::to_string() - from double", "[variant]")
{
    SECTION("conversion positive")
    {
        variant var = 5000000000.9;
        REQUIRE(var.can_convert<std::string>() == true);
        bool ok = false;
        CHECK(var.to_string(&ok) == "5000000000.9");
        CHECK(ok == true);

        REQUIRE(var.convert(type::get<std::string>()) == true);
        CHECK(var.get_value<std::string>() == "5000000000.9");
    }

    SECTION("conversion negative")
    {
        variant var = -5000000000.9;
        bool ok = false;
        CHECK(var.to_string(&ok) == "-5000000000.9");
        CHECK(ok == true);
        CHECK(var.convert(type::get<std::string>()) == true);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("variant::to_string() - from int8_t", "[variant]")
{
    SECTION("valid conversion positive")
    {
        variant var = int8_t(50);
        REQUIRE(var.can_convert<std::string>() == true);
        bool ok = false;
        CHECK(var.to_string(&ok) == "50");
        CHECK(ok == true);

        REQUIRE(var.convert(type::get<std::string>()) == true);
        CHECK(var.get_value<std::string>() == "50");
    }

    SECTION("valid conversion negative")
    {
        variant var = int8_t(-60);
        bool ok = false;
        CHECK(var.to_string(&ok) == "-60");
        CHECK(ok == true);
        CHECK(var.convert(type::get<std::string>()) == true);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("variant::to_string() - from int16_t", "[variant]")
{
    SECTION("valid conversion positive")
    {
        variant var = int16_t(32760);
        REQUIRE(var.can_convert<std::string>() == true);
        bool ok = false;
        CHECK(var.to_string(&ok) == "32760");
        CHECK(ok == true);

        REQUIRE(var.convert(type::get<std::string>()) == true);
        CHECK(var.get_value<std::string>() == "32760");
    }

    SECTION("valid conversion negative")
    {
        variant var = int16_t(-32760);
        bool ok = false;
        CHECK(var.to_string(&ok) == "-32760");
        CHECK(ok == true);
        CHECK(var.convert(type::get<std::string>()) == true);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("variant::to_string() - from int32_t", "[variant]")
{
    SECTION("valid conversion positive")
    {
        variant var = int32_t(2147483640);
        REQUIRE(var.can_convert<std::string>() == true);
        bool ok = false;
        CHECK(var.to_string(&ok) == "2147483640");
        CHECK(ok == true);

        REQUIRE(var.convert(type::get<std::string>()) == true);
        CHECK(var.get_value<std::string>() == "2147483640");
    }

    SECTION("valid conversion negative")
    {
        variant var = int32_t(-2147483640);
        bool ok = false;
        CHECK(var.to_string(&ok) == "-2147483640");
        CHECK(ok == true);
        CHECK(var.convert(type::get<std::string>()) == true);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("variant::to_string() - from int64_t", "[variant]")
{
    SECTION("valid conversion positive")
    {
        variant var = int64_t(9023372036854775807L);
        REQUIRE(var.can_convert<std::string>() == true);
        bool ok = false;
        CHECK(var.to_string(&ok) == "9023372036854775807");
        CHECK(ok == true);

        REQUIRE(var.convert(type::get<std::string>()) == true);
        CHECK(var.get_value<std::string>() == "9023372036854775807");
    }

    SECTION("valid conversion negative")
    {
        variant var = int64_t(-9023372036854775808L);
        bool ok = false;
        CHECK(var.to_string(&ok) == "-9023372036854775808");
        CHECK(ok == true);
        CHECK(var.convert(type::get<std::string>()) == true);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("variant::to_string() - from uint8_t", "[variant]")
{
    SECTION("valid conversion positive")
    {
        variant var = uint8_t(50);
        REQUIRE(var.can_convert<std::string>() == true);
        bool ok = false;
        CHECK(var.to_string(&ok) == "50");
        CHECK(ok == true);

        REQUIRE(var.convert(type::get<std::string>()) == true);
        CHECK(var.get_value<std::string>() == "50");
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("variant::to_string() - from uint16_t", "[variant]")
{
    SECTION("valid conversion positive")
    {
        variant var = uint16_t(32760);
        REQUIRE(var.can_convert<std::string>() == true);
        bool ok = false;
        CHECK(var.to_string(&ok) == "32760");
        CHECK(ok == true);

        REQUIRE(var.convert(type::get<std::string>()) == true);
        CHECK(var.get_value<std::string>() == "32760");
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("variant::to_string() - from uint32_t", "[variant]")
{
    SECTION("valid conversion positive")
    {
        variant var = uint32_t(32760);
        REQUIRE(var.can_convert<std::string>() == true);
        bool ok = false;
        CHECK(var.to_string(&ok) == "32760");
        CHECK(ok == true);

        REQUIRE(var.convert(type::get<std::string>()) == true);
        CHECK(var.get_value<std::string>() == "32760");
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("variant::to_string() - from uint64_t", "[variant]")
{
    SECTION("valid conversion positive")
    {
        variant var = uint64_t(2147483640);
        REQUIRE(var.can_convert<std::string>() == true);
        bool ok = false;
        CHECK(var.to_string(&ok) == "2147483640");
        CHECK(ok == true);

        REQUIRE(var.convert(type::get<std::string>()) == true);
        CHECK(var.get_value<std::string>() == "2147483640");
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

enum class unregisterd_enum { VALUE_1 };

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("variant::to_string() - from enum", "[variant]")
{
    SECTION("valid conversion")
    {
        variant var = enum_int64_t::VALUE_1;
        REQUIRE(var.can_convert<int64_t>() == true);
        bool ok = false;
        CHECK(var.to_string(&ok) == "VALUE_1");
        CHECK(ok == true);

        REQUIRE(var.convert(type::get<std::string>()) == true);
        CHECK(var.get_value<std::string>() == "VALUE_1");
    }

    SECTION("invalid conversion")
    {
        variant var = unregisterd_enum::VALUE_1;
        bool ok = false;
        CHECK(var.to_string(&ok) == "");
        CHECK(ok == false);
        CHECK(var.convert(type::get<std::string>()) == false);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////
