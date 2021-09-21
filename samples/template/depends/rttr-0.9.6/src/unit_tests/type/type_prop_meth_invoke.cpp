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
#include <catch/catch.hpp>

struct type_prop_invoke_test_base
{
    int p1 = 12;

    RTTR_ENABLE()
};

struct type_prop_invoke_test : type_prop_invoke_test_base
{
    std::string p2 = "text";

    RTTR_ENABLE(type_prop_invoke_test_base)
};

static int g_prop_invoke = 42;


using namespace rttr;

RTTR_REGISTRATION
{
    registration::class_<type_prop_invoke_test_base>("type_prop_invoke_test_base")
        .property("p1", &type_prop_invoke_test_base::p1);

    registration::class_<type_prop_invoke_test>("type_prop_invoke_test")
        .property("p2", &type_prop_invoke_test::p2);

    registration::property("g_prop_invoke", &g_prop_invoke);
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("Test property shortcuts to set/get property", "[property]")
{
    SECTION("test set property with instance")
    {
        type_prop_invoke_test_base obj;
        variant var = type::get(obj).get_property_value("p1", obj);
        REQUIRE(var.is_type<int>() == true);
        CHECK(var.get_value<int>() == 12);

        bool success = type::get(obj).set_property_value("p1", obj, 500);
        CHECK(success == true);
        CHECK(obj.p1 == 500);
    }

    SECTION("test set property with derived instance")
    {
        // derived obj
        type_prop_invoke_test obj;

        variant var = type::get(obj).get_property_value("p2", obj);
        REQUIRE(var.is_type<std::string>() == true);
        CHECK(var.get_value<std::string>() == "text");

        bool success = type::get(obj).set_property_value("p2", obj, std::string("Hello World"));
        CHECK(success == true);
        CHECK(obj.p2 == "Hello World");
    }

    SECTION("test set global property")
    {
        bool success = type::set_property_value("g_prop_invoke", 23);
        CHECK(success == true);

        variant var = type::get_property_value("g_prop_invoke");
        REQUIRE(var.is_type<int>() == true);
        CHECK(var.get_value<int>() == 23);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////
