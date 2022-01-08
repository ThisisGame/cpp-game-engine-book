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

struct prop_access_level_test
{
public:
    int value_1;
    int value_4;
    bool value_5;
    double value_8;

    int get_value_1() { return value_1; }

    double get_value_2() const { return value_8; }
    void set_value_2(double value) { value_8 = value; }

protected:
    int value_2;
    bool value_6;
private:
    int value_3;
    bool value_7;

    RTTR_REGISTRATION_FRIEND
};

/////////////////////////////////////////////////////////////////////////////////////////

RTTR_REGISTRATION
{
    registration::class_<prop_access_level_test>("prop_access_level_test")
        .property("val_1", &prop_access_level_test::value_1) // default, should be "public_access"
        .property("val_2", &prop_access_level_test::value_2, registration::private_access)
        .property("val_3", &prop_access_level_test::value_3, registration::protected_access)
        .property("val_4", &prop_access_level_test::value_4, registration::public_access)
        .property("val_5", &prop_access_level_test::value_5) // default, should be "public_access"
        (
            metadata(23, 42) // this metadata is just for testing, whether the accessor will be forwarded when using additional binding data
        )
        .property("val_6", &prop_access_level_test::value_6, registration::protected_access)
        (
            metadata(23, 42)
        )
        .property("val_7", &prop_access_level_test::value_7, registration::private_access)
        (
            metadata(23, 42)
        )
        .property("val_8", &prop_access_level_test::value_8, registration::public_access)
        (
            metadata(23, 42)
        )
        /////////////////////////////////////////////////////////////////////////////////
        // property with getter/setter function
        .property("prop_func_1", &prop_access_level_test::get_value_2, &prop_access_level_test::set_value_2) // default, should be "public_access"
        .property("prop_func_2", &prop_access_level_test::get_value_2, &prop_access_level_test::set_value_2, registration::private_access)
        .property("prop_func_3", &prop_access_level_test::get_value_2, &prop_access_level_test::set_value_2, registration::protected_access)
        .property("prop_func_4", &prop_access_level_test::get_value_2, &prop_access_level_test::set_value_2, registration::public_access)
        .property("prop_func_5", &prop_access_level_test::get_value_2, &prop_access_level_test::set_value_2) // default, should be "public_access"
        (
            metadata(23, 42)
        )
        .property("prop_func_6", &prop_access_level_test::get_value_2, &prop_access_level_test::set_value_2, registration::protected_access)
        (
            metadata(23, 42)
        )
        .property("prop_func_7", &prop_access_level_test::get_value_2, &prop_access_level_test::set_value_2, registration::private_access)
        (
            metadata(23, 42)
        )
        .property("prop_func_8", &prop_access_level_test::get_value_2, &prop_access_level_test::set_value_2, registration::public_access)
        (
            metadata(23, 42)
        )
        /////////////////////////////////////////////////////////////////////////////////
        // property_readonly
        .property_readonly("const_val_1", &prop_access_level_test::get_value_1) // default, should be "public_access"
        .property_readonly("const_val_2", &prop_access_level_test::get_value_1, registration::private_access)
        .property_readonly("const_val_3", &prop_access_level_test::get_value_1, registration::protected_access)
        .property_readonly("const_val_4", &prop_access_level_test::get_value_1, registration::public_access)
        .property_readonly("const_val_5", &prop_access_level_test::get_value_1) // default, should be "public_access"
        (
            metadata(23, 42)
        )
        .property_readonly("const_val_6", &prop_access_level_test::get_value_1, registration::protected_access)
        (
            metadata(23, 42)
        )
        .property_readonly("const_val_7", &prop_access_level_test::get_value_1, registration::private_access)
        (
            metadata(23, 42)
        )
        .property_readonly("const_val_8", &prop_access_level_test::get_value_1, registration::public_access)
        (
            metadata(23, 42)
        )
       ;
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("property - access_levels test - object ptr", "[property]")
{
    type t = type::get_by_name("prop_access_level_test");
    REQUIRE(t.is_valid() == true);

    // has to be checked, because get_access_level() default return value is public_access
    CHECK(t.get_property("val_1").is_valid() == true);
    CHECK(t.get_property("val_1").get_access_level() == access_levels::public_access);

    CHECK(t.get_property("val_2").get_access_level() == access_levels::private_access);
    CHECK(t.get_property("val_3").get_access_level() == access_levels::protected_access);
    CHECK(t.get_property("val_4").get_access_level() == access_levels::public_access);

    // has to be checked, because get_access_level() default return value is public_access
    CHECK(t.get_property("val_5").is_valid() == true);
    CHECK(t.get_property("val_5").get_access_level() == access_levels::public_access);

    CHECK(t.get_property("val_6").get_access_level() == access_levels::protected_access);
    CHECK(t.get_property("val_7").get_access_level() == access_levels::private_access);
    CHECK(t.get_property("val_8").get_access_level() == access_levels::public_access);
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("property - access_levels test - getter/setter function", "[property]")
{
    type t = type::get_by_name("prop_access_level_test");
    REQUIRE(t.is_valid() == true);

    // has to be checked, because get_access_level() default return value is public_access
    CHECK(t.get_property("prop_func_1").is_valid() == true);
    CHECK(t.get_property("prop_func_1").get_access_level() == access_levels::public_access);

    CHECK(t.get_property("prop_func_2").get_access_level() == access_levels::private_access);
    CHECK(t.get_property("prop_func_3").get_access_level() == access_levels::protected_access);
    CHECK(t.get_property("prop_func_4").get_access_level() == access_levels::public_access);

    // has to be checked, because get_access_level() default return value is public_access
    CHECK(t.get_property("prop_func_5").is_valid() == true);
    CHECK(t.get_property("prop_func_5").get_access_level() == access_levels::public_access);

    CHECK(t.get_property("prop_func_6").get_access_level() == access_levels::protected_access);
    CHECK(t.get_property("prop_func_7").get_access_level() == access_levels::private_access);
    CHECK(t.get_property("prop_func_8").get_access_level() == access_levels::public_access);
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("property - access_levels test - readonly property", "[property]")
{
    type t = type::get_by_name("prop_access_level_test");
    REQUIRE(t.is_valid() == true);

    // has to be checked, because get_access_level() default return value is public_access
    CHECK(t.get_property("const_val_1").is_valid() == true);
    CHECK(t.get_property("const_val_1").get_access_level() == access_levels::public_access);

    CHECK(t.get_property("const_val_2").get_access_level() == access_levels::private_access);
    CHECK(t.get_property("const_val_3").get_access_level() == access_levels::protected_access);
    CHECK(t.get_property("const_val_4").get_access_level() == access_levels::public_access);

    // has to be checked, because get_access_level() default return value is public_access
    CHECK(t.get_property("const_val_5").is_valid() == true);
    CHECK(t.get_property("const_val_5").get_access_level() == access_levels::public_access);

    CHECK(t.get_property("const_val_6").get_access_level() == access_levels::protected_access);
    CHECK(t.get_property("const_val_7").get_access_level() == access_levels::private_access);
    CHECK(t.get_property("const_val_8").get_access_level() == access_levels::public_access);
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("property - NEGATIVE - access_levels", "[property]")
{
    type t = type::get_by_name("prop_access_level_test");
    REQUIRE(t.is_valid() == true);

    CHECK(t.get_property("").is_valid() == false);
    CHECK(t.get_property("").get_access_level() == access_levels::public_access);
}

/////////////////////////////////////////////////////////////////////////////////////////
