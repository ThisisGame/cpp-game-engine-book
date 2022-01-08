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

#include <rttr/type>


#include <catch/catch.hpp>

using namespace rttr;
using namespace std;

enum class enum_test_flag
{
    value_1 = 1,
    value_2 = 2,
    value_3 = 4,
    value_4 = 8
};

RTTR_DECLARE_FLAGS(enum_test_flags, enum_test_flag)
RTTR_DECLARE_ENUM_FLAGS_OPERATORS(enum_test_flags)

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("enum_flags<T> - default ctor", "[enum_flags<T>]")
{
    enum_test_flags f1;
    enum_test_flags f2;

    CHECK(f1 == f2);
    CHECK(!f1 == true);
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("enum_flags<T> - enum_flags(enum)", "[enum_flags<T>]")
{
    enum_test_flags f1(enum_test_flag::value_1);
    enum_test_flags f2(enum_test_flag::value_1);

    CHECK(f1 == f2);
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("enum_flags<T> - enum_flags(enum_flags)", "[enum_flags<T>]")
{
    enum_test_flags f1(enum_test_flag::value_1);
    enum_test_flags f2(f1);

    CHECK(f1 == f2);
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("enum_flags<T> - operator&", "[enum_flags<T>]")
{
    SECTION("return reference")
    {
        enum_test_flags f1(enum_test_flag::value_1 | enum_test_flag::value_2);

        f1 &= enum_test_flag::value_2;

        CHECK(f1.test_flag(enum_test_flag::value_1) == false);
        CHECK(f1.test_flag(enum_test_flag::value_2) == true);

        f1 &= uint32_t(2);
        CHECK(f1 == 2);

        f1 &= enum_test_flag::value_3;
        CHECK(f1 == 0);
    }

    SECTION("return value")
    {
        enum_test_flags f1(enum_test_flag::value_1 | enum_test_flag::value_2);

        enum_test_flags f2 = f1 & enum_test_flag::value_2;

        CHECK(f2.test_flag(enum_test_flag::value_1) == false);
        CHECK(f2.test_flag(enum_test_flag::value_2) == true);

        f2 = f1 & uint32_t(2);
        CHECK(f2 == 2);

        f2 = f1 & enum_test_flag::value_3;
        CHECK(f2 == 0);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("enum_flags<T> - operator|", "[enum_flags<T>]")
{
    SECTION("return reference")
    {
        enum_test_flags f1(enum_test_flag::value_1 | enum_test_flag::value_2);

        f1 |= enum_test_flag::value_3;

        CHECK(f1.test_flag(enum_test_flag::value_1) == true);
        CHECK(f1.test_flag(enum_test_flag::value_2) == true);
        CHECK(f1.test_flag(enum_test_flag::value_3) == true);

        enum_test_flags f2(enum_test_flag::value_4);
        f1 |= f2;
        CHECK(f1.test_flag(enum_test_flag::value_4) == true);
    }

    SECTION("return value")
    {
        enum_test_flags f1(enum_test_flag::value_1 | enum_test_flag::value_2);

        enum_test_flags f2 = f1 | enum_test_flag::value_3;

        CHECK(f2.test_flag(enum_test_flag::value_1) == true);
        CHECK(f2.test_flag(enum_test_flag::value_2) == true);
        CHECK(f2.test_flag(enum_test_flag::value_3) == true);

        enum_test_flags f3(enum_test_flag::value_4);
        f2 = f1 | f3;

        CHECK(f2.test_flag(enum_test_flag::value_4) == true);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("enum_flags<T> - operator^", "[enum_flags<T>]")
{
    SECTION("return reference")
    {
        enum_test_flags f1(enum_test_flag::value_1 | enum_test_flag::value_2);

        f1 ^= enum_test_flag::value_3;

        CHECK(f1.test_flag(enum_test_flag::value_1) == true);
        CHECK(f1.test_flag(enum_test_flag::value_2) == true);
        CHECK(f1.test_flag(enum_test_flag::value_3) == true);

        f1 ^= enum_test_flag::value_2;

        CHECK(f1.test_flag(enum_test_flag::value_1) == true);
        CHECK(f1.test_flag(enum_test_flag::value_2) == false);
        CHECK(f1.test_flag(enum_test_flag::value_3) == true);

        enum_test_flags f3(enum_test_flag::value_3);
        f1 ^= f3;

        CHECK(f1.test_flag(enum_test_flag::value_1) == true);
        CHECK(f1.test_flag(enum_test_flag::value_2) == false);
        CHECK(f1.test_flag(enum_test_flag::value_3) == false);
    }

    SECTION("return value")
    {
        enum_test_flags f1(enum_test_flag::value_1 | enum_test_flag::value_2);

        enum_test_flags f2 = f1 ^ enum_test_flag::value_3;

        CHECK(f2.test_flag(enum_test_flag::value_1) == true);
        CHECK(f2.test_flag(enum_test_flag::value_2) == true);
        CHECK(f2.test_flag(enum_test_flag::value_3) == true);

        f2 = f2 ^ enum_test_flag::value_2;

        CHECK(f2.test_flag(enum_test_flag::value_1) == true);
        CHECK(f2.test_flag(enum_test_flag::value_2) == false);
        CHECK(f2.test_flag(enum_test_flag::value_3) == true);

        enum_test_flags f3(enum_test_flag::value_3);
        f2 = f2 ^ f3;

        CHECK(f2.test_flag(enum_test_flag::value_1) == true);
        CHECK(f2.test_flag(enum_test_flag::value_2) == false);
        CHECK(f2.test_flag(enum_test_flag::value_3) == false);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("enum_flags<T> - operator~", "[enum_flags<T>]")
{
    enum_test_flags f1 = enum_test_flag::value_1 | enum_test_flag::value_2;

    enum_test_flags f2 =~f1;

    CHECK(f2.test_flag(enum_test_flag::value_1) == false);
    CHECK(f2.test_flag(enum_test_flag::value_2) == false);
    CHECK(f2.test_flag(enum_test_flag::value_3) == true);
    CHECK(f2.test_flag(enum_test_flag::value_4) == true);
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("enum_flags<T> - test_flag", "[enum_flags<T>]")
{
    enum_test_flags f = enum_test_flag::value_1 | enum_test_flag::value_2;


    CHECK(f.test_flag(enum_test_flag::value_1) == true);
    CHECK(f.test_flag(enum_test_flag::value_2) == true);

    f = 0;
    // negative
    CHECK(f.test_flag(enum_test_flag::value_3) == false);
    CHECK(f.test_flag(enum_test_flag::value_4) == false);
}

/////////////////////////////////////////////////////////////////////////////////////////
