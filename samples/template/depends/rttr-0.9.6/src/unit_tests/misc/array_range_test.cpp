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

#include <iostream>
#include <memory>
#include <functional>
#include <type_traits>

#include <catch/catch.hpp>

using namespace rttr;
using namespace std;


/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("array_range<T> - empty", "[array_range]")
{
    auto range = array_range<int>();

    CHECK(range.begin() == range.end());
    CHECK(range.rbegin() == range.rend());
    CHECK(range.empty() == true);
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("array_range<T> - size", "[array_range]")
{
    int list[5] = {1, 2, 3, 4, 5};
    auto range = array_range<int>(&list[0], 5);
    CHECK(range.size() == 5);
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("array_range<T> - begin()", "[array_range]")
{
    int list[5] = {1, 2, 3, 4, 5};
    auto range = array_range<int>(&list[0], 5);
    using value_begin_t = decltype((*range.begin()));
    static_assert(std::is_same<const int&, value_begin_t>::value, "value should be const!");
    static_assert(std::is_same<decltype(range.begin()), decltype(range.end())>::value, "begin and end should not be different iterator types!");
    int index = 0;
    for (auto& i : range)
    {
        CHECK(i == list[index]);
        ++index;
    }

    CHECK(index == 5);
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("array_range<T> - begin() const", "[array_range]")
{
    int list[5] = {1, 2, 3, 4, 5};
    const auto range = array_range<int>(&list[0], 5);
    using value_begin_t = decltype((*range.begin()));
    static_assert(std::is_same<const int&, value_begin_t>::value, "value should be const!");
    static_assert(std::is_same<decltype(range.begin()), decltype(range.end())>::value, "begin and end should not be different iterator types!");
    int index = 0;
    for (auto& i : range)
    {
        CHECK(i == list[index]);
        ++index;
    }

    CHECK(index == 5);
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("array_range<T> - cbegin()", "[array_range]")
{
    int list[5] = {1, 2, 3, 4, 5};
    auto range = array_range<int>(&list[0], 5);
    using value_begin_t = decltype((*range.cbegin()));
    static_assert(std::is_same<const int&, value_begin_t>::value, "value should be const!");
    static_assert(std::is_same<decltype(range.rbegin()), decltype(range.rend())>::value, "begin and end should not be different iterator types!");
    int index = 0;
    auto itr = range.cbegin();
    for (; itr != range.cend(); ++itr)
    {
        CHECK(*itr == list[index]);
        ++index;
    }
    CHECK(index == 5);
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("array_range<T> - rbegin()", "[array_range]")
{
    int list[5] = {1, 2, 3, 4, 5};
    auto range = array_range<int>(&list[0], 5);
    using value_begin_t = decltype((*range.rbegin()));
    static_assert(std::is_same<const int&, value_begin_t>::value, "value should be non const!");
    static_assert(std::is_same<decltype(range.rbegin()), decltype(range.rend())>::value, "begin and end should not be different iterator types!");
    int index = 4;
    for (auto& i : detail::reverse(range))
    {
        CHECK(i == list[index]);
        --index;
    }

    CHECK(index == -1);
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("array_range<T> - rbegin() const", "[array_range]")
{
    int list[5] = {1, 2, 3, 4, 5};
    const auto range = array_range<int>(&list[0], 5);
    using value_begin_t = decltype((*range.rbegin()));
    static_assert(std::is_same<const int&, value_begin_t>::value, "value should be const!");
    static_assert(std::is_same<decltype(range.rbegin()), decltype(range.rend())>::value, "begin and end should not be different iterator types!");
    int index = 4;
    for (auto& i : detail::reverse(range))
    {
        CHECK(i == list[index]);
        --index;
    }
    CHECK(index == -1);
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("array_range<T> - crbegin()", "[array_range]")
{
    int list[5] = {1, 2, 3, 4, 5};
    auto range = array_range<int>(&list[0], 5);
    using value_begin_t = decltype((*range.crbegin()));
    static_assert(std::is_same<const int&, value_begin_t>::value, "value should be const!");
    static_assert(std::is_same<decltype(range.rbegin()), decltype(range.rend())>::value, "begin and end should not be different iterator types!");
    int index = 4;
    auto itr = range.crbegin();
    for (; itr != range.crend(); ++itr)
    {
        CHECK(*itr == list[index]);
        --index;
    }
    CHECK(index == -1);
}

/////////////////////////////////////////////////////////////////////////////////////////
