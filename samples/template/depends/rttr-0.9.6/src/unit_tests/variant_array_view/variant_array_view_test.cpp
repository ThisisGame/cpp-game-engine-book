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

#include <vector>
#include <map>
#include <string>

using namespace rttr;
using namespace std;

RTTR_BEGIN_DISABLE_DEPRECATED_WARNING

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("variant_array_view::ctor", "[variant_array_view]")
{
   SECTION("empty")
   {
        variant_array_view a;
        CHECK(a.is_valid() == false);

        variant_array_view b(a);
        CHECK(b.is_valid() == false);

        variant var;
        variant_array_view c = var.create_array_view();

        CHECK(c.is_valid() == false);
        CHECK(c.is_dynamic() == false);
        CHECK(c.get_rank_type(0).is_valid() == false);
        CHECK(c.get_rank() == 0);
        CHECK(c.get_type().is_valid() == false);
        CHECK(c.get_size() == 0);
        CHECK(c.get_size(0) == 0);
        CHECK(c.get_size(0, 0) == 0);
        CHECK(c.get_size_variadic({}) == 0);

        CHECK(c.set_size(0) == false);
        CHECK(c.set_size(0, 0) == false);
        CHECK(c.set_size(0, 0, 0) == false);
        CHECK(c.set_size_variadic(0, {}) == false);

        CHECK(c.set_value(0) == false);
        CHECK(c.set_value(0, 0) == false);
        CHECK(c.set_value(0, 0, 0) == false);
        CHECK(c.set_value(0, 0, 0, 0) == false);
        CHECK(c.set_value_variadic({}, 0) == false);

        CHECK(c.get_value(0).is_valid() == false);
        CHECK(c.get_value(0, 0).is_valid() == false);
        CHECK(c.get_value(0, 0, 0).is_valid() == false);
        CHECK(c.get_value_variadic({}).is_valid() == false);

        CHECK(c.insert_value(0, 0) == false);
        CHECK(c.insert_value(0, 0, 0) == false);
        CHECK(c.insert_value(0, 0, 0, 0) == false);
        CHECK(c.insert_value_variadic({}, 0) == false);

        CHECK(c.remove_value(0) == false);
        CHECK(c.remove_value(0, 0) == false);
        CHECK(c.remove_value(0, 0, 0) == false);
        CHECK(c.remove_value_variadic({}) == false);
   }

   SECTION("invalid")
   {
       variant var = 2;
       variant_array_view a = var.create_array_view();
       CHECK(a.is_valid() == false);
   }

   SECTION("full")
   {
        variant var = std::array<int, 10>();
        variant_array_view a = var.create_array_view();
        CHECK(a.is_valid() == true);

        variant_array_view b(a);
        CHECK(b.is_valid() == true);
   }
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("variant_array_view::assignment", "[variant_array_view]")
{
   SECTION("empty")
   {
        variant_array_view a;
        variant_array_view b;
        b = a;
        CHECK(b.is_valid() == false);

        variant var;
        variant_array_view c;
        c = var.create_array_view();
        CHECK(c.is_valid() == false);
   }

   SECTION("full")
   {
        variant var = std::array<int, 10>();
        variant_array_view a;
        a = var.create_array_view();
        CHECK(a.is_valid() == true);

        variant_array_view b;
        b = a;
        CHECK(b.is_valid() == true);
   }
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("variant_array_view::get_type", "[variant_array_view]")
{
   SECTION("empty")
   {
        variant_array_view a;
        CHECK(a.get_type().is_valid() == false);
   }

   SECTION("array")
   {
        variant var = std::array<int, 10>();
        variant_array_view a = var.create_array_view();
        CHECK((a.get_type() == type::get<std::array<int, 10>>()));
   }

   SECTION("raw array")
   {
        int array[2] = {1, 2};
        variant var = array;
        variant_array_view a = var.create_array_view();
        CHECK((a.get_type() == type::get<int[2]>()));
   }

   SECTION("pointer")
   {
        std::array<int, 10> array;
        variant var = &array;
        CHECK((var.get_type() == type::get<std::array<int, 10>*>()));

        variant_array_view a = var.create_array_view();
        CHECK((a.get_type() == type::get<std::array<int, 10>>()));
   }

   SECTION("wrapper")
   {
        std::array<int, 10> array;
        variant var = std::ref(array);
        CHECK((var.get_type() == type::get<std::reference_wrapper<std::array<int, 10>>>()));

        variant_array_view a = var.create_array_view();
        CHECK((a.get_type() == type::get<std::array<int, 10>>()));
   }
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("variant_array_view::swap", "[variant_array_view]")
{
   SECTION("empty")
   {
        variant_array_view a;
        variant_array_view b;
        a.swap(b);
        CHECK(b.is_valid() == false);
        CHECK(a.is_valid() == false);
   }

   SECTION("full")
   {
        variant var = std::array<int, 10>();
        variant_array_view a = var.create_array_view();
        variant_array_view b;
        a.swap(b);

        CHECK(a.is_valid() == false);
        CHECK(b.is_valid() == true);

        a = b;
        CHECK(a.is_valid() == true);
   }
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("variant_array_view::is_dynamic", "[variant_array_view]")
{
    SECTION("static")
    {
        variant var = std::array<int, 10>();
        variant_array_view a = var.create_array_view();
        CHECK(a.is_dynamic() == false);
    }

    SECTION("dynamic")
    {
        variant var = std::vector<int>();
        variant_array_view a = var.create_array_view();
        CHECK(a.is_dynamic() == true);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("variant_array_view::get_rank", "[variant_array_view]")
{
    SECTION("1D")
    {
        int obj[2] = {0, 0};
        variant var = obj;

        variant_array_view array = var.create_array_view();
        CHECK(array.get_rank() == 1);
    }

    SECTION("2D")
    {
        int obj[2][2] = {{0, 0}, {0, 0}};
        variant var = obj;

        variant_array_view array = var.create_array_view();
        CHECK(array.get_rank() == 2);
    }

    SECTION("3D")
    {
        int obj[2][2][2] = {{{0, 0}, {0, 0}}, {{0, 0}, {0, 0}}};
        variant var = obj;

        variant_array_view array = var.create_array_view();
        CHECK(array.get_rank() == 3);
    }

    SECTION("dynamic 2D")
    {
        std::vector<std::vector<int>> vec_2d;
        variant var = vec_2d;

        variant_array_view array = var.create_array_view();
        CHECK(array.get_rank() == 2);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("variant_array_view::get_rank_type", "[variant_array_view]")
{
    SECTION("2D")
    {
        int obj[2][10] = {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}};
        variant var = obj;

        variant_array_view array = var.create_array_view();
        CHECK(array.get_rank_type(0) == type::get<int[2][10]>());
        CHECK(array.get_rank_type(1) == type::get<int[10]>());
        CHECK(array.get_rank_type(2) == type::get<int>());
        CHECK(array.get_rank_type(3).is_valid() == false);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("variant_array_view::get_size", "[variant_array_view]")
{
    SECTION("1D")
    {
        variant var = std::vector<int>(50, 0);
        variant_array_view array = var.create_array_view();

        CHECK(array.get_size()  == 50);
        CHECK(array.get_size(0) == 0);
    }

    SECTION("2D")
    {
        int obj[2][10] = {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}};
        variant var = obj;
        variant_array_view array = var.create_array_view();

        CHECK(array.get_size()  == 2);
        CHECK(array.get_size(0) == 10);
        CHECK(array.get_size(1) == 10);
        CHECK(array.get_size(3) == 0);
    }

    SECTION("2D")
    {
        std::vector<std::vector<int>> vec;
        vec.push_back(std::vector<int>(10, 0));
        vec.push_back(std::vector<int>(20, 0));
        vec.push_back(std::vector<int>(30, 0));
        vec.push_back(std::vector<int>(40, 0));
        vec.push_back(std::vector<int>(50, 0));

        variant var = vec;
        variant_array_view array = var.create_array_view();

        CHECK(array.get_size()  == 5);
        CHECK(array.get_size(0) == 10);
        CHECK(array.get_size(1) == 20);
        CHECK(array.get_size(2) == 30);
        CHECK(array.get_size(3) == 40);
        CHECK(array.get_size(4) == 50);
        CHECK(array.get_size(5) == 0);
    }

    SECTION("3D")
    {
        int vec[4][3][2] = { {{0, 1}, {2, 3}, {4, 5}},
                             {{6, 7}, {8, 9}, {10, 11}},
                             {{12, 13}, {14, 15}, {16, 17}},
                             {{18, 19}, {20, 21}, {22, 23}}
                           };

        variant var = vec;
        variant_array_view array = var.create_array_view();

        CHECK(array.get_size()  == 4);
        CHECK(array.get_size(0) == 3);
        CHECK(array.get_size(1) == 3);
        CHECK(array.get_size(2) == 3);
        CHECK(array.get_size(3) == 3);
        CHECK(array.get_size(4) == 0);
        CHECK(array.get_size(0, 0) == 2);
        CHECK(array.get_size(0, 1) == 2);
        CHECK(array.get_size(0, 2) == 2);
        CHECK(array.get_size(0, 3) == 0);
    }

    SECTION("get_size_variadic")
    {
        int vec[6][5][4][3] = {};

        variant var = vec;
        variant_array_view array = var.create_array_view();

        CHECK(array.get_size()  == 6);
        CHECK(array.get_size(0) == 5);
        CHECK(array.get_size(0, 0) == 4);

        CHECK(array.get_size_variadic({}) == 6);
        CHECK(array.get_size_variadic({0}) == 5);
        CHECK((array.get_size_variadic({0, 0}) == 4));
        CHECK((array.get_size_variadic({0, 0, 0}) == 3));
        CHECK((array.get_size_variadic({0, 0, 0, 0}) == 0));
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("variant_array_view::set_size", "[variant_array_view]")
{
    SECTION("empty")
    {
        variant var;
        variant_array_view array = var.create_array_view();

        CHECK(array.set_size(10) == false);
    }

    SECTION("static array")
    {
        int obj[10] = {0};
        variant var = obj;
        variant_array_view array = var.create_array_view();

        CHECK(array.set_size(5) == false);
    }

    SECTION("dynamic array")
    {
        variant var = std::vector<int>(50, 0);
        variant_array_view array = var.create_array_view();

        CHECK(array.get_size()      == 50);
        CHECK(array.set_size(10)    == true);
        CHECK(array.get_size()      == 10);
    }

    SECTION("2D - dynamic")
    {
        std::vector<std::vector<int>> vec;

        variant var = vec;
        variant_array_view array = var.create_array_view();

        CHECK(array.get_size()      == 0);
        CHECK(array.set_size(10)    == true);
        CHECK(array.get_size()      == 10);

        CHECK(array.get_size(0)     == 0);
        CHECK(array.set_size(10, 0) == true);
        CHECK(array.get_size(0)     == 10);

        CHECK(array.get_size(9)     == 0);
        CHECK(array.set_size(10, 9) == true);
        CHECK(array.get_size(9)     == 10);

        CHECK(array.get_size(10)    == 0);
        CHECK(array.set_size(10, 10)== false);
        CHECK(array.get_size(10)    == 0);
    }

    SECTION("3D - dynamic")
    {
        std::vector<std::vector<std::vector<int>>> vec;

        variant var = vec;
        variant_array_view array = var.create_array_view();

        CHECK(array.get_size()      == 0);
        CHECK(array.set_size(10)    == true);
        CHECK(array.get_size()      == 10);

        CHECK(array.get_size(0)      == 0);
        CHECK(array.set_size(8, 0)   == true);
        CHECK(array.get_size(0)      == 8);

        CHECK(array.get_size(0, 0)   == 0);
        CHECK(array.set_size(6, 0, 0)== true);
        CHECK(array.get_size(0, 0)   == 6);

        CHECK(array.get_size(0, 1)   == 0);
        CHECK(array.set_size(4, 0, 1)== true);
        CHECK(array.get_size(0, 1)   == 4);
    }

    SECTION("set_size_variadic")
    {
        std::vector<std::vector<std::vector<int>>> vec;

        variant var = vec;
        variant_array_view array = var.create_array_view();

        CHECK(array.get_size_variadic({})       == 0);
        CHECK(array.set_size_variadic(10, {})   == true);
        CHECK(array.get_size_variadic({})       == 10);

        CHECK(array.get_size_variadic({0})      == 0);
        CHECK(array.set_size_variadic(8, {0})   == true);
        CHECK(array.get_size_variadic({0})      == 8);

        CHECK(array.get_size_variadic({0, 0})   == 0);
        CHECK(array.set_size_variadic(6, {0, 0})== true);
        CHECK(array.get_size_variadic({0, 0})   == 6);

        CHECK(array.get_size_variadic({0, 1})   == 0);
        CHECK(array.set_size_variadic(4, {0, 1})== true);
        CHECK(array.get_size_variadic({0, 1})   == 4);

        // negative test
        CHECK(array.get_size_variadic({0, 1, 2})    == 0);
        CHECK(array.set_size_variadic(4, {0, 1, 2}) == false);
    }

    SECTION("Mix dynamic/static")
    {
        std::vector<std::array<int, 5>> vec;
        variant var = vec;
        variant_array_view array = var.create_array_view();

        CHECK(array.get_size()      == 0);
        CHECK(array.set_size(10)    == true);
        CHECK(array.get_size()      == 10);

        CHECK(array.get_size(0)      == 5);
        CHECK(array.set_size(8, 0)   == false);
        CHECK(array.get_size(0)      == 5);
    }

    SECTION("test const array")
    {
        const std::vector<int> vec(50, 0);
        variant var = &vec;
        variant_array_view vec_array = var.create_array_view();
        // pointer to const array cannot be changed
        REQUIRE(vec_array.set_size(0)   == false);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("variant_array_view::get_value", "[variant_array_view]")
{
    SECTION("empty")
    {
        variant var;
        variant_array_view array = var.create_array_view();

        CHECK(array.get_value(0).is_valid() == false);
        CHECK(array.get_value(0, 0).is_valid() == false);
        CHECK(array.get_value(0, 0, 0).is_valid() == false);
        CHECK(array.get_value_variadic({0, 0, 0}).is_valid() == false);
    }

    SECTION("raw array")
    {
        int obj[2][10] = {{0, 1, 2, 3, 4, 5, 6, 7, 8, 9}, {10, 11, 12, 13, 14, 15, 16, 17, 18, 19}};
        variant var = &obj;
        variant_array_view array = var.create_array_view();

        variant var_1 = array.get_value(0);

        CHECK(var_1.is_valid() == true);
        CHECK(var_1.get_type() == type::get<int[10]>());
        CHECK(array.get_value(0, 0).get_type() == type::get<int>());
        CHECK(array.get_value(0, 0).to_int() == 0);
        CHECK(array.get_value(0, 5).to_int() == 5);

        CHECK(array.get_value(1, 0).to_int() == 10);
        CHECK(array.get_value(1, 5).to_int() == 15);

        obj[0][0] = 23;
        obj[0][5] = 23;
        obj[1][0] = 23;
        obj[1][5] = 23;

        CHECK(array.get_value(0, 0).to_int() == 23);
        CHECK(array.get_value(0, 5).to_int() == 23);

        CHECK(array.get_value(1, 0).to_int() == 23);
        CHECK(array.get_value(1, 5).to_int() == 23);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("variant_array_view::set_value", "[variant_array_view]")
{
    SECTION("empty")
    {
        variant var;
        variant_array_view array = var.create_array_view();

        CHECK(array.set_value("test") == false);
        CHECK(array.set_value(0, "test") == false);
        CHECK(array.set_value(0, 0, "test") == false);
        CHECK(array.set_value_variadic({0, 0, 0}, "test") == false);
    }

    SECTION("1D")
    {
        int obj[42] = { 0 };
        variant var = &obj;
        variant_array_view array = var.create_array_view();

        for (std::size_t i = 0; i < array.get_size(); ++i)
            array.set_value(i, 23);

        CHECK(obj[0]    == 23);
        CHECK(obj[41]   == 23);

        // negative test
        CHECK(array.set_value(0, 0, 12) == false);
    }

    SECTION("2D")
    {
        int obj[2][10] = {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}};
        variant var = &obj;
        variant_array_view array = var.create_array_view();

        int sub_array_int[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
        for (std::size_t i = 0; i < array.get_size(); ++i)
            array.set_value(i, sub_array_int);

        CHECK(obj[0][5] == 5);
        CHECK(obj[1][9] == 9);

        for (std::size_t i = 0; i < array.get_size(); ++i)
            for (std::size_t j = 0; j < array.get_size(i); ++j)
                array.set_value(i, j, -1);

        CHECK(obj[0][5] == -1);
        CHECK(obj[1][9] == -1);

        // negative test
        CHECK(array.set_value(0, 0, 0, 12) == false);
    }

    SECTION("3D")
    {
        int obj[3][3][3] = {};
        variant var = &obj;
        variant_array_view array = var.create_array_view();

        for (std::size_t i = 0; i < array.get_size(); ++i)
            for (std::size_t j = 0; j < array.get_size(i); ++j)
                for (std::size_t k = 0; k < array.get_size(i, j); ++k)
                    array.set_value(i, j, k, -1);

        CHECK(obj[0][0][0] == -1);
        CHECK(obj[2][2][2] == -1);

        // negative test
        CHECK(array.set_value_variadic({0, 0, 0, 0}, 12) == false);
    }

    SECTION("variadic")
    {
        int obj[3][3][3][3] = {};
        variant var = &obj;
        variant_array_view array = var.create_array_view();

        for (std::size_t i = 0; i < array.get_size_variadic({}); ++i)
            for (std::size_t j = 0; j < array.get_size_variadic({i}); ++j)
                for (std::size_t k = 0; k < array.get_size_variadic({i, j}); ++k)
                    for (std::size_t l = 0; l < array.get_size_variadic({i, j, k}); ++l)
                        array.set_value_variadic({i, j, k, l}, -1);

        CHECK(obj[0][0][0][0] == -1);
        CHECK(obj[2][2][2][2] == -1);

        // negative test
        CHECK(array.set_value_variadic({0, 0, 0, 0, 0}, 12) == false);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("variant_array_view::insert_value", "[variant_array_view]")
{
    SECTION("static array")
    {
        int obj[50] = { 0 };
        variant var = &obj;
        variant_array_view array = var.create_array_view();

        CHECK(array.insert_value(15, 23) == false);

        CHECK(obj[15] == 0);
    }

    SECTION("1D")
    {
        std::vector<int> obj(10, 0);
        variant var = &obj;
        variant_array_view array = var.create_array_view();

        const auto size = array.get_size();

        CHECK(array.insert_value(5, 23) == true);
        CHECK(obj[5] == 23);

        CHECK(array.get_size() == size + 1);

        // negative test
        CHECK(array.insert_value(0, 0, 23) == false);
        CHECK(array.get_size() == size + 1);
    }

    SECTION("2D")
    {
        std::vector<std::vector<int>> obj(10, std::vector<int>(10, 0));
        variant var = &obj;
        variant_array_view array = var.create_array_view();

        {
            const auto size = array.get_size();

            CHECK(array.insert_value(5, std::vector<int>()) == true);
            CHECK(obj[5] == std::vector<int>());

            CHECK(array.get_size() == size + 1);
        }

        {
            const auto size = array.get_size(5);

            CHECK(array.insert_value(5, 0, 23) == true);
            CHECK(obj[5][0] == 23);

            CHECK(array.get_size(5) == size + 1);
        }

        // negative test
        CHECK(array.insert_value(5, 23) == false);
    }

    SECTION("3D")
    {
        std::vector<std::vector<std::vector<int>>> obj(10, std::vector<std::vector<int>>(10, std::vector<int>(10, 0)));
        variant var = &obj;
        variant_array_view array = var.create_array_view();

        const auto size = array.get_size(5, 5);
        CHECK(array.insert_value(5, 5, 5, 23)   == true);
        CHECK(obj[5][5][5] == 23);
        CHECK(array.get_size(5, 5) == size + 1);

        // negative test
        CHECK(array.insert_value(5, 23)     == false);
        CHECK(array.insert_value(5, 5, 23)  == false);
    }

    SECTION("variadic")
    {
        std::vector<std::vector<std::vector<int>>> obj(10, std::vector<std::vector<int>>(10, std::vector<int>(10, 0)));
        variant var = &obj;
        variant_array_view array = var.create_array_view();

        const auto size = array.get_size(5, 5);
        CHECK(array.insert_value_variadic({5, 5, 5}, 23)   == true);
        CHECK(obj[5][5][5] == 23);
        CHECK(array.get_size_variadic({5, 5}) == size + 1);

        // negative test
        CHECK(array.insert_value_variadic({5}, 23)      == false);
        CHECK(array.insert_value_variadic({5, 5}, 23)   == false);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("variant_array_view::remove_value", "[variant_array_view]")
{
    SECTION("static array")
    {
        int obj[50] = { 0 };
        variant var = &obj;
        variant_array_view array = var.create_array_view();

        CHECK(array.remove_value(15, 23) == false);

        CHECK(obj[15] == 0);
    }

    SECTION("1D")
    {
        std::vector<int> obj(10, 0);
        variant var = &obj;
        variant_array_view array = var.create_array_view();

        const auto size = array.get_size();
        obj[5] = 23;
        CHECK(array.remove_value(5) == true);
        CHECK(obj[5] == 0);
        CHECK(array.get_size() == size - 1);

        // negative test
        CHECK(array.remove_value(0, 0) == false);
        CHECK(array.remove_value(100) == false);
        CHECK(array.get_size() == size - 1);
    }

    SECTION("2D")
    {
        std::vector<std::vector<int>> obj(10, std::vector<int>(10, 0));
        variant var = &obj;
        variant_array_view array = var.create_array_view();

        {
            const auto size = array.get_size();
            CHECK(array.remove_value(5) == true);
            CHECK(array.get_size() == size - 1);
        }

        {
            const auto size = array.get_size(5);
            obj[5][5] = 23;
            CHECK(array.remove_value(5, 0) == true);
            CHECK(obj[5][5] == 0);
            CHECK(array.get_size(5) == size - 1);
        }

        // negative test
        CHECK(array.remove_value(5, 5, 5) == false);
    }

    SECTION("3D")
    {
        std::vector<std::vector<std::vector<int>>> obj(10, std::vector<std::vector<int>>(10, std::vector<int>(10, 0)));
        variant var = &obj;
        variant_array_view array = var.create_array_view();

        const auto size = array.get_size(5, 5);
        obj[5][5][5] = 23;
        CHECK(array.remove_value(5, 5, 5) == true);
        CHECK(obj[5][5][5] == 0);
        CHECK(array.get_size(5, 5) == size - 1);

        // negative test
        CHECK(array.remove_value(5, 5, 222) == false);
    }

    SECTION("variadic")
    {
        std::vector<std::vector<std::vector<int>>> obj(10, std::vector<std::vector<int>>(10, std::vector<int>(10, 0)));
        variant var = &obj;
        variant_array_view array = var.create_array_view();

        const auto size = array.get_size(5, 5);
        obj[5][5][5] = 23;
        CHECK(array.remove_value_variadic({5, 5, 5})   == true);
        CHECK(obj[5][5][5] == 0);
        CHECK(array.get_size_variadic({5, 5}) == size - 1);

        // negative test
        CHECK(array.remove_value_variadic({5, 5, 5, 5}) == false);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("variant_array_view::get_value_as_ref", "[variant_array_view]")
{
    SECTION("positiv test")
    {
        std::vector<int> vec(50, 0);
        int i = 0;
        for (auto& value : vec)
        {
            value = ++i;
        }

        variant var = &vec;

        auto array_view = var.create_array_view();

        variant var_value = array_view.get_value_as_ref(0);
        CHECK(var_value.get_type() == type::get<std::reference_wrapper<int>>());

        auto& value_ref = var_value.get_wrapped_value<int>();
        CHECK(value_ref == 1);

        variant extr_var = var_value.extract_wrapped_value();
        REQUIRE(extr_var.get_type()     == type::get<int>());
        CHECK(extr_var.get_value<int>() == 1);
    }

    SECTION("negative test")
    {
        std::vector <bool> vec(50, true);

        variant var = &vec;

        auto array_view = var.create_array_view();

        variant var_value = array_view.get_value_as_ref(0);
        CHECK(var_value.is_valid() == false);

        var_value = array_view.get_value(0);
        CHECK(var_value.is_valid() == true);
        CHECK(var_value.get_type() == type::get<bool>());
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("variant_array_view::misc", "[variant_array_view]")
{
    SECTION("check support of vector<bool>()")
    {
        auto orig_vec = std::vector<bool>({true, false, false});
        variant var = &orig_vec;
        CHECK(*var.get_value<std::vector<bool>*>() == orig_vec);

        variant_array_view var_arr = var.create_array_view();
        var_arr.set_value(0, false);
        CHECK(orig_vec[0] == false);

        var_arr.set_size(100);
        CHECK(orig_vec.size() == 100);

        var_arr.set_size(50);
        CHECK(orig_vec.size() == 50);

        var_arr.insert_value(25, true);
        CHECK(orig_vec[25] == true);

        // check copied array
        var =  std::vector<bool>({true, false, false});
        var_arr = var.create_array_view();
        REQUIRE(var_arr.get_size() == 3);

        var_arr.set_value(0, false);

        CHECK(var_arr.get_value(0).get_value<bool>() == false);
        CHECK(var_arr.get_value(1).get_value<bool>() == false);
        CHECK(var_arr.get_value(2).get_value<bool>() == false);

        var_arr.set_value(1, true);
        var_arr.set_value(2, true);
        CHECK(var_arr.get_value(1).get_value<bool>() == true);
        CHECK(var_arr.get_value(2).get_value<bool>() == true);
    }

    SECTION("check array with wrapper type")
    {
        std::vector<int> vec(50, 0);

        std::reference_wrapper<decltype(vec)> ref_vec = std::ref(vec);
        variant var = ref_vec;
        variant_array_view array = var.create_array_view();
        REQUIRE(array.is_valid()    == true);
        REQUIRE(array.get_type()    == type::get<decltype(vec)>());

        array.set_size(70);
        CHECK(array.get_size()      == 70);
        CHECK(vec.size()            == 70);

        array.set_value(50, 23);
        variant ret = array.get_value(50);
        REQUIRE(ret.is_type<int>()      == true);
        REQUIRE(ret.get_value<int>()    == 23);
    }
}

RTTR_END_DISABLE_DEPRECATED_WARNING

/////////////////////////////////////////////////////////////////////////////////////////
