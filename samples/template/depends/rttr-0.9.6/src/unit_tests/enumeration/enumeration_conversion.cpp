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

using namespace rttr;
using namespace std;

#include <iostream>
#include <memory>
#include <functional>

#include <catch/catch.hpp>

enum class access_t : int
{
    read = 1,
    write = 2,
    exec = 4
};


/////////////////////////////////////////////////////////////////////////////////////////

RTTR_REGISTRATION
{
    registration::enumeration<access_t>("access_t")
    (
        value("read",   access_t::read),
        value("write",  access_t::write),
        value("exec",   access_t::exec)
    );
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("enumeration - value_to_name()", "[enumeration]")
{
    enumeration e = type::get_by_name("access_t").get_enumeration();

    // normal
    CHECK(e.value_to_name(access_t::read)     == "read");
    CHECK(e.value_to_name(access_t::write)    == "write");
    CHECK(e.value_to_name(access_t::exec)     == "exec");

    // implicit
    CHECK(e.value_to_name(4)                  == "exec");

    // invalid
    CHECK(e.value_to_name("test")             == "");
    CHECK(e.value_to_name(42)                 == "");

    // negative
    e = type::get_by_name("access_unknown").get_enumeration();
    CHECK(e.value_to_name(access_t::read).empty() == true);
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("enumeration - name_to_value()", "[enumeration]")
{
    enumeration e = type::get_by_name("access_t").get_enumeration();

    REQUIRE(e.name_to_value("read").is_type<access_t>()   == true);
    REQUIRE(e.name_to_value("write").is_type<access_t>()  == true);
    REQUIRE(e.name_to_value("write").is_type<access_t>()  == true);

    CHECK(e.name_to_value("read").get_value<access_t>()   == access_t::read);
    CHECK(e.name_to_value("write").get_value<access_t>()  == access_t::write);
    CHECK(e.name_to_value("exec").get_value<access_t>()   == access_t::exec);

    // invalid
    CHECK(e.name_to_value("some name").is_valid()   == false);

    // negative
    e = type::get_by_name("access_unknown").get_enumeration();
    REQUIRE(e.name_to_value("write").is_type<access_t>() == false);
}

/////////////////////////////////////////////////////////////////////////////////////////
