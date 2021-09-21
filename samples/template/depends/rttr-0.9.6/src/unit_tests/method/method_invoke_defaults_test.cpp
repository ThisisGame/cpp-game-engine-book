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


struct method_invoke_defaults_test
{
    method_invoke_defaults_test() { for (auto& item : m_invoked) { item = false; } }
    void func_0(){ m_invoked[0] = true; }
    void func_1(int) { m_invoked[1] = true; }
    void func_2(int, int) { m_invoked[2] = true; }
    void func_3(int, int, int) { m_invoked[3] = true; }
    void func_4(int, int, int, int) { m_invoked[4] = true; }
    void func_5(int, int, int, int, int) { m_invoked[5] = true; }
    void func_6(int, int, int, int, int, int) { m_invoked[6] = true; }
    void func_7(int, int, int, int, int, int, int) { m_invoked[7] = true; }

    std::array<bool, 8> m_invoked;
};



/////////////////////////////////////////////////////////////////////////////////////////

RTTR_REGISTRATION
{
    registration::class_<method_invoke_defaults_test>("method_invoke_defaults_test")
        .method("func_0", &method_invoke_defaults_test::func_0)
        .method("func_1", &method_invoke_defaults_test::func_1) ( default_arguments(1) )
        .method("func_2", &method_invoke_defaults_test::func_2) ( default_arguments(1, 2) )
        .method("func_3", &method_invoke_defaults_test::func_3) ( default_arguments(1, 2, 3))
        .method("func_4", &method_invoke_defaults_test::func_4) ( default_arguments(1, 2, 3, 4) )
        .method("func_5", &method_invoke_defaults_test::func_5) ( default_arguments(1, 2, 3, 4, 5) )
        .method("func_6", &method_invoke_defaults_test::func_6) ( default_arguments(1, 2, 3, 4, 5, 6) )
        .method("func_7", &method_invoke_defaults_test::func_7) ( default_arguments(1, 2, 3, 4, 5, 6, 7) )
        ;
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("method - invoke - defaults", "[method]")
{
    type t = type::get<method_invoke_defaults_test>();
    const auto meth_range = t.get_methods();
    std::vector<method> meth_list(meth_range.cbegin(), meth_range.cend());
    REQUIRE(meth_list.size() == 8);

    method_invoke_defaults_test obj;
    CHECK(meth_list[0].invoke(obj).is_valid() == true);
    CHECK(obj.m_invoked[0] == true);

    CHECK(meth_list[1].invoke(obj, 1).is_valid() == true);
    CHECK(obj.m_invoked[1] == true);

    CHECK(meth_list[2].invoke(obj, 1, 2).is_valid() == true);
    CHECK(obj.m_invoked[2] == true);

    CHECK(meth_list[3].invoke(obj, 1, 2, 3).is_valid() == true);
    CHECK(obj.m_invoked[3] == true);

    CHECK(meth_list[4].invoke(obj, 1, 2, 3, 4).is_valid() == true);
    CHECK(obj.m_invoked[4] == true);

    CHECK(meth_list[5].invoke(obj, 1, 2, 3, 4, 5).is_valid() == true);
    CHECK(obj.m_invoked[5] == true);

    CHECK(meth_list[6].invoke(obj, 1, 2, 3, 4, 5, 6).is_valid() == true);
    CHECK(obj.m_invoked[6] == true);

    CHECK(meth_list[7].invoke_variadic(obj, {1, 2, 3, 4, 5, 6, 7}).is_valid() == true);
    CHECK(obj.m_invoked[7] == true);
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("method - invoke - defaults - skip last value", "[method]")
{
    type t = type::get<method_invoke_defaults_test>();
    const auto meth_range = t.get_methods();
    std::vector<method> meth_list(meth_range.cbegin(), meth_range.cend());
    REQUIRE(meth_list.size() == 8);

    method_invoke_defaults_test obj;

    CHECK(meth_list[1].invoke(obj).is_valid() == true);
    CHECK(obj.m_invoked[1] == true);

    CHECK(meth_list[2].invoke(obj, 1).is_valid() == true);
    CHECK(obj.m_invoked[2] == true);

    CHECK(meth_list[3].invoke(obj, 1, 2).is_valid() == true);
    CHECK(obj.m_invoked[3] == true);

    CHECK(meth_list[4].invoke(obj, 1, 2, 3).is_valid() == true);
    CHECK(obj.m_invoked[4] == true);

    CHECK(meth_list[5].invoke(obj, 1, 2, 3, 4).is_valid() == true);
    CHECK(obj.m_invoked[5] == true);

    CHECK(meth_list[6].invoke(obj, 1, 2, 3, 4, 5).is_valid() == true);
    CHECK(obj.m_invoked[6] == true);

    CHECK(meth_list[7].invoke_variadic(obj, {1, 2, 3, 4, 5, 6}).is_valid() == true);
    CHECK(obj.m_invoked[7] == true);
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("method - invoke - NEGATIVE - defaults", "[method]")
{
    method meth = type::get_by_name("").get_method("");
    REQUIRE(meth.is_valid() == false);

    method_invoke_defaults_test obj;
    CHECK(meth.invoke(obj).is_valid() == false);
    CHECK(obj.m_invoked[0] == false);

    CHECK(meth.invoke(obj, 1).is_valid() == false);
    CHECK(obj.m_invoked[1] == false);

    CHECK(meth.invoke(obj, 1, 2).is_valid() == false);
    CHECK(obj.m_invoked[2] == false);

    CHECK(meth.invoke(obj, 1, 2, 3).is_valid() == false);
    CHECK(obj.m_invoked[3] == false);

    CHECK(meth.invoke(obj, 1, 2, 3, 4).is_valid() == false);
    CHECK(obj.m_invoked[4] == false);

    CHECK(meth.invoke(obj, 1, 2, 3, 4, 5).is_valid() == false);
    CHECK(obj.m_invoked[5] == false);

    CHECK(meth.invoke(obj, 1, 2, 3, 4, 5, 6).is_valid() == false);
    CHECK(obj.m_invoked[6] == false);

    CHECK(meth.invoke_variadic(obj, {1, 2, 3, 4, 5, 6, 7}).is_valid() == false);
    CHECK(obj.m_invoked[7] == false);

}

/////////////////////////////////////////////////////////////////////////////////////////
