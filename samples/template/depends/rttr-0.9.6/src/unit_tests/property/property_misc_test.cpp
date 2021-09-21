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
#include <rttr/registration_friend>
#include <catch/catch.hpp>

using namespace rttr;

enum class color
{
    red     = 0,
    green   = 1,
    blue    = 2
};

struct prop_misc_test
{

    int value_1;
    bool value_2;
    color color_value;
    std::vector<int> list;
    static int s_value;

    RTTR_REGISTRATION_FRIEND
};

int prop_misc_test::s_value = 0;

/////////////////////////////////////////////////////////////////////////////////////////

RTTR_REGISTRATION
{
    registration::class_<prop_misc_test>("prop_misc_test")
        .property("value_1", &prop_misc_test::value_1)
        (
            metadata("Text", "This is some description text")
        )
        .property_readonly("value_2", &prop_misc_test::value_2)
        .property("color", &prop_misc_test::color_value)
        .property("list", &prop_misc_test::list)
        .property("s_value", &prop_misc_test::s_value)
       ;

    registration::enumeration<color>("color")
                    (
                        value("red",    color::red),
                        value("green",  color::green),
                        value("blue",   color::blue)
                    );
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("property - is_valid()", "[property]")
{
    property prop = type::get<prop_misc_test>().get_property("value_1");
    CHECK(prop.is_valid() == true);

    prop = type::get<prop_misc_test>().get_property("");
    CHECK(prop.is_valid() == false);
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("property - is_readonly()", "[property]")
{
    property prop = type::get<prop_misc_test>().get_property("value_1");
    CHECK(prop.is_readonly() == false);

    prop = type::get<prop_misc_test>().get_property("value_2");
    CHECK(prop.is_readonly() == true);

    prop = type::get<prop_misc_test>().get_property("");
    REQUIRE(prop.is_valid() == false);
    CHECK(prop.is_readonly() == false);
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("property - is_static()", "[property]")
{
    property prop = type::get<prop_misc_test>().get_property("value_1");
    CHECK(prop.is_static() == false);

    prop = type::get<prop_misc_test>().get_property("s_value");
    CHECK(prop.is_static() == true);

    prop = type::get<prop_misc_test>().get_property("");
    REQUIRE(prop.is_valid() == false);
    CHECK(prop.is_static() == false);
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("property - is_enumeration()", "[property]")
{
    property prop = type::get<prop_misc_test>().get_property("value_1");
    CHECK(prop.is_enumeration() == false);

    prop = type::get<prop_misc_test>().get_property("color");
    CHECK(prop.is_enumeration() == true);

    prop = type::get<prop_misc_test>().get_property("");
    REQUIRE(prop.is_valid() == false);
    CHECK(prop.is_enumeration() == false);
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("property - get_enumeration()", "[property]")
{
    property prop = type::get<prop_misc_test>().get_property("value_1");
    CHECK(prop.get_enumeration().is_valid() == false);

    prop = type::get<prop_misc_test>().get_property("color");
    CHECK(prop.get_enumeration().is_valid() == true);

    prop = type::get<prop_misc_test>().get_property("");
    CHECK(prop.is_valid() == false);
    CHECK(prop.get_enumeration().is_valid() == false);
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("property - is_array()", "[property]")
{
    property prop = type::get<prop_misc_test>().get_property("value_1");
    CHECK(prop.get_type().is_sequential_container() == false);

    prop = type::get<prop_misc_test>().get_property("list");
    CHECK(prop.get_type().is_sequential_container() == true);

    prop = type::get<prop_misc_test>().get_property("");
    CHECK(prop.is_valid() == false);
    CHECK(prop.get_type().is_sequential_container() == false);
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("property - get_name()", "[property]")
{
    property prop = type::get<prop_misc_test>().get_property("value_1");
    CHECK(prop.get_name() == "value_1");

    prop = type::get<prop_misc_test>().get_property("");
    CHECK(prop.is_valid() == false);
    CHECK(prop.get_name().empty() == true);
    CHECK(prop.get_name() == "");
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("property - get_type()", "[property]")
{
    property prop = type::get<prop_misc_test>().get_property("value_1");
    CHECK(prop.get_type() == type::get<int>());

    prop = type::get<prop_misc_test>().get_property("");
    CHECK(prop.is_valid() == false);
    CHECK(prop.get_type().is_valid() == false);
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("property - get_declaring_type()", "[property]")
{
    property prop = type::get<prop_misc_test>().get_property("value_1");
    CHECK(prop.get_declaring_type() == type::get<prop_misc_test>());

    prop = type::get<prop_misc_test>().get_property("");
    CHECK(prop.is_valid() == false);
    CHECK(prop.get_declaring_type().is_valid() == false);
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("property - get_metadata()", "[property]")
{
    property prop = type::get<prop_misc_test>().get_property("value_1");
    CHECK(prop.get_metadata("Text") == "This is some description text");
    CHECK(prop.get_metadata("Invalid Key").is_valid() == false);

    prop = type::get<prop_misc_test>().get_property("");
    CHECK(prop.is_valid() == false);
    CHECK(prop.get_metadata("Invalid Key").is_valid() == false);
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("property - compare operators()", "[property]")
{
    property prop1  = type::get<prop_misc_test>().get_property("value_1");
    property prop1a = type::get<prop_misc_test>().get_property("value_1");
    property prop2  = type::get<prop_misc_test>().get_property("value_2");

    CHECK(prop1 == prop1a);
    CHECK(prop1 != prop2);
}

/////////////////////////////////////////////////////////////////////////////////////////
