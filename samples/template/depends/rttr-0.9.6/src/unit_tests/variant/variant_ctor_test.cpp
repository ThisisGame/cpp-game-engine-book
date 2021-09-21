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
#include <iostream>
#include <rttr/type>

#include <type_traits>

#include <vector>
#include <map>
#include <string>

using namespace rttr;
using namespace std;

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("constructor", "[variant]")
{
    SECTION("empty type")
    {
        variant var;
        CHECK(var.is_valid() == false);
        CHECK((bool)var      == false);
        CHECK(var.get_type().is_valid() == false);

        variant var_2 = var;
        CHECK(var_2.is_valid() == false);
        CHECK((bool)var_2      == false);
        CHECK(var_2.get_type().is_valid() == false);

        variant var_3 = std::move(var_2);
        CHECK(var_3.is_valid() == false);
        CHECK((bool)var_3      == false);
        CHECK(var_3.get_type().is_valid() == false);
    }

    SECTION("insert arithmetic type")
    {
        variant var = 42;
        CHECK(var.is_valid() == true);
        CHECK((bool)var      == true);
        CHECK(var.get_type().is_valid() == true);
        CHECK(var.get_type() == type::get<int>());

        variant var_2 = var;
        CHECK(var_2.is_valid() == true);
        CHECK((bool)var_2      == true);
        CHECK(var_2.get_type().is_valid() == true);
        CHECK(var_2.get_type() == type::get<int>());

        variant var_3 = std::move(var_2);
        CHECK(var_2.is_valid() == false);
        CHECK(var_3.is_valid() == true);
        CHECK((bool)var_3      == true);
        CHECK(var_3.get_type().is_valid() == true);
        CHECK(var_3.get_type() == type::get<int>());
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

struct simple_type
{
    simple_type() {}
    simple_type(const simple_type& other)
    :   moved(other.moved),
        moved_from(other.moved_from)
    {
    }
    simple_type(simple_type&& other)
    :   moved(true)
    {
        other.moved_from = true;
    }

    bool moved = false;
    bool moved_from = false;
};

struct big_type : simple_type
{
    big_type() {}
    big_type(const big_type& other) : simple_type(other) {}

    big_type(big_type&& other) : simple_type(std::forward<simple_type>(other)) {}

    double value;
    double value_2;
};

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("move ctor", "[variant]")
{
    SECTION("small type")
    {
        simple_type obj;
        variant var(obj);

        CHECK(obj.moved_from == false);

        variant var_2(std::move(obj));
        CHECK(obj.moved_from == true);
        CHECK(var_2.get_value<simple_type>().moved == true );
    }

    SECTION("big type")
    {
        big_type obj;
        variant var(obj);

        CHECK(obj.moved_from == false);

        variant var_2(std::move(obj));
        CHECK(obj.moved_from == true);
        CHECK(var_2.get_value<simple_type>().moved == true );
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

bool is_stored_internally(const void* obj, const rttr::variant& var)
{
  std::uintptr_t a_addr = reinterpret_cast<std::uintptr_t>(&var);
  std::uintptr_t a_end = a_addr + sizeof(var);
  std::uintptr_t obj_addr = reinterpret_cast<std::uintptr_t>(obj);
  return (a_addr <= obj_addr) && (obj_addr < a_end);
}

/////////////////////////////////////////////////////////////////////////////////////////

struct big_custom_type
{
    // two doubles, cannot be stored internally inside variant
    std::aligned_storage<sizeof(double[2]), 8>::type m_data;
};

// this type should be stored internally inside variant class.
struct small_custom_type
{
    float value;
};

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("check storage type", "[variant]")
{
    {
        variant var = big_custom_type{};
        const big_custom_type& obj_big = var.get_value<big_custom_type>();
        CHECK( !is_stored_internally(&obj_big, var) );
    }

    {
        variant var = small_custom_type{12.0f};
        const small_custom_type& obj_small = var.get_value<small_custom_type>();
        CHECK( is_stored_internally(&obj_small, var) );
    }

    {
        variant var = true;
        const bool& ref_b = var.get_value<bool>();
        CHECK( is_stored_internally(&ref_b, var) );
    }

    {
        variant var = 'D';
        const char& ref_c = var.get_value<char>();
        CHECK( is_stored_internally(&ref_c, var) );
    }

    {
        variant var = 23;
        const int& ref_i = var.get_value<int>();
        CHECK( is_stored_internally(&ref_i, var) );
    }

    {
        variant var = 42.0;
        const double& ref_d = var.get_value<double>();
        CHECK( is_stored_internally(&ref_d, var) );
    }

    {
        bool bool_array[4] = {true, false, true, false};
        variant var = bool_array;
        auto& ref_b_array = var.get_value<bool[4]>();
        CHECK( is_stored_internally(&ref_b_array, var) );
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

struct self_aware
{
    self_aware() { }
    ~self_aware()
    {
        CHECK(self == this);
    }
    self_aware(const self_aware&) { }
    self_aware& operator=(const self_aware&) { return *this; }
    self_aware(self_aware&&) RTTR_NOEXCEPT_OR_NOTHROW { }
    self_aware& operator=(self_aware&&) RTTR_NOEXCEPT_OR_NOTHROW { return *this; }

    const self_aware* self = this;
};

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("copy non trivial type", "[variant]")
{
    static_assert(std::is_nothrow_move_constructible<self_aware>::value, "");

#if __GNUG__ && __GNUC__ < 5
    static_assert(!__has_trivial_copy(self_aware), "");
#else
    static_assert(!std::is_trivially_copyable<self_aware>::value, "");
#endif

    SECTION("basic")
    {
        self_aware obj;
        variant var = obj;
    }

    SECTION("swap")
    {
        self_aware obj;
        variant a = obj;
        variant b = a;
        a.swap(b);
    }

    SECTION("move")
    {
        self_aware obj;
        variant a = obj;
        variant b = a;
        {
            variant tmp = std::move(a);
            a = std::move(b);
            b = std::move(tmp);
        }
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("variant - copy nullptr type", "[variant]")
{

    SECTION("nullptr type")
    {
        variant var = nullptr;

        CHECK(var.is_valid() == true);
        CHECK(var.get_type() == type::get<std::nullptr_t>());
        CHECK(var.get_value<std::nullptr_t>() == nullptr);
    }

    SECTION("copy nullptr type")
    {
        variant var = nullptr;
        variant var2 = var;

        CHECK(var2.is_valid() == true);
        CHECK(var2.get_type() == type::get<std::nullptr_t>());
        CHECK(var2.get_value<std::nullptr_t>() == nullptr);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////
