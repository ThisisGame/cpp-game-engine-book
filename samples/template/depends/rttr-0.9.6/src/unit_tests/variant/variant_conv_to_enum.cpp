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

#include "unit_tests/variant/test_enums.h"
#include <rttr/registration>

using namespace rttr;

RTTR_REGISTRATION
{
    registration::enumeration<variant_enum_test>("variant_enum_test")
    (
        value("VALUE_0", variant_enum_test::VALUE_0),
        value("VALUE_1", variant_enum_test::VALUE_1),
        value("VALUE_2", variant_enum_test::VALUE_2)
    );

    registration::enumeration<variant_enum_test_big>("variant_enum_test_big")
    (
        value("VALUE_0", variant_enum_test_big::VALUE_0),
        value("VALUE_1", variant_enum_test_big::VALUE_1),
        value("VALUE_2", variant_enum_test_big::VALUE_2)
    );

    registration::enumeration<enum_int8_t>("enum_int8_t")
    (
        value("VALUE_1", enum_int8_t::VALUE_1),
        value("VALUE_2", enum_int8_t::VALUE_2),
        value("VALUE_3", enum_int8_t::VALUE_3),
        value("VALUE_4", enum_int8_t::VALUE_4),
        value("VALUE_NEG", enum_int8_t::VALUE_NEG)
    );
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("variant::to enum - from empty", "[variant]")
{
    variant var;
    bool ok = false;
    CHECK(var.convert(type::get<variant_enum_test>()) == false);
    var.convert<variant_enum_test>(&ok);
    CHECK(ok == false);

    variant_enum_test value;
    CHECK(var.convert(value) == false);
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("variant:to enum - from bool", "[variant]")
{
    SECTION("valid conversion")
    {
        variant var = true;
        REQUIRE(var.is_valid() == true);
        REQUIRE(var.can_convert<variant_enum_test>() == true);

        // true case
        bool ok = false;
        CHECK(var.convert<variant_enum_test>(&ok) == variant_enum_test::VALUE_1);
        CHECK(ok == true);

        REQUIRE(var.convert(type::get<variant_enum_test>()) == true);
        CHECK(var.get_value<variant_enum_test>() == variant_enum_test::VALUE_1);

        // false case
        var = false;
        CHECK(var.convert<variant_enum_test>(&ok) == variant_enum_test::VALUE_0);
        CHECK(ok == true);

        REQUIRE(var.convert(type::get<variant_enum_test>()) == true);
        CHECK(var.get_value<variant_enum_test>() == variant_enum_test::VALUE_0);
    }

    SECTION("invalid conversion")
    {
        variant var = true;
        REQUIRE(var.can_convert<variant_enum_test_big>() == true);
        bool ok = false;
        var.convert<variant_enum_test_big>(&ok);
        CHECK(ok == false);

        CHECK(var.convert(type::get<variant_enum_test_big>()) == false);
        CHECK(var.is_type<bool>() == true);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("variant:to enum - from char", "[variant]")
{
    SECTION("valid conversion positive")
    {
        variant var = char('d'); // the dec value for ACII char 'd' is 100
        REQUIRE(var.can_convert<enum_int8_t>() == true);
        bool ok = false;
        CHECK(var.convert<enum_int8_t>(&ok) == enum_int8_t::VALUE_1);
        CHECK(ok == true);

        CHECK(var.convert(type::get<enum_int8_t>()) == true);
        REQUIRE(var.is_type<enum_int8_t>() == true);
        CHECK(var.get_value<enum_int8_t>() == enum_int8_t::VALUE_1);
    }

    SECTION("invalid conversion")
    {
        variant var = char('+');
        REQUIRE(var.can_convert<enum_int8_t>() == true);
        bool ok = false;
        var.convert<enum_int8_t>(&ok); // undefined return value.. thats why no compare is done.
        CHECK(ok == false);

        CHECK(var.convert(type::get<enum_int8_t>()) == false);
        CHECK(var.is_type<char>() == true);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("variant:to enum - from std::string", "[variant]")
{
    SECTION("valid conversion")
    {
        variant var = std::string("VALUE_2");
        REQUIRE(var.can_convert<variant_enum_test>() == true);
        bool ok = false;
        CHECK(var.convert<variant_enum_test>(&ok) == variant_enum_test::VALUE_2);
        CHECK(ok == true);

        CHECK(var.convert(type::get<variant_enum_test>()) == true);
        REQUIRE(var.is_type<variant_enum_test>() == true);
        CHECK(var.get_value<variant_enum_test>() == variant_enum_test::VALUE_2);
    }

    SECTION("invalid conversion")
    {
        variant var = std::string("VALUE_XY");
        REQUIRE(var.can_convert<variant_enum_test>() == true);
        bool ok = false;
        var.convert<variant_enum_test>(&ok); // undefined return value.. thats why no compare is done.
        CHECK(ok == false);

        CHECK(var.convert(type::get<variant_enum_test>()) == false);
        CHECK(var.is_type<std::string>() == true);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("variant:to enum - from int", "[variant]")
{
    SECTION("valid conversion positive")
    {
        variant var = int(2);
        REQUIRE(var.can_convert<variant_enum_test>() == true);
        bool ok = false;
        CHECK(var.convert<variant_enum_test>(&ok) == variant_enum_test::VALUE_2);
        CHECK(ok == true);

        CHECK(var.convert(type::get<variant_enum_test>()) == true);
        REQUIRE(var.is_type<variant_enum_test>() == true);
        CHECK(var.get_value<variant_enum_test>() == variant_enum_test::VALUE_2);
    }

    SECTION("invalid conversion")
    {
        variant var = int(5);
        REQUIRE(var.can_convert<variant_enum_test>() == true);
        bool ok = false;
        var.convert<variant_enum_test>(&ok); // undefined return value.. thats why no compare is done.
        CHECK(ok == false);

        CHECK(var.convert(type::get<variant_enum_test>()) == false);
        CHECK(var.is_type<int>() == true);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("variant:to enum - from float", "[variant]")
{
    SECTION("valid conversion positive")
    {
        variant var = 2.0f;
        REQUIRE(var.can_convert<variant_enum_test>() == true);
        bool ok = false;
        CHECK(var.convert<variant_enum_test>(&ok) == variant_enum_test::VALUE_2);
        CHECK(ok == true);

        CHECK(var.convert(type::get<variant_enum_test>()) == true);
        REQUIRE(var.is_type<variant_enum_test>() == true);
        CHECK(var.get_value<variant_enum_test>() == variant_enum_test::VALUE_2);
    }

    SECTION("invalid conversion")
    {
        variant var = 2.6f;
        REQUIRE(var.can_convert<variant_enum_test>() == true);
        bool ok = false;
        var.convert<variant_enum_test>(&ok); // undefined return value.. thats why no compare is done.
        CHECK(ok == false);

        CHECK(var.convert(type::get<variant_enum_test>()) == false);
        CHECK(var.is_type<float>() == true);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("variant:to enum - from double", "[variant]")
{
    SECTION("valid conversion positive")
    {
        variant var = 2.0;
        REQUIRE(var.can_convert<variant_enum_test>() == true);
        bool ok = false;
        CHECK(var.convert<variant_enum_test>(&ok) == variant_enum_test::VALUE_2);
        CHECK(ok == true);

        CHECK(var.convert(type::get<variant_enum_test>()) == true);
        REQUIRE(var.is_type<variant_enum_test>() == true);
        CHECK(var.get_value<variant_enum_test>() == variant_enum_test::VALUE_2);
    }

    SECTION("invalid conversion")
    {
        variant var = 2.6;
        REQUIRE(var.can_convert<variant_enum_test>() == true);
        bool ok = false;
        var.convert<variant_enum_test>(&ok); // undefined return value.. thats why no compare is done.
        CHECK(ok == false);

        CHECK(var.convert(type::get<variant_enum_test>()) == false);
        CHECK(var.is_type<double>() == true);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("variant:to enum - from int8_t", "[variant]")
{
    SECTION("valid conversion positive")
    {
        variant var = int8_t(2);
        REQUIRE(var.can_convert<variant_enum_test>() == true);
        bool ok = false;
        CHECK(var.convert<variant_enum_test>(&ok) == variant_enum_test::VALUE_2);
        CHECK(ok == true);

        CHECK(var.convert(type::get<variant_enum_test>()) == true);
        REQUIRE(var.is_type<variant_enum_test>() == true);
        CHECK(var.get_value<variant_enum_test>() == variant_enum_test::VALUE_2);
    }

    SECTION("invalid conversion")
    {
        variant var = int8_t(5);
        REQUIRE(var.can_convert<variant_enum_test>() == true);
        bool ok = false;
        var.convert<variant_enum_test>(&ok); // undefined return value.. thats why no compare is done.
        CHECK(ok == false);

        CHECK(var.convert(type::get<variant_enum_test>()) == false);
        CHECK(var.is_type<int8_t>() == true);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("variant:to enum - from int16_t", "[variant]")
{
    SECTION("valid conversion positive")
    {
        variant var = int16_t(2);
        REQUIRE(var.can_convert<variant_enum_test>() == true);
        bool ok = false;
        CHECK(var.convert<variant_enum_test>(&ok) == variant_enum_test::VALUE_2);
        CHECK(ok == true);

        CHECK(var.convert(type::get<variant_enum_test>()) == true);
        REQUIRE(var.is_type<variant_enum_test>() == true);
        CHECK(var.get_value<variant_enum_test>() == variant_enum_test::VALUE_2);
    }

    SECTION("invalid conversion")
    {
        variant var = int16_t(5);
        REQUIRE(var.can_convert<variant_enum_test>() == true);
        bool ok = false;
        var.convert<variant_enum_test>(&ok); // undefined return value.. thats why no compare is done.
        CHECK(ok == false);

        CHECK(var.convert(type::get<variant_enum_test>()) == false);
        CHECK(var.is_type<int16_t>() == true);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("variant:to enum - from int32_t", "[variant]")
{
    SECTION("valid conversion positive")
    {
        variant var = int32_t(2);
        REQUIRE(var.can_convert<variant_enum_test>() == true);
        bool ok = false;
        CHECK(var.convert<variant_enum_test>(&ok) == variant_enum_test::VALUE_2);
        CHECK(ok == true);

        CHECK(var.convert(type::get<variant_enum_test>()) == true);
        REQUIRE(var.is_type<variant_enum_test>() == true);
        CHECK(var.get_value<variant_enum_test>() == variant_enum_test::VALUE_2);
    }

    SECTION("invalid conversion")
    {
        variant var = int32_t(5);
        REQUIRE(var.can_convert<variant_enum_test>() == true);
        bool ok = false;
        var.convert<variant_enum_test>(&ok); // undefined return value.. thats why no compare is done.
        CHECK(ok == false);

        CHECK(var.convert(type::get<variant_enum_test>()) == false);
        CHECK(var.is_type<int32_t>() == true);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("variant:to enum - from int64_t", "[variant]")
{
    SECTION("valid conversion positive")
    {
        variant var = int64_t(2);
        REQUIRE(var.can_convert<variant_enum_test>() == true);
        bool ok = false;
        CHECK(var.convert<variant_enum_test>(&ok) == variant_enum_test::VALUE_2);
        CHECK(ok == true);

        CHECK(var.convert(type::get<variant_enum_test>()) == true);
        REQUIRE(var.is_type<variant_enum_test>() == true);
        CHECK(var.get_value<variant_enum_test>() == variant_enum_test::VALUE_2);
    }

    SECTION("invalid conversion")
    {
        variant var = int64_t(5);
        REQUIRE(var.can_convert<variant_enum_test>() == true);
        bool ok = false;
        var.convert<variant_enum_test>(&ok); // undefined return value.. thats why no compare is done.
        CHECK(ok == false);

        CHECK(var.convert(type::get<variant_enum_test>()) == false);
        CHECK(var.is_type<int64_t>() == true);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("variant:to enum - from uint8_t", "[variant]")
{
    SECTION("valid conversion positive")
    {
        variant var = uint8_t(2);
        REQUIRE(var.can_convert<variant_enum_test>() == true);
        bool ok = false;
        CHECK(var.convert<variant_enum_test>(&ok) == variant_enum_test::VALUE_2);
        CHECK(ok == true);

        CHECK(var.convert(type::get<variant_enum_test>()) == true);
        REQUIRE(var.is_type<variant_enum_test>() == true);
        CHECK(var.get_value<variant_enum_test>() == variant_enum_test::VALUE_2);
    }

    SECTION("invalid conversion")
    {
        variant var = uint8_t(5);
        REQUIRE(var.can_convert<variant_enum_test>() == true);
        bool ok = false;
        var.convert<variant_enum_test>(&ok); // undefined return value.. thats why no compare is done.
        CHECK(ok == false);

        CHECK(var.convert(type::get<variant_enum_test>()) == false);
        CHECK(var.is_type<uint8_t>() == true);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("variant:to enum - from uint16_t", "[variant]")
{
    SECTION("valid conversion positive")
    {
        variant var = uint16_t(2);
        REQUIRE(var.can_convert<variant_enum_test>() == true);
        bool ok = false;
        CHECK(var.convert<variant_enum_test>(&ok) == variant_enum_test::VALUE_2);
        CHECK(ok == true);

        CHECK(var.convert(type::get<variant_enum_test>()) == true);
        REQUIRE(var.is_type<variant_enum_test>() == true);
        CHECK(var.get_value<variant_enum_test>() == variant_enum_test::VALUE_2);
    }

    SECTION("invalid conversion")
    {
        variant var = uint16_t(5);
        REQUIRE(var.can_convert<variant_enum_test>() == true);
        bool ok = false;
        var.convert<variant_enum_test>(&ok); // undefined return value.. thats why no compare is done.
        CHECK(ok == false);

        CHECK(var.convert(type::get<variant_enum_test>()) == false);
        CHECK(var.is_type<uint16_t>() == true);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("variant:to enum - from uint32_t", "[variant]")
{
    SECTION("valid conversion positive")
    {
        variant var = uint32_t(2);
        REQUIRE(var.can_convert<variant_enum_test>() == true);
        bool ok = false;
        CHECK(var.convert<variant_enum_test>(&ok) == variant_enum_test::VALUE_2);
        CHECK(ok == true);

        CHECK(var.convert(type::get<variant_enum_test>()) == true);
        REQUIRE(var.is_type<variant_enum_test>() == true);
        CHECK(var.get_value<variant_enum_test>() == variant_enum_test::VALUE_2);
    }

    SECTION("invalid conversion")
    {
        variant var = uint32_t(5);
        REQUIRE(var.can_convert<variant_enum_test>() == true);
        bool ok = false;
        var.convert<variant_enum_test>(&ok); // undefined return value.. thats why no compare is done.
        CHECK(ok == false);

        CHECK(var.convert(type::get<variant_enum_test>()) == false);
        CHECK(var.is_type<uint32_t>() == true);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("variant:to enum - from uint64_t", "[variant]")
{
    SECTION("valid conversion positive")
    {
        variant var = uint64_t(2);
        REQUIRE(var.can_convert<variant_enum_test>() == true);
        bool ok = false;
        CHECK(var.convert<variant_enum_test>(&ok) == variant_enum_test::VALUE_2);
        CHECK(ok == true);

        CHECK(var.convert(type::get<variant_enum_test>()) == true);
        REQUIRE(var.is_type<variant_enum_test>() == true);
        CHECK(var.get_value<variant_enum_test>() == variant_enum_test::VALUE_2);
    }

    SECTION("invalid conversion")
    {
        variant var = uint64_t(5);
        REQUIRE(var.can_convert<variant_enum_test>() == true);
        bool ok = false;
        var.convert<variant_enum_test>(&ok); // undefined return value.. thats why no compare is done.
        CHECK(ok == false);

        CHECK(var.convert(type::get<variant_enum_test>()) == false);
        CHECK(var.is_type<uint64_t>() == true);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////
