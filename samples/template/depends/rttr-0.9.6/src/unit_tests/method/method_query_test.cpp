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



struct method_query_test_base
{
    method_query_test_base() {  }

    void func_1() {}
    static void func_2() {}

    RTTR_ENABLE()

};

struct method_query_test_derived : method_query_test_base
{
    method_query_test_derived() {  }

    void func_3() {}
    static void func_4() {}

    RTTR_ENABLE(method_query_test_base)
};



/////////////////////////////////////////////////////////////////////////////////////////

RTTR_REGISTRATION
{
    registration::class_<method_query_test_base>("method_query_test_base")
        .method("base", &method_query_test_base::func_1)
        .method("base-private", &method_query_test_base::func_1, registration::private_access)
        .method("base-protected", &method_query_test_base::func_1, registration::protected_access)
        .method("base-static", &method_query_test_base::func_2)
        .method("base-static-protected", &method_query_test_base::func_2, registration::protected_access)
        ;

    registration::class_<method_query_test_derived>("method_query_test_derived")
        .method("derived", &method_query_test_derived::func_3)
        .method("derived-private", &method_query_test_derived::func_3, registration::private_access)
        .method("derived-protected", &method_query_test_derived::func_3, registration::protected_access)
        .method("derived-static", &method_query_test_derived::func_4)
        .method("derived-static-protected", &method_query_test_derived::func_4, registration::protected_access)
        ;
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("method - get_methods()", "[method]")
{
    type t = type::get<method_query_test_derived>();
    auto range = t.get_methods();
    REQUIRE(range.size() == 4);

    std::vector<method> meths(range.begin(), range.end());
    REQUIRE(meths.size() == 4);

    CHECK(meths[0].get_name() == "base");
    CHECK(meths[1].get_name() == "base-static");
    CHECK(meths[2].get_name() == "derived");
    CHECK(meths[3].get_name() == "derived-static");
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("method - get_methods(filter_items)", "[method]")
{
    type t = type::get<method_query_test_derived>();

    SECTION("no valid search query")
    {
        CHECK(t.get_methods(filter_item::public_access).empty()      == true);
        CHECK(t.get_methods(filter_item::non_public_access).empty()  == true);
        CHECK(t.get_methods(filter_item::instance_item).empty()      == true);
        CHECK(t.get_methods(filter_item::static_item).empty()        == true);
        CHECK(t.get_methods(filter_item::declared_only).empty()      == true);

        CHECK(t.get_methods(filter_item::public_access       | filter_item::declared_only).empty()  == true);
        CHECK(t.get_methods(filter_item::non_public_access   | filter_item::declared_only).empty()  == true);

        CHECK(t.get_methods(filter_item::instance_item   | filter_item::declared_only).empty()  == true);
        CHECK(t.get_methods(filter_item::static_item     | filter_item::declared_only).empty()  == true);
    }

    SECTION("instance_item | public_access")
    {
        auto range = t.get_methods(filter_item::instance_item | filter_item::public_access);
        REQUIRE(range.size() == 2);

        std::vector<method> meths(range.begin(), range.end());
        REQUIRE(meths.size() == 2);

        CHECK(meths[0].get_name() == "base");
        CHECK(meths[1].get_name() == "derived");
    }

    SECTION("instance_item | non_public_access")
    {
        auto range = t.get_methods(filter_item::instance_item | filter_item::non_public_access);
        REQUIRE(range.size() == 4);

        std::vector<method> meths(range.begin(), range.end());
        REQUIRE(meths.size() == 4);

        CHECK(meths[0].get_name() == "base-private");
        CHECK(meths[1].get_name() == "base-protected");

        CHECK(meths[2].get_name() == "derived-private");
        CHECK(meths[3].get_name() == "derived-protected");
    }

    SECTION("static_item | public_access")
    {
        auto range = t.get_methods(filter_item::static_item | filter_item::public_access);
        REQUIRE(range.size() == 2);

        std::vector<method> meths(range.begin(), range.end());
        REQUIRE(meths.size() == 2);

        CHECK(meths[0].get_name() == "base-static");
        CHECK(meths[1].get_name() == "derived-static");
    }

     SECTION("static_item | non_public_access")
    {
        auto range = t.get_methods(filter_item::static_item | filter_item::non_public_access);
        REQUIRE(range.size() == 2);

        std::vector<method> meths(range.begin(), range.end());
        REQUIRE(meths.size() == 2);

        CHECK(meths[0].get_name() == "base-static-protected");
        CHECK(meths[1].get_name() == "derived-static-protected");
    }

    SECTION("instance_item | static_item | public_access")
    {
        auto range = t.get_methods(filter_item::instance_item | filter_item::static_item |
                                   filter_item::public_access);
        REQUIRE(range.size() == 4);

        std::vector<method> meths(range.begin(), range.end());
        REQUIRE(meths.size() == 4);

        CHECK(meths[0].get_name() == "base");
        CHECK(meths[1].get_name() == "base-static");
        CHECK(meths[2].get_name() == "derived");
        CHECK(meths[3].get_name() == "derived-static");
    }

    SECTION("instance_item | static_item | non_public_access")
    {
        auto range = t.get_methods(filter_item::instance_item | filter_item::static_item |
                                   filter_item::non_public_access);
        REQUIRE(range.size() == 6);

        std::vector<method> meths(range.begin(), range.end());
        REQUIRE(meths.size() == 6);

        CHECK(meths[0].get_name() == "base-private");
        CHECK(meths[1].get_name() == "base-protected");
        CHECK(meths[2].get_name() == "base-static-protected");
        CHECK(meths[3].get_name() == "derived-private");
        CHECK(meths[4].get_name() == "derived-protected");
        CHECK(meths[5].get_name() == "derived-static-protected");
    }

    SECTION("instance_item | static_item | public_access | non_public_access")
    {
        auto range = t.get_methods(filter_item::instance_item | filter_item::static_item |
                                   filter_item::public_access | filter_item::non_public_access);
        REQUIRE(range.size() == 10);

        std::vector<method> meths(range.begin(), range.end());
        REQUIRE(meths.size() == 10);

        CHECK(meths[0].get_name() == "base");
        CHECK(meths[1].get_name() == "base-private");
        CHECK(meths[2].get_name() == "base-protected");
        CHECK(meths[3].get_name() == "base-static");
        CHECK(meths[4].get_name() == "base-static-protected");

        CHECK(meths[5].get_name() == "derived");
        CHECK(meths[6].get_name() == "derived-private");
        CHECK(meths[7].get_name() == "derived-protected");
        CHECK(meths[8].get_name() == "derived-static");
        CHECK(meths[9].get_name() == "derived-static-protected");
    }

    SECTION("instance_item | public_access | declared_only")
    {
        auto range = t.get_methods(filter_item::instance_item | filter_item::public_access |
                                   filter_item::declared_only);
        REQUIRE(range.size() == 1);

        std::vector<method> meths(range.begin(), range.end());
        REQUIRE(meths.size() == 1);

        CHECK(meths[0].get_name() == "derived");
    }

    SECTION("static_item | public_access | declared_only")
    {
        auto range = t.get_methods(filter_item::static_item | filter_item::public_access |
                                   filter_item::declared_only);
        REQUIRE(range.size() == 1);

        std::vector<method> meths(range.begin(), range.end());
        REQUIRE(meths.size() == 1);

        CHECK(meths[0].get_name() == "derived-static");
    }

    SECTION("instance_item | non_public_access | declared_only")
    {
        auto range = t.get_methods(filter_item::instance_item | filter_item::non_public_access |
                                   filter_item::declared_only);
        REQUIRE(range.size() == 2);

        std::vector<method> meths(range.begin(), range.end());
        REQUIRE(meths.size() == 2);

        CHECK(meths[0].get_name() == "derived-private");
        CHECK(meths[1].get_name() == "derived-protected");
    }

    SECTION("static_item | non_public_access | declared_only")
    {
        auto range = t.get_methods(filter_item::static_item | filter_item::non_public_access |
                                   filter_item::declared_only);
        REQUIRE(range.size() == 1);

        std::vector<method> meths(range.begin(), range.end());
        REQUIRE(meths.size() == 1);

        CHECK(meths[0].get_name() == "derived-static-protected");
    }

    SECTION("instance_item | static_item | public_access | declared_only")
    {
        auto range = t.get_methods(filter_item::instance_item | filter_item::static_item |
                                   filter_item::public_access | filter_item::declared_only);
        REQUIRE(range.size() == 2);

        std::vector<method> meths(range.begin(), range.end());
        REQUIRE(meths.size() == 2);

        CHECK(meths[0].get_name() == "derived");
        CHECK(meths[1].get_name() == "derived-static");
    }

    SECTION("instance_item | static_item | non_public_access | declared_only")
    {
        auto range = t.get_methods(filter_item::instance_item | filter_item::static_item |
                                   filter_item::non_public_access | filter_item::declared_only);
        REQUIRE(range.size() == 3);

        std::vector<method> meths(range.begin(), range.end());
        REQUIRE(meths.size() == 3);

        CHECK(meths[0].get_name() == "derived-private");
        CHECK(meths[1].get_name() == "derived-protected");
        CHECK(meths[2].get_name() == "derived-static-protected");
    }

    SECTION("instance_item | static_item | public_access | non_public_access | declared_only")
    {
        auto range = t.get_methods(filter_item::instance_item | filter_item::static_item |
                                   filter_item::public_access | filter_item::non_public_access |
                                   filter_item::declared_only);
        REQUIRE(range.size() == 5);

        std::vector<method> meths(range.begin(), range.end());
        REQUIRE(meths.size() == 5);

        CHECK(meths[0].get_name() == "derived");
        CHECK(meths[1].get_name() == "derived-private");
        CHECK(meths[2].get_name() == "derived-protected");
        CHECK(meths[3].get_name() == "derived-static");
        CHECK(meths[4].get_name() == "derived-static-protected");
    }
}

/////////////////////////////////////////////////////////////////////////////////////////
