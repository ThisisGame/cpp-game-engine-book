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

struct dtor_misc_test
{
    dtor_misc_test()
    {

    }

};

RTTR_REGISTRATION
{
    registration::class_<dtor_misc_test>("dtor_misc_test")
        .constructor<>() (policy::ctor::as_raw_ptr)
       ;
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("destructor - general test", "[destructor]")
{
    destructor dtor = type::get_by_name("dtor_misc_test").get_destructor();
    CHECK(dtor.is_valid() == true);
    CHECK(static_cast<bool>(dtor) == true);

    //negative test
    dtor = type::get_by_name("").get_destructor();
    CHECK(dtor.is_valid() == false);
    CHECK(static_cast<bool>(dtor) == false);
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("destructor - get_destructed_type()", "[destructor]")
{
    CHECK(type::get<dtor_misc_test>().get_destructor().get_destructed_type() == type::get<dtor_misc_test*>());

    //negative test
    CHECK(type::get_by_name("").get_destructor().get_destructed_type().is_valid() == false);
}


/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("destructor - compare operator", "[destructor]")
{
    destructor dtor1 = type::get_by_name("dtor_misc_test").get_destructor();
    destructor dtor1a = type::get_by_name("dtor_misc_test").get_destructor();
    destructor dtor2 = type::get_by_name("").get_destructor();

    CHECK(dtor1 == dtor1a);
    CHECK(dtor1 != dtor2);
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("destructor::get_declaring_type()", "[destructor]")
{
    destructor dtor = type::get_by_name("dtor_misc_test").get_destructor();

    destructor invalid_dtor = type::get_by_name("foobar").get_destructor();

    CHECK(dtor.get_declaring_type() == type::get<dtor_misc_test>());
    CHECK(invalid_dtor.get_declaring_type().is_valid() == false);
}

/////////////////////////////////////////////////////////////////////////////////////////
