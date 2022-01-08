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

using namespace rttr;
using namespace std;

#include <iostream>
#include <memory>
#include <functional>

#include <catch/catch.hpp>

enum ui_metainfo
{
    tool_tip    = 0,
    description = 1
};

enum class weekday : int
{
    monday,
    tuesday,
    wednesday,
    thursday,
    friday,
    saturday,
    sunday,
};

enum class some_enum
{
    nothing_fancy
};

struct enum_test_class
{
    enum class color : int
    {
        red,
        green,
        blue
    };
};

/////////////////////////////////////////////////////////////////////////////////////////

RTTR_REGISTRATION
{
    registration::enumeration<weekday>("weekday")
    (
        value("Monday",    weekday::monday),
        value("Tuesday",   weekday::tuesday),
        value("Wednesday", weekday::wednesday),
        value("Thursday",  weekday::thursday),
        value("Friday",    weekday::friday),
        value("Saturday",  weekday::saturday),
        value("Sunday",    weekday::sunday),
        metadata(ui_metainfo::description, "List of Weekdays.")
    );

    registration::enumeration<some_enum>("some_enum")
    (
        value("nothing_fancy", some_enum::nothing_fancy)
    );

    registration::class_<enum_test_class>("enum_test_class")
        .enumeration<enum_test_class::color>("enum_test_class::color")
        (
            value("red",    enum_test_class::color::red),
            value("green",  enum_test_class::color::green),
            value("blue",   enum_test_class::color::blue)
        );
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("enumeration - is_valid()", "[enumeration]")
{
    enumeration e = type::get_by_name("weekday").get_enumeration();
    CHECK(e.is_valid() == true);
    CHECK(static_cast<bool>(e) == true);

    // negative
    e = type::get_by_name("weekday_unknown").get_enumeration();
    CHECK(e.is_valid() == false);
    CHECK(static_cast<bool>(e) == false);
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("enumeration - get_name()", "[enumeration]")
{
    enumeration e = type::get_by_name("weekday").get_enumeration();
    CHECK(e.get_name() == "weekday");

    // negative
    e = type::get_by_name("weekday_unknown").get_enumeration();
    CHECK(e.get_name() == "");
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("enumeration - get_underlying_type()", "[enumeration]")
{
    enumeration e = type::get_by_name("weekday").get_enumeration();
    CHECK(e.get_underlying_type() == type::get<int>());

    // negative
    e = type::get_by_name("weekday_unknown").get_enumeration();
    CHECK(e.get_underlying_type().is_valid() == false);
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("enumeration - get_type()", "[enumeration]")
{
    enumeration e = type::get_by_name("weekday").get_enumeration();
    CHECK(e.get_type() == type::get<weekday>());

    // negative
    e = type::get_by_name("weekday_unknown").get_enumeration();
    CHECK(e.get_type().is_valid() == false);
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("enumeration - get_declaring_type()", "[enumeration]")
{
    enumeration e = type::get_by_name("weekday").get_enumeration();
    CHECK(e.get_declaring_type().is_valid() == false);

    e = type::get_by_name("enum_test_class::color").get_enumeration();
    CHECK(e.get_declaring_type() == type::get<enum_test_class>());


    // negative
    e = type::get_by_name("weekday_unknown").get_enumeration();
    CHECK(e.get_declaring_type().is_valid() == false);
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("enumeration - get_names()", "[enumeration]")
{
    enumeration e = type::get_by_name("weekday").get_enumeration();
    std::vector<std::string> name_list(e.get_names().begin(), e.get_names().end());
    REQUIRE(name_list.size() == 7);
    CHECK(name_list[0] == "Monday");
    CHECK(name_list[1] == "Tuesday");
    CHECK(name_list[2] == "Wednesday");
    CHECK(name_list[3] == "Thursday");
    CHECK(name_list[4] == "Friday");
    CHECK(name_list[5] == "Saturday");
    CHECK(name_list[6] == "Sunday");

    // negative
    e = type::get_by_name("weekday_unknown").get_enumeration();
    CHECK(e.get_names().size() == 0);
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("enumeration - get_values()", "[enumeration]")
{
    enumeration e = type::get_by_name("weekday").get_enumeration();
    std::vector<variant> value_list(e.get_values().begin(), e.get_values().end());
    REQUIRE(value_list.size() == 7);

    for (auto& var : value_list)
    {
        REQUIRE(var.is_type<weekday>() == true);
    }

    CHECK(value_list[0].get_value<weekday>() == weekday::monday);
    CHECK(value_list[1].get_value<weekday>() == weekday::tuesday);
    CHECK(value_list[2].get_value<weekday>() == weekday::wednesday);
    CHECK(value_list[3].get_value<weekday>() == weekday::thursday);
    CHECK(value_list[4].get_value<weekday>() == weekday::friday);
    CHECK(value_list[5].get_value<weekday>() == weekday::saturday);
    CHECK(value_list[6].get_value<weekday>() == weekday::sunday);

    // negative
    e = type::get_by_name("weekday_unknown").get_enumeration();
    CHECK(e.get_values().size() == 0);
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("enumeration - compare operators()", "[enumeration]")
{
    enumeration e1 = type::get_by_name("weekday").get_enumeration();
    enumeration e1_a = type::get_by_name("weekday").get_enumeration();
    enumeration e2 = type::get_by_name("some_enum").get_enumeration();
    enumeration e2_invalid = type::get_by_name("weekday_unknown").get_enumeration();

    CHECK(e1 == e1_a);
    CHECK(e1 != e2);

    // negative test
    CHECK(e1 != e2_invalid);
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("enumeration - get_metadata()", "[enumeration]")
{
    enumeration e = type::get_by_name("weekday").get_enumeration();
    variant var = e.get_metadata(ui_metainfo::description);
    REQUIRE(var.is_type<std::string>() == true);

    CHECK(var.get_value<std::string>() == "List of Weekdays.");

    // negative test
    e = type::get_by_name("weekday_unknown").get_enumeration();
    var = e.get_metadata(ui_metainfo::description);
    REQUIRE(var.is_valid() == false);
}

/////////////////////////////////////////////////////////////////////////////////////////
