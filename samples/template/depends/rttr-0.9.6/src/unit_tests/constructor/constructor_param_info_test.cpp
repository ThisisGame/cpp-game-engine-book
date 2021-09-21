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

struct ctor_param_info_test
{
    ctor_param_info_test()
    {

    }

    ctor_param_info_test(const std::string& val_1, int val_2)
    {

    }

    ctor_param_info_test(bool val_1, int val_2, float val_3)
    {

    }


    static ctor_param_info_test create_with_default(std::string val_1, int val_2, bool val_3)
    {
        return ctor_param_info_test();
    }

    static ctor_param_info_test create_with_default_2(double val_1, int val_2, bool val_3)
    {
        return ctor_param_info_test();
    }
};

/////////////////////////////////////////////////////////////////////////////////////////

RTTR_REGISTRATION
{
    registration::class_<ctor_param_info_test>("ctor_param_info_test")
        .constructor<>() // no params
        .constructor<std::string, int>()
        .constructor<bool, int, float>()
        (
            parameter_names("val_1", "val_2", "val_3")
        )
        .constructor(&ctor_param_info_test::create_with_default)
        (
            default_arguments(42, true)
        )
        .constructor(&ctor_param_info_test::create_with_default_2)
        (
            default_arguments(23, true),
            parameter_names("val_1", "val_2", "val_3")
        );
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("constructor - parameter info - no info", "[constructor]")
{
    constructor ctor = type::get<ctor_param_info_test>().get_constructor();
    REQUIRE(ctor.is_valid() == true);

    auto infos = ctor.get_parameter_infos();
    REQUIRE(infos.size() == 0);

    //negative test
    CHECK(type::get_by_name("").get_constructor().get_parameter_infos().size() == 0);
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("constructor - parameter info - no names provided", "[constructor]")
{
    constructor ctor = type::get<ctor_param_info_test>().get_constructor({type::get<std::string>(), type::get<int>()});
    REQUIRE(ctor.is_valid() == true);

    std::vector<parameter_info> infos(ctor.get_parameter_infos().begin(), ctor.get_parameter_infos().end());
    REQUIRE(infos.size() == 2);

    CHECK(infos[0].get_name()           == std::string());
    CHECK(infos[0].has_default_value()  == false);
    CHECK(infos[0].get_type()           == type::get<std::string>());
    CHECK(infos[0].get_index()          == 0);
    CHECK(infos[0].get_default_value()  == variant());

    CHECK(infos[1].get_name()           == std::string());
    CHECK(infos[1].has_default_value()  == false);
    CHECK(infos[1].get_type()           == type::get<int>());
    CHECK(infos[1].get_index()          == 1);
    CHECK(infos[1].get_default_value()  == variant());
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("constructor - parameter info - names provided", "[constructor]")
{
    constructor ctor = type::get<ctor_param_info_test>().get_constructor({type::get<bool>(), type::get<int>(), type::get<float>()});
    REQUIRE(ctor.is_valid() == true);

    std::vector<parameter_info> infos(ctor.get_parameter_infos().begin(), ctor.get_parameter_infos().end());
    REQUIRE(infos.size() == 3);

    CHECK(infos[0].get_name()           == std::string("val_1"));
    CHECK(infos[0].has_default_value()  == false);
    CHECK(infos[0].get_type()           == type::get<bool>());
    CHECK(infos[0].get_index()          == 0);
    CHECK(infos[0].get_default_value()  == variant());

    CHECK(infos[1].get_name()           == std::string("val_2"));
    CHECK(infos[1].has_default_value()  == false);
    CHECK(infos[1].get_type()           == type::get<int>());
    CHECK(infos[1].get_index()          == 1);
    CHECK(infos[1].get_default_value()  == variant());

    CHECK(infos[2].get_name()           == std::string("val_3"));
    CHECK(infos[2].has_default_value()  == false);
    CHECK(infos[2].get_type()           == type::get<float>());
    CHECK(infos[2].get_index()          == 2);
    CHECK(infos[2].get_default_value()  == variant());
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("constructor - parameter info - no names provided & default values", "[constructor]")
{
    constructor ctor = type::get<ctor_param_info_test>().get_constructor({type::get<std::string>(), type::get<int>(), type::get<bool>()});
    REQUIRE(ctor.is_valid() == true);

    std::vector<parameter_info> infos(ctor.get_parameter_infos().begin(), ctor.get_parameter_infos().end());
    REQUIRE(infos.size() == 3);

    CHECK(infos[0].get_name()           == std::string());
    CHECK(infos[0].has_default_value()  == false);
    CHECK(infos[0].get_type()           == type::get<std::string>());
    CHECK(infos[0].get_index()          == 0);
    CHECK(infos[0].get_default_value()  == variant());

    CHECK(infos[1].get_name()           == std::string());
    CHECK(infos[1].has_default_value()  == true);
    CHECK(infos[1].get_type()           == type::get<int>());
    CHECK(infos[1].get_index()          == 1);
    CHECK(infos[1].get_default_value()  == 42);

    CHECK(infos[2].get_name()           == std::string());
    CHECK(infos[2].has_default_value()  == true);
    CHECK(infos[2].get_type()           == type::get<bool>());
    CHECK(infos[2].get_index()          == 2);
    CHECK(infos[2].get_default_value()  == true);
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("constructor - parameter info - names provided & default values", "[constructor]")
{
     constructor ctor = type::get<ctor_param_info_test>().get_constructor({type::get<double>(), type::get<int>(), type::get<bool>()});
    REQUIRE(ctor.is_valid() == true);

    std::vector<parameter_info> infos(ctor.get_parameter_infos().begin(), ctor.get_parameter_infos().end());
    REQUIRE(infos.size() == 3);

    CHECK(infos[0].get_name()           == std::string("val_1"));
    CHECK(infos[0].has_default_value()  == false);
    CHECK(infos[0].get_type()           == type::get<double>());
    CHECK(infos[0].get_index()          == 0);
    CHECK(infos[0].get_default_value()  == variant());

    CHECK(infos[1].get_name()           == std::string("val_2"));
    CHECK(infos[1].has_default_value()  == true);
    CHECK(infos[1].get_type()           == type::get<int>());
    CHECK(infos[1].get_index()          == 1);
    CHECK(infos[1].get_default_value()  == 23);

    CHECK(infos[2].get_name()           == std::string("val_3"));
    CHECK(infos[2].has_default_value()  == true);
    CHECK(infos[2].get_type()           == type::get<bool>());
    CHECK(infos[2].get_index()          == 2);
    CHECK(infos[2].get_default_value()  == true);
}

/////////////////////////////////////////////////////////////////////////////////////////
