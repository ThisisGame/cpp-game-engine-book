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

#include "unit_tests/test_classes.h"

#include <rttr/type>

using namespace rttr;
using namespace std;

#include <iostream>
#include <memory>
#include <functional>

#include <catch/catch.hpp>

////////////////////////////////////////////////////////////////////////////////////////

namespace
{
    struct custom_type
    {
        custom_type(){}
        int value = 23;

        int my_func() { return value; }
        int my_const_func() const { return value; }
    };

    typedef int(custom_type::*mem_obj_ptr_t);
    typedef int(custom_type::*mem_func_ptr_t)(void);
    typedef int(custom_type::*mem_const_func_ptr_t)(void)const;

    int my_free_func() { return 42; }

    typedef int(*func_ptr_t)(void);
} // end namespace anonymous


TEST_CASE("Test raw_adressof", "[raw_adressof]")
{
    SECTION("Test pointer type")
    {
        int value = 23;
        int* result = detail::raw_addressof(value);
        CHECK(result ==  &value);

        int* int_ptr = &value;
        int* result_2 = detail::raw_addressof(int_ptr);
        CHECK(result_2 ==  &value);

        int** int_ptr_ptr = &int_ptr;
        int* result_3 = detail::raw_addressof(int_ptr_ptr);
        CHECK(result_3 ==  &value);

        int*** int_ptr_ptr_ptr = &int_ptr_ptr;
        int* result_4 = detail::raw_addressof(int_ptr_ptr_ptr);
        CHECK(result_4 ==  &value);
    }

    SECTION("Test const pointer type")
    {
        const int value = 23;
        const int* int_ptr = &value;
        const int* result = detail::raw_addressof(value);
        CHECK(result ==  &value);

        const int** int_ptr_ptr = &int_ptr;
        const int* result_2 = detail::raw_addressof(int_ptr_ptr);
        CHECK(result_2 ==  &value);

        const int*** int_ptr_ptr_ptr = &int_ptr_ptr;
        const int* result_3 = detail::raw_addressof(int_ptr_ptr_ptr);
        CHECK(result_3 ==  &value);
    }

    SECTION("Test custom type")
    {
        custom_type obj;
        custom_type* obj_ptr = &obj;
        custom_type* result = detail::raw_addressof(obj);
        CHECK(result ==  &obj);

        custom_type** obj_ptr_ptr = &obj_ptr;
        custom_type* result_2 = detail::raw_addressof(obj_ptr_ptr);
        CHECK(result_2 ==  &obj);

        custom_type*** obj_ptr_ptr_ptr = &obj_ptr_ptr;
        custom_type* result_3 = detail::raw_addressof(obj_ptr_ptr_ptr);
        CHECK(result_3 ==  &obj);
    }

    SECTION("Test function type")
    {
        func_ptr_t func_ptr = &my_free_func;
        func_ptr_t* result = detail::raw_addressof(func_ptr);
        CHECK(*result ==  &my_free_func);

        func_ptr_t* func_ptr_ptr = &func_ptr;
        func_ptr_t* result_2 = detail::raw_addressof(func_ptr_ptr);
        CHECK(*result_2 ==  &my_free_func);

        func_ptr_t** func_ptr_ptr_ptr = &func_ptr_ptr;
        func_ptr_t* result_3 = detail::raw_addressof(func_ptr_ptr_ptr);
        CHECK(*result_3 ==  &my_free_func);
    }

    SECTION("Test const function type")
    {
        const func_ptr_t func_ptr = &my_free_func;
        const func_ptr_t* result = detail::raw_addressof(func_ptr);
        CHECK(*result ==  &my_free_func);

        const func_ptr_t* func_ptr_ptr = &func_ptr;
        const func_ptr_t* result_2 = detail::raw_addressof(func_ptr_ptr);
        CHECK(*result_2 ==  &my_free_func);

        const func_ptr_t** func_ptr_ptr_ptr = &func_ptr_ptr;
        const func_ptr_t* result_3 = detail::raw_addressof(func_ptr_ptr_ptr);
        CHECK(*result_3 ==  &my_free_func);
    }

    SECTION("Test member object type")
    {
        custom_type obj;
        mem_obj_ptr_t mem_obj_ptr = &custom_type::value;
        mem_obj_ptr_t* result = detail::raw_addressof(mem_obj_ptr);
        CHECK(&(obj.**result) ==  &obj.value);

        mem_obj_ptr_t* mem_obj_ptr_ptr = &mem_obj_ptr;
        mem_obj_ptr_t* result_2 = detail::raw_addressof(mem_obj_ptr_ptr);
        CHECK(&(obj.**result_2) ==  &obj.value);

        mem_obj_ptr_t** mem_obj_ptr_ptr_ptr = &mem_obj_ptr_ptr;
        mem_obj_ptr_t* result_3 = detail::raw_addressof(mem_obj_ptr_ptr_ptr);
        CHECK(&(obj.**result_3) ==  &obj.value);
    }

    SECTION("Test const member object type")
    {
        const custom_type obj;
        const mem_obj_ptr_t mem_obj_ptr = &custom_type::value;
        const mem_obj_ptr_t* result = detail::raw_addressof(mem_obj_ptr);
        CHECK(&(obj.**result) ==  &obj.value);

        const mem_obj_ptr_t* mem_obj_ptr_ptr = &mem_obj_ptr;
        const mem_obj_ptr_t* result_2 = detail::raw_addressof(mem_obj_ptr_ptr);
        CHECK(&(obj.**result_2) ==  &obj.value);

        const mem_obj_ptr_t** mem_obj_ptr_ptr_ptr = &mem_obj_ptr_ptr;
        const mem_obj_ptr_t* result_3 = detail::raw_addressof(mem_obj_ptr_ptr_ptr);
        CHECK(&(obj.**result_3) ==  &obj.value);
    }

    SECTION("Test member function type")
    {
        custom_type obj;
        mem_func_ptr_t mem_func_ptr = &custom_type::my_func;
        mem_func_ptr_t* result = detail::raw_addressof(mem_func_ptr);
        CHECK(*result == mem_func_ptr);

        mem_func_ptr_t* mem_func_ptr_ptr = &mem_func_ptr;
        mem_func_ptr_t* result_2 = detail::raw_addressof(mem_func_ptr_ptr);
        CHECK(*result_2 == mem_func_ptr);

        mem_func_ptr_t* mem_func_ptr_ptr_ptr = &mem_func_ptr;
        mem_func_ptr_t* result_3 = detail::raw_addressof(mem_func_ptr_ptr_ptr);
        CHECK(*result_3 == mem_func_ptr);
    }

    SECTION("Test const member function type")
    {
        const custom_type obj;
        const mem_func_ptr_t mem_func_ptr = &custom_type::my_func;
        const mem_func_ptr_t* result = detail::raw_addressof(mem_func_ptr);
        CHECK(*result == mem_func_ptr);

        const mem_func_ptr_t* mem_func_ptr_ptr = &mem_func_ptr;
        const mem_func_ptr_t* result_2 = detail::raw_addressof(mem_func_ptr_ptr);
        CHECK(*result_2 == mem_func_ptr);

        const mem_func_ptr_t* mem_func_ptr_ptr_ptr = &mem_func_ptr;
        const mem_func_ptr_t* result_3 = detail::raw_addressof(mem_func_ptr_ptr_ptr);
        CHECK(*result_3 == mem_func_ptr);
    }

    SECTION("Test member function const type")
    {
        custom_type obj;
        mem_const_func_ptr_t mem_func_ptr = &custom_type::my_const_func;
        mem_const_func_ptr_t* result = detail::raw_addressof(mem_func_ptr);
        CHECK(*result == mem_func_ptr);

        mem_const_func_ptr_t* mem_func_ptr_ptr = &mem_func_ptr;
        mem_const_func_ptr_t* result_2 = detail::raw_addressof(mem_func_ptr_ptr);
        CHECK(*result_2 == mem_func_ptr);

        mem_const_func_ptr_t* mem_func_ptr_ptr_ptr = &mem_func_ptr;
        mem_const_func_ptr_t* result_3 = detail::raw_addressof(mem_func_ptr_ptr_ptr);
        CHECK(*result_3 == mem_func_ptr);
    }

    SECTION("Test const member function const type")
    {
        const custom_type obj;
        const mem_const_func_ptr_t mem_func_ptr = &custom_type::my_const_func;
        const mem_const_func_ptr_t* result = detail::raw_addressof(mem_func_ptr);
        CHECK(*result == mem_func_ptr);

        const mem_const_func_ptr_t* mem_func_ptr_ptr = &mem_func_ptr;
        const mem_const_func_ptr_t* result_2 = detail::raw_addressof(mem_func_ptr_ptr);
        CHECK(*result_2 == mem_func_ptr);

        const mem_const_func_ptr_t* mem_func_ptr_ptr_ptr = &mem_func_ptr;
        const mem_const_func_ptr_t* result_3 = detail::raw_addressof(mem_func_ptr_ptr_ptr);
        CHECK(*result_3 == mem_func_ptr);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("Test get_wrapper_adressof", "[wrapped_raw_addressof]")
{
    SECTION("Test std::shared_ptr")
    {
        std::shared_ptr<int> obj = std::make_shared<int>(23);
        int* int_ptr = detail::wrapped_raw_addressof(obj);
        CHECK(int_ptr == obj.get());

        std::shared_ptr<const int> obj_c = std::make_shared<const int>(23);
        const int* int_ptr_c = detail::wrapped_raw_addressof(obj_c);
        CHECK(int_ptr_c == obj_c.get());
    }

    SECTION("Test std::reference_wrapper")
    {
        int value = 23;
        std::reference_wrapper<int> obj = std::ref(value);
        int* result = detail::wrapped_raw_addressof(obj);
        CHECK(result == &obj.get());

        const int value_c = 42;
        std::reference_wrapper<const int> obj_c = std::cref(value_c);
        const int* result_2 = detail::wrapped_raw_addressof(obj_c);
        CHECK(result_2 == &obj_c.get());

        const func_ptr_t func_ptr = &my_free_func;
        std::reference_wrapper<const func_ptr_t> obj_func = std::cref(func_ptr);
        const func_ptr_t* result_3 = detail::wrapped_raw_addressof(obj_func);
        CHECK(*result_3 == &my_free_func);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

template<typename T1, typename T2>
bool raw_type_check()
{
    static_assert(std::is_same<typename detail::raw_type<T1>::type, T2>::value, "Types are not the same!");
    return true;
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("Test raw_type type traits", "[raw_type<T>]")
{
    SECTION("Test pointer type")
    {
        raw_type_check<int,                     int >();
        raw_type_check<int*,                    int >();
        raw_type_check<int**,                   int >();

        raw_type_check<const int,               int >();
        raw_type_check<const int*,              int >();
        raw_type_check<const int**,             int >();

        raw_type_check<volatile int,            int >();
        raw_type_check<volatile int*,           int >();
        raw_type_check<volatile int**,          int >();

        raw_type_check<const volatile int,      int >();
        raw_type_check<const volatile int*,     int >();
        raw_type_check<const volatile int**,    int >();
    }

    SECTION("Test pointer type to cv type")
    {
        raw_type_check<const int* const,                    int >();
        raw_type_check<const int** const,                   int >();

        raw_type_check<volatile int* const,                 int >();
        raw_type_check<volatile int** const,                int >();

        raw_type_check<const volatile int* const,           int >();
        raw_type_check<const volatile int** const,          int >();

        //
        raw_type_check<const int* volatile,                 int >();
        raw_type_check<const int** volatile,                int >();

        raw_type_check<volatile int* volatile,              int >();
        raw_type_check<volatile int** volatile,             int >();

        raw_type_check<const volatile int* volatile,        int >();
        raw_type_check<const volatile int** volatile,       int >();

        //
        raw_type_check<const int* const volatile,           int >();
        raw_type_check<const int** const volatile,          int >();

        raw_type_check<volatile int* const volatile,        int >();
        raw_type_check<volatile int** const volatile,       int >();

        raw_type_check<const volatile int* const volatile,  int >();
        raw_type_check<const volatile int** const volatile, int >();
    }

    SECTION("Test reference types")
    {
        raw_type_check<int&,                        int >();
        raw_type_check<int&&,                       int >();

        raw_type_check<const int&,                  int >();
        raw_type_check<const int&&,                 int >();

        raw_type_check<volatile int&,               int >();
        raw_type_check<volatile int&&,              int >();

        raw_type_check<const volatile int&,         int >();
        raw_type_check<const volatile int&&,        int >();
    }

    SECTION("Test reference to pointer types")
    {
        raw_type_check<int*&,                        int >();
        raw_type_check<std::vector<int> const &,     std::vector<int> >();
    }

    SECTION("Test array types")
    {
        raw_type_check<int[10],                     int[10]>();
        raw_type_check<int(*)[10],                  int[10]>();
        raw_type_check<int(**)[10],                 int[10] >();

        raw_type_check<const int[10],               int[10] >();
        raw_type_check<const int(*)[10],            int[10] >();
        raw_type_check<const int(**)[10],           int[10] >();

        raw_type_check<volatile int[10],            int[10] >();
        raw_type_check<volatile int(*)[10],         int[10] >();
        raw_type_check<volatile int(**)[10],        int[10] >();

        raw_type_check<const volatile int[10],      int[10] >();
        raw_type_check<const volatile int(*)[10],   int[10] >();
        raw_type_check<const volatile int(**)[10],  int[10] >();
    }

    SECTION("Test array pointer cv types")
    {
        raw_type_check<int(*const)[10],             int[10]>();
        raw_type_check<int(*volatile)[10],          int[10]>();
        raw_type_check<int(*const volatile)[10],    int[10]>();

         raw_type_check<int(**const)[10],           int[10]>();
        raw_type_check<int(**volatile)[10],         int[10]>();
        raw_type_check<int(**const volatile)[10],   int[10]>();
    }

    SECTION("Test multi dimensional array types")
    {
        raw_type_check<int[10][25],                     int[10][25]>();
        raw_type_check<int(*)[10][25],                  int[10][25]>();
        raw_type_check<int(**)[10][25],                 int[10][25] >();

        raw_type_check<const int[10][25],               int[10][25] >();
        raw_type_check<const int(*)[10][25],            int[10][25] >();
        raw_type_check<const int(**)[10][25],           int[10][25] >();

        raw_type_check<volatile int[10][25],            int[10][25] >();
        raw_type_check<volatile int(*)[10][25],         int[10][25] >();
        raw_type_check<volatile int(**)[10][25],        int[10][25] >();

        raw_type_check<const volatile int[10][25],      int[10][25] >();
        raw_type_check<const volatile int(*)[10][25],   int[10][25] >();
        raw_type_check<const volatile int(**)[10][25],  int[10][25] >();
    }

    SECTION("Test multi dimensional array pointer cv types")
    {
        raw_type_check<int(*const)[10][25],             int[10][25]>();
        raw_type_check<int(*volatile)[10][25],          int[10][25]>();
        raw_type_check<int(*const volatile)[10][25],    int[10][25]>();

        raw_type_check<int(**const)[10][25],            int[10][25]>();
        raw_type_check<int(**volatile)[10][25],         int[10][25]>();
        raw_type_check<int(**const volatile)[10][25],   int[10][25]>();
    }

    SECTION("Test free function types")
    {
        raw_type_check<func_ptr_t,                  func_ptr_t>();
        raw_type_check<const func_ptr_t,            func_ptr_t>();
        raw_type_check<const volatile func_ptr_t,   func_ptr_t>();

        raw_type_check<func_ptr_t*,                 func_ptr_t>();
        raw_type_check<const func_ptr_t*,           func_ptr_t>();
        raw_type_check<const volatile func_ptr_t*,  func_ptr_t>();
    }

    SECTION("Test member object type")
    {
        raw_type_check<mem_obj_ptr_t,                  mem_obj_ptr_t>();
        raw_type_check<const mem_obj_ptr_t,            mem_obj_ptr_t>();
        raw_type_check<const volatile mem_obj_ptr_t,   mem_obj_ptr_t>();

        raw_type_check<mem_obj_ptr_t*,                 mem_obj_ptr_t>();
        raw_type_check<const mem_obj_ptr_t*,           mem_obj_ptr_t>();
        raw_type_check<const volatile mem_obj_ptr_t*,  mem_obj_ptr_t>();
    }

    SECTION("Test member function type")
    {
        raw_type_check<mem_func_ptr_t,                       mem_func_ptr_t>();
        raw_type_check<const mem_func_ptr_t,                 mem_func_ptr_t>();
        raw_type_check<const volatile mem_func_ptr_t,        mem_func_ptr_t>();

        raw_type_check<mem_func_ptr_t*,                      mem_func_ptr_t>();
        raw_type_check<const mem_func_ptr_t*,                mem_func_ptr_t>();
        raw_type_check<const volatile mem_func_ptr_t*,       mem_func_ptr_t>();

        raw_type_check<mem_const_func_ptr_t,                 mem_const_func_ptr_t>();
        raw_type_check<const mem_const_func_ptr_t,           mem_const_func_ptr_t>();
        raw_type_check<const volatile mem_const_func_ptr_t,  mem_const_func_ptr_t>();

        raw_type_check<mem_const_func_ptr_t*,                mem_const_func_ptr_t>();
        raw_type_check<const mem_const_func_ptr_t*,          mem_const_func_ptr_t>();
        raw_type_check<const volatile mem_const_func_ptr_t*, mem_const_func_ptr_t>();
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

template<typename T1, typename T2>
bool raw_array_type_check()
{
    static_assert(std::is_same<typename detail::raw_array_type<T1>::type, T2>::value, "Types are not the same!");
    return true;
}

////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("Test raw_array_type type traits", "[raw_array_type<T>]")
{
    SECTION("Test array types")
    {
        raw_array_type_check<int[100], int>();
        raw_array_type_check<const int[100], int>();
        raw_array_type_check<const volatile int[100], int>();
        raw_array_type_check<const volatile int*[100], int>();
        raw_array_type_check<const volatile int* const[100], int>();
        raw_array_type_check<const volatile int* const volatile[100], int>();
    }

    SECTION("Test multi dimensional array types")
    {
        raw_array_type_check<int[100][25], int>();
        raw_array_type_check<const int[100][25], int>();
        raw_array_type_check<const volatile int[100][25], int>();
        raw_array_type_check<const volatile int*[100][25], int>();
        raw_array_type_check<const volatile int* const[100][25], int>();
        raw_array_type_check<const volatile int* const volatile[100][25], int>();
    }
}

/////////////////////////////////////////////////////////////////////////////////////////
