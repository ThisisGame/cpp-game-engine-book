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
#include <tuple>

using namespace rttr;
using namespace std;

struct type_with_no_less_than_operator
{
    int i;
};

struct type_with_less_than_operator
{
    int i;

    bool operator<(const type_with_less_than_operator& rhs) const { return (i < rhs.i); }
};

template<typename T>
struct template_type_with_less_than_operator
{
    T i;

    bool operator<(const template_type_with_less_than_operator<T>& rhs) const { return (i < rhs.i); }
};

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("variant::operator<() - empty", "[variant]")
{
    SECTION("empty type")
    {
        variant a;
        variant b;

        CHECK((a < b) == false);
    }

    SECTION("full type lhs")
    {
        variant a = 23;
        variant b;

        CHECK((a < b) == false);
    }

    SECTION("full type rhs")
    {
        variant a;
        variant b = 23;

        CHECK((a < b) == false);
    }

}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("variant::operator<() - integer", "[variant]")
{
    SECTION("equal")
    {
        variant a = static_cast<int8_t>(100);
        variant b = static_cast<int32_t>(100);

        CHECK((a < b) == false);
    }

    SECTION("lower < bigger")
    {
        variant a = static_cast<int8_t>(100);
        variant b = static_cast<int32_t>(10000);

        CHECK((a < b) == true);
    }

    SECTION("bigger < lower")
    {
        variant a = static_cast<int32_t>(10000);
        variant b = static_cast<int8_t>(100);

        CHECK((a < b) == false);
    }

    SECTION("lower < bigger[unsigned]")
    {
        variant a = static_cast<int32_t>(-10000);
        variant b = static_cast<int8_t>(100);

        CHECK((a < b) == true);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("variant::operator<() - float", "[variant]")
{
    SECTION("lower < bigger")
    {
        variant a = static_cast<int8_t>(123);
        variant b = 123.2f;

        CHECK((a < b) == true);
    }

    SECTION("bigger < lower")
    {
        variant a = 123.2f;
        variant b = static_cast<int8_t>(123);

        CHECK((a < b) == false);
    }

    SECTION("lower < bigger")
    {
        variant a = -1230.3f;
        variant b = static_cast<int8_t>(100);

        CHECK((a < b) == true);
    }

     SECTION("lower < bigger")
    {
        variant a = -1230.37f;
        variant b = -1230.31;

        CHECK((a < b) == true);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("variant::operator<() - double", "[variant]")
{
    SECTION("lower < bigger")
    {
        variant a = static_cast<int8_t>(123);
        variant b = 123.2;

        CHECK((a < b) == true);
    }

    SECTION("int - bigger < lower")
    {
        variant a = 123.2;
        variant b = static_cast<int8_t>(123);

        CHECK((a < b) == false);
    }

    SECTION("int UnSigned - lower < bigger")
    {
        variant a = -1230.3;
        variant b = static_cast<int8_t>(100);

        CHECK((a < b) == true);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////
TEST_CASE("variant::operator<() - Non-template type", "[variant]")
{
    SECTION("lower < bigger")
    {
        variant a = type_with_less_than_operator{ 23 };
        variant b = type_with_less_than_operator{ 42 };

        CHECK((a < b) == true);
        CHECK((b < a) == false);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("variant::operator<() - template type - comparator registered", "[variant]")
{
    SECTION("lower < bigger")
    {
        variant a = std::make_tuple<int>(23);
        variant b = std::make_tuple<int>(42);

        CHECK((a < b) == false);
        CHECK((b < a) == false);

        type::register_comparators<std::tuple<int>>();

        CHECK((a < b) == true);
        CHECK((b < a) == false);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("variant::operator<() - template type - no comparator registered", "[variant]")
{
    SECTION("lower < bigger, use std::string comparator")
    {
        variant a = std::make_tuple<int, int>(1, 2);
        variant b = std::make_tuple<int, int>(3, 4);

        CHECK((a < b) == false);
        CHECK((b < a) == false);

        type::register_converter_func(std::function<std::string(const std::tuple<int, int>& p, bool& ok)>(
                                     [](const std::tuple<int, int>& p, bool& ok) -> std::string
                                     {
                                        ok = true;
                                        return "[" + std::to_string(std::get<0>(p)) + ", " + std::to_string(std::get<1>(p)) + "]";
                                     }));

        CHECK((a < b) == true);
        CHECK((b < a) == false);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("variant::operator<() - with nullptr", "[variant]")
{
    SECTION("nullptr < nullptr")
    {
        variant a = nullptr;
        variant b = nullptr;

        CHECK((a < b) == false);
        CHECK((b < a) == false);
    }

    SECTION("nullptr < valid")
    {
        int obj = 12;
        variant a = nullptr;
        variant b = &obj;

        CHECK((a < b) == true);
    }

    SECTION("valid < nullptr")
    {
        int obj = 12;
        variant a = &obj;
        variant b = nullptr;

        CHECK((a < b) == false);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("variant::operator<() - raw arrays", "[variant]")
{
    SECTION("int - pos.")
    {
        int array[2][5]     = {{1, 2, 3, 4, 5}, {1, 2, 3, 4, 5}};
        int arrays[2][5]    = {{1, 2, 3, 4, 5}, {1, 2, 3, 4, 5}};
        variant a = array;
        variant b = arrays;

        CHECK((a < b) == false);
        CHECK((a == b) == true);
    }

    SECTION("int - neg.")
    {
        int array[2][5]     = {{1, 2, 3, 4, 5}, {1, 2, 3, 0, 5}};
        int arrays[2][5]    = {{1, 2, 3, 4, 5}, {1, 2, 3, 5, 5}};

        variant a = array;
        variant b = arrays;

        CHECK((a < b) == true);
        CHECK((a != b) == true);
    }

    SECTION("type with no less than operator")
    {
        type_with_no_less_than_operator array[5]    = {{1}, {2}, {3}, {0}, {5}};
        type_with_no_less_than_operator arrays[5]   = {{1}, {2}, {3}, {4}, {5}};
        variant a = array;
        variant b = arrays;

        CHECK((a < b) == false);
    }

    SECTION("register less than operator - raw array")
    {
        type_with_less_than_operator array[5]   = {{1}, {2}, {3}, {0}, {5}};
        type_with_less_than_operator arrays[5]  = {{1}, {2}, {3}, {4}, {5}};
        variant a = array;
        variant b = arrays;

        CHECK((a < b) == true);
    }

    SECTION("register less than operator - raw array with template type")
    {
        {
            template_type_with_less_than_operator<int> array[5] = { { 1 }, { 2 }, { 3 }, { 0 }, { 5 } };
            template_type_with_less_than_operator<int> arrays[5] = { { 1 }, { 2 }, { 3 }, { 4 }, { 5 } };
            variant a = array;
            variant b = arrays;

            CHECK((a < b) == false);

            type::register_less_than_comparator<template_type_with_less_than_operator<int>>();

            CHECK((a < b) == true);
        }

        {
            template_type_with_less_than_operator<int> array[5] = { { 1 }, { 2 }, { 3 }, { 5 }, { 5 } };
            template_type_with_less_than_operator<int> arrays[5] = { { 1 }, { 2 }, { 3 }, { 4 }, { 5 } };
            variant a = array;
            variant b = arrays;

            CHECK((a < b) == false);
        }
    }

    SECTION("register less than operator - std::array")
    {
        std::array<int, 5> array_a = { {1, 2, 0, 4, 5} };
        std::array<int, 5> array_b = { {1, 2, 3, 4, 5} };

        variant a = array_a;
        variant b = array_b;

        CHECK((a < b) == false);

        type::register_less_than_comparator<std::array<int, 5>>();

        CHECK((a < b) == true);
    }

}

/////////////////////////////////////////////////////////////////////////////////////////
