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

struct type_with_equal_operator
{
    int i;

    bool operator ==(const type_with_equal_operator& other) const { return (i == other.i); }
};

struct type_with_no_equal_operator
{
    int i;
};

struct other_type_with_no_equal_operator
{
    int i;
};

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("variant::operator==() - empty", "[variant]")
{
    SECTION("empty type")
    {
        variant a;
        variant b;

        CHECK((a == b) == true);
        CHECK((a != b) == false);
    }

    SECTION("valid type - lhs")
    {
        variant a = 23;
        variant b;

        CHECK((a == b) == false);
        CHECK((a != b) == true);
    }

    SECTION("valid type - rhs")
    {
        variant a;
        variant b = 23;

        CHECK((a == b) == false);
        CHECK((a != b) == true);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("variant::operator==() - basic", "[variant]")
{
    SECTION("int - equal")
    {
        variant a = 23;
        variant b = 23;

        CHECK((a == b) == true);
        CHECK((a != b) == false);
    }

    SECTION("int - NOT equal")
    {
        variant a = 23;
        variant b = 12;

        CHECK((a == b) == false);
        CHECK((a != b) == true);
    }

    SECTION("int - uint8_t")
    {
        variant a = 12;
        variant b = static_cast<uint8_t>(12);

        CHECK((a == b) == true);
        CHECK((a != b) == false);
    }

    SECTION("int[neg] - uint8_t")
    {
        variant a = -12;
        variant b = static_cast<uint8_t>(12);

        CHECK((a == b) == false);
        CHECK((a != b) == true);
    }

     SECTION("uint8_t - int[neg]")
    {
        variant a = static_cast<uint8_t>(12);
        variant b = -12;

        CHECK((a == b) == false);
        CHECK((a != b) == true);
    }

    SECTION("double - float")
    {
        variant a = 12.5f;
        variant b = 12.5;

        CHECK((a == b) == true);
        CHECK((a != b) == false);
    }

    SECTION("float - double")
    {
        variant a = 12.1234;
        variant b = 12.1234f;

        CHECK((a == b) == false);
        CHECK((a != b) == true);
    }

    SECTION("double - int")
    {
        variant a = 12.1234;
        variant b = 12;

        CHECK((a == b) == false);
        CHECK((a != b) == true);
    }

    SECTION("int - double")
    {
        variant a = 12;
        variant b = 12.1234;

        CHECK((a == b) == false);
        CHECK((a != b) == true);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

enum class COLOR
{
    RED,
    GREEN,
    BLUE
};

TEST_CASE("variant::operator==() - enums", "[variant]")
{
    variant a = COLOR::RED;
    variant b = 0;

    CHECK((a == b) == true);
    CHECK((a != b) == false);

    // commutative
    CHECK((b == a) == true);
    CHECK((b != a) == false);

    b = COLOR::RED;
    CHECK((a == b) == true);
    CHECK((a != b) == false);

     a = COLOR::BLUE;

    CHECK((a == b) == false);
    CHECK((a != b) == true);
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("variant::operator==() - raw arrays", "[variant]")
{
    SECTION("int - pos.")
    {
        int array[2][5]     = {{1, 2, 3, 4, 5}, {1, 2, 3, 4, 5}};
        int arrays[2][5]    = {{1, 2, 3, 4, 5}, {1, 2, 3, 4, 5}};
        variant a = array;
        variant b = arrays;

        CHECK((a == b) == true);
        CHECK((a != b) == false);
    }

    SECTION("int - neg.")
    {
        int array[2][5]     = {{1, 2, 3, 4, 5}, {1, 2, 3, 4, 5}};
        int arrays[2][5]    = {{1, 2, 3, 4, 5}, {1, 2, 3, 0, 5}};
        variant a = array;
        variant b = arrays;

        CHECK((a == b) == false);
        CHECK((a != b) == true);
    }

    SECTION("type_with_no_equal_operator")
    {
        type_with_no_equal_operator array[5]    = {{1}, {2}, {3}, {4}, {5}};
        type_with_no_equal_operator arrays[5]   = {{1}, {2}, {3}, {4}, {5}};
        variant a = array;
        variant b = arrays;

        CHECK((a == b) == false);
        CHECK((a != b) == true);
    }

    SECTION("type_with_equal_operator")
    {
        type_with_equal_operator array_a[5]     = {{1}, {2}, {3}, {4}, {5}};
        type_with_equal_operator array_b[5]     = {{1}, {2}, {3}, {4}, {5}};
        type_with_equal_operator array_c[5]     = {{1}, {2}, {0}, {4}, {5}};
        type_with_equal_operator array_d[6]     = {{1}, {2}, {3}, {4}, {5}, {6} };
        variant a = array_a;
        variant b = array_b;
        variant c = array_c;
        variant d = array_d;

        CHECK((a == b) == true);
        CHECK((a != b) == false);

        CHECK((a == c) == false);
        CHECK((a == d) == false);
    }

    SECTION("type_with_no_equal_operator")
    {
        type_with_no_equal_operator array[5]    = {{1}, {2}, {3}, {4}, {5}};
        type_with_no_equal_operator arrays[5]   = {{1}, {2}, {3}, {0}, {5}};
        variant a = array;
        variant b = arrays;

        CHECK((a == b) == false);
        CHECK((a != b) == true);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("variant::operator==() - custom type with equal operator", "[variant]")
{
    SECTION("equal - type_with_equal_operator")
    {
        type_with_equal_operator value_1 = { 1 };
        type_with_equal_operator value_2 = { 1 };
        variant a = value_1;
        variant b = value_2;

        CHECK((a == b) == true);
        CHECK((a != b) == false);
    }

    SECTION("not equal - type_with_equal_operator")
    {
        type_with_equal_operator value_1 = { 1 };
        type_with_equal_operator value_2 = { 2 };
        variant a = value_1;
        variant b = value_2;

        CHECK((a == b) == false);
        CHECK((a != b) == true);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("variant::operator==() - std::string", "[variant]")
{
    SECTION("equal")
    {
        variant a = "text";
        variant b = "text";

        CHECK((a == b) == true);
        CHECK((a != b) == false);
    }

    SECTION("case sensitive")
    {
        variant a = "Text";
        variant b = "text";

        CHECK((a == b) == false);
        CHECK((a != b) == true);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

struct point
{
    point(int x, int y) : _x(x), _y(y) {}

    point(const point& other) : _x(other._x), _y(other._y) { }

    point(point&& other) : _x(other._x), _y(other._y) { other._x = 0; other._y = 0; }

    bool operator ==(const point& other) const { return (_x == other._x && _y == other._y); }
    int _x;
    int _y;
};

static std::string convert_to_string(const point& p, bool& ok)
{
    ok = true;
    return "[" + std::to_string(p._x) + ", " + std::to_string(p._y) + "]";
}

TEST_CASE("variant::operator==() - custom", "[variant]")
{
    type::register_converter_func(convert_to_string);

    SECTION("equal")
    {
        variant a = point(12, 12);
        variant b = point(12, 12);

        CHECK((a == b) == true);
        CHECK((a != b) == false);
    }

    SECTION("NOT equal")
    {
        variant a = point(12, 23);
        variant b = point(12, 12);

        CHECK((a == b) == false);
        CHECK((a != b) == true);
    }

    SECTION("different types")
    {
        variant a = point(12, 23);
        variant b = type_with_no_equal_operator{12};

        CHECK((a == b) == false);
        CHECK((a != b) == true);
    }

    SECTION("compare std::string with point")
    {
        variant a = "[12, 23]";
        variant b = point{12, 23};

        CHECK((a == b) == true);
        CHECK((a != b) == false);

        CHECK((b == a) == true);
        CHECK((b != a) == false);

        CHECK(a.is_type<std::string>()  == true);
        CHECK(b.is_type<point>()        == true);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("variant::operator==() - no build in == operator", "[variant]")
{
    SECTION("no equal operator, same types, same values")
    {
        variant a = type_with_no_equal_operator{12};
        variant b = type_with_no_equal_operator{12};

        CHECK((a == b) == false);
        CHECK((a != b) == true);
    }

    SECTION("no equal operator, same types, different values")
    {
        variant a = type_with_no_equal_operator{12};
        variant b = type_with_no_equal_operator{23};

        CHECK((a == b) == false);
        CHECK((a != b) == true);
    }

     SECTION("no equal operator, different types, different values")
    {
        variant a = other_type_with_no_equal_operator{12};
        variant b = type_with_no_equal_operator{23};

        CHECK((a == b) == false);
        CHECK((a != b) == true);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("variant::operator==() - template type - comparator registered", "[variant]")
{
    SECTION("same types, same content")
    {
        variant a = std::make_tuple(23, std::string("some long long text"));
        variant b = std::make_tuple(23, std::string("some long long text"));

        CHECK((a == b) == false);
        CHECK((a != b) == true);

        type::register_comparators<std::tuple<int, std::string>>();

        CHECK((a == b) == true);
        CHECK((a != b) == false);
    }

    SECTION("same types, different content")
    {
        variant a = std::make_tuple(23, std::string("some long long text"));
        variant b = std::make_tuple(23, std::string("different content"));

        CHECK((a == b) == false);
        CHECK((a != b) == true);
    }

    SECTION("self equal test")
    {
        variant a = std::make_tuple(23, std::string("some long long text"));

        CHECK((a == a) == true);
        CHECK((a != a) == false);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("variant::operator==() - nullptr type", "[variant]")
{
    SECTION("nullptr == nullptr")
    {
        variant a = nullptr;
        variant b = nullptr;

        CHECK((a == b) == true);
    }

    SECTION("valid ptr == nullptr")
    {
        int int_obj = 12;
        variant a = &int_obj;
        variant b = nullptr;

        CHECK((a == b) == false);
    }

    SECTION("nullptr == valid ptr")
    {
        int int_obj = 12;
        variant a = nullptr;
        variant b = &int_obj;

        CHECK((a == b) == false);
    }

    SECTION("invalid ptr == nullptr")
    {
        int* int_obj = nullptr;
        variant a = int_obj;
        variant b = nullptr;

        CHECK((a == b) == true);
    }

    SECTION("nullptr == invalid ptr")
    {
        int* int_obj = nullptr;
        variant a = nullptr;
        variant b = int_obj;

        CHECK((a == b) == true);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("variant::operator==() - pointer type", "[variant]")
{
    SECTION("valid pointer compare")
    {
        int value = 23;
        variant a = &value;
        variant b = &value;

         CHECK((a == b) == true);
    }

    SECTION("invalid pointer compare")
    {
        int value1 = 23;
        int value2 = 42;
        variant a = &value1;
        variant b = &value2;

         CHECK((a == b) == false);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////
