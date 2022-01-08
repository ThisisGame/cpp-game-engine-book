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

struct method_param_info_test
{
    void method_1()
    {

    }

    void method_2(bool val_1, int val_2, float val_3)
    {

    }

    void method_3(bool val_1, int val_2, float val_3)
    {

    }

    bool method_with_default(int val_1, double val_2, bool val_3)
    {
        return false;
    }
};

/////////////////////////////////////////////////////////////////////////////////////////

RTTR_REGISTRATION
{
    registration::class_<method_param_info_test>("method_param_info_test")
        .method("method_1", &method_param_info_test::method_1) // no params
        .method("method_2", &method_param_info_test::method_2)
        .method("method_3", &method_param_info_test::method_3)
        (
            parameter_names("val_1", "val_2", "val_3")
        )
        .method("method_4", &method_param_info_test::method_with_default)
        (
            default_arguments(23.0, true)
        )
        .method("method_5", &method_param_info_test::method_with_default)
        (
            default_arguments(23.0, true),
            parameter_names("val_1", "val_2", "val_3")
        );
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("method - parameter info - no info", "[method]")
{
    method meth = type::get<method_param_info_test>().get_method("method_1");
    REQUIRE(meth.is_valid() == true);

    auto infos = meth.get_parameter_infos();
    REQUIRE(infos.size() == 0);

    // negative test
    meth = type::get<method_param_info_test>().get_method("");
    REQUIRE(meth.is_valid() == false);
    CHECK(meth.get_parameter_infos().size() == 0);
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("method - parameter info - no names provided", "[method]")
{
    method meth = type::get<method_param_info_test>().get_method("method_2");
    REQUIRE(meth.is_valid() == true);

    std::vector<parameter_info> infos(meth.get_parameter_infos().begin(), meth.get_parameter_infos().end());
    REQUIRE(infos.size() == 3);

    CHECK(infos[0].get_name()           == std::string());
    CHECK(infos[0].has_default_value()  == false);
    CHECK(infos[0].get_type()           == type::get<bool>());
    CHECK(infos[0].get_index()          == 0);
    CHECK(infos[0].get_default_value()  == variant());

    CHECK(infos[1].get_name()           == std::string());
    CHECK(infos[1].has_default_value()  == false);
    CHECK(infos[1].get_type()           == type::get<int>());
    CHECK(infos[1].get_index()          == 1);
    CHECK(infos[1].get_default_value()  == variant());

    CHECK(infos[2].get_name()           == std::string());
    CHECK(infos[2].has_default_value()  == false);
    CHECK(infos[2].get_type()           == type::get<float>());
    CHECK(infos[2].get_index()          == 2);
    CHECK(infos[2].get_default_value()  == variant());
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("method - parameter info - names provided", "[method]")
{
    method meth = type::get<method_param_info_test>().get_method("method_3");
    REQUIRE(meth.is_valid() == true);

    std::vector<parameter_info> infos(meth.get_parameter_infos().begin(), meth.get_parameter_infos().end());
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

TEST_CASE("method - parameter info - no names provided & default values", "[method]")
{
    method meth = type::get<method_param_info_test>().get_method("method_4");
    REQUIRE(meth.is_valid() == true);

    std::vector<parameter_info> infos(meth.get_parameter_infos().begin(), meth.get_parameter_infos().end());
    REQUIRE(infos.size() == 3);

    CHECK(infos[0].get_name()           == std::string());
    CHECK(infos[0].has_default_value()  == false);
    CHECK(infos[0].get_type()           == type::get<int>());
    CHECK(infos[0].get_index()          == 0);
    CHECK(infos[0].get_default_value()  == variant());

    CHECK(infos[1].get_name()           == std::string());
    CHECK(infos[1].has_default_value()  == true);
    CHECK(infos[1].get_type()           == type::get<double>());
    CHECK(infos[1].get_index()          == 1);
    CHECK(infos[1].get_default_value()  == 23.0);

    CHECK(infos[2].get_name()           == std::string());
    CHECK(infos[2].has_default_value()  == true);
    CHECK(infos[2].get_type()           == type::get<bool>());
    CHECK(infos[2].get_index()          == 2);
    CHECK(infos[2].get_default_value()  == true);
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("method - parameter info - names provided & default values", "[method]")
{
    method meth = type::get<method_param_info_test>().get_method("method_5");
    REQUIRE(meth.is_valid() == true);

    std::vector<parameter_info> infos(meth.get_parameter_infos().begin(), meth.get_parameter_infos().end());
    REQUIRE(infos.size() == 3);

    CHECK(infos[0].get_name()           == std::string("val_1"));
    CHECK(infos[0].has_default_value()  == false);
    CHECK(infos[0].get_type()           == type::get<int>());
    CHECK(infos[0].get_index()          == 0);
    CHECK(infos[0].get_default_value()  == variant());

    CHECK(infos[1].get_name()           == std::string("val_2"));
    CHECK(infos[1].has_default_value()  == true);
    CHECK(infos[1].get_type()           == type::get<double>());
    CHECK(infos[1].get_index()          == 1);
    CHECK(infos[1].get_default_value()  == 23.0);

    CHECK(infos[2].get_name()           == std::string("val_3"));
    CHECK(infos[2].has_default_value()  == true);
    CHECK(infos[2].get_type()           == type::get<bool>());
    CHECK(infos[2].get_index()          == 2);
    CHECK(infos[2].get_default_value()  == true);
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("method - parameter info - compare", "[method]")
{
    method meth = type::get<method_param_info_test>().get_method("method_3");
    REQUIRE(meth.is_valid() == true);

    std::vector<parameter_info> infos(meth.get_parameter_infos().begin(), meth.get_parameter_infos().end());
    REQUIRE(infos.size() == 3);

    CHECK(infos[0] == infos[0]);
    CHECK(infos[0] != infos[1]);
}

/////////////////////////////////////////////////////////////////////////////////////////

