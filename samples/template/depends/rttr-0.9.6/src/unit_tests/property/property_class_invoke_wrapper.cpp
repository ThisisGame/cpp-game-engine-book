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


struct property_wrapper_test_base
{
    virtual ~property_wrapper_test_base() {}
    int p1;

    RTTR_ENABLE();
};

struct property_wrapper_test : property_wrapper_test_base
{
    bool p2;

    RTTR_ENABLE(property_wrapper_test_base)
};

/////////////////////////////////////////////////////////////////////////////////////////

RTTR_REGISTRATION
{
    registration::class_<property_wrapper_test_base>("property_wrapper_test_base")
        .constructor<>()
        .property("p1", &property_wrapper_test_base::p1)
        ;

    registration::class_<property_wrapper_test>("property_wrapper_test")
        .constructor<>()
        .property("p2", &property_wrapper_test::p2)
        ;
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("property - class - get/set - std::shared_ptr", "[property]")
{
    std::shared_ptr<property_wrapper_test_base> obj = std::make_shared<property_wrapper_test_base>();

    type obj_t = type::get(obj);
    REQUIRE(obj_t.is_wrapper() == true);

    type wrapper_t = obj_t.get_wrapped_type();
    REQUIRE(wrapper_t.is_wrapper() == false);
    REQUIRE(wrapper_t == type::get<property_wrapper_test_base*>());
    property p1 = wrapper_t.get_property("p1");
    CHECK(p1.is_readonly() == false);
    // access
    bool ret = p1.set_value(obj, 2);
    CHECK(ret == true);

    variant val = p1.get_value(obj);
    REQUIRE(val.is_type<int>() == true);

    CHECK(val.get_value<int>() == 2);
    CHECK(obj.get()->p1 == 2);
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("property - class - get/set - std::shared_ptr in variant", "[property]")
{
    variant var = std::make_shared<property_wrapper_test_base>();
    CHECK(var.get_type().is_wrapper() == true);
    CHECK(var.get_type() == type::get<shared_ptr<property_wrapper_test_base>>());
    CHECK(var.get_type().get_wrapped_type() == type::get<property_wrapper_test_base*>());

    type wrapper_t = var.get_type().get_wrapped_type();
    property p1 = wrapper_t.get_property("p1");
    CHECK(p1.is_readonly() == false);
    // access
    bool ret = p1.set_value(var, 2);
    CHECK(ret == true);

    variant val = p1.get_value(var);
    REQUIRE(val.is_type<int>() == true);
    CHECK(val.get_value<int>() == 2);
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("property - class - get/set - reference_wrapper", "[property]")
{
    property_wrapper_test_base instance;
    std::reference_wrapper<property_wrapper_test_base> obj = std::ref(instance);

    type obj_t = type::get(obj);
    REQUIRE(obj_t.is_wrapper() == true);

    type wrapper_t = obj_t.get_wrapped_type();
    CHECK(wrapper_t.is_wrapper() == false);
    CHECK(wrapper_t == type::get<property_wrapper_test_base>());

    property p1 = wrapper_t.get_property("p1");
    CHECK(p1.is_readonly() == false);
    // access
    bool ret = p1.set_value(obj, 2);
    CHECK(ret == true);

    variant val = p1.get_value(obj);
    REQUIRE(val.is_type<int>() == true);
    CHECK(val.get_value<int>() == 2);
    CHECK(obj.get().p1 == 2);
    CHECK(instance.p1 == 2);
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("property - class - get/set - shared_ptr with inheritance", "[property]")
{
    SECTION("from bottom to base")
    {
        std::shared_ptr<property_wrapper_test> obj = std::make_shared<property_wrapper_test>();
        property p1 = type::get(obj).get_wrapped_type().get_property("p1");
        CHECK(p1.is_valid() == true);
        // access
        bool ret = p1.set_value(obj, 2);
        CHECK(ret == true);

        variant val = p1.get_value(obj);
        REQUIRE(val.is_type<int>() == true);

        CHECK(val.get_value<int>() == 2);
        CHECK(obj.get()->p1 == 2);
    }

    SECTION("from base to bottom")
    {
        std::shared_ptr<property_wrapper_test_base> obj = std::make_shared<property_wrapper_test>();

        property p2 = type::get(*obj.get()).get_property("p2");
        CHECK(p2.is_valid() == true);
        // access
        bool ret = p2.set_value(obj, true);
        CHECK(ret == true);

        variant var = p2.get_value(obj);
        REQUIRE(var.is_type<bool>() == true);
        CHECK(var.get_value<bool>() == true);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////
