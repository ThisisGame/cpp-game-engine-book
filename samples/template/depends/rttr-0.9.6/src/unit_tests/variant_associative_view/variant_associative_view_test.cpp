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

TEST_CASE("variant_associative_view::ctor", "[variant_associative_view]")
{
   SECTION("empty")
   {
        variant_associative_view a;
        CHECK(a.is_valid() == false);

        variant_associative_view b(a);
        CHECK(b.is_valid() == false);
        a.swap(b);
        CHECK(a.is_valid() == false);
        CHECK(b.is_valid() == false);

        variant var;
        variant_associative_view view = var.create_associative_view();

        CHECK(view.is_valid() == false);
        CHECK(static_cast<bool>(view) == false);
        CHECK(view.is_empty() == true);
        CHECK(view.get_size() == 0);
        CHECK(view.get_type().is_valid() == false);
        CHECK(view.erase(23) == 0);
   }

   SECTION("invalid")
   {
        variant var = 2;
        variant_associative_view view = var.create_associative_view();
        CHECK(view.is_valid() == false);
        CHECK(static_cast<bool>(view) == false);
        CHECK(view.is_empty() == true);
        CHECK(view.get_size() == 0);
   }

   SECTION("valid")
   {
        variant var = std::set<int>({1, 2, 3});
        variant_associative_view a = var.create_associative_view();
        CHECK(a.is_valid() == true);
        CHECK(static_cast<bool>(a) == true);
        CHECK(a.is_empty() == false);
        CHECK(a.get_size() == 3);

        variant_associative_view b(a);
        CHECK(b.is_valid() == true);
        CHECK(b.is_empty() == false);
        CHECK(b.get_size() == 3);
   }
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("variant_associative_view::other ctor", "[variant_associative_view]")
{
    variant var = std::set<int>({1, 2, 3});
    variant_associative_view a(var.create_associative_view());
    CHECK(a.is_valid() == true);
    CHECK(a.is_empty() == false);
    CHECK(a.get_size() == 3);
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("variant_associative_view::get_type", "[variant_associative_view]")
{

    SECTION("valid test")
    {
        variant var = std::set<int>({ 1, 2, 3 });
        variant_associative_view view = var.create_associative_view();

        CHECK(view.get_type() == type::get<std::set<int>>());
        CHECK(view.get_type() == var.get_type());
    }


    SECTION("invalid test")
    {
        variant var = 23;
        variant_associative_view view = var.create_associative_view();

        CHECK(view.get_type().is_valid() == false);

        var = variant();
        view = var.create_associative_view();
        CHECK(view.get_type().is_valid() == false);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("variant_associative_view::get_key_type/get_value_type/has_value_type", "[variant_associative_view]")
{

    SECTION("valid test")
    {
        variant var = std::set<int>({ 1, 2, 3 });
        variant_associative_view view = var.create_associative_view();

        CHECK(view.get_key_type()               == type::get<int>());
        CHECK(view.get_value_type().is_valid()  == false);
        CHECK(view.is_key_only_type()           == true);

        var = std::map<int, std::string>{ { 1, "one" }, { 2, "two" }, { 3, "three" } };
        view = var.create_associative_view();

        CHECK(view.get_key_type()     == type::get<int>());
        CHECK(view.get_value_type()   == type::get<std::string>());
        CHECK(view.is_key_only_type() == false);
    }


    SECTION("invalid test")
    {
        variant var = 23;
        variant_associative_view view = var.create_associative_view();

        CHECK(view.get_key_type().is_valid()    == false);
        CHECK(view.get_value_type().is_valid()  == false);
        CHECK(view.is_key_only_type()           == false);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("variant::is_associative_container", "[variant_associative_view]")
{
    SECTION("invalid")
    {
        variant var;
        CHECK(var.is_associative_container() == false);

        var = 23;
        CHECK(var.is_associative_container() == false);

        var = std::vector<int>({1, 2, 3});
        CHECK(var.is_associative_container() == false);

        auto vec = std::vector<int>({ 1, 2, 3 });
        var = std::ref(vec);
        CHECK(var.is_associative_container() == false);
    }


    SECTION("valid - set")
    {
        auto set = std::set<int>({ 1, 2, 3 });

        variant var = set;
        CHECK(var.is_associative_container() == true);

        var = std::ref(set);
        CHECK(var.is_associative_container() == true);

        var = &set;
        CHECK(var.is_associative_container() == true);
    }

    SECTION("valid - map")
    {
        auto map = std::map<int, std::string>{ { 1, "one" }, { 2, "two" }, { 3, "three" } };

        variant var = map;
        CHECK(var.is_associative_container() == true);
    }

    SECTION("valid - multiset")
    {
        auto multiset = std::multiset<int>{ 1, 2, 2, 3, 4 };

        variant var = multiset;
        CHECK(var.is_associative_container() == true);
    }

    SECTION("valid - multimap")
    {
        auto multimap = std::multimap<int, std::string>{ { 1, "one" }, { 2, "two" },
                                                         { 2, "two" }, { 3, "three" } };

        variant var = multimap;
        CHECK(var.is_associative_container() == true);
    }

    SECTION("valid - unordered_set")
    {
        auto unordered_set = std::unordered_set<int>{ 1, 2, 2, 3, 4 };

        variant var = unordered_set;
        CHECK(var.is_associative_container() == true);
    }

    SECTION("valid - unordered_map")
    {
        auto unordered_set = std::unordered_set<int>{ 1, 2, 2, 3, 4 };

        variant var = unordered_set;
        CHECK(var.is_associative_container() == true);
    }

    SECTION("valid - unordered_multiset")
    {
        auto unordered_multiset = std::unordered_multiset<int>{ 1, 2, 2, 3, 4 };

        variant var = unordered_multiset;
        CHECK(var.is_associative_container() == true);
    }

    SECTION("valid - unordered_multimap")
    {
        auto unordered_multimap = std::unordered_multimap<int, std::string>{ { 1, "one" }, { 2, "two" },
                                                                             { 2, "two" }, { 3, "three" } };

        variant var = unordered_multimap;
        CHECK(var.is_associative_container() == true);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("variant_associative_view::iterator operations", "[variant_associative_view]")
{
    auto set = std::set<int>{ 1, 2, 3, 4 };

    variant var = set;

    auto view = var.create_associative_view();

    auto itr = view.begin();
    REQUIRE(itr != view.end());

    itr++;
    CHECK(itr.get_key().to_int() == 2);
    itr--;
    CHECK(itr.get_key().to_int() == 1);
    ++itr;
    CHECK(itr.get_key().to_int() == 2);
    --itr;
    CHECK(itr.get_key().to_int() == 1);

    itr = view.begin();
    itr += 2;
    CHECK(itr.get_key().to_int() == 3);
    itr += 1;
    itr -= 3;
    CHECK(itr.get_key().to_int() == 1);

    itr = view.begin();
    itr = itr + 3;
    CHECK(itr.get_key().to_int() == 4);
    itr = itr - 3;
    CHECK(itr.get_key().to_int() == 1);
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("variant_associative_view::insert", "[variant_associative_view]")
{
    SECTION("valid test - std::set")
    {
        auto set = std::set<int>();
        variant var = set;
        variant_associative_view view = var.create_associative_view();

        REQUIRE(view.is_valid() == true);

        auto ret = view.insert(1);
        REQUIRE(ret.first != view.end());

        CHECK(ret.first.get_key().to_int() == 1);
        CHECK(ret.first.get_value().is_valid() == false);
        CHECK(ret.second == true);

        ret = view.insert(std::string("one"));

        CHECK(ret.first == view.end());
        CHECK(ret.second == false);
    }

    SECTION("valid test - const view")
    {
        {
            const std::set<int> set = { 1, 2, 3, 4, 5 };
            variant var = std::cref(set);
            variant_associative_view view = var.create_associative_view();

            REQUIRE(view.is_valid() == true);

            auto ret = view.insert(1);
            CHECK(ret.first == view.end());
        }

        {
            const std::map<int, const std::string> map = { { 1, "one" }, { 2, "two" }, { 3, "three" } };

            variant var = std::cref(map);
            variant_associative_view view = var.create_associative_view();

            REQUIRE(view.is_valid() == true);

            auto ret = view.insert(1, std::string("test"));
            CHECK(ret.first == view.end());
        }

    }

    SECTION("valid test - std::map")
    {
        auto map = std::map<int, std::string>();
        variant var = map;
        variant_associative_view view = var.create_associative_view();

        REQUIRE(view.is_valid() == true);

        auto ret = view.insert(1, std::string("one"));

        REQUIRE(ret.first != view.end());

        CHECK(ret.first.get_key().to_int() == 1);
        CHECK(ret.first.get_value().to_string() == "one");
        CHECK(ret.second == true);

        ret = view.insert(std::make_pair(std::string("one"), 1));

        CHECK(ret.first == view.end());
        CHECK(ret.second == false);
    }

    SECTION("invalid test")
    {
        {
            variant var;
            auto view = var.create_associative_view();
            auto ret = view.insert(2);

            CHECK(ret.first == view.end());
            CHECK(ret.second == false);
        }
        {
            auto map = std::map<int, std::string>();
            variant var = map;
            auto map_view = var.create_associative_view();

            auto ret = map_view.insert(23);
            CHECK(ret.first == map_view.end());
        }
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("variant_associative_view::find", "[variant_associative_view]")
{
    SECTION("valid test")
    {
        auto map = std::map<int, std::string>{ { 1, "one" }, { 2, "two" }, { 3, "three" } };
        variant var = map;
        variant_associative_view view = var.create_associative_view();

        REQUIRE(view.is_valid() == true);

        auto itr = view.find(1);

        CHECK(itr != view.end());
        CHECK(itr.get_key().to_int() == 1);
        CHECK(itr.get_value().to_string() == "one");

        // negative test
        itr = view.find(4);
        CHECK(itr == view.end());

        itr = view.find("invalid key");
        CHECK(itr == view.end());
    }

    SECTION("invalid test")
    {
        variant var_empty;
        auto view = var_empty.create_associative_view();
        CHECK(view.find(2) == view.end());
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("variant_associative_view::equal_range", "[variant_associative_view]")
{
    SECTION("std::set")
    {
        auto set = std::set<int>{ 1, 2, 3, 4 };

        variant var = set;

        auto view = var.create_associative_view();
        auto range = view.equal_range(3);

        for (auto itr = range.first; itr != range.second; ++itr)
        {
            CHECK(itr.get_key().to_int() == 3);
        }

        // invalid equal_range search
        range = view.equal_range(5);

        CHECK(range.first == view.end());
        CHECK(range.second == view.end());

        // invalid key
        range = view.equal_range("invalid key");

        CHECK(range.first == view.end());
        CHECK(range.second == view.end());
    }

    SECTION("std::multimap")
    {
        auto multimap = std::multimap<int, std::string>{ { 1, "A" }, { 2, "B" },  { 2, "C" },
                                                         { 2, "D" }, { 3, "E" } };

        variant var = multimap;

        auto view = var.create_associative_view();
        auto range = view.equal_range(2);

        int count = 0;
        for (auto itr = range.first; itr != range.second; ++itr)
        {
            ++count;
        }
        REQUIRE(count == 3);

        auto itr = range.first;
        CHECK(itr.get_value().to_string() == "B");
        ++itr;
        CHECK(itr.get_value().to_string() == "C");
        ++itr;
        CHECK(itr.get_value().to_string() == "D");

        ++itr;
        CHECK(itr == range.second);
    }

    SECTION("invalid")
    {
        variant var;
        auto view = var.create_associative_view();
        auto ret = view.equal_range(23);

        CHECK(ret.first == view.end());
        CHECK(ret.second == view.end());
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("variant_associative_view::erase", "[variant_associative_view]")
{
    SECTION("std::set")
    {
        auto set = std::set<int>{ 1, 2, 3, 4 };
        variant var = set;
        auto view = var.create_associative_view();

        CHECK(view.erase(42) == 0);
        CHECK(view.erase(3) == 1);

        CHECK(view.get_size() == 3);
    }

    SECTION("const std::set")
    {
        auto set = std::set<int>{ 1, 2, 3, 4 };
        variant var = std::cref(set);
        auto view = var.create_associative_view();

        CHECK(view.erase(3) == 0);
        CHECK(view.get_size() == 4);
    }

    SECTION("std::map")
    {
        auto multimap = std::multimap<int, std::string>{ { 1, "A" }, { 2, "B" },  { 2, "C" },
                                                         { 2, "D" }, { 3, "E" } };
        variant var = multimap;
        auto view = var.create_associative_view();

        CHECK(view.erase(42) == 0);

        CHECK(view.erase(2) == 3);
        CHECK(view.get_size() == 2);
        CHECK(view.erase(3) == 1);
        CHECK(view.get_size() == 1);
    }

    SECTION("invalid")
    {
        auto set = std::set<int>{ 1, 2, 3, 4 };
        variant var = set;
        auto view = var.create_associative_view();

        CHECK(view.erase("invalid key") == 0);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("variant_associative_view::clear", "[variant_associative_view]")
{
    SECTION("std::set")
    {
        auto set = std::set<int>{ 1, 2, 3, 4 };
        variant var = set;
        auto view = var.create_associative_view();

        CHECK(view.get_size() == 4);
        CHECK(view.is_empty() == false);

        view.clear();

        CHECK(view.get_size() == 0);
        CHECK(view.is_empty() == true);
    }

    SECTION("const std::set")
    {
        auto set = std::set<int>{ 1, 2, 3, 4 };
        variant var = std::cref(set);
        auto view = var.create_associative_view();

        CHECK(view.get_size() == 4);
        CHECK(view.is_empty() == false);

        view.clear();

        CHECK(view.get_size() == 4);
        CHECK(view.is_empty() == false);
    }

    SECTION("std::map")
    {
        auto multimap = std::multimap<int, std::string>{ { 1, "A" }, { 2, "B" },  { 2, "C" },
                                                         { 2, "D" }, { 3, "E" } };
        variant var = multimap;
        auto view = var.create_associative_view();

        CHECK(view.get_size() == 5);
        CHECK(view.is_empty() == false);

        view.clear();

        CHECK(view.get_size() == 0);
        CHECK(view.is_empty() == true);
    }

    SECTION("invalid")
    {
        variant var;
        auto view = var.create_associative_view();
        view.clear();

        CHECK(view.get_size() == 0);
        CHECK(view.is_empty() == true);
    }
}

TEST_CASE("variant_associative_view::begin/end", "[variant_associative_view]")
{

    SECTION("valid test")
    {
        variant var = std::set<int>({ 1, 2, 3 });
        variant_associative_view view = var.create_associative_view();

        int i = 0;
        for (auto& item : view)
        {
            CHECK(item.first.to_int() == ++i);
        }

        auto itr_begin = view.begin();
        if (itr_begin != view.end())
        {
            CHECK(itr_begin.get_key().to_int() == 1);
        }
    }


    SECTION("invalid test")
    {
        variant var;
        variant_associative_view view = var.create_associative_view();

        CHECK(view.begin() == view.end());
    }
}

/////////////////////////////////////////////////////////////////////////////////////////
