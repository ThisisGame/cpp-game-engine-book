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

#include <rttr/registration>
#include <catch/catch.hpp>

using namespace rttr;

struct ctor_default_arg_test
{
    ctor_default_arg_test(int value1, bool value2, const std::string& text)
    {
    }

    ctor_default_arg_test(int value1)
    {
    }

    static std::shared_ptr<ctor_default_arg_test> create_object(int value1, bool value2, const std::string& value3)
    {
        if (value1 == 23 && value2 == true && value3 == std::string("default text"))
            return std::make_shared<ctor_default_arg_test>(value1, value2, value3);
        else
            return std::shared_ptr<ctor_default_arg_test>();
    }
};

struct ctor_default_arg_invoke_test
{
    ctor_default_arg_invoke_test(int) {};
    ctor_default_arg_invoke_test(int, int) {};
    ctor_default_arg_invoke_test(int, int, int) {};
    ctor_default_arg_invoke_test(int, int, int, int) {};
    ctor_default_arg_invoke_test(int, int, int, int, int) {};
    ctor_default_arg_invoke_test(int, int, int, int, int, int) {};
    ctor_default_arg_invoke_test(int, int, int, int, int, int, int) {};

    static ctor_default_arg_invoke_test create(int a) { return ctor_default_arg_invoke_test(a); }
    static ctor_default_arg_invoke_test create(int a, int b) { return ctor_default_arg_invoke_test(a, b); }
    static ctor_default_arg_invoke_test create(int a, int b, int c) { return ctor_default_arg_invoke_test(a, b, c); }
    static ctor_default_arg_invoke_test create(int a, int b, int c, int d) { return ctor_default_arg_invoke_test(a, b, c, d); }
    static ctor_default_arg_invoke_test create(int a, int b, int c, int d, int e) { return ctor_default_arg_invoke_test(a, b, c, d, e); }
    static ctor_default_arg_invoke_test create(int a, int b, int c, int d, int e, int f) { return ctor_default_arg_invoke_test(a, b, c, d, e, f); }
    static ctor_default_arg_invoke_test create(int a, int b, int c, int d, int e, int f, int g) { return ctor_default_arg_invoke_test(a, b, c, d, e, f, g); }
};

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

RTTR_REGISTRATION
{
    registration::class_<ctor_default_arg_test>("ctor_default_arg_test")
        .constructor<int, bool, const std::string&>()
        (
            default_arguments(true, std::string("default text")),
            policy::ctor::as_std_shared_ptr
        )
        .constructor(&ctor_default_arg_test::create_object)
        (
            default_arguments(true, std::string("default text"))
        )
        .constructor<int>()
        (
            default_arguments(23),
            policy::ctor::as_std_shared_ptr
        );

     registration::class_<ctor_default_arg_invoke_test>("ctor_default_arg_invoke_test")
        .constructor<int>() ( default_arguments(1) )
        .constructor<int, int>() ( default_arguments(1, 2) )
        .constructor<int, int, int>() ( default_arguments(1, 2, 3) )
        .constructor<int, int, int, int>() ( default_arguments(1, 2, 3, 4) )
        .constructor<int, int, int, int, int>() ( default_arguments(1, 2, 3, 4, 5) )
        .constructor<int, int, int, int, int, int>() ( default_arguments(1, 2, 3, 4, 5, 6) )
        .constructor<int, int, int, int, int, int, int>() ( default_arguments(1, 2, 3, 4, 5, 6, 7) )
        ///
        .constructor(static_cast<ctor_default_arg_invoke_test(*)(int)>(&ctor_default_arg_invoke_test::create))
        ( default_arguments(1) )
        .constructor(static_cast<ctor_default_arg_invoke_test(*)(int, int)>(&ctor_default_arg_invoke_test::create))
        ( default_arguments(1, 2) )
        .constructor(static_cast<ctor_default_arg_invoke_test(*)(int, int, int)>(&ctor_default_arg_invoke_test::create))
        ( default_arguments(1, 2, 3) )
        .constructor(static_cast<ctor_default_arg_invoke_test(*)(int, int, int, int)>(&ctor_default_arg_invoke_test::create))
        ( default_arguments(1, 2, 3, 4) )
        .constructor(static_cast<ctor_default_arg_invoke_test(*)(int, int, int, int, int)>(&ctor_default_arg_invoke_test::create))
        ( default_arguments(1, 2, 3, 4, 5) )
        .constructor(static_cast<ctor_default_arg_invoke_test(*)(int, int, int, int, int, int)>(&ctor_default_arg_invoke_test::create))
        ( default_arguments(1, 2, 3, 4, 5, 6) )
        .constructor(static_cast<ctor_default_arg_invoke_test(*)(int, int, int, int, int, int, int)>(&ctor_default_arg_invoke_test::create))
        ( default_arguments(1, 2, 3, 4, 5, 6, 7) )
        ;

}

////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("constructor - default argument test (real ctor)", "[constructor]")
{
    type t = type::get<ctor_default_arg_test>();
    REQUIRE(t.is_valid() == true);

    auto range = t.get_constructors();
    std::vector<constructor> ctor_list(range.cbegin(), range.cend());
    REQUIRE(ctor_list.size() > 1);

    constructor ctor = ctor_list[0];
    using create_type = std::shared_ptr<ctor_default_arg_test>;
    // too less arguments for invoke
    variant var = ctor.invoke();
    CHECK(var.is_valid() == false);

    // too much arguments
    var = ctor.invoke(23, true, std::string("default text"), 43.0);
    CHECK(var.is_valid() == false);

    // using 2 default arguments
    var = ctor.invoke(23);
    REQUIRE(var.is_type<create_type>() == true);
    CHECK(var.get_value<create_type>().get() != nullptr);

    // using 1 default arguments
    var = ctor.invoke(23, true);
    REQUIRE(var.is_type<create_type>() == true);
    CHECK(var.get_value<create_type>().get() != nullptr);

    // using NO default arguments
    var = ctor.invoke(23, true, std::string("default text"));
    REQUIRE(var.is_type<create_type>() == true);
    CHECK(var.get_value<create_type>().get() != nullptr);
}

////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("constructor - default argument test (func ctor)", "[constructor]")
{
    type t = type::get<ctor_default_arg_test>();
    REQUIRE(t.is_valid() == true);

    auto range = t.get_constructors();
    std::vector<constructor> ctor_list(range.cbegin(), range.cend());
    REQUIRE(ctor_list.size() > 1);
    constructor ctor = ctor_list[1];
    using create_type = std::shared_ptr<ctor_default_arg_test>;
    // too less arguments for invoke
    variant var = ctor.invoke();
    CHECK(var.is_valid() == false);

    // too much arguments
    var = ctor.invoke(23, true, std::string("default text"), 43.0);
    CHECK(var.is_valid() == false);

    // using 2 default arguments
    var = ctor.invoke(23);
    REQUIRE(var.is_type<create_type>() == true);
    CHECK(var.get_value<create_type>().get() != nullptr);

    // using 1 default arguments
    var = ctor.invoke(23, true);
    REQUIRE(var.is_type<create_type>() == true);
    CHECK(var.get_value<create_type>().get() != nullptr);

    // using NO default arguments
    var = ctor.invoke(23, true, std::string("default text"));
    REQUIRE(var.is_type<create_type>() == true);
    CHECK(var.get_value<create_type>().get() != nullptr);
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("constructor - default argument test (invoke variadic; real ctor)", "[constructor]")
{
    type t = type::get<ctor_default_arg_test>();
    REQUIRE(t.is_valid() == true);

    auto range = t.get_constructors();
    std::vector<constructor> ctor_list(range.cbegin(), range.cend());
    REQUIRE(ctor_list.size() > 1);
    constructor ctor = ctor_list[0];
    using create_type = std::shared_ptr<ctor_default_arg_test>;
    // too less arguments for invoke
    variant var = ctor.invoke();
    CHECK(var.is_valid() == false);

    // too much arguments
#if RTTR_COMPILER == RTTR_COMPILER_MSVC && RTTR_COMP_VER <= 1800
    int value1 = 23;
    bool value2 = true;
    std::string value3 = ("default text");
    var = ctor.invoke_variadic({value1, value2, value3, 43.0});
#else
    var = ctor.invoke_variadic({23, true, std::string("default text"), 43.0});
#endif
    CHECK(var.is_valid() == false);

    // using 2 default arguments
#if RTTR_COMPILER == RTTR_COMPILER_MSVC && RTTR_COMP_VER <= 1800
    var = ctor.invoke_variadic({value1});
#else
    var = ctor.invoke_variadic({23});
#endif
    REQUIRE(var.is_type<create_type>() == true);
    CHECK(var.get_value<create_type>().get() != nullptr);

    // using 1 default arguments
#if RTTR_COMPILER == RTTR_COMPILER_MSVC && RTTR_COMP_VER <= 1800
    var = ctor.invoke_variadic({value1, value2});
#else
    var = ctor.invoke_variadic({23, true});
#endif
    REQUIRE(var.is_type<create_type>() == true);
    CHECK(var.get_value<create_type>().get() != nullptr);

    // using NO default arguments
#if RTTR_COMPILER == RTTR_COMPILER_MSVC && RTTR_COMP_VER <= 1800
    var = ctor.invoke_variadic({value1, value2, value3});
#else
    var = ctor.invoke_variadic({23, true, std::string("default text")});
#endif
    REQUIRE(var.is_type<create_type>() == true);
    CHECK(var.get_value<create_type>().get() != nullptr);
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("constructor - default argument test (invoke variadic; func ctor)", "[constructor]")
{
    type t = type::get<ctor_default_arg_test>();
    REQUIRE(t.is_valid() == true);

    auto range = t.get_constructors();
    std::vector<constructor> ctor_list(range.cbegin(), range.cend());
    REQUIRE(ctor_list.size() > 2);
    constructor ctor = ctor_list[1];
    using create_type = std::shared_ptr<ctor_default_arg_test>;
    // too less arguments for invoke
    variant var = ctor.invoke();
    CHECK(var.is_valid() == false);

    // too much arguments
#if RTTR_COMPILER == RTTR_COMPILER_MSVC && RTTR_COMP_VER <= 1800
    int value1 = 23;
    bool value2 = true;
    std::string value3 = ("default text");
    var = ctor.invoke_variadic({value1, value2, value3, 43.0});
#else
    var = ctor.invoke_variadic({23, true, std::string("default text"), 43.0});
#endif
    CHECK(var.is_valid() == false);

    // using 2 default arguments
#if RTTR_COMPILER == RTTR_COMPILER_MSVC && RTTR_COMP_VER <= 1800
    var = ctor.invoke_variadic({value1});
#else
    var = ctor.invoke_variadic({23});
#endif
    REQUIRE(var.is_type<create_type>() == true);
    CHECK(var.get_value<create_type>().get() != nullptr);

    // using 1 default arguments
#if RTTR_COMPILER == RTTR_COMPILER_MSVC && RTTR_COMP_VER <= 1800
    var = ctor.invoke_variadic({value1, value2});
#else
    var = ctor.invoke_variadic({23, true});
#endif
    REQUIRE(var.is_type<create_type>() == true);
    CHECK(var.get_value<create_type>().get() != nullptr);

    // using NO default arguments
#if RTTR_COMPILER == RTTR_COMPILER_MSVC && RTTR_COMP_VER <= 1800
    var = ctor.invoke_variadic({value1, value2, value3});
#else
    var = ctor.invoke_variadic({23, true, std::string("default text")});
#endif
    REQUIRE(var.is_type<create_type>() == true);
    CHECK(var.get_value<create_type>().get() != nullptr);
}

////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("constructor - default argument via type (real ctor)", "[constructor]")
{
    type t = type::get<ctor_default_arg_test>();
    REQUIRE(t.is_valid() == true);

    CHECK(t.create().is_valid()     == true);
    CHECK(t.create({23}).is_valid() == true); // this will not invoke the same ctor like above, the first registered
    CHECK(t.create({23, false}).is_valid() == true);
    CHECK(t.create({23, false, std::string("text")}).is_valid() == true);
    // too much arguments
    CHECK(t.create({23, false, std::string("text"), 42}).is_valid() == false);
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("constructor - invoke ctor with defaults valid", "[constructor]")
{
    auto range = type::get<ctor_default_arg_invoke_test>().get_constructors();
    std::vector<constructor> ctor_list(range.cbegin(), range.cend());
    REQUIRE(ctor_list.size() == 14);

    CHECK(ctor_list[0].invoke(1).is_valid() == true);
    CHECK(ctor_list[1].invoke(1, 2).is_valid() == true);
    CHECK(ctor_list[2].invoke(1, 2, 3).is_valid() == true);
    CHECK(ctor_list[3].invoke(1, 2, 3, 4).is_valid() == true);
    CHECK(ctor_list[4].invoke(1, 2, 3, 4, 5).is_valid() == true);
    CHECK(ctor_list[5].invoke(1, 2, 3, 4, 5, 6).is_valid() == true);
    CHECK(ctor_list[6].invoke_variadic({1, 2, 3, 4, 5, 6, 7}).is_valid() == true);
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("constructor - invoke ctor func with defaults valid", "[constructor]")
{
    auto range = type::get<ctor_default_arg_invoke_test>().get_constructors();
    std::vector<constructor> ctor_list(range.cbegin(), range.cend());
    REQUIRE(ctor_list.size() == 14);

    CHECK(ctor_list[7].invoke(1).is_valid() == true);
    CHECK(ctor_list[8].invoke(1, 2).is_valid() == true);
    CHECK(ctor_list[9].invoke(1, 2, 3).is_valid() == true);
    CHECK(ctor_list[10].invoke(1, 2, 3, 4).is_valid() == true);
    CHECK(ctor_list[11].invoke(1, 2, 3, 4, 5).is_valid() == true);
    CHECK(ctor_list[12].invoke(1, 2, 3, 4, 5, 6).is_valid() == true);
    CHECK(ctor_list[13].invoke_variadic({1, 2, 3, 4, 5, 6, 7}).is_valid() == true);
}

////////////////////////////////////////////////////////////////////////////////////////
