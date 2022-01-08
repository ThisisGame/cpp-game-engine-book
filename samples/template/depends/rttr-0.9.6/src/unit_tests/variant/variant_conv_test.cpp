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

#include <rttr/registration>

using namespace rttr;

namespace
{

struct point
{
    point(int x, int y) : _x(x), _y(y) {}

    point(const point& other) : _x(other._x), _y(other._y) { }

    point(point&& other) : _x(other._x), _y(other._y) { other._x = 0; other._y = 0; }

    bool operator ==(const point& other) const { return (_x == other._x && _y == other._y); }
    int _x;
    int _y;
};

template<class T>
static
typename std::enable_if<!std::numeric_limits<T>::is_integer, bool>::type
almost_equal(T x, T y)
{
    return std::abs(x - y) <= std::numeric_limits<T>::epsilon();
}

struct vector2d
{
    vector2d(int x, int y) : _x(x), _y(y) {}
    int _x;
    int _y;
};

static std::string convert_to_string(const point& p, bool& ok)
{
    ok = true;
    return std::to_string(p._x) + ", " + std::to_string(p._y);
}

static vector2d convert_to_vector(const point& p, bool& ok)
{
    ok = true;
    return vector2d(p._x, p._y);
}

struct base
{
    virtual ~base() {};
    int dummy;
    RTTR_ENABLE()
};

struct derived : virtual base
{
    virtual ~derived() {};
    double dummy2;
    RTTR_ENABLE(base)
};

struct other_derived : virtual base
{
    virtual ~other_derived() {};
    double dummy3;
    RTTR_ENABLE(base)
};

enum class test_enum
{
    first = 1,
    second = 2
};

} // end anonymous namespace

RTTR_REGISTRATION
{
    type::register_converter_func(convert_to_string);
    type::register_converter_func(convert_to_vector);

    registration::enumeration<test_enum>("test_enum")
        (
            value("first", test_enum::first),
            value("second", test_enum::second)
    );
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("variant - basic - can_convert()", "[variant]")
{
    variant var;
    CHECK(var.can_convert(type::get<int>()) == false);
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("variant conversion - to int", "[variant]")
{
    SECTION("int to int")
    {
        variant var = 12;
        REQUIRE(var.is_valid() == true);
        REQUIRE(var.can_convert<int>() == true);

        CHECK(var.to_int() == 12);
        CHECK(var.convert<int>() == 12);
        CHECK(var.convert(type::get<int>()) == true);
        CHECK(var.get_value<int>() == 12);

        var = -23;
        CHECK(var.to_int() == -23);
        CHECK(var.convert<int>() == -23);
        CHECK(var.convert(type::get<int>()) == true);
        CHECK(var.get_value<int>() == -23);
    }

    SECTION("char to int")
    {
        variant var = "23";
        REQUIRE(var.is_valid() == true);
        REQUIRE(var.can_convert<int>() == true);
        CHECK(var.can_convert(type::get<int>()) == true);

        CHECK(var.to_int() == 23);
        CHECK(var.convert<int>() == 23);
        CHECK(var.convert(type::get<int>()) == true);
        CHECK(var.get_value<int>() == 23);

        var = "-12";
        CHECK(var.to_int() == -12);

        var = "text 34 and text";
        bool ok = false;
        CHECK(var.to_int(&ok) == 0);
        CHECK(ok == false);

        var = "34 and text";
        ok = false;
        CHECK(var.to_int(&ok) == 0);
        CHECK(ok == false);
        CHECK(var.convert<int>() == 0);
        CHECK(var.convert(type::get<int>()) == false);
    }

    SECTION("std::string to int")
    {
        variant var = std::string("23");
        REQUIRE(var.is_valid() == true);
        REQUIRE(var.can_convert<int>() == true);

        CHECK(var.to_int() == 23);
        CHECK(var.convert<int>() == 23);
        CHECK(var.convert(type::get<int>()) == true);
        CHECK(var.get_value<int>() == 23);

        var = std::string("-12");
        CHECK(var.to_int() == -12);

        var = std::string("text 34 and text");
        bool ok = false;
        CHECK(var.to_int(&ok) == 0);
        CHECK(ok == false);

        var = std::string("34 and text");
        ok = false;
        CHECK(var.to_int(&ok) == 0);
        CHECK(ok == false);
    }

    SECTION("bool to int")
    {
        variant var = true;
        REQUIRE(var.is_valid() == true);
        REQUIRE(var.can_convert<int>() == true);

        CHECK(var.to_int() == 1);

        var = false;
        CHECK(var.to_int() == 0);
    }

    SECTION("float to int")
    {
        variant var = 1.5f;
        REQUIRE(var.is_valid() == true);
        REQUIRE(var.can_convert<int>() == true);

        CHECK(var.to_int() == 1);

        var = 3.1423f;
        CHECK(var.to_int() == 3);

        var = 0.0f;
        CHECK(var.to_int() == 0);
    }

    SECTION("double to int")
    {
        variant var = 1.5;
        REQUIRE(var.is_valid() == true);
        REQUIRE(var.can_convert<int>() == true);

        CHECK(var.to_int() == 1);

        var = 3.1423f;
        CHECK(var.to_int() == 3);

        var = 0.0;
        CHECK(var.to_int() == 0);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("variant conversion - to std::string", "[variant]")
{
    SECTION("int to std::string")
    {
        variant var = 12;
        REQUIRE(var.is_valid() == true);
        REQUIRE(var.can_convert<std::string>() == true);

        CHECK(var.to_string() == "12");
        CHECK(var.convert<std::string>() == "12");
        CHECK(var.convert(type::get<std::string>()) == true);
        CHECK(var.get_value<std::string>() == "12");

        var = -23;
        CHECK(var.to_string() == "-23");
    }

    SECTION("char to std::string")
    {
        variant var = "text";
        REQUIRE(var.is_valid() == true);
        REQUIRE(var.can_convert<std::string>() == true);

        CHECK(var.to_string() == "text");

        var = "text 42";
        CHECK(var.to_string() == "text 42");
    }

    SECTION("std::string to std::string")
    {
        variant var = std::string("23");
        REQUIRE(var.is_valid() == true);
        REQUIRE(var.can_convert<std::string>() == true);

        CHECK(var.to_string() == "23");

        var = std::string("-12");
        CHECK(var.to_string() == "-12");
    }

    SECTION("bool to std::string")
    {
        variant var = true;
        REQUIRE(var.is_valid() == true);
        REQUIRE(var.can_convert<std::string>() == true);

        CHECK(var.to_string() == "true");

        var = false;
        CHECK(var.to_string() == "false");
    }

    SECTION("float to std::string")
    {
        variant var = 1.567f;
        REQUIRE(var.is_valid() == true);
        REQUIRE(var.can_convert<std::string>() == true);

        CHECK(var.to_string() == "1.567");

        var = 3.12345678f;
        CHECK(var.to_string() == "3.12346");

        var = 0.0f;
        CHECK(var.to_string() == "0");
    }

    SECTION("double to std::string")
    {
        variant var = 1.567;
        REQUIRE(var.is_valid() == true);
        REQUIRE(var.can_convert<std::string>() == true);

        CHECK(var.to_string() == "1.567");

        var = 3.12345678;
        CHECK(var.to_string() == "3.12345678");

        var = 0.0;
        CHECK(var.to_string() == "0");
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("variant conversion - to float", "[variant]")
{
    SECTION("int to float")
    {
        variant var = 12;
        REQUIRE(var.is_valid() == true);
        REQUIRE(var.can_convert<float>() == true);

        CHECK(var.to_float() == 12.0f);
        CHECK(var.convert<float>() == 12.0f);
        CHECK(var.convert(type::get<float>()) == true);
        CHECK(var.get_value<float>() == 12.0f);

        var = -23;
        CHECK(var.to_float() == -23.0f);
    }

    SECTION("char to float")
    {
        variant var = "23.0";
        REQUIRE(var.is_valid() == true);
        REQUIRE(var.can_convert<float>() == true);

        CHECK(var.to_float() == 23.0f);
        CHECK(var.convert<float>() == 23.0f);
        CHECK(var.convert(type::get<float>()) == true);
        CHECK(var.get_value<float>() == 23.0f);

        var = "text 42";
        bool ok = false;
        CHECK(var.to_float(&ok) == 0);
        CHECK(ok == false);

        var = "1.23456";
        ok = false;
        CHECK(var.to_float(&ok) == 1.23456f);
        CHECK(ok == true);

        var = "1.23456 Text";
        ok = false;
        CHECK(var.to_float(&ok) == 0);
        CHECK(ok == false);

    }

    SECTION("std::string to float")
    {
        variant var = std::string("23.0");
        REQUIRE(var.is_valid() == true);
        REQUIRE(var.can_convert<float>() == true);

        CHECK(var.to_float() == 23.0f);

        var = std::string("text 42");
        bool ok = false;
        CHECK(var.to_float(&ok) == 0);
        CHECK(ok == false);

        var = std::string("1.23456");
        ok = false;
        CHECK(var.to_float(&ok) == 1.23456f);
        CHECK(ok == true);

        var = std::string("1.23456 Text");
        ok = false;
        CHECK(var.to_float(&ok) == 0);
        CHECK(ok == false);
    }

    SECTION("bool to float")
    {
        variant var = true;
        REQUIRE(var.is_valid() == true);
        REQUIRE(var.can_convert<float>() == true);

        CHECK(var.to_float() == 1.0f);

        var = false;
        CHECK(var.to_float() == 0.0f);
    }

    SECTION("float to float")
    {
        variant var = 1.567f;
        REQUIRE(var.is_valid() == true);
        REQUIRE(var.can_convert<float>() == true);

        CHECK(almost_equal(var.to_float(), 1.567f) == true);

        var = 3.12345678f;
        CHECK(almost_equal(var.to_float(), 3.1234567f) == true);

        var = 0.0f;
        CHECK(almost_equal(var.to_float(), 0.0f) == true);
    }

    SECTION("double to float")
    {
        variant var = 1.567;
        REQUIRE(var.is_valid() == true);
        REQUIRE(var.can_convert<float>() == true);

        CHECK(almost_equal(var.to_float(), 1.567f) == true);

        var = 3.12345678;
        CHECK(almost_equal(var.to_float(), 3.1234567f) == true);

        var = 0.0;
        CHECK(almost_equal(var.to_float(), 0.0f) == true);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("variant conversion - to double", "[variant]")
{
    SECTION("int to double")
    {
        variant var = 12;
        REQUIRE(var.is_valid() == true);
        REQUIRE(var.can_convert<double>() == true);

        CHECK(var.to_double() == 12.0);
        CHECK(var.convert<double>() == 12.0);
        CHECK(var.convert(type::get<double>()) == true);
        CHECK(var.get_value<double>() == 12.0);

        var = -23;
        CHECK(var.to_double() == -23.0);
    }

    SECTION("char to double")
    {
        variant var = "23.0";
        REQUIRE(var.is_valid() == true);
        REQUIRE(var.can_convert<double>() == true);

        CHECK(var.to_double() == 23.0);

        var = "text 42";
        bool ok = false;
        CHECK(var.to_double(&ok) == 0);
        CHECK(ok == false);

        var = "1.23456";
        ok = false;
        CHECK(var.to_double(&ok) == 1.23456);
        CHECK(ok == true);

        var = "1.23456 Text";
        ok = false;
        CHECK(var.to_double(&ok) == 0.0);
        CHECK(ok == false);
    }

    SECTION("std::string to double")
    {
        variant var = std::string("23.0");
        REQUIRE(var.is_valid() == true);
        REQUIRE(var.can_convert<double>() == true);

        REQUIRE(var.to_double() == 23.0);

        var = std::string("text 42");
        bool ok = false;
        CHECK(var.to_double(&ok) == 0.0);
        CHECK(ok == false);

        var = std::string("1.23456");
        ok = false;
        CHECK(var.to_double(&ok) == 1.23456);
        CHECK(ok == true);

        var = std::string("1.23456 Text");
        ok = false;
        CHECK(var.to_double(&ok) == 0.0);
        CHECK(ok == false);
    }

    SECTION("bool to double")
    {
        variant var = true;
        REQUIRE(var.is_valid() == true);
        REQUIRE(var.can_convert<double>() == true);

        CHECK(var.to_double() == 1.0);

        var = false;
        CHECK(var.to_double() == 0.0);
    }

    SECTION("float to double")
    {
        variant var = 1.567f;
        REQUIRE(var.is_valid() == true);
        REQUIRE(var.can_convert<double>() == true);

        CHECK(almost_equal(var.to_double(), 1.5670000314712524) == true);

        var = 3.123456f;
        CHECK(almost_equal(var.to_double(), 3.1234560012817383) == true);

        var = 0.0f;
        CHECK(almost_equal(var.to_double(), 0.0) == true);
    }

    SECTION("double to double")
    {
        variant var = 1.567;
        REQUIRE(var.is_valid() == true);
        REQUIRE(var.can_convert<double>() == true);

        CHECK(almost_equal(var.to_double(), 1.567) == true);

        var = 3.12345678;
        CHECK(almost_equal(var.to_double(), 3.12345678) == true);

        var = 0.0;
        CHECK(almost_equal(var.to_double(), 0.0) == true);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("variant test - convert custom types", "[variant]")
{
    variant var = point(12, 34);
    REQUIRE(var.is_type<point>() == true);
    bool ok = false;
    std::string text = var.to_string(&ok);
    REQUIRE(ok == true);

    REQUIRE(var.can_convert<std::string>() == true);
    REQUIRE(var.can_convert<vector2d>() == true);

    CHECK(var.convert(type::get<std::string>())   == true);
    REQUIRE(var.is_type<std::string>()              == true);
    CHECK(var.get_value<std::string>()            == "12, 34");

    // convert to other custom type
    var = point(12, 34);
    bool ret = var.convert(type::get<vector2d>());
    CHECK(ret == true);
    CHECK(var.is_type<vector2d>() == true);
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("variant test - convert internal", "[variant]")
{
    variant var = 23;
    bool ret = var.convert(type::get<std::string>());
    REQUIRE(ret == true);
    REQUIRE(var.is_type<std::string>() == true);

    auto ptr = detail::make_unique<derived>();
    derived* d = ptr.get();
    base* b = d;
    var = b;
    REQUIRE(var.can_convert(type::get<derived>())   == false);
    REQUIRE(var.can_convert(type::get<derived*>())  == true);
    REQUIRE(var.can_convert(type::get<derived**>()) == false);
    REQUIRE(var.can_convert(type::get<other_derived*>())  == false);

    bool could_convert = var.convert(type::get<derived**>());
    CHECK(could_convert == false);

    could_convert = var.convert(type::get<other_derived*>());
    CHECK(could_convert == false);

    could_convert = var.convert(type::get<derived*>());
    CHECK(could_convert == true);
    CHECK(var.is_type<derived*>() == true);
    CHECK(var.get_value<derived*>() == d);
    CHECK(var.convert<base*>() == b);

    derived* d2 = nullptr;
    var = d2;
    could_convert = false;
    CHECK(var.convert<base*>(&could_convert) == nullptr);
    CHECK(could_convert == true);
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("variant test - convert to nullptr", "[variant]")
{
    SECTION("Invalid conversion")
    {
        int obj = 42;
        variant var = &obj;

        CHECK(var.can_convert(type::get<std::nullptr_t>()) == false);

        bool ok = false;
        var.convert<std::nullptr_t>(&ok);
        CHECK(ok == false);

        CHECK(var.convert(type::get<std::nullptr_t>()) == false);

        std::nullptr_t null_obj;
        CHECK(var.convert<std::nullptr_t>(null_obj) == false);
    }

    SECTION("valid conversion")
    {
        int* obj = nullptr;
        variant var = obj;

        CHECK(var.can_convert(type::get<std::nullptr_t>()) == true);
        std::nullptr_t null_obj;
        CHECK(var.convert<std::nullptr_t>(null_obj) == true);
        CHECK(null_obj == nullptr);

        bool ok = false;
        var.convert<std::nullptr_t>(&ok);
        CHECK(ok == true);

        CHECK(var.convert(type::get<std::nullptr_t>()) == true);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("variant test - convert from wrapped value", "[variant]")
{
    SECTION("valid conversion")
    {
        int obj = 42;
        variant var = std::ref(obj);

        CHECK(var.can_convert(type::get<int>()) == true);

        bool ok = false;
        int val = var.convert<int>(&ok);
        CHECK(ok == true);
        CHECK(val == obj);

        CHECK(var.convert(type::get<int>()) == true);

        int val_2;
        CHECK(var.convert<int>(val_2) == true);
        CHECK(val_2 == obj);
    }

    SECTION("valid conversion std::shared_ptr")
    {
        auto raw_ptr = new int(42);
        variant var = std::shared_ptr<int>(raw_ptr);

        CHECK(var.can_convert(type::get<int*>()) == true);

        bool ok = false;
        auto val = var.convert<int*>(&ok);
        CHECK(ok == true);
        CHECK(val == raw_ptr);

        int* val_2;
        CHECK(var.convert<int*>(val_2) == true);
        CHECK(val_2 == raw_ptr);

        CHECK(var.convert(type::get<int*>()) == true);
    }

    SECTION("invalid conversion")
    {
        int obj = 42;
        int* obj_ptr = &obj;
        variant var = std::ref(obj_ptr);

        // cannot convert from int* to int automatically
        CHECK(var.can_convert(type::get<int>()) == false);

        bool ok = false;
        int val = var.convert<int>(&ok);
        CHECK(ok == false);
        CHECK(val == 0);

        CHECK(var.convert(type::get<int>()) == false);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("variant test - convert to wrapped value", "[variant]")
{
    SECTION("valid conversion")
    {
        auto raw_ptr = new int(42);
        variant var = raw_ptr;

        REQUIRE(var.can_convert(type::get<std::shared_ptr<int>>()) == true);

        auto result = var.convert(type::get<std::shared_ptr<int>>());
        CHECK(result == true);
        CHECK(var.get_type() == type::get<std::shared_ptr<int>>());
        CHECK(var.get_value<std::shared_ptr<int>>().get() == raw_ptr);
    }

    SECTION("convert and return wrapper")
    {
        auto raw_ptr = new int(42);
        variant var = raw_ptr;


        CHECK(var.can_convert<std::shared_ptr<int>>() == true);

        bool ok = false;
        auto ptr = var.convert<std::shared_ptr<int>>(&ok);
        CHECK(ok == true);
        CHECK(ptr.get() == raw_ptr);
        CHECK(var.get_type() == type::get<int*>());
    }

    SECTION("convert to existing wrapper")
    {
        auto raw_ptr = new int(42);
        variant var = raw_ptr;
        std::shared_ptr<int> ptr;
        CHECK(var.convert(ptr)  == true);
        CHECK(ptr.get()         == raw_ptr);
        CHECK(var.get_type()    == type::get<int*>());
    }

    SECTION("invalid conversion")
    {
        int obj = 42;
        variant var = obj;

        CHECK(var.can_convert(type::get<std::unique_ptr<int>>())        == false);
        CHECK(var.can_convert(type::get<std::reference_wrapper<int>>()) == false);
        CHECK(var.can_convert(type::get<std::weak_ptr<int>>())          == false);
        CHECK(var.can_convert(type::get<std::shared_ptr<int>>())        == false);

        var = &obj;

        CHECK(var.can_convert(type::get<std::unique_ptr<int>>())        == false);
        CHECK(var.can_convert(type::get<std::reference_wrapper<int>>()) == false);
        CHECK(var.can_convert(type::get<std::weak_ptr<int>>())          == false);
        // bool is wrong wrapped type!
        CHECK(var.can_convert(type::get<std::shared_ptr<bool>>())       == false);

        auto result = var.convert(type::get<std::shared_ptr<bool>>());
        CHECK(result == false);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("variant test - convert from enum", "[variant]")
{
    auto ok = false;
    rttr::variant var = type::get<test_enum>().get_enumeration().name_to_value("second");
    test_enum converted = var.convert<test_enum>(&ok);

    CHECK(ok == true);
    CHECK(converted != test_enum::first);
    CHECK(converted == test_enum::second);
    CHECK(static_cast<int>(converted) == 2);
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("variant test - register_wrapper_converter_for_base_classes<std::shared_ptr<T>>", "[variant]")
{
    variant var = std::make_shared<derived>();
    CHECK(var.convert(type::get<std::shared_ptr<base>>())           == false);

    type::register_wrapper_converter_for_base_classes<std::shared_ptr<derived>>();

    CHECK(var.convert(type::get<std::shared_ptr<base>>())           == true);
    CHECK(var.convert(type::get<std::shared_ptr<derived>>())        == true);

    type::register_wrapper_converter_for_base_classes<std::shared_ptr<other_derived>>();

    // negative test, we need first make a down cast, otherwise the target_type converter cannot be found
    CHECK(var.convert(type::get<std::shared_ptr<base>>())           == true);
    CHECK(var.convert(type::get<std::shared_ptr<other_derived>>())  == false);
}

/////////////////////////////////////////////////////////////////////////////////////////
