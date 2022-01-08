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

struct dtor_invoke_test
{
    dtor_invoke_test()
    {

    }

};

RTTR_REGISTRATION
{
    registration::class_<dtor_invoke_test>("dtor_invoke_test")
        .constructor<>() (policy::ctor::as_raw_ptr)
       ;
}

////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("destructor - invoke", "[destructor]")
{
    type t = type::get<dtor_invoke_test>();
    REQUIRE(t.is_valid() == true);

    SECTION("Invoke positive")
    {
        variant var = t.create();

        CHECK(var.is_valid() == true);

        destructor dtor = var.get_type().get_destructor();
        CHECK(dtor.is_valid() == true);
        CHECK(static_cast<bool>(dtor) == true);

        CHECK(dtor.invoke(var) == true);
        CHECK(var.is_valid() == false);

        // check that bot dtors are the same
        CHECK(type::get<dtor_invoke_test>().get_destructor() == type::get<dtor_invoke_test*>().get_destructor());
    }

    SECTION("Invoke negative")
    {
        variant var;
        CHECK(var.is_valid() == false);

        destructor dtor_invalid = type::get_by_name("").get_destructor();
        REQUIRE(dtor_invalid.is_valid() == false);

        // cannot invoke destructor, because dtor wrapper is invalid
        CHECK(dtor_invalid.invoke(var) == false);

        // cannot invoke destructor, because given type is invalid
        CHECK(type::get<dtor_invoke_test>().get_destructor().invoke(var) == false);
    }
}

////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("destructor - via type", "[destructor]")
{
    variant var = type::get<dtor_invoke_test>().create();
    REQUIRE(var.get_type() == type::get<dtor_invoke_test*>());
    CHECK(var.get_type().destroy(var) == true);
    CHECK(var.is_valid() == false);
}

/////////////////////////////////////////////////////////////////////////////////////////
