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


#include <catch/catch.hpp>
#include <rttr/type>

using namespace rttr;

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("type get_name", "[type]")
{
   CHECK(type::get<int*>().get_name() == "int*");
   CHECK(type::get<const int*>().get_name() == "const int*");
   CHECK(type::get<const int&>().get_name() == "int");
   CHECK(type::get<const int**&>().get_name() == "const int**");
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("Test rttr::type - type::get_by_name", "[type]")
{
    CHECK(type::get_by_name("std::string").is_valid()   == true);
    CHECK(type::get_by_name("std::string*").is_valid()  == true);

    CHECK(type::get_by_name("std::string[100]").is_valid()   == false);
    type::get<std::string[100]>(); // register it first, now it is available also by name lookup too
    CHECK(type::get_by_name("std::string[100]").is_valid()   == true);
}

/////////////////////////////////////////////////////////////////////////////////////////

