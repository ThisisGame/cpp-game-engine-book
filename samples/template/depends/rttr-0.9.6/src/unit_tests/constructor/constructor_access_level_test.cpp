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
#include <rttr/registration_friend>
#include <catch/catch.hpp>

using namespace rttr;

struct ctor_access_level_test
{
    public:
        ctor_access_level_test() { }
        ctor_access_level_test(float) { }
        ctor_access_level_test(float, float) { }

        static ctor_access_level_test create_object(std::string) { return {}; }
        static ctor_access_level_test create_object_2(std::string, float) { return {}; }
        static ctor_access_level_test create_object_3(std::string, int) { return {}; }
        static ctor_access_level_test create_object_4(std::string, bool) { return {}; }

        // ctors which gets metadata
        static ctor_access_level_test create_object_5(std::size_t) { return {}; }
        static ctor_access_level_test create_object_6(std::size_t, float) { return {}; }
        static ctor_access_level_test create_object_7(std::size_t, int) { return {}; }
        static ctor_access_level_test create_object_8(std::size_t, bool) { return {}; }

    protected:
        ctor_access_level_test(int) {}
        ctor_access_level_test(int, int) {}
    private:
        ctor_access_level_test(bool) {}
        ctor_access_level_test(bool, bool) {}

    RTTR_REGISTRATION_FRIEND
};

RTTR_REGISTRATION
{
    registration::class_<ctor_access_level_test>("ctor_access_level_test")
        // native ctor
        .constructor<>() // default, should be "public_access"
        .constructor<int>(registration::protected_access)
        .constructor<bool>(registration::private_access)
        .constructor<float>(registration::public_access)
        /////////////////////////////////////////////////////////////////////////////////
        // native ctor with metadata
        .constructor<int, int>(registration::protected_access)
        (
            metadata(23, 42),
            default_arguments(0)
        )
        .constructor<bool, bool>(registration::private_access)
        (
            metadata(23, 42)
        )
        .constructor<float, float>(registration::public_access)
        (
            metadata(23, 42)
        )
        /////////////////////////////////////////////////////////////////////////////////
        // ctor via function register
        .constructor(&ctor_access_level_test::create_object) // default, should be "public_access"
        .constructor(&ctor_access_level_test::create_object_2, registration::protected_access)
        .constructor(&ctor_access_level_test::create_object_3, registration::private_access)
        .constructor(&ctor_access_level_test::create_object_4, registration::public_access)
        /////////////////////////////////////////////////////////////////////////////////
        // ctor via function register, with metadata
        .constructor(&ctor_access_level_test::create_object_5) // default, should be "public_access"
        (
            metadata(23, 42),
            default_arguments(std::size_t(0))
        )
        .constructor(&ctor_access_level_test::create_object_6, registration::protected_access)
        (
            metadata(23, 42)
        )
        .constructor(&ctor_access_level_test::create_object_7, registration::private_access)
        (
            metadata(23, 42)
        )
        .constructor(&ctor_access_level_test::create_object_8, registration::public_access)
        (
            metadata(23, 42)
        )
        ;
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("constructor - access_levels test - direct", "[constructor]")
{
    type t = type::get_by_name("ctor_access_level_test");
    REQUIRE(t.is_valid() == true);

    // has to be checked, because get_access_level() default return value is public_access
    CHECK(t.get_constructor({}).is_valid() == true);
    CHECK(t.get_constructor({}).get_access_level() == access_levels::public_access);

    CHECK(t.get_constructor({type::get<bool>()}).get_access_level()     == access_levels::private_access);
    CHECK(t.get_constructor({type::get<int>()}).get_access_level()      == access_levels::protected_access);
    CHECK(t.get_constructor({type::get<float>()}).get_access_level()    == access_levels::public_access);


    CHECK(t.get_constructor({type::get<bool>(), type::get<bool>()}).get_access_level()      == access_levels::private_access);
    CHECK(t.get_constructor({type::get<int>(), type::get<int>()}).get_access_level()        == access_levels::protected_access);
    CHECK(t.get_constructor({type::get<float>(), type::get<float>()}).get_access_level()    == access_levels::public_access);
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("constructor - access_levels test - via function", "[constructor]")
{
    type t = type::get_by_name("ctor_access_level_test");
    REQUIRE(t.is_valid() == true);

    // has to be checked, because get_access_level() default return value is public_access
    CHECK(t.get_constructor({type::get<std::string>()}).is_valid() == true);
    CHECK(t.get_constructor({type::get<std::string>()}).get_access_level() == access_levels::public_access);

    CHECK(t.get_constructor({type::get<std::string>(), type::get<float>()}).get_access_level()  == access_levels::protected_access);
    CHECK(t.get_constructor({type::get<std::string>(), type::get<int>()}).get_access_level()    == access_levels::private_access);
    CHECK(t.get_constructor({type::get<std::string>(), type::get<bool>()}).get_access_level()   == access_levels::public_access);

    // with metadata
    // has to be checked, because get_access_level() default return value is public_access
    CHECK(t.get_constructor({type::get<std::size_t>()}).is_valid() == true);
    CHECK(t.get_constructor({type::get<std::size_t>()}).get_access_level() == access_levels::public_access);

    CHECK(t.get_constructor({type::get<std::size_t>(), type::get<float>()}).get_access_level()  == access_levels::protected_access);
    CHECK(t.get_constructor({type::get<std::size_t>(), type::get<int>()}).get_access_level()    == access_levels::private_access);
    CHECK(t.get_constructor({type::get<std::size_t>(), type::get<bool>()}).get_access_level()   == access_levels::public_access);
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("constructor - access_levels test - negative", "[constructor]")
{
    constructor ctor = type::get_by_name("wrong_type").get_constructor();

    CHECK(ctor.is_valid() == false);
    CHECK(ctor.get_access_level() == access_levels::public_access);
}

/////////////////////////////////////////////////////////////////////////////////////////
