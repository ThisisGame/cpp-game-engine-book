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
using namespace std;

struct constructor_query_test
{
    constructor_query_test() {  }
    constructor_query_test(int) {  }
    constructor_query_test(std::string) {  }
    constructor_query_test(const constructor_query_test&){}

    static constructor_query_test create_object(int v) { return constructor_query_test(v); }
};

/////////////////////////////////////////////////////////////////////////////////////////

RTTR_REGISTRATION
{
    registration::class_<constructor_query_test>("constructor_query_test")
        .constructor<>()( policy::ctor::as_object )
        .constructor<const constructor_query_test&>() ( policy::ctor::as_object )
        .constructor<int>(registration::private_access) ( policy::ctor::as_object )
        .constructor<std::string>(registration::protected_access) ( policy::ctor::as_object )
        .constructor(&constructor_query_test::create_object, registration::protected_access)
        ;
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("constructor - get_constructors()", "[constructor]")
{
    type t = type::get<constructor_query_test>();
    auto range = t.get_constructors();
    REQUIRE(range.size() == 2);

    std::vector<constructor> ctors(range.begin(), range.end());
    REQUIRE(ctors.size() == 2);

    CHECK(ctors[0].get_signature() == "constructor_query_test( )");
    CHECK(ctors[1].get_signature() == "constructor_query_test( constructor_query_test const & )");
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("constructor - get_constructors(filter_items)", "[constructor]")
{
    type t = type::get<constructor_query_test>();

    SECTION("no valid search query")
    {
        CHECK(t.get_constructors(filter_item::public_access).empty()      == true);
        CHECK(t.get_constructors(filter_item::non_public_access).empty()  == true);
        CHECK(t.get_constructors(filter_item::instance_item).empty()      == true);
        CHECK(t.get_constructors(filter_item::static_item).empty()        == true);
        CHECK(t.get_constructors(filter_item::declared_only).empty()      == true);

        CHECK(t.get_constructors(filter_item::public_access       | filter_item::declared_only).empty()  == true);
        CHECK(t.get_constructors(filter_item::non_public_access   | filter_item::declared_only).empty()  == true);

        CHECK(t.get_constructors(filter_item::instance_item   | filter_item::declared_only).empty()  == true);
        CHECK(t.get_constructors(filter_item::static_item     | filter_item::declared_only).empty()  == true);
    }

    SECTION("instance_item | public_access")
    {
        auto range = t.get_constructors(filter_item::instance_item | filter_item::public_access);
        REQUIRE(range.size() == 2);

        std::vector<constructor> ctors(range.begin(), range.end());
        REQUIRE(ctors.size() == 2);

        CHECK(ctors[0].get_signature() == "constructor_query_test( )");
        CHECK(ctors[1].get_signature() == "constructor_query_test( constructor_query_test const & )");
    }

    SECTION("instance_item | non_public_access")
    {
        auto range = t.get_constructors(filter_item::instance_item | filter_item::non_public_access);
        REQUIRE(range.size() == 3);

        std::vector<constructor> ctors(range.begin(), range.end());
        REQUIRE(ctors.size() == 3);

        CHECK(ctors[0].get_signature() == "constructor_query_test( int )");
        CHECK(ctors[1].get_signature() == "constructor_query_test( std::string )");

        CHECK(ctors[2].get_signature() == "constructor_query_test( int )");
    }

    SECTION("static_item | public_access")
    {
        auto range = t.get_constructors(filter_item::static_item | filter_item::public_access);
        REQUIRE(range.size() == 2);

        std::vector<constructor> ctors(range.begin(), range.end());
        REQUIRE(ctors.size() == 2);

        CHECK(ctors[0].get_signature() == "constructor_query_test( )");
        CHECK(ctors[1].get_signature() == "constructor_query_test( constructor_query_test const & )");
    }

     SECTION("static_item | non_public_access")
    {
        auto range = t.get_constructors(filter_item::static_item | filter_item::non_public_access);
        REQUIRE(range.size() == 3);

        std::vector<constructor> ctors(range.begin(), range.end());
        REQUIRE(ctors.size() == 3);

        CHECK(ctors[0].get_signature() == "constructor_query_test( int )");
        CHECK(ctors[1].get_signature() == "constructor_query_test( std::string )");

        CHECK(ctors[2].get_signature() == "constructor_query_test( int )");
    }

    SECTION("instance_item | static_item | public_access")
    {
        auto range = t.get_constructors(filter_item::instance_item | filter_item::static_item |
                                        filter_item::public_access);
        REQUIRE(range.size() == 2);

        std::vector<constructor> ctors(range.begin(), range.end());
        REQUIRE(ctors.size() == 2);

        CHECK(ctors[0].get_signature() == "constructor_query_test( )");
        CHECK(ctors[1].get_signature() == "constructor_query_test( constructor_query_test const & )");
    }

    SECTION("instance_item | static_item | non_public_access")
    {
        auto range = t.get_constructors(filter_item::instance_item | filter_item::static_item |
                                   filter_item::non_public_access);
        REQUIRE(range.size() == 3);

        std::vector<constructor> ctors(range.begin(), range.end());
        REQUIRE(ctors.size() == 3);

        CHECK(ctors[0].get_signature() == "constructor_query_test( int )");
        CHECK(ctors[1].get_signature() == "constructor_query_test( std::string )");

        CHECK(ctors[2].get_signature() == "constructor_query_test( int )");
    }

    SECTION("instance_item | static_item | public_access | non_public_access")
    {
        auto range = t.get_constructors(filter_item::instance_item | filter_item::static_item |
                                        filter_item::public_access | filter_item::non_public_access);
        REQUIRE(range.size() == 5);

        std::vector<constructor> ctors(range.begin(), range.end());
        REQUIRE(ctors.size() == 5);

        CHECK(ctors[0].get_signature() == "constructor_query_test( )");
        CHECK(ctors[1].get_signature() == "constructor_query_test( constructor_query_test const & )");
        CHECK(ctors[2].get_signature() == "constructor_query_test( int )");
        CHECK(ctors[3].get_signature() == "constructor_query_test( std::string )");
        CHECK(ctors[4].get_signature() == "constructor_query_test( int )");
    }

    SECTION("instance_item | public_access | declared_only")
    {
        auto range = t.get_constructors(filter_item::instance_item | filter_item::public_access |
                                        filter_item::declared_only);
        REQUIRE(range.size() == 2);

        std::vector<constructor> ctors(range.begin(), range.end());
        REQUIRE(ctors.size() == 2);

        CHECK(ctors[0].get_signature() == "constructor_query_test( )");
        CHECK(ctors[1].get_signature() == "constructor_query_test( constructor_query_test const & )");
    }

    SECTION("static_item | public_access | declared_only")
    {
        auto range = t.get_constructors(filter_item::static_item | filter_item::public_access |
                                        filter_item::declared_only);
        REQUIRE(range.size() == 2);

        std::vector<constructor> ctors(range.begin(), range.end());
        REQUIRE(ctors.size() == 2);

        CHECK(ctors[0].get_signature() == "constructor_query_test( )");
        CHECK(ctors[1].get_signature() == "constructor_query_test( constructor_query_test const & )");
    }

    SECTION("instance_item | non_public_access | declared_only")
    {
        auto range = t.get_constructors(filter_item::instance_item | filter_item::non_public_access |
                                        filter_item::declared_only);
        REQUIRE(range.size() == 3);

        std::vector<constructor> ctors(range.begin(), range.end());
        REQUIRE(ctors.size() == 3);

        CHECK(ctors[0].get_signature() == "constructor_query_test( int )");
        CHECK(ctors[1].get_signature() == "constructor_query_test( std::string )");

        CHECK(ctors[2].get_signature() == "constructor_query_test( int )");
    }

    SECTION("static_item | non_public_access | declared_only")
    {
        auto range = t.get_constructors(filter_item::static_item | filter_item::non_public_access |
                                        filter_item::declared_only);
        REQUIRE(range.size() == 3);

        std::vector<constructor> ctors(range.begin(), range.end());
        REQUIRE(ctors.size() == 3);

        CHECK(ctors[0].get_signature() == "constructor_query_test( int )");
        CHECK(ctors[1].get_signature() == "constructor_query_test( std::string )");

        CHECK(ctors[2].get_signature() == "constructor_query_test( int )");
    }

    SECTION("instance_item | static_item | public_access | declared_only")
    {
        auto range = t.get_constructors(filter_item::instance_item | filter_item::static_item |
                                        filter_item::public_access | filter_item::declared_only);
        REQUIRE(range.size() == 2);

        std::vector<constructor> ctors(range.begin(), range.end());
        REQUIRE(ctors.size() == 2);

        CHECK(ctors[0].get_signature() == "constructor_query_test( )");
        CHECK(ctors[1].get_signature() == "constructor_query_test( constructor_query_test const & )");
    }

    SECTION("instance_item | static_item | non_public_access | declared_only")
    {
        auto range = t.get_constructors(filter_item::instance_item | filter_item::static_item |
                                        filter_item::non_public_access | filter_item::declared_only);
        REQUIRE(range.size() == 3);

        std::vector<constructor> ctors(range.begin(), range.end());
        REQUIRE(ctors.size() == 3);

        CHECK(ctors[0].get_signature() == "constructor_query_test( int )");
        CHECK(ctors[1].get_signature() == "constructor_query_test( std::string )");

        CHECK(ctors[2].get_signature() == "constructor_query_test( int )");
    }

    SECTION("instance_item | static_item | public_access | non_public_access | declared_only")
    {
        auto range = t.get_constructors(filter_item::instance_item | filter_item::static_item |
                                        filter_item::public_access | filter_item::non_public_access |
                                        filter_item::declared_only);
        REQUIRE(range.size() == 5);

        std::vector<constructor> ctors(range.begin(), range.end());
        REQUIRE(ctors.size() == 5);

        CHECK(ctors[0].get_signature() == "constructor_query_test( )");
        CHECK(ctors[1].get_signature() == "constructor_query_test( constructor_query_test const & )");
        CHECK(ctors[2].get_signature() == "constructor_query_test( int )");
        CHECK(ctors[3].get_signature() == "constructor_query_test( std::string )");
        CHECK(ctors[4].get_signature() == "constructor_query_test( int )");
    }
}

/////////////////////////////////////////////////////////////////////////////////////////
