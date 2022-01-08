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

using namespace rttr;
using namespace std;

enum E_MetaData
{
    SCRIPTABLE  = 0,
    TOOL_TIP    = 1,
    DESCRIPTION = 2
};

struct method_misc_test
{
    method_misc_test() { }
    void func(){ }

    int func_return(){ return 42; }

    void default_func(std::string value = "text") {}

    static void static_func() {}
};

/////////////////////////////////////////////////////////////////////////////////////////

struct base_class_with_methods
{
    base_class_with_methods(){}
    void some_method() {}

    RTTR_ENABLE()
};

struct derived_class_without_registered_methods : base_class_with_methods
{
    RTTR_ENABLE(base_class_with_methods)
};


/////////////////////////////////////////////////////////////////////////////////////////

RTTR_REGISTRATION
{
    registration::class_<method_misc_test>("method_misc_test")
        .method("func", &method_misc_test::func)
        (
            metadata(E_MetaData::SCRIPTABLE, true),
            metadata("Text",  "Some funky description")
        )
        .method("func_return", &method_misc_test::func_return)
        .method("static_func", &method_misc_test::static_func)

        .method("default_func", &method_misc_test::default_func)
        (
            default_arguments(std::string("text")),
            metadata(E_MetaData::SCRIPTABLE, true),
            metadata("Text",  "Some funky description")
        )
        ;

    registration::class_<base_class_with_methods>("base_class_with_methods")
        .method("some_method", &base_class_with_methods::some_method);

    registration::class_<derived_class_without_registered_methods>("derived_class_without_registered_methods");
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("method - get_name()", "[method]")
{
    method meth = type::get_by_name("method_misc_test").get_method("func");
    CHECK(meth.get_name() == "func");

    meth = type::get_by_name("method_misc_test").get_method("default_func");
    CHECK(meth.get_name() == "default_func");

    // negative test
    meth = type::get_by_name("method_misc_test").get_method("");
    CHECK(meth.get_name() == "");
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("method - is_static()", "[method]")
{
    method meth = type::get_by_name("method_misc_test").get_method("static_func");
    CHECK(meth.is_static() == true);

    meth = type::get_by_name("method_misc_test").get_method("func");
    CHECK(meth.is_static() == false);

    meth = type::get_by_name("method_misc_test").get_method("default_func");
    CHECK(meth.is_static() == false);

    // negative test
    meth = type::get_by_name("method_misc_test").get_method("");
    CHECK(meth.is_static() == false);
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("method - get_return_type()", "[method]")
{
    method meth = type::get_by_name("method_misc_test").get_method("func_return");
    CHECK(meth.get_return_type() == type::get<int>());

    meth = type::get_by_name("method_misc_test").get_method("default_func");
    CHECK(meth.get_return_type() == type::get<void>());

    // negative test
    meth = type::get_by_name("method_misc_test").get_method("");
    CHECK(meth.get_return_type().is_valid() == false);
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("method - get_declaring_type()", "[method]")
{
    method meth = type::get_by_name("method_misc_test").get_method("func");
    CHECK(meth.get_declaring_type() == type::get<method_misc_test>());

    meth = type::get_by_name("method_misc_test").get_method("default_func");
    CHECK(meth.get_declaring_type() == type::get<method_misc_test>());

    // negative test
    meth = type::get_by_name("method_misc_test").get_method("");
    CHECK(meth.get_declaring_type().is_valid() == false);
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("method - get_signature()", "[method]")
{
    method meth = type::get_by_name("method_misc_test").get_method("func");
    CHECK(meth.get_signature() == "func( )");

    meth = type::get_by_name("method_misc_test").get_method("default_func");
    CHECK(meth.get_signature() == "default_func( std::string )");

    // negative test
    meth = type::get_by_name("method_misc_test").get_method("");
    CHECK(meth.get_signature() == "");
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("method - compare operators", "[method]")
{
    method meth_1 = type::get_by_name("method_misc_test").get_method("func");
    method meth_1a = type::get_by_name("method_misc_test").get_method("func");
    method meth_2 = type::get_by_name("method_misc_test").get_method("func_return");
    method meth_invalid = type::get_by_name("method_misc_test").get_method("");

    CHECK(meth_1 == meth_1a);
    CHECK(meth_1 != meth_2);

    // negative test
    CHECK(meth_1 != meth_invalid);
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("method - get_metadata()", "[method]")
{
    method meth = type::get_by_name("method_misc_test").get_method("func");
    variant var = meth.get_metadata(E_MetaData::SCRIPTABLE);
    REQUIRE(var.is_type<bool>() == true);
    CHECK(var.get_value<bool>() == true);

    var = meth.get_metadata("Text");
    REQUIRE(var.is_type<std::string>() == true);
    CHECK(var.get_value<std::string>() == "Some funky description");
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("method - default func - get_metadata()", "[method]")
{
    method meth = type::get_by_name("method_misc_test").get_method("default_func");
    variant var = meth.get_metadata(E_MetaData::SCRIPTABLE);
    REQUIRE(var.is_type<bool>() == true);
    CHECK(var.get_value<bool>() == true);

    var = meth.get_metadata("Text");
    REQUIRE(var.is_type<std::string>() == true);
    CHECK(var.get_value<std::string>() == "Some funky description");
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("method - NEGATIVE - get_metadata()", "[method]")
{
    method meth = type::get_by_name("method_misc_test").get_method("func");
    // no metadata found
    variant var = meth.get_metadata(E_MetaData::TOOL_TIP);
    CHECK(var.is_type<bool>() == false);
    CHECK(var.is_valid() == false);

    // no method found, no metadata found
    meth = type::get_by_name("method_misc_test").get_method("");
    CHECK(meth.is_valid() == false);
    var = meth.get_metadata(E_MetaData::TOOL_TIP);
    CHECK(var.is_valid() == false);
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("method - default_func - NEGATIVE - get_metadata()", "[method]")
{
    method meth = type::get_by_name("method_misc_test").get_method("default_func");
    // no metadata found
    variant var = meth.get_metadata(E_MetaData::TOOL_TIP);
    CHECK(var.is_type<bool>() == false);
    CHECK(var.is_valid() == false);
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("method - check inheritance of methods", "[method]")
{
    // base class has registered methodes, the derived class not
    type t = type::get<derived_class_without_registered_methods>();
    auto meth_range = t.get_methods();
    REQUIRE(meth_range.size() == 1);

    CHECK((*meth_range.begin()).get_name() == "some_method");
}

/////////////////////////////////////////////////////////////////////////////////////////

