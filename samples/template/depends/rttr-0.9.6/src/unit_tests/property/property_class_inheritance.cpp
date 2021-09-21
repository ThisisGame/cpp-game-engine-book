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

#include <iostream>
#include <memory>
#include <functional>

#include <catch/catch.hpp>

using namespace rttr;
using namespace std;


/////////////////////////////////////////////////////////////////////////////////////////
// test derived properties

namespace ns_property
{
struct top
{
    virtual ~top() {}
    top() : _p1(12){}
    int _p1;
    RTTR_ENABLE()
};

/////////////////////////////////////////////////////////////////////////////////////////

struct left : virtual top
{

    left() : _p2(true){}
    bool _p2;

    RTTR_ENABLE(top)
};

/////////////////////////////////////////////////////////////////////////////////////////

struct right : virtual top
{

    right() : _p3(true){}
    bool _p3;

    RTTR_ENABLE(top)
};

/////////////////////////////////////////////////////////////////////////////////////////

struct right_2
{
    virtual ~right_2() {}
    right_2() : _p4(true){}
    bool _p4;
    RTTR_ENABLE()
};

/////////////////////////////////////////////////////////////////////////////////////////

struct bottom : left, right, right_2
{
    bottom() : _p5(23.0){}

    double _p5;

    RTTR_ENABLE(left, right, right_2)
};

}

/////////////////////////////////////////////////////////////////////////////////////////

struct base_prop_not_registered
{
    base_prop_not_registered() : value(100) {}
    int value;
};

struct derived_registered_prop : base_prop_not_registered
{

};

/////////////////////////////////////////////////////////////////////////////////////////

struct base_class_with_props
{
    base_class_with_props() : value(100) {}
    int value;

    RTTR_ENABLE()
};

struct derived_class_without_registered_props : base_class_with_props
{
    RTTR_ENABLE(base_class_with_props)
};

/////////////////////////////////////////////////////////////////////////////////////////

static double g_name;

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

RTTR_REGISTRATION
{
    registration::class_<ns_property::top>("ns_property::top")
        .property("top", &ns_property::top::_p1)
        .property("top-private", &ns_property::top::_p1, registration::private_access)
        .property("top-protected", &ns_property::top::_p1, registration::protected_access)
        .property("top-static", &g_name)
        .property("top-static-protected", &g_name, registration::protected_access);

    registration::class_<ns_property::left>("ns_property::left")
        .constructor<>()
        .property("left", &ns_property::left::_p2)
        .property("left-private", &ns_property::left::_p2, registration::private_access)
        .property("left-protected", &ns_property::left::_p2, registration::protected_access)
        .property("left-static", &g_name)
        .property("left-static-protected", &g_name, registration::protected_access)
        ;

    registration::class_<ns_property::right>("ns_property::right")
        .constructor<>()
        .property("right", &ns_property::right::_p3)
        .property("right-private", &ns_property::right::_p3, registration::private_access)
        .property("right-protected", &ns_property::right::_p3, registration::protected_access)
        .property("right-static", &g_name) // double property
        .property("right-static-protected", &g_name, registration::protected_access)
        ;

    registration::class_<ns_property::right_2>("ns_property::right_2")
        .constructor<>()
        .property("right_2", &ns_property::right_2::_p4)
        .property("right_2-private", &ns_property::right_2::_p4, registration::private_access)
        .property("right_2-protected", &ns_property::right_2::_p4, registration::protected_access)
        .property("right_2-static", &g_name) // double property
        .property("right_2-static-protected", &g_name, registration::protected_access)
        ;

    registration::class_<ns_property::bottom>("ns_property::bottom")
        .constructor<>()
        .property("bottom", &ns_property::bottom::_p5)
        .property("bottom-private", &ns_property::bottom::_p5, registration::private_access)
        .property("bottom-protected", &ns_property::bottom::_p5, registration::protected_access)
        .property("bottom-static", &g_name) // double property
        .property("bottom-static-protected", &g_name, registration::protected_access)
        ;

    registration::class_<derived_registered_prop>("derived_registered_prop")
        .property("value", &derived_registered_prop::value);


    registration::class_<base_class_with_props>("base_class_with_props")
        .property("value", &base_class_with_props::value);

    registration::class_<derived_class_without_registered_props>("derived_class_without_registered_props");
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("property - get_properties()", "[property]")
{
    type t = type::get<ns_property::bottom>();
    auto range = t.get_properties();
    REQUIRE(range.size() == 10);

    std::vector<property> props(range.begin(), range.end());
    REQUIRE(props.size() == 10);

    CHECK(props[0].get_name() == "top");
    CHECK(props[1].get_name() == "top-static");
    CHECK(props[2].get_name() == "left");
    CHECK(props[3].get_name() == "left-static");
    CHECK(props[4].get_name() == "right");
    CHECK(props[5].get_name() == "right-static");
    CHECK(props[6].get_name() == "right_2");
    CHECK(props[7].get_name() == "right_2-static");
    CHECK(props[8].get_name() == "bottom");
    CHECK(props[9].get_name() == "bottom-static");
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("property - class - query", "[property]")
{
    type t = type::get<ns_property::bottom>();

    SECTION("no valid search query")
    {
        CHECK(t.get_properties(filter_item::public_access).empty()      == true);
        CHECK(t.get_properties(filter_item::non_public_access).empty()  == true);
        CHECK(t.get_properties(filter_item::instance_item).empty()      == true);
        CHECK(t.get_properties(filter_item::static_item).empty()        == true);
        CHECK(t.get_properties(filter_item::declared_only).empty()      == true);

        CHECK(t.get_properties(filter_item::public_access       | filter_item::declared_only).empty()  == true);
        CHECK(t.get_properties(filter_item::non_public_access   | filter_item::declared_only).empty()  == true);

        CHECK(t.get_properties(filter_item::instance_item   | filter_item::declared_only).empty()  == true);
        CHECK(t.get_properties(filter_item::static_item     | filter_item::declared_only).empty()  == true);
    }

    SECTION("instance_item | public_access")
    {
        auto range = t.get_properties(filter_item::instance_item | filter_item::public_access);
        REQUIRE(range.size() == 5);

        std::vector<property> props(range.begin(), range.end());
        REQUIRE(props.size() == 5);

        CHECK(props[0].get_name() == "top");
        CHECK(props[1].get_name() == "left");
        CHECK(props[2].get_name() == "right");
        CHECK(props[3].get_name() == "right_2");
        CHECK(props[4].get_name() == "bottom");
    }

    SECTION("instance_item | non_public_access")
    {
        auto range = t.get_properties(filter_item::instance_item | filter_item::non_public_access);
        REQUIRE(range.size() == 10);

        std::vector<property> props(range.begin(), range.end());
        REQUIRE(props.size() == 10);

        CHECK(props[0].get_name() == "top-private");
        CHECK(props[1].get_name() == "top-protected");

        CHECK(props[2].get_name() == "left-private");
        CHECK(props[3].get_name() == "left-protected");

        CHECK(props[4].get_name() == "right-private");
        CHECK(props[5].get_name() == "right-protected");

        CHECK(props[6].get_name() == "right_2-private");
        CHECK(props[7].get_name() == "right_2-protected");

        CHECK(props[8].get_name() == "bottom-private");
        CHECK(props[9].get_name() == "bottom-protected");
    }

    SECTION("static_item | public_access")
    {
        auto range = t.get_properties(filter_item::static_item | filter_item::public_access);
        REQUIRE(range.size() == 5);

        std::vector<property> props(range.begin(), range.end());
        REQUIRE(props.size() == 5);

        CHECK(props[0].get_name() == "top-static");
        CHECK(props[1].get_name() == "left-static");
        CHECK(props[2].get_name() == "right-static");
        CHECK(props[3].get_name() == "right_2-static");
        CHECK(props[4].get_name() == "bottom-static");
    }

     SECTION("static_item | non_public_access")
    {
        auto range = t.get_properties(filter_item::static_item | filter_item::non_public_access);
        REQUIRE(range.size() == 5);

        std::vector<property> props(range.begin(), range.end());
        REQUIRE(props.size() == 5);

        CHECK(props[0].get_name() == "top-static-protected");
        CHECK(props[1].get_name() == "left-static-protected");
        CHECK(props[2].get_name() == "right-static-protected");
        CHECK(props[3].get_name() == "right_2-static-protected");
        CHECK(props[4].get_name() == "bottom-static-protected");
    }

    SECTION("instance_item | static_item | public_access")
    {
        auto range = t.get_properties(filter_item::instance_item | filter_item::static_item |
                                      filter_item::public_access);
        REQUIRE(range.size() == 10);

        std::vector<property> props(range.begin(), range.end());
        REQUIRE(props.size() == 10);

        CHECK(props[0].get_name() == "top");
        CHECK(props[1].get_name() == "top-static");

        CHECK(props[2].get_name() == "left");
        CHECK(props[3].get_name() == "left-static");

        CHECK(props[4].get_name() == "right");
        CHECK(props[5].get_name() == "right-static");

        CHECK(props[6].get_name() == "right_2");
        CHECK(props[7].get_name() == "right_2-static");

        CHECK(props[8].get_name() == "bottom");
        CHECK(props[9].get_name() == "bottom-static");
    }

    SECTION("instance_item | static_item | non_public_access")
    {
        auto range = t.get_properties(filter_item::instance_item | filter_item::static_item |
                                      filter_item::non_public_access);
        REQUIRE(range.size() == 15);

        std::vector<property> props(range.begin(), range.end());
        REQUIRE(props.size() == 15);

        CHECK(props[0].get_name() == "top-private");
        CHECK(props[1].get_name() == "top-protected");
        CHECK(props[2].get_name() == "top-static-protected");

        CHECK(props[3].get_name() == "left-private");
        CHECK(props[4].get_name() == "left-protected");
        CHECK(props[5].get_name() == "left-static-protected");

        CHECK(props[6].get_name() == "right-private");
        CHECK(props[7].get_name() == "right-protected");
        CHECK(props[8].get_name() == "right-static-protected");

        CHECK(props[9].get_name()  == "right_2-private");
        CHECK(props[10].get_name() == "right_2-protected");
        CHECK(props[11].get_name() == "right_2-static-protected");

        CHECK(props[12].get_name() == "bottom-private");
        CHECK(props[13].get_name() == "bottom-protected");
        CHECK(props[14].get_name() == "bottom-static-protected");
    }

    SECTION("instance_item | static_item | public_access | non_public_access")
    {
        auto range = t.get_properties(filter_item::instance_item | filter_item::static_item |
                                      filter_item::public_access | filter_item::non_public_access);
        REQUIRE(range.size() == 25);

        std::vector<property> props(range.begin(), range.end());
        REQUIRE(props.size() == 25);

        CHECK(props[0].get_name() == "top");
        CHECK(props[1].get_name() == "top-private");
        CHECK(props[2].get_name() == "top-protected");
        CHECK(props[3].get_name() == "top-static");
        CHECK(props[4].get_name() == "top-static-protected");

        CHECK(props[5].get_name() == "left");
        CHECK(props[6].get_name() == "left-private");
        CHECK(props[7].get_name() == "left-protected");
        CHECK(props[8].get_name() == "left-static");
        CHECK(props[9].get_name() == "left-static-protected");

        CHECK(props[10].get_name() == "right");
        CHECK(props[11].get_name() == "right-private");
        CHECK(props[12].get_name() == "right-protected");
        CHECK(props[13].get_name() == "right-static");
        CHECK(props[14].get_name() == "right-static-protected");

        CHECK(props[15].get_name() == "right_2");
        CHECK(props[16].get_name() == "right_2-private");
        CHECK(props[17].get_name() == "right_2-protected");
        CHECK(props[18].get_name() == "right_2-static");
        CHECK(props[19].get_name() == "right_2-static-protected");

        CHECK(props[20].get_name() == "bottom");
        CHECK(props[21].get_name() == "bottom-private");
        CHECK(props[22].get_name() == "bottom-protected");
        CHECK(props[23].get_name() == "bottom-static");
        CHECK(props[24].get_name() == "bottom-static-protected");
    }

    SECTION("instance_item | public_access | filter_item::declared_only")
    {
        auto range = t.get_properties(filter_item::instance_item | filter_item::public_access | filter_item::declared_only);
        REQUIRE(range.size() == 1);

        std::vector<property> props(range.begin(), range.end());
        REQUIRE(props.size() == 1);

        CHECK(props[0].get_name() == "bottom");
    }

    SECTION("static_item | public_access | filter_item::declared_only")
    {
        auto range = t.get_properties(filter_item::static_item | filter_item::public_access | filter_item::declared_only);
        REQUIRE(range.size() == 1);

        std::vector<property> props(range.begin(), range.end());
        REQUIRE(props.size() == 1);

        CHECK(props[0].get_name() == "bottom-static");
    }

    SECTION("instance_item | non_public_access | filter_item::declared_only")
    {
        auto range = t.get_properties(filter_item::instance_item | filter_item::non_public_access | filter_item::declared_only);
        REQUIRE(range.size() == 2);

        std::vector<property> props(range.begin(), range.end());
        REQUIRE(props.size() == 2);

        CHECK(props[0].get_name() == "bottom-private");
        CHECK(props[1].get_name() == "bottom-protected");
    }

    SECTION("static_item | non_public_access | filter_item::declared_only")
    {
        auto range = t.get_properties(filter_item::static_item | filter_item::non_public_access | filter_item::declared_only);
        REQUIRE(range.size() == 1);

        std::vector<property> props(range.begin(), range.end());
        REQUIRE(props.size() == 1);

        CHECK(props[0].get_name() == "bottom-static-protected");
    }

    SECTION("instance_item | static_item | public_access | declared_only")
    {
        auto range = t.get_properties(filter_item::instance_item | filter_item::static_item |
                                      filter_item::public_access | filter_item::declared_only);
        REQUIRE(range.size() == 2);

        std::vector<property> props(range.begin(), range.end());
        REQUIRE(props.size() == 2);

        CHECK(props[0].get_name() == "bottom");
        CHECK(props[1].get_name() == "bottom-static");
    }

    SECTION("instance_item | static_item | non_public_access | declared_only")
    {
        auto range = t.get_properties(filter_item::instance_item | filter_item::static_item |
                                      filter_item::non_public_access | filter_item::declared_only);
        REQUIRE(range.size() == 3);

        std::vector<property> props(range.begin(), range.end());
        REQUIRE(props.size() == 3);

        CHECK(props[0].get_name() == "bottom-private");
        CHECK(props[1].get_name() == "bottom-protected");
        CHECK(props[2].get_name() == "bottom-static-protected");
    }

    SECTION("instance_item | static_item | public_access | non_public_access | declared_only")
    {
        auto range = t.get_properties(filter_item::instance_item | filter_item::static_item |
                                      filter_item::public_access | filter_item::non_public_access |
                                      filter_item::declared_only);
        REQUIRE(range.size() == 5);

        std::vector<property> props(range.begin(), range.end());
        REQUIRE(props.size() == 5);

        CHECK(props[0].get_name() == "bottom");
        CHECK(props[1].get_name() == "bottom-private");
        CHECK(props[2].get_name() == "bottom-protected");
        CHECK(props[3].get_name() == "bottom-static");
        CHECK(props[4].get_name() == "bottom-static-protected");
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("property - class - inheritance - invoke", "[property]")
{
    type t = type::get<ns_property::bottom>();


    ns_property::bottom instance;
    ns_property::top& top = instance;
    // try access from top instance a property in the most derived class (bottom)
    property base_prop = t.get_property("top");

    variant ret = base_prop.get_value(top);
    REQUIRE(ret.is_type<int>() == true);
    CHECK(ret.get_value<int>() == 12);
    // try to change the value
    base_prop.set_value(top, 2000);
    CHECK(instance._p1 == 2000);

    // and now the other way around, from bottom a top property
    property bottom_prop = t.get_property("bottom");
    ret = bottom_prop.get_value(instance);
    REQUIRE(ret.is_type<double>() == true);
    CHECK(ret.get_value<double>() == 23.0);
    // try to change the value
    bottom_prop.set_value(top, 42.0);
    CHECK(instance._p5 == 42.0);
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("property - base class not registered", "[property]")
{
    type t_prop = type::get<derived_registered_prop>();
    property prop = t_prop.get_property("value");
    derived_registered_prop obj;

    auto ret = prop.set_value(obj, 23);

    CHECK(ret == true);
    CHECK(obj.value == 23);

    auto base_type = type::get<base_prop_not_registered>();

    CHECK(t_prop.is_derived_from(base_type) == true);

    auto range = base_type.get_derived_classes();

    REQUIRE(range.size() == 1);
    CHECK(*range.begin() == t_prop);
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("property - check inheritance of probs", "[property]")
{
    // base class has registered properties, the derived class not
    type t_prop = type::get<derived_class_without_registered_props>();
    auto prop_range = t_prop.get_properties();
    REQUIRE(prop_range.size() == 1);

    CHECK((*prop_range.begin()).get_name() == "value");
}

/////////////////////////////////////////////////////////////////////////////////////////
