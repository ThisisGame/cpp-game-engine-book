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

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("variant_sequential_view::ctor", "[variant_sequential_view]")
{
   SECTION("empty")
   {
        variant_sequential_view a;
        CHECK(a.is_valid() == false);

        variant_sequential_view b(a);
        CHECK(b.is_valid() == false);
        a.swap(b);
        CHECK(a.is_valid() == false);
        CHECK(b.is_valid() == false);

        variant var;
        variant_sequential_view view = var.create_sequential_view();

        CHECK(view.is_valid() == false);
        CHECK(static_cast<bool>(view) == false);
        CHECK(view.is_empty() == true);
        CHECK(view.get_size() == 0);
        CHECK(view.get_type().is_valid() == false);
   }

   SECTION("invalid")
   {
        variant var = 2;
        variant_sequential_view view = var.create_sequential_view();
        CHECK(view.is_valid() == false);
        CHECK(static_cast<bool>(view) == false);
        CHECK(view.is_empty() == true);
        CHECK(view.get_size() == 0);
   }

   SECTION("valid")
   {
        variant var = std::vector<int>({1, 2, 3});
        auto a_view = var.create_sequential_view();
        CHECK(a_view.is_valid() == true);
        CHECK(static_cast<bool>(a_view) == true);
        CHECK(a_view.is_empty() == false);
        CHECK(a_view.get_size() == 3);

        auto b_view(a_view);
        CHECK(b_view.is_valid() == true);
        CHECK(b_view.is_empty() == false);
        CHECK(b_view.get_size() == 3);

        variant var_b = std::array<int, 4>{ {1, 2, 3, 4}};
        a_view = var_b.create_sequential_view();
        CHECK(a_view.get_size() == 4);
        b_view.swap(a_view);
        CHECK(a_view.get_size() == 3);
        CHECK(b_view.get_size() == 4);
   }
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("variant_sequential_view::other ctor", "[variant_sequential_view]")
{
    variant var = std::vector<int>({1, 2, 3});
    variant_sequential_view a(var.create_sequential_view());
    CHECK(a.is_valid() == true);
    CHECK(a.is_empty() == false);
    CHECK(a.get_size() == 3);
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("variant_sequential_view::get_type", "[variant_sequential_view]")
{
    SECTION("valid test")
    {
        variant var = std::vector<int>({ 1, 2, 3 });
        auto view = var.create_sequential_view();

        CHECK(view.get_type() == type::get<std::vector<int>>());
        CHECK(view.get_type() == var.get_type());
    }

    SECTION("invalid test")
    {
        variant var = 23;
        auto view = var.create_sequential_view();

        CHECK(view.get_type().is_valid() == false);

        var = variant();
        view = var.create_sequential_view();
        CHECK(view.get_type().is_valid() == false);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("variant_sequential_view::get_value_type", "[variant_sequential_view]")
{
    SECTION("valid test")
    {
        variant var = std::vector<int>({ 1, 2, 3 });
        auto view = var.create_sequential_view();

        CHECK(view.get_value_type() == type::get<int>());
    }


    SECTION("invalid test")
    {
        variant var = 23;
        auto view = var.create_sequential_view();

        CHECK(view.get_value_type().is_valid()  == false);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("variant::is_sequential_container", "[variant_sequential_view]")
{
    SECTION("invalid")
    {
        variant var;
        CHECK(var.is_sequential_container() == false);
        CHECK(var.create_sequential_view().is_valid() == false);

        var = 23;
        CHECK(var.is_sequential_container() == false);
        CHECK(var.create_sequential_view().is_valid() == false);

        var = std::set<int>({1, 2, 3});
        CHECK(var.is_sequential_container() == false);
        CHECK(var.create_sequential_view().is_valid() == false);

        auto set = std::set<int>({ 1, 2, 3 });
        var = std::ref(set);
        CHECK(var.is_sequential_container() == false);
        CHECK(var.create_sequential_view().is_valid() == false);
    }


    SECTION("valid - std::vector")
    {
        auto vec = std::vector<int>({1, 2, 3});

        variant var = vec;
        CHECK(var.is_sequential_container() == true);
        CHECK(var.create_sequential_view().is_valid() == true);

        var = std::ref(vec);
        CHECK(var.is_sequential_container() == true);
        CHECK(var.create_sequential_view().is_valid() == true);

        var = &vec;
        CHECK(var.is_sequential_container() == true);
        CHECK(var.create_sequential_view().is_valid() == true);
    }

    SECTION("valid - std::vector<bool>")
    {
        auto vec = std::vector<bool>({true, false, true});

        variant var = vec;
        CHECK(var.is_sequential_container() == true);
        CHECK(var.create_sequential_view().is_valid() == true);
    }

    SECTION("valid - std::list")
    {
        auto list = std::list<int>({1, 2, 3});

        variant var = list;
        CHECK(var.is_sequential_container() == true);
        CHECK(var.create_sequential_view().is_valid() == true);
    }

    SECTION("valid - std::deque")
    {
        auto deque = std::deque<int>({1, 2, 3});

        variant var = deque;
        CHECK(var.is_sequential_container() == true);
        CHECK(var.create_sequential_view().is_valid() == true);
    }

    SECTION("valid - std::array")
    {
        auto array = std::array<int, 4>{{1, 2, 3}};

        variant var = array;
        CHECK(var.is_sequential_container() == true);
        CHECK(var.create_sequential_view().is_valid() == true);
    }

    SECTION("valid - std::initializer_list")
    {
        auto init_list = {1, 2, 3};

        variant var = init_list;
        CHECK(var.is_sequential_container() == true);
        CHECK(var.create_sequential_view().is_valid() == true);
    }

    SECTION("valid - raw array")
    {
        int array[3] = {1, 2, 3};

        variant var = array;
        CHECK(var.is_sequential_container() == true);
        CHECK(var.create_sequential_view().is_valid() == true);
    }

    SECTION("valid - raw array ptr")
    {
        int array[3] = {1, 2, 3};

        variant var = &array;
        CHECK(var.is_sequential_container() == true);
        CHECK(var.create_sequential_view().is_valid() == true);
    }

}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("variant_sequential_view::is_empty()", "[variant_sequential_view]")
{
    SECTION("valid")
    {
        auto vec = std::vector<int>({1, 2, 3});

        variant var = std::ref(vec);
        auto view = var.create_sequential_view();
        CHECK(view.is_empty() == false);

        vec.clear();

        CHECK(view.is_empty() == true);
    }

    SECTION("invalid")
    {
        variant var = 1;
        auto view = var.create_sequential_view();
        CHECK(view.is_empty() == true);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("variant_sequential_view::is_dynamic()", "[variant_sequential_view]")
{
    SECTION("std::vector")
    {
        variant var = std::vector<int>({1, 2, 3});
        auto view = var.create_sequential_view();
        CHECK(view.is_dynamic() == true);
    }

    SECTION("std::array")
    {
        std::array<int, 3> array = {{1, 2, 3}};
        variant var = array;
        auto view = var.create_sequential_view();
        CHECK(view.is_dynamic() == false);
    }

    SECTION("invalid")
    {
        variant var = 1;
        auto view = var.create_sequential_view();
        CHECK(view.is_dynamic() == false);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("variant_sequential_view::get_rank()", "[variant_sequential_view]")
{
    SECTION("std::vector")
    {
        variant var = std::vector<int>({1, 2, 3});
        auto view = var.create_sequential_view();
        CHECK(view.get_rank() == 1);


        var = std::vector<std::vector<int>>({ { 1, 2, 3 }, {1, 2, 3} });
        view = var.create_sequential_view();
        CHECK(view.get_rank() == 2);
    }

    SECTION("std::vector, list")
    {
        variant var = std::vector<std::list<int>>({ { 1, 2, 3 }, {1, 2, 3} });
        auto view = var.create_sequential_view();
        CHECK(view.get_rank() == 2);
    }

    SECTION("std::array")
    {
        std::array<int, 3> array = {{1, 2, 3}};
        variant var = array;
        auto view = var.create_sequential_view();
        CHECK(view.get_rank() == 1);
    }

    SECTION("invalid")
    {
        variant var = 1;
        auto view = var.create_sequential_view();
        CHECK(view.get_rank() == 0);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("variant_sequential_view::get_rank_type()", "[variant_sequential_view]")
{
    SECTION("std::vector")
    {
        variant var = std::vector<int>({1, 2, 3});
        auto view = var.create_sequential_view();
        CHECK(view.get_rank_type(0) == type::get<std::vector<int>>());
        CHECK(view.get_rank_type(1) == type::get<int>());
    }

    SECTION("std::vector, list")
    {
        variant var = std::vector<std::list<int>>({ { 1, 2, 3 }, {1, 2, 3} });
        auto view = var.create_sequential_view();
        CHECK(view.get_rank_type(0) == type::get<std::vector<std::list<int>>>());
        CHECK(view.get_rank_type(1) == type::get<std::list<int>>());
        CHECK(view.get_rank_type(2) == type::get<int>());
        CHECK(view.get_rank_type(3).is_valid() == false);
    }

    SECTION("raw array")
    {
        int obj[2][10] = { { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } };
        variant var = obj;

        auto view = var.create_sequential_view();
        CHECK(view.get_rank_type(0) == type::get<int[2][10]>());
        CHECK(view.get_rank_type(1) == type::get<int[10]>());
        CHECK(view.get_rank_type(2) == type::get<int>());
        CHECK(view.get_rank_type(3).is_valid() == false);
    }

    SECTION("invalid")
    {
        variant var = 1;
        auto view = var.create_sequential_view();
        CHECK(view.get_rank_type(0).is_valid() == false);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("variant_sequential_view::get_size()", "[variant_sequential_view]")
{
    SECTION("std::vector")
    {
        variant var = std::vector<int>({1, 2, 3});
        auto view = var.create_sequential_view();
        CHECK(view.get_size() == 3);
    }

    SECTION("std::vector, list")
    {
        variant var = std::vector<std::list<int>>({ { 1, 2, 3 }, {1, 2, 3} });
        auto view = var.create_sequential_view();
        CHECK(view.get_size() == 2);
        auto itr = view.begin();

        REQUIRE(itr != view.end());
        auto sub_view = itr.get_data().create_sequential_view();
        CHECK(sub_view.is_valid() == true);
        CHECK(sub_view.get_size() == 3);
    }

    SECTION("raw array")
    {
        int obj[2][10] = { { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } };
        variant var = obj;
        auto view = var.create_sequential_view();

        CHECK(view.get_size() == 2);
        auto itr = view.begin();

        REQUIRE(itr != view.end());
        auto sub_view = itr.get_data().create_sequential_view();
        CHECK(sub_view.is_valid() == true);
        CHECK(sub_view.get_size() == 10);
    }

    SECTION("invalid")
    {
        variant var = 1;
        auto view = var.create_sequential_view();
        CHECK(view.get_size() == 0);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("variant_sequential_view::set_size()", "[variant_sequential_view]")
{
    SECTION("std::vector")
    {
        variant var = std::vector<int>();
        auto view = var.create_sequential_view();
        CHECK(view.get_size() == 0);

        CHECK(view.set_size(10) == true);

        CHECK(view.get_size() == 10);
    }

    SECTION("std::vector, list")
    {
        variant var = std::vector<std::list<int>>();
        auto view = var.create_sequential_view();
        CHECK(view.get_size() == 0);
        CHECK(view.set_size(2) == true);
        CHECK(view.get_size() == 2);
        auto itr = view.begin();

        REQUIRE(itr != view.end());
        auto sub_view = itr.get_data().create_sequential_view();
        CHECK(sub_view.is_valid() == true);
        CHECK(sub_view.get_size() == 0);
        CHECK(sub_view.set_size(10) == true);
        CHECK(sub_view.get_size() == 10);
    }

    SECTION("raw array")
    {
        int obj[2] = { 0, 0 };
        variant var = obj;
        auto view = var.create_sequential_view();
        CHECK(view.is_valid()   == true);
        CHECK(view.set_size(10) == false);
    }

    SECTION("const std::vector")
    {
        auto vec = std::vector<int>({ 1, 2, 3, 4, 5 });
        variant var = std::cref(vec);
        auto view = var.create_sequential_view();

        CHECK(view.set_size(100) == false);
        CHECK(view.get_size()    == 5);
    }

    SECTION("invalid")
    {
        variant var = 1;
        auto view = var.create_sequential_view();
        CHECK(view.set_size(42) == false);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("variant_sequential_view::insert()", "[variant_sequential_view]")
{
    SECTION("std::vector")
    {
        variant var = std::vector<int>();
        auto view = var.create_sequential_view();
        CHECK(view.get_size() == 0);

        auto itr = view.insert(view.begin(), 3);
        CHECK(itr != view.end());

        itr = view.insert(view.begin(), 2);
        CHECK(itr != view.end());

        itr = view.insert(view.begin(), 1);
        CHECK(itr != view.end());

        CHECK(view.get_size() == 3);
    }

    SECTION("std::array")
    {
        variant var = std::array<int, 5>();
        auto view = var.create_sequential_view();
        CHECK(view.get_size() == 5);

        auto itr = view.insert(view.begin(), 3);

        CHECK(itr == view.end());
        CHECK(view.get_size() == 5);
    }

    SECTION("raw array")
    {
        int obj[2] = { 0, 0 };
        variant var = obj;
        auto view = var.create_sequential_view();

        CHECK(view.insert(view.begin(), 12) == view.end());
    }

    SECTION("const std::vector")
    {
        auto vec = std::vector<int>({ 1, 2, 3, 4, 5 });
        variant var = std::cref(vec);
        auto view = var.create_sequential_view();

        CHECK(view.insert(view.begin(), 42) == view.end());
    }

    SECTION("invalid")
    {
        variant var = 1;
        auto view = var.create_sequential_view();

        auto itr = view.insert(view.begin(), 3);
        CHECK(itr == view.end());
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("variant_sequential_view::erase()", "[variant_sequential_view]")
{
    SECTION("std::vector")
    {
        variant var = std::vector<int>({1, 2, 3});
        auto view = var.create_sequential_view();

        auto itr = view.erase(view.begin());
        CHECK(itr != view.end());
        CHECK(itr == view.begin());

        itr = view.erase(++view.begin());
        CHECK(itr == view.end()); // remove element at the back

        itr = view.erase(view.begin());
        CHECK(itr == view.end());   // remove last element

        CHECK(view.get_size() == 0);
    }

    SECTION("std::array")
    {
        variant var = std::array<int, 5>({ {1, 2, 3, 4, 5} });
        auto view = var.create_sequential_view();
        CHECK(view.get_size() == 5);

        auto itr = view.erase(view.begin());
        CHECK(itr == view.end());
        CHECK(view.get_size() == 5);
    }

    SECTION("const std::vector")
    {
        auto vec = std::vector<int>({ 1, 2, 3, 4, 5 });
        variant var = std::cref(vec);
        auto view = var.create_sequential_view();

        CHECK(view.erase(view.begin()) == view.end());
    }

    SECTION("invalid")
    {
        variant var = 1;
        auto view = var.create_sequential_view();
        CHECK(view.erase(view.begin()) == view.end());
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("variant_sequential_view::clear()", "[variant_sequential_view]")
{
    SECTION("std::vector")
    {
        variant var = std::vector<int>({1, 2, 3});
        auto view = var.create_sequential_view();

        CHECK(view.get_size() == 3);
        view.clear();
        CHECK(view.get_size() == 0);
    }

    SECTION("std::array")
    {
        variant var = std::array<int, 5>({ {1, 2, 3, 4, 5} });
        auto view = var.create_sequential_view();

        CHECK(view.get_size() == 5);
        view.clear();
        CHECK(view.get_size() == 5);
    }

    SECTION("const std::vector")
    {
        auto vec = std::vector<int>({ 1, 2, 3, 4, 5 });
        variant var = std::cref(vec);
        auto view = var.create_sequential_view();

        view.clear();
        CHECK(view.get_size() == 5);
    }

    SECTION("invalid")
    {
        variant var = 1;
        auto view = var.create_sequential_view();
        CHECK(view.get_size() == 0);
        view.clear();
        CHECK(view.get_size() == 0);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("variant_sequential_view::set_value()", "[variant_sequential_view]")
{
    SECTION("std::vector")
    {
        variant var = std::vector<int>({ 1, 2, 3, 4, 5});
        auto view = var.create_sequential_view();

        REQUIRE(view.get_size() == 5);

        CHECK(view.set_value(0, 42) == true);
        CHECK(view.set_value(1, 42) == true);
        CHECK(view.set_value(2, 42) == true);
        CHECK(view.set_value(3, 42) == true);
        CHECK(view.set_value(4, 42) == true);

        CHECK(view.get_value(0).to_int() == 42);
        CHECK(view.get_value(1).to_int() == 42);
        CHECK(view.get_value(2).to_int() == 42);
        CHECK(view.get_value(3).to_int() == 42);
        CHECK(view.get_value(4).to_int() == 42);
    }

    SECTION("std::vector<bool>")
    {
        variant var = std::vector<bool>( {true, false, false, true, true} );
        auto view = var.create_sequential_view();

        REQUIRE(view.get_size() == 5);

        CHECK(view.set_value(0, false) == true);
        CHECK(view.set_value(1, false) == true);
        CHECK(view.set_value(2, false) == true);
        CHECK(view.set_value(3, false) == true);
        CHECK(view.set_value(4, false) == true);

        CHECK(view.get_value(0).to_bool() == false);
        CHECK(view.get_value(1).to_bool() == false);
        CHECK(view.get_value(2).to_bool() == false);
        CHECK(view.get_value(3).to_bool() == false);
        CHECK(view.get_value(4).to_bool() == false);
    }

    SECTION("std::list")
    {
        variant var = std::list<int>({ 1, 2, 3, 4, 5} );
        auto view = var.create_sequential_view();

        REQUIRE(view.get_size() == 5);

        CHECK(view.set_value(0, 42) == true);
        CHECK(view.set_value(1, 42) == true);
        CHECK(view.set_value(2, 42) == true);
        CHECK(view.set_value(3, 42) == true);
        CHECK(view.set_value(4, 42) == true);

        CHECK(view.get_value(0).to_int() == 42);
        CHECK(view.get_value(1).to_int() == 42);
        CHECK(view.get_value(2).to_int() == 42);
        CHECK(view.get_value(3).to_int() == 42);
        CHECK(view.get_value(4).to_int() == 42);
    }

    SECTION("std::array")
    {
        variant var = std::array<int, 5>{ {1, 2, 3, 4, 5} };
        auto view = var.create_sequential_view();

        REQUIRE(view.get_size() == 5);

        CHECK(view.set_value(0, 42) == true);
        CHECK(view.set_value(1, 42) == true);
        CHECK(view.set_value(2, 42) == true);
        CHECK(view.set_value(3, 42) == true);
        CHECK(view.set_value(4, 42) == true);

        CHECK(view.get_value(0).to_int() == 42);
        CHECK(view.get_value(1).to_int() == 42);
        CHECK(view.get_value(2).to_int() == 42);
        CHECK(view.get_value(3).to_int() == 42);
        CHECK(view.get_value(4).to_int() == 42);
    }

    SECTION("raw array")
    {
        int obj[2][5] = { { 1, 2, 3, 4, 5}, { 6, 7, 8, 9, 10 } };
        int sub_array[5] = { 42, 42, 42, 42, 42};

        variant var = &obj;
        auto view = var.create_sequential_view();

        REQUIRE(view.get_size() == 2);

        CHECK(view.set_value(1, sub_array) == true);

        auto sub_view_1 = view.get_value(0).create_sequential_view();
        auto sub_view_2 = view.get_value(1).create_sequential_view();
        CHECK(sub_view_1.get_value(0).to_int() == 1);
        CHECK(sub_view_2.get_value(0).to_int() == 42);
    }

    SECTION("const std::vector")
    {
        auto vec = std::vector<int>({ 1, 2, 3, 4, 5 });
        variant var = std::cref(vec);
        auto view = var.create_sequential_view();

        REQUIRE(view.get_size() == 5);

        CHECK(view.set_value(0, 42)      == false);
        CHECK(view.get_value(0).to_int() == 1);
    }

    SECTION("invalid")
    {
        variant var = 1;
        auto view = var.create_sequential_view();
        CHECK(view.set_value(0, 42) == false);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("variant_sequential_view::get_value()", "[variant_sequential_view]")
{
    SECTION("std::vector")
    {
        variant var = std::vector<int>({ 1, 2, 3, 4, 5});
        auto view = var.create_sequential_view();

        REQUIRE(view.get_size() == 5);

        CHECK(view.get_value(0).to_int() == 1);
        CHECK(view.get_value(1).to_int() == 2);
        CHECK(view.get_value(2).to_int() == 3);
        CHECK(view.get_value(3).to_int() == 4);
        CHECK(view.get_value(4).to_int() == 5);
    }

    SECTION("std::vector<bool>")
    {
        variant var = std::vector<bool>( {true, false, false, true, true} );
        auto view = var.create_sequential_view();

        REQUIRE(view.get_size() == 5);

        CHECK(view.get_value(0).to_bool() == true);
        CHECK(view.get_value(1).to_bool() == false);
        CHECK(view.get_value(2).to_bool() == false);
        CHECK(view.get_value(3).to_bool() == true);
        CHECK(view.get_value(4).to_bool() == true);
    }

    SECTION("std::list")
    {
        variant var = std::list<int>({ 1, 2, 3, 4, 5} );
        auto view = var.create_sequential_view();

        REQUIRE(view.get_size() == 5);

        CHECK(view.get_value(0).to_int() == 1);
        CHECK(view.get_value(1).to_int() == 2);
        CHECK(view.get_value(2).to_int() == 3);
        CHECK(view.get_value(3).to_int() == 4);
        CHECK(view.get_value(4).to_int() == 5);
    }

    SECTION("std::array")
    {
        variant var = std::array<int, 5>{ {1, 2, 3, 4, 5} };
        auto view = var.create_sequential_view();

        REQUIRE(view.get_size() == 5);

        CHECK(view.get_value(0).to_int() == 1);
        CHECK(view.get_value(1).to_int() == 2);
        CHECK(view.get_value(2).to_int() == 3);
        CHECK(view.get_value(3).to_int() == 4);
        CHECK(view.get_value(4).to_int() == 5);
    }

    SECTION("raw array")
    {
        int obj[2][5] = { { 1, 2, 3, 4, 5}, { 6, 7, 8, 9, 10 } };
        variant var = &obj;
        auto view = var.create_sequential_view();

        REQUIRE(view.get_size() == 2);
#if RTTR_COMPILER == RTTR_COMPILER_MSVC && RTTR_COMP_VER <= 1800
        CHECK((view.get_value(0).is_type<int(*)[5]>() == true));
#else
        CHECK((view.get_value(0).is_type<std::reference_wrapper<int[5]>>() == true));
#endif
        auto sub_view = view.get_value(1).create_sequential_view();
        CHECK(sub_view.get_value(0).is_type<std::reference_wrapper<int>>() == true);
        CHECK(sub_view.get_value(0).to_int() == 6);
    }

    SECTION("invalid")
    {
        variant var = 1;
        auto view = var.create_sequential_view();
        CHECK(view.get_value(0).is_valid() == false);
    }
}


/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("variant_sequential_view::begin/end", "[variant_sequential_view]")
{

    SECTION("valid test")
    {
        variant var = std::vector<int>({ 1, 2, 3 });
        auto view = var.create_sequential_view();

        int i = 0;
        for (auto& item : view)
        {
            CHECK(item.to_int() == ++i);
        }

        auto itr_begin = view.begin();
        if (itr_begin != view.end())
        {
            CHECK(itr_begin.get_data().to_int() == 1);
        }
    }

    SECTION("initializer_list")
    {
        auto array = { 1, 2, 3, 4, 5 };
        variant var = array;
        auto view = var.create_sequential_view();

        int i = 0;
        for (auto& item : view)
        {
            CHECK(item.to_int() == ++i);
        }
    }

    SECTION("raw array")
    {
        int obj[2][5] = { { 1, 2, 3, 4, 5}, { 6, 7, 8, 9, 10 } };
        variant var = &obj;
        auto view = var.create_sequential_view();

        int i = 0;
        for (auto& item : view)
        {
            if (item.is_sequential_container())
            {
                for (auto& sub_item : item.create_sequential_view())
                {
                    CHECK(sub_item.to_int() == ++i);
                }
            }
        }
    }

    SECTION("invalid test")
    {
        variant var;
        auto view = var.create_sequential_view();

        CHECK(view.begin() == view.end());
    }
}

/////////////////////////////////////////////////////////////////////////////////////////
