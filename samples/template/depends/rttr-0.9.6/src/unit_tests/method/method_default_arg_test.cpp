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

/////////////////////////////////////////////////////////////////////////////////////////

struct default_arg_test
{
    bool method_with_default(int val_1, const std::string& val_2 = "This is a default", const int* val_3 = nullptr)
    {
        if (val_2 == "This is a default" && val_3 == nullptr)
            return true;
        else
            return false;
    }
};

/////////////////////////////////////////////////////////////////////////////////////////

static bool global_meth_with_defaults(int val_1, const std::string& val_2 = "This is a default", const int* val_3 = nullptr)
{
    if (val_2 == "This is a default" && val_3 == nullptr)
        return true;
    else
        return false;
}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

RTTR_REGISTRATION
{
    registration::class_<default_arg_test>("default_arg_test")
        .method("method_with_default", &default_arg_test::method_with_default)
        (
            default_arguments(std::string("This is a default"), nullptr)
        );

    registration::method("global_meth_with_defaults", &global_meth_with_defaults)
    (
        default_arguments(std::string("This is a default"), nullptr)
    );
}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("method - default argument test", "[method]")
{
    type t = type::get_by_name("default_arg_test");
    method meth = t.get_method("method_with_default");

    default_arg_test obj;

    // using two default arguments
    variant var = meth.invoke(obj, 23);
    REQUIRE(var.is_type<bool>());
    // invoke was successfully
    CHECK(var.get_value<bool>() == true);

    // using one default argument
    var = meth.invoke(obj, 23, std::string("This is a default"));
    REQUIRE(var.is_type<bool>());
    CHECK(var.get_value<bool>() == true);

    // using no default argument
    const int* ptr = nullptr;
    var = meth.invoke(obj, 23, std::string("This is a default"), ptr);
    REQUIRE(var.is_type<bool>());
    CHECK(var.get_value<bool>() == true);

    // too less arguments
    var = meth.invoke(obj);
    CHECK(var.is_valid() == false);

    // too much arguments
    var = meth.invoke(obj, 23, std::string("This is a default"), ptr, 45);
    CHECK(var.is_valid() == false);
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("method - default argument test (invoke_variadic)", "[method]")
{
    type t = type::get_by_name("default_arg_test");
    method meth = t.get_method("method_with_default");

    default_arg_test obj;

    // using two default arguments
    variant var = meth.invoke_variadic(obj, {23});
    REQUIRE(var.is_type<bool>());
    // invoke was successfully
    CHECK(var.get_value<bool>() == true);

    // using one default argument
#if RTTR_COMPILER == RTTR_COMPILER_MSVC && RTTR_COMP_VER <= 1800
    std::string string_value = "This is a default";
    var = meth.invoke_variadic(obj, {23, string_value});
#else
    var = meth.invoke_variadic(obj, {23, std::string("This is a default")});
#endif

    REQUIRE(var.is_type<bool>());
    CHECK(var.get_value<bool>() == true);

    // using no default argument
    const int* ptr = nullptr;
#if RTTR_COMPILER == RTTR_COMPILER_MSVC && RTTR_COMP_VER <= 1800
    var = meth.invoke_variadic(obj, {23, string_value, ptr});
#else
    var = meth.invoke_variadic(obj, {23, std::string("This is a default"), ptr});
#endif
    REQUIRE(var.is_type<bool>());
    CHECK(var.get_value<bool>() == true);


    // too less arguments
    var = meth.invoke_variadic(obj, {});
    CHECK(var.is_valid() == false);

    // too much arguments
#if RTTR_COMPILER == RTTR_COMPILER_MSVC && RTTR_COMP_VER <= 1800
    var = meth.invoke_variadic(obj, {23, string_value, ptr, 45});
#else
    var = meth.invoke_variadic(obj, {23, std::string("This is a default"), ptr, 45});
#endif
    CHECK(var.is_valid() == false);
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("method - default argument test - invoke via type - member method", "[method]")
{
    type t = type::get_by_name("default_arg_test");
    REQUIRE(t.is_valid() == true);

    default_arg_test obj;

    CHECK(t.invoke("method_with_default", obj, {}).is_valid()   == false);
    CHECK(t.invoke("method_with_default", obj, {23}).is_valid() == true);
    CHECK(t.invoke("method_with_default", obj, {23, std::string("This is a default")}).is_valid() == true);
    CHECK(t.invoke("method_with_default", obj, {23, std::string("This is a default"), nullptr, 45}).is_valid() == false);
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("method - default argument test - invoke via type - global method", "[method]")
{
    CHECK(type::invoke("global_meth_with_defaults", {}).is_valid()   == false);
    CHECK(type::invoke("global_meth_with_defaults", {23}).is_valid() == true);
    CHECK(type::invoke("global_meth_with_defaults", {23, std::string("This is a default")}).is_valid() == true);
    CHECK(type::invoke("global_meth_with_defaults", {23, std::string("This is a default"), nullptr, 45}).is_valid() == false);
}

/////////////////////////////////////////////////////////////////////////////////////////
