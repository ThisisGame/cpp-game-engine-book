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

struct ctor_misc_test
{
    ctor_misc_test(){}

    ctor_misc_test(int value){}

    static ctor_misc_test create_object(int value) { return ctor_misc_test(value); }

    int m_value;
};

struct not_copyable_ctor
{
    not_copyable_ctor(){}

    private:
        not_copyable_ctor(const not_copyable_ctor&);
};

enum class E_MetaData
{
    SCRIPTABLE  = 0,
    TOOL_TIP    = 1,
    DESCRIPTION = 2
};

RTTR_REGISTRATION
{
   registration::class_<ctor_misc_test>("ctor_misc_test")
        (
            metadata(E_MetaData::SCRIPTABLE, true),
            metadata(E_MetaData::TOOL_TIP, "This is a type ToolTip."),
            policy::ctor::as_raw_ptr
        )
        .constructor<>()
        (
            metadata(E_MetaData::SCRIPTABLE, true),
            metadata(E_MetaData::TOOL_TIP, "This is a ToolTip."),
            policy::ctor::as_raw_ptr
        )
        .constructor<>()
        (
            policy::ctor::as_std_shared_ptr
        )
        .constructor<>()
        (
            policy::ctor::as_object
        )
        .constructor(&ctor_misc_test::create_object)
        (
            metadata(E_MetaData::SCRIPTABLE, false),
            metadata(E_MetaData::TOOL_TIP, "This is another ToolTip.")
        )
        .constructor<int>()
        (
            policy::ctor::as_object,
            default_arguments(23),
            metadata(E_MetaData::SCRIPTABLE, true),
            parameter_names("value")
        )
        .constructor(&ctor_misc_test::create_object)
        (
            default_arguments(23),
            metadata(E_MetaData::SCRIPTABLE, true)
        )
        ;

   registration::class_<not_copyable_ctor>("not_copyable_ctor")
        .constructor<>();


   registration::class_<ctor_misc_test>("ctor_misc_test")
       .constructor<int>()
       (
           policy::ctor::as_object,
           default_arguments(23),
           metadata(E_MetaData::SCRIPTABLE, true)
           );

}

////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("constructor - default ctor binding type", "[constructor]")
{
    variant var = type::get<not_copyable_ctor>().create();

    CHECK(var.get_type() == type::get<std::shared_ptr<not_copyable_ctor>>());
}

////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("constructor - get_instantiated_type", "[constructor]")
{
    auto range = type::get<ctor_misc_test>().get_constructors();
    std::vector<constructor> ctor_list(range.cbegin(), range.cend());
    REQUIRE(ctor_list.size() >= 4);

    CHECK(ctor_list[0].get_instantiated_type() == type::get<ctor_misc_test*>());
    CHECK(ctor_list[1].get_instantiated_type() == type::get<std::shared_ptr<ctor_misc_test>>());
    CHECK(ctor_list[2].get_instantiated_type() == type::get<ctor_misc_test>());
    CHECK(ctor_list[3].get_instantiated_type() == type::get<ctor_misc_test>());
    //negative test
    CHECK(type::get_by_name("").get_constructor().get_instantiated_type().is_valid() == false);
}

////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("constructor - get_signature", "[constructor]")
{
    auto range = type::get<ctor_misc_test>().get_constructors();
    std::vector<constructor> ctor_list(range.cbegin(), range.cend());
    REQUIRE(ctor_list.size() >= 4);

    CHECK(ctor_list[0].get_signature() == "ctor_misc_test( )");
    CHECK(ctor_list[3].get_signature() == "ctor_misc_test( int )");
    CHECK(ctor_list[4].get_signature() == "ctor_misc_test( int )");
    CHECK(ctor_list[5].get_signature() == "ctor_misc_test( int )");

    //negative test
    CHECK(type::get_by_name("").get_constructor().get_signature() == "");
}

////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("constructor - get_parameter_infos", "[constructor]")
{
    constructor ctor = type::get<ctor_misc_test>().get_constructor();
    CHECK(ctor.get_parameter_infos().empty() == true);

    auto range = type::get<ctor_misc_test>().get_constructors();
    std::vector<constructor> ctor_list(range.cbegin(), range.cend());
    REQUIRE(ctor_list.size() >= 6);
    ctor = ctor_list[4];

    REQUIRE(ctor.get_parameter_infos().size() == 1);
    auto info = *ctor.get_parameter_infos().begin();
    CHECK(info.get_type() == type::get<int>());
    CHECK(info.has_default_value() == true);
    CHECK(info.get_default_value() == 23);
}

////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("ctor - get_declaring_type", "[constructor]")
{
    auto range = type::get<ctor_misc_test>().get_constructors();
    std::vector<constructor> ctor_list(range.cbegin(), range.cend());
    REQUIRE(ctor_list.size() >= 6);

    CHECK(ctor_list[0].get_declaring_type() == type::get<ctor_misc_test>());
    CHECK(ctor_list[3].get_declaring_type() == type::get<ctor_misc_test>());
    CHECK(ctor_list[4].get_declaring_type() == type::get<ctor_misc_test>());
    CHECK(ctor_list[5].get_declaring_type() == type::get<ctor_misc_test>());

    //negative test
    CHECK(type::get_by_name("").get_constructor().get_declaring_type().is_valid() == false);
}

////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("constructor - get_metadata", "[constructor]")
{
    auto range = type::get<ctor_misc_test>().get_constructors();
    std::vector<constructor> ctor_list(range.cbegin(), range.cend());
    REQUIRE(ctor_list.size() >= 6);

    SECTION("default ctor")
    {
        constructor ctor = type::get<ctor_misc_test>().get_constructor();
        variant value = ctor.get_metadata(E_MetaData::SCRIPTABLE);
        REQUIRE(value.is_type<bool>() == true);
        CHECK(value.get_value<bool>() == true);

        value = ctor.get_metadata(E_MetaData::TOOL_TIP);
        REQUIRE(value.is_type<std::string>() == true);
        CHECK(value.get_value<std::string>() == "This is a ToolTip.");
    }

    SECTION("function as ctor")
    {
        constructor ctor = type::get<ctor_misc_test>().get_constructor({type::get<int>()});
        variant value = ctor.get_metadata(E_MetaData::SCRIPTABLE);
        REQUIRE(value.is_type<bool>() == true);
        CHECK(value.get_value<bool>() == false);

        value = ctor.get_metadata(E_MetaData::TOOL_TIP);
        REQUIRE(value.is_type<std::string>() == true);
        CHECK(value.get_value<std::string>() == "This is another ToolTip.");
    }

    SECTION("normal ctor - with default argument")
    {
        REQUIRE(ctor_list[4].get_metadata(E_MetaData::SCRIPTABLE).is_type<bool>() == true);
        CHECK(ctor_list[4].get_metadata(E_MetaData::SCRIPTABLE).get_value<bool>() == true);
    }

    SECTION("function as ctor - with default arguments")
    {
        REQUIRE(ctor_list[5].get_metadata(E_MetaData::SCRIPTABLE).is_type<bool>() == true);
        CHECK(ctor_list[5].get_metadata(E_MetaData::SCRIPTABLE).get_value<bool>() == true);
    }

    //negative test
    CHECK(type::get_by_name("").get_constructor().get_metadata("42").is_valid() == false);
}

////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("constructor - get_metadata - type", "[constructor]")
{
    variant var = type::get<ctor_misc_test>().get_metadata(E_MetaData::SCRIPTABLE);
    REQUIRE(var.is_type<bool>() == true);
    CHECK(var.get_value<bool>() == true);

    var = type::get<ctor_misc_test>().get_metadata(E_MetaData::TOOL_TIP);
    REQUIRE(var.is_type<std::string>() == true);
    CHECK(var.get_value<std::string>() == "This is a type ToolTip.");
}

////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("constructor - compare - type", "[constructor]")
{
    constructor ctor1 = type::get<ctor_misc_test>().get_constructor();
    constructor ctor2 = type::get<ctor_misc_test>().get_constructor();

    CHECK(ctor1 == ctor2);

    auto range = type::get<ctor_misc_test>().get_constructors();
    std::vector<constructor> ctor_list(range.cbegin(), range.cend());
    REQUIRE(ctor_list.size() >= 2);

    CHECK(ctor_list[0] != ctor_list[1]);
}

/////////////////////////////////////////////////////////////////////////////////////////
