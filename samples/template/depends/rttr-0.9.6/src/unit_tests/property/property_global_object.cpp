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

static std::string g_name;
static const int g_int_value = 23;
static std::vector<int> g_my_array(1000, 42);

/////////////////////////////////////////////////////////////////////////////////////////

RTTR_REGISTRATION
{
    registration::property("global_obj_1", &g_name) ( metadata("Description", "Some Text") )
        .property_readonly("global_obj_2", &g_int_value) ( metadata("Description", "Some Text") )
        .property("global_obj_3", &g_my_array)
        (
            metadata("Description", "Some Text"),
            policy::prop::bind_as_ptr
        )
        .property_readonly("global_obj_4", &g_int_value)
        (
            metadata("Description", "Some Text"),
            policy::prop::bind_as_ptr
        )
        .property("global_obj_5", &g_my_array)
        (
            metadata("Description", "Some Text"),
            policy::prop::as_reference_wrapper
        )
        .property_readonly("global_obj_6", &g_int_value)
        (
            metadata("Description", "Some Text"),
            policy::prop::as_reference_wrapper
        )
        ;

    registration::property("global_obj_1", &g_name); // cannot register the same object twice
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("property - global object", "[property]")
{
    property prop = type::get_global_property("global_obj_1");
    REQUIRE(prop.is_valid() == true);

    // metadata
    CHECK(prop.is_readonly() == false);
    CHECK(prop.is_static() == true);
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

TEST_CASE("property - global object - read only", "[property]")
{
    property prop = type::get_global_property("global_obj_2");
    REQUIRE(prop.is_valid() == true);

    // metadata
    CHECK(prop.is_readonly() == true);
    CHECK(prop.is_static() == true);
    CHECK(prop.get_type() == type::get<int>());
    CHECK(prop.get_access_level() == rttr::access_levels::public_access);
    CHECK(prop.get_metadata("Description") == "Some Text");

    // valid invoke
    CHECK(prop.get_value(instance()).is_type<int>() == true);
    CHECK(prop.get_value(instance()).get_value<int>() == 23);

    // invalid invoke
    CHECK(prop.set_value(instance(), 42) == false);
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("property - global object - bind as ptr", "[property]")
{
    property prop = type::get_global_property("global_obj_3");
    REQUIRE(prop.is_valid() == true);

    // metadata
    CHECK(prop.is_readonly() == false);
    CHECK(prop.is_static() == false);
    CHECK(prop.get_type().get_raw_type().is_sequential_container() == true);
    CHECK(prop.get_type() == type::get<std::vector<int>*>());
    CHECK(prop.get_access_level() == rttr::access_levels::public_access);
    CHECK(prop.get_metadata("Description") == "Some Text");

    // invoke
    REQUIRE(prop.get_value(instance()).is_type<std::vector<int>*>() == true);
    auto ptr = prop.get_value(instance()).get_value<std::vector<int>*>();
    CHECK(ptr == &g_my_array);
    CHECK(prop.set_value(instance(), ptr) == true);

    std::vector<int> some_vec(1, 12);
    CHECK(prop.set_value(instance(), &some_vec) == true);
    CHECK(some_vec == g_my_array);

    // negative invoke
    CHECK(prop.set_value(instance(), "test") == false);
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("property - global object - read only - bind as ptr", "[property]")
{
    property prop = type::get_global_property("global_obj_4");
    REQUIRE(prop.is_valid() == true);

    // metadata
    CHECK(prop.is_readonly() == true);
    CHECK(prop.is_static() == true);
    CHECK(prop.get_type() == type::get<const int*>());
    CHECK(prop.get_access_level() == rttr::access_levels::public_access);
    CHECK(prop.get_metadata("Description") == "Some Text");

    // valid invoke
    CHECK(prop.get_value(instance()).is_type<const int*>() == true);
    CHECK(*prop.get_value(instance()).get_value<const int*>() == 23);

    // invalid invoke
    CHECK(prop.set_value(instance(), 42) == false);
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("property - global object - as_reference_wrapper", "[property]")
{
    property prop = type::get_global_property("global_obj_5");
    REQUIRE(prop.is_valid() == true);

    // metadata
    CHECK(prop.is_readonly() == false);
    CHECK(prop.is_static() == false);
    CHECK(prop.get_type().get_wrapped_type().is_sequential_container() == true);
    CHECK(prop.get_type() == type::get< std::reference_wrapper<std::vector<int>> >());
    CHECK(prop.get_type().is_wrapper() == true);
    CHECK(prop.get_access_level() == rttr::access_levels::public_access);
    CHECK(prop.get_metadata("Description") == "Some Text");

    // invoke
    REQUIRE(prop.get_value(instance()).is_type< std::reference_wrapper<std::vector<int>> >() == true);
    auto value = prop.get_value(instance()).get_value< std::reference_wrapper<std::vector<int>> >();
    CHECK(value.get() == g_my_array);
    CHECK(prop.set_value(instance(), value) == true);

    std::vector<int> some_vec(1, 12);
    CHECK(prop.set_value(instance(), std::ref(some_vec)) == true);
    CHECK(some_vec == g_my_array);

    // negative invoke
    CHECK(prop.set_value(instance(), "test") == false);
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("property - global object - read only - as_reference_wrapper", "[property]")
{
    property prop = type::get_global_property("global_obj_6");
    REQUIRE(prop.is_valid() == true);

    // metadata
    CHECK(prop.is_readonly() == true);
    CHECK(prop.is_static() == true);
    CHECK(prop.get_type() == type::get< std::reference_wrapper<const int> >());
    CHECK(prop.get_type().is_wrapper() == true);
    CHECK(prop.get_access_level() == rttr::access_levels::public_access);
    CHECK(prop.get_metadata("Description") == "Some Text");

    // valid invoke
    CHECK(prop.get_value(instance()).is_type< std::reference_wrapper<const int> >() == true);
    CHECK(prop.get_value(instance()).get_value< std::reference_wrapper<const int> >().get() == 23);

    // invalid invoke
    int value = 42;
    CHECK(prop.set_value(instance(), std::cref(value)) == false);
}

/////////////////////////////////////////////////////////////////////////////////////////
