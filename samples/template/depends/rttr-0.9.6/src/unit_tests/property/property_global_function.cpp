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

#include <iostream>
#include <memory>
#include <functional>

#include <catch/catch.hpp>

using namespace rttr;
using namespace std;


/////////////////////////////////////////////////////////////////////////////////////////
// init static variables and some global functions added as properties to the test class

static std::string g_name;

int get_value() { return 512; }

static const std::string& get_name()
{
    return g_name;
}

static void set_name(const std::string& text)
{
    g_name = text;
}

static int& get_int_value()
{
    static int value = 42;
    return value;
}


/////////////////////////////////////////////////////////////////////////////////////////

RTTR_REGISTRATION
{
    registration::property("global_func_1", &get_name, &set_name) ( metadata("Description", "Some Text") )
        .property_readonly("global_func_2", &get_value) ( metadata("Description", "Some Text") )
        .property("global_func_3", &get_name, &set_name)
        (
            metadata("Description", "Some Text"),
            policy::prop::bind_as_ptr
        )
        .property_readonly("global_func_4", &get_int_value)
        (
            metadata("Description", "Some Text"),
            policy::prop::bind_as_ptr
        )
        .property_readonly("global_func_5", [](){ return 45;})
        .property_readonly("global_func_6", &get_int_value)
        (
            metadata("Description", "Some Text"),
            policy::prop::as_reference_wrapper
        )
        .property("global_func_7", &get_name, &set_name)
        (
            metadata("Description", "Some Text"),
            policy::prop::as_reference_wrapper
        )
        ;
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("property - global function", "[property]")
{
    property prop = type::get_global_property("global_func_1");
    REQUIRE(prop.is_valid() == true);

    // metadata
    CHECK(prop.is_readonly() == false);
    CHECK(prop.is_static() == true);
    CHECK(prop.get_declaring_type().is_valid() == false);
    CHECK(prop.get_type() == type::get<std::string>());
    CHECK(prop.get_access_level() == rttr::access_levels::public_access);
    CHECK(prop.get_metadata("Description") == "Some Text");

    // valid invoke
    CHECK(prop.set_value(instance(), std::string("New Text")) == true);
    CHECK(prop.get_value(instance()).is_type<std::string>() == true);
    CHECK(prop.get_value(instance()).get_value<std::string>() == "New Text");

    // invalid invoke
    CHECK(prop.set_value(instance(), 42) == false);
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("property - global function - read only", "[property]")
{
    property prop = type::get_global_property("global_func_2");
    REQUIRE(prop.is_valid() == true);

    // metadata
    CHECK(prop.is_readonly() == true);
    CHECK(prop.is_static() == true);
    CHECK(prop.get_type() == type::get<int>());
    CHECK(prop.get_access_level() == rttr::access_levels::public_access);
    CHECK(prop.get_metadata("Description") == "Some Text");

    // valid invoke
    CHECK(prop.get_value(instance()).is_type<int>() == true);
    CHECK(prop.get_value(instance()).get_value<int>() == 512);

    // invalid invoke
    CHECK(prop.set_value(instance(), 23) == false);
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("property - global function - bind as ptr", "[property]")
{
    property prop = type::get_global_property("global_func_3");
    REQUIRE(prop.is_valid() == true);

    // metadata
    CHECK(prop.is_readonly() == false);
    CHECK(prop.is_static() == true);
    CHECK(prop.get_type() == type::get<const std::string*>());
    CHECK(prop.get_access_level() == rttr::access_levels::public_access);
    CHECK(prop.get_metadata("Description") == "Some Text");

    // valid invoke
    const std::string text("Hello World");
    CHECK(prop.set_value(instance(), &text) == true);
    CHECK(prop.get_value(instance()).is_type<const std::string*>() == true);
    CHECK(*prop.get_value(instance()).get_value<const std::string*>() == "Hello World");

    // invalid invoke
    CHECK(prop.set_value(instance(), 42) == false);
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("property - global function - read only - bind as ptr", "[property]")
{
    property prop = type::get_global_property("global_func_4");
    REQUIRE(prop.is_valid() == true);

    // metadata
    CHECK(prop.is_readonly() == true);
    CHECK(prop.is_static() == true);
    CHECK(prop.get_declaring_type().is_valid() == false);
    CHECK(prop.get_type() == type::get<const int*>());
    CHECK(prop.get_access_level() == rttr::access_levels::public_access);
    CHECK(prop.get_metadata("Description") == "Some Text");

    // valid invoke
    CHECK(prop.get_value(instance()).is_type<const int*>() == true);
    CHECK(*prop.get_value(instance()).get_value<const int*>() == 42);

    // invalid invoke
    CHECK(prop.set_value(instance(), 23) == false);
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("property - global function - read only - std::function", "[property]")
{
    property prop = type::get_global_property("global_func_5");
    REQUIRE(prop.is_valid() == true);

    CHECK(prop.is_static() == true);
    CHECK(prop.set_value(instance(), 23) == false);

    variant var = prop.get_value(instance());
    CHECK(var.is_type<int>() == true);
    CHECK(var.get_value<int>() == 45);
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("property - global function - read only - as_reference_wrapper", "[property]")
{
    property prop = type::get_global_property("global_func_6");
    REQUIRE(prop.is_valid() == true);

    // metadata
    CHECK(prop.is_readonly() == true);
    CHECK(prop.is_static() == true);
    CHECK(prop.get_type() == type::get<std::reference_wrapper<const int>>());
    CHECK(prop.get_type().is_wrapper() == true);
    CHECK(prop.get_access_level() == rttr::access_levels::public_access);
    CHECK(prop.get_metadata("Description") == "Some Text");

    // valid invoke
    CHECK(prop.get_value(instance()).is_type<std::reference_wrapper<const int>>() == true);
    CHECK(prop.get_value(instance()).get_value<std::reference_wrapper<const int>>().get() == 42);

    // invalid invoke
    CHECK(prop.set_value(instance(), 23) == false);
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("property - global function - as_reference_wrapper", "[property]")
{
    property prop = type::get_global_property("global_func_7");
    REQUIRE(prop.is_valid() == true);
    // metadata
    CHECK(prop.is_readonly() == false);
    CHECK(prop.is_static() == true);
    CHECK(prop.get_type() == type::get< std::reference_wrapper<const std::string> >());
    CHECK(prop.get_type().is_wrapper() == true);
    CHECK(prop.get_access_level() == rttr::access_levels::public_access);
    CHECK(prop.get_metadata("Description") == "Some Text");

    // valid invoke
    const std::string text("Hello World");
    CHECK(prop.set_value(instance(), std::ref(text)) == true);
    CHECK(prop.get_value(instance()).is_type< std::reference_wrapper<const std::string> >() == true);
    CHECK(prop.get_value(instance()).get_value< std::reference_wrapper<const std::string> >().get() == "Hello World");

    // invalid invoke
    CHECK(prop.set_value(instance(), 42) == false);
}

/////////////////////////////////////////////////////////////////////////////////////////
