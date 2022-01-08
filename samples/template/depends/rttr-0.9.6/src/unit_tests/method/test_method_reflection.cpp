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

#include "unit_tests/method/test_method_reflection.h"

#include <rttr/registration>

using namespace rttr;
using namespace std;

#include <iostream>
#include <memory>
#include <functional>
#include <cmath>

#include <catch/catch.hpp>

enum E_MetaData
{
    SCRIPTABLE  = 0,
    TOOL_TIP    = 1,
    DESCRIPTION = 2
};

static bool free_function_called = false;

void free_function(bool value)
{
    free_function_called = value;
}

void free_function(int& value)
{
    value = 42;
}

std::string& get_global_string()
{
    static std::string text = "hello world";
    return text;
}

struct base_not_registered
{
    bool some_method()
    {
        return true;
    }

    void other_method(int i)
    {
    }
};

struct derive_registered : base_not_registered
{

};


struct derive_registered_with_base_class_list : base_not_registered
{
    RTTR_ENABLE() // but forgot the base class to insert in the macro
};


/////////////////////////////////////////////////////////////////////////////////////////

bool method_test::method_7_called = false;

RTTR_REGISTRATION
{
    registration::class_<method_test>("method_test")
        .constructor<>() (policy::ctor::as_raw_ptr)
        .method("method_1", &method_test::method_1)
        .method("method_2", &method_test::method_2)
        .method("method_3", &method_test::method_3)
#if RTTR_COMPILER == RTTR_COMPILER_MSVC && RTTR_ARCH_TYPE == RTTR_ARCH_32
        .method("method_4", static_cast<void(method_test::*)(std::string&)>(&method_test::method_4))
        .method("method_4", static_cast<void(method_test::*)(std::string&) const>(&method_test::method_4))
        .method("method_5", static_cast<int(method_test::*)(double*)>(&method_test::method_5))
        .method("method_5", static_cast<int(method_test::*)(int, double)>(&method_test::method_5))
        .method("method_5", static_cast<int(method_test::*)(int, double) const>(&method_test::method_5))
#else
        .method("method_4", select_non_const(&method_test::method_4))
        .method("method_4", select_const(&method_test::method_4))
        .method("method_5", select_overload<int(double*)>(&method_test::method_5))
        .method("method_5", select_overload<int(int, double)>(&method_test::method_5))
        .method("method_5", select_const(&method_test::method_5))
#endif
        .method("method_6", &method_test::method_6)
        .method("method_7", &method_test::method_7)
        .method("method_8", &method_test::method_8)
        (
            metadata(E_MetaData::SCRIPTABLE, true),
            metadata("TAG",  42)
        )
        .method("method_9", &method_test::method_9)
        (
            metadata(E_MetaData::SCRIPTABLE, false)
        )
        .method("method_10", [](double, bool)->int{ return 42;})
        .method("method_raw_array", &method_test::method_raw_array)
        .method("method_default",   &method_test::method_default_arg)
        .method("method_6_ret_ptr", &method_test::method_6)
        (
            policy::meth::return_ref_as_ptr
        )
        .method("method_6_void",     &method_test::method_6)
        (
            policy::meth::discard_return
        )
        .method("method_fun_ptr_arg", &method_test::method_fun_ptr_arg)
        .method("method_with_ptr", &method_test::method_with_ptr)
        .method("variant_func", &method_test::set_func_via_variant)
        .method("func_with_noexcept", &method_test::func_with_noexcept)
        ;

    registration::class_<method_test_derived>("method_test_derived")
        .constructor<>()
        .method("method_11", &method_test_derived::method_11);

    registration::class_<method_test_right>("method_test_right")
        .method("method_12", &method_test_right::method_12);

    registration::class_<method_test_final>("method_test_final")
        .method("method_13", &method_test_final::method_13);

    // test free functions
    registration::method("free_function", static_cast<void(*)(bool)>(&free_function))
        .method("free_function", static_cast<void(*)(int&)>(&free_function))
        .method("get_global_string_ptr", &get_global_string)
        (
            policy::meth::return_ref_as_ptr
        )
        .method("get_global_string_void", &get_global_string)
        (
            policy::meth::discard_return
        )
        ;

    // register same method again, should not register anything
    registration::method("get_global_string_void", &get_global_string);

    // the class 'derive_registered' has a base class 'base_not_registered'
    // which is not registered explictely via rttr, however the base-derived relationship
    // will be established by rttr internaly
    registration::class_<derive_registered>("derive_registered")
        .method("some_method", &derive_registered::some_method)
        .method("other_method", &derive_registered::other_method)
        ;

    registration::class_<derive_registered_with_base_class_list>("derive_registered_with_base_class_list")
        .method("some_method", &derive_registered_with_base_class_list::some_method)
        ;
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("Test method", "[method]")
{
    type t_meth = type::get<method_test>();
    REQUIRE(t_meth.is_valid() == true);
    variant inst = t_meth.create({});
    REQUIRE(inst.is_type<method_test*>() == true);
    method_test* null = nullptr; // workaround for catch compile error
    REQUIRE(inst.get_value<method_test*>() != null);
    method_test& obj = *inst.get_value<method_test*>();

    ////////////////////////////////////////////////////////////
    // invoke tests
    variant ret = t_meth.get_method("method_1").invoke(inst);
    REQUIRE(obj.method_1_called == true);
    REQUIRE(ret.is_valid() == true);
    REQUIRE(ret.is_type<void>() == true);

    ////////////////////////////////////////
    obj.method_1_called = false; // reset
    method meth = t_meth.get_method("method_1");
    meth.invoke_variadic(inst, {});
    REQUIRE(obj.method_1_called == true);
    REQUIRE(meth.get_name() == "method_1");
    REQUIRE(meth.get_parameter_infos().empty() == true);
    REQUIRE(meth.get_declaring_type().is_valid() == true);

    ////////////////////////////////////////
    t_meth.get_method("method_2").invoke(inst);
    REQUIRE(obj.method_2_called == true);
    obj.method_2_called = false;
    meth = t_meth.get_method("method_2");
    meth.invoke_variadic(inst, {});
    REQUIRE(obj.method_2_called == true);

    ////////////////////////////////////////
    t_meth.get_method("method_3").invoke(inst, 35);
    REQUIRE(obj.method_3_called == true);
    REQUIRE(obj.method_3_value == 35);
    obj.method_3_called = false;
    obj.method_3_value = 0;
    t_meth.get_method("method_3").invoke_variadic(inst, {42});
    REQUIRE(obj.method_3_called == true);
    REQUIRE(obj.method_3_value == 42);

    ////////////////////////////////////////
    t_meth.get_method("method_4").invoke(inst, string("test"));
    REQUIRE(obj.method_4_called == true);
    string ref_for_method4;
    t_meth.get_method("method_4").invoke(inst, ref_for_method4);
    REQUIRE(ref_for_method4 == "Text Changed");

    obj.method_4_called = false;
    ref_for_method4 = "";
    t_meth.get_method("method_4").invoke_variadic(inst, {ref_for_method4});
    REQUIRE(obj.method_4_called == true);
    REQUIRE(ref_for_method4 == "Text Changed");

    ////////////////////////////////////////
    double arg = 0.0;
    method m5_overloaded_1 = t_meth.get_method("method_5", {type::get<double*>()});
    ret = m5_overloaded_1.invoke(inst, &arg);
    REQUIRE(obj.method_5_called == true);
    REQUIRE(ret.is_type<int>() == true);
    REQUIRE(ret.get_value<int>() == 42);
    REQUIRE(arg == 22.0);

    ////////////////////////////////////////
    arg = 0.0;
    ret = m5_overloaded_1.invoke_variadic(inst, {&arg});
    REQUIRE(obj.method_5_called == true);
    REQUIRE(ret.is_type<int>() == true);
    REQUIRE(ret.get_value<int>() == 42);
    REQUIRE(arg == 22.0);

    method m5_overloaded_2 = t_meth.get_method("method_5", {type::get<int>(), type::get<double>()});
    REQUIRE(bool(m5_overloaded_1) == true);
    REQUIRE(bool(m5_overloaded_2) == true);
    REQUIRE(m5_overloaded_1 != m5_overloaded_2);

    ////////////////////////////////////////
    method m6 = t_meth.get_method("method_6");
    REQUIRE(m6.get_return_type() == type::get<std::string>());

    ret = m6.invoke(inst);
    REQUIRE(obj.method_6_called == true);
    REQUIRE(ret.is_type<string>() == true);
    REQUIRE(ret.get_value<string>() == "Hello World");

    ////////////////////////////////////////
    REQUIRE(t_meth.get_method("method_7").is_static() == true);
    ret = t_meth.get_method("method_7").invoke(instance(), 34.0);
    REQUIRE(obj.method_7_called == true);
    REQUIRE(ret.is_type<int>() == true);
    REQUIRE(ret.get_value<int>() == 23);

    ret = t_meth.get_method("method_8").invoke(inst);
    REQUIRE(obj.method_8_called == true);

    ////////////////////////////////////////
    method_test_derived derived_inst;
    derived_inst.get_type().get_method("method_8").invoke(derived_inst);
    REQUIRE(derived_inst.method_8_derived_called == true);  // the derived virtual function was called
    REQUIRE(derived_inst.method_8_called == false);         // and not the base function

    ////////////////////////////////////////
    method m9 = t_meth.get_method("method_9");
    std::vector<parameter_info> infos(m9.get_parameter_infos().begin(), m9.get_parameter_infos().end());
    REQUIRE(infos.size() == 10);
    REQUIRE(infos[4].get_type() == type::get<bool>());

    ret = m9.invoke_variadic(inst, {1, 2, 3, 4, true, 6, 7, 8, 9, 10});
    REQUIRE(obj.method_9_called == true);

    ////////////////////////////////////////
    // function pointer argument
    typedef void(*func_ptr)(int);
    func_ptr func = &my_global_func;
    ret = t_meth.get_method("method_fun_ptr_arg").invoke(obj, func);
    REQUIRE(obj.method_func_ptr_arg_called == true);
    REQUIRE(obj.m_func_ptr == &my_global_func);


    ////////////////////////////////////////
    t_meth.get_method("method_default").invoke(derived_inst, 3);
    REQUIRE(derived_inst.method_default_arg_called == true);

    ////////////////////////////////////////////////////////////
    // check up_cast, cross cast and middle in the hierarchy cast through invoke
    method_test_final final_obj;
    type t_final = type::get(final_obj);
    REQUIRE(t_final.get_methods().size() == 22); // +1 overloaded
    // test the up cast
    t_final.get_method("method_3").invoke(final_obj, 1000);
    REQUIRE(final_obj.method_3_called == true);
    REQUIRE(final_obj.method_3_value == 1000);
    REQUIRE(t_final.get_method("method_3").get_declaring_type() == type::get<method_test>());

    method_test& up_cast_test = final_obj;
    // test the cross cast
    t_final.get_method("method_12").invoke(up_cast_test);
    REQUIRE(final_obj.method_12_right_called == true);

    // test the middle cast
    t_final.get_method("method_11").invoke(up_cast_test, 45);
    REQUIRE(final_obj.method_11_derived_called == true);

    ////////////////////////////////////////////////////////////
    // test compare operator
    REQUIRE(t_meth.get_method("method_default") == t_meth.get_method("method_default"));
    REQUIRE(t_meth.get_method("method_default") != t_meth.get_method("method_4"));

    ////////////////////////////////////////////////////////////
    // clean up
    t_meth.destroy(inst);
    REQUIRE(inst.is_valid() == false);
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("global methods", "[method]")
{
    free_function_called = false;
    method global_meth_1 = type::get_global_method("free_function", {type::get<bool>()});
    REQUIRE(global_meth_1.get_declaring_type().is_valid() == false);
    REQUIRE(bool(global_meth_1) == true);
    REQUIRE(global_meth_1.get_parameter_infos().begin()->get_type() == type::get<bool>());
    variant success = global_meth_1.invoke(instance());
    REQUIRE(success.is_valid() == false);
    REQUIRE(free_function_called == false);

    success = global_meth_1.invoke(instance(), true);
    REQUIRE(success.is_valid() == true);
    REQUIRE(free_function_called == true);

    method global_meth_2 = type::get_global_method("free_function", {type::get<int>()});
    REQUIRE(bool(global_meth_2) == true);
    int arg = 0;
    global_meth_2.invoke(instance(), arg);
    REQUIRE(arg == 42);

    REQUIRE(global_meth_1 != global_meth_2);

    auto list = type::get_global_methods();
    REQUIRE(list.size() >= 2);
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("ShortCut via type - method invoke", "[method]")
{
    // with instance
    method_test_final obj;
    variant success = type::get<method_test_final>().invoke("method_5", obj, {5, 23.0});
    REQUIRE(success.is_valid() == true);
    REQUIRE(obj.method_5_overloaded_called == true);

    double arg = 0.0;
    success = type::get<method_test_final>().invoke("method_5", obj, {&arg});
    REQUIRE(success.is_valid() == true);
    REQUIRE(obj.method_5_called == true);


    // global method
    free_function_called = false;
    success = type::invoke("free_function", {true});
    REQUIRE(success.is_valid() == true);
    REQUIRE(free_function_called == true);
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("Test method arrays", "[method]")
{
    method_test obj;
    method meth_array = type::get(obj).get_method("method_raw_array");
    REQUIRE(meth_array.is_valid() == true);

    int raw_int_9[9];
    variant ret = meth_array.invoke(obj, raw_int_9);
    REQUIRE(obj.method_raw_array_called == false);
    REQUIRE(ret.is_valid() == false);

    int raw_int[10];
    ret = meth_array.invoke(obj, raw_int);
    REQUIRE(obj.method_raw_array_called == true);
    REQUIRE(ret.is_valid() == true);
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("Test method signature", "[method]")
{
    const auto meth_range = type::get<method_test_final>().get_methods();
    std::vector<method> methods(meth_range.cbegin(), meth_range.cend());
    REQUIRE(methods.size() == 22);

    REQUIRE(methods[0].get_signature() ==  "method_1( )");
    REQUIRE(methods[3].get_signature() ==  std::string("method_4( ") + type::get<std::string>().get_name() + " & )");
    REQUIRE(methods[4].get_signature() ==  "method_5( double* )");
    REQUIRE(methods[5].get_signature() ==  "method_5( int, double )");
    REQUIRE(methods[21].get_signature() == "method_13( )");
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("method policies", "[method]")
{
    method_test obj;
    type meth_type = type::get(obj);
    method m6_ptr = meth_type.get_method("method_6_ret_ptr");
    REQUIRE(m6_ptr.get_return_type() == type::get<const std::string*>());
    variant ret = m6_ptr.invoke(obj);
    REQUIRE(ret.is_valid() == true);
    REQUIRE(ret.is_type<const std::string*>() == true);

    method m6_void = meth_type.get_method("method_6_void");
    REQUIRE(m6_void.get_return_type() == type::get<void>());
    ret = m6_void.invoke(obj);
    REQUIRE(ret.is_valid() == true);
    REQUIRE(ret.is_type<void>() == true);

    // global functions
    method meth_g_ptr   = type::get_global_method("get_global_string_ptr");
    REQUIRE(meth_g_ptr.get_return_type() == type::get<std::string*>());
    ret = meth_g_ptr.invoke(instance());
    REQUIRE(ret.is_valid() == true);
    REQUIRE(ret.is_type<std::string*>() == true);

    method meth_g_void  = type::get_global_method("get_global_string_void");
    REQUIRE(meth_g_void.get_return_type() == type::get<void>());
    ret = meth_g_void.invoke(instance());
    REQUIRE(ret.is_valid() == true);
    REQUIRE(ret.is_type<void>() == true);
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("Invoke method via wrapper", "[method]")
{
    SECTION("test method invoke via shared_ptr wrapper")
    {
        std::shared_ptr<method_test> obj = std::make_shared<method_test>();

        type obj_t = type::get(obj);
        REQUIRE(obj_t.is_wrapper() == true);

        type wrapper_t = obj_t.get_wrapped_type();
        REQUIRE(wrapper_t.is_wrapper() == false);
        REQUIRE(wrapper_t == type::get<method_test*>());
        method m1 = wrapper_t.get_method("method_1");

        variant ret = m1.invoke(obj);
        CHECK(obj->method_1_called == true);
        CHECK(ret.is_valid() == true);
        CHECK(ret.is_type<void>() == true);
    }

    SECTION("test method invoke via variant with shared_ptr wrapper")
    {
        variant var = std::make_shared<method_test>();
        type obj_t = var.get_type();
        REQUIRE(obj_t.is_wrapper() == true);

        type wrapper_t = obj_t.get_wrapped_type();
        REQUIRE(wrapper_t.is_wrapper() == false);
        REQUIRE(wrapper_t == type::get<method_test*>());
        method m1 = wrapper_t.get_method("method_1");

        variant ret = m1.invoke(var);
        CHECK(var.get_value<std::shared_ptr<method_test>>()->method_1_called == true);
        CHECK(ret.is_valid() == true);
        CHECK(ret.is_type<void>() == true);
    }

    SECTION("test method invoke via reference_wrapper")
    {
        method_test instance;
        std::reference_wrapper<method_test> obj = std::ref(instance);

        type obj_t = type::get(obj);
        REQUIRE(obj_t.is_wrapper() == true);

        type wrapper_t = obj_t.get_wrapped_type();
        CHECK(wrapper_t.is_wrapper() == false);
        CHECK(wrapper_t == type::get<method_test>());

        method m1 = wrapper_t.get_method("method_1");
        variant ret = m1.invoke(obj);
        CHECK(obj.get().method_1_called == true);
        CHECK(ret.is_valid() == true);
        CHECK(ret.is_type<void>() == true);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("method - invoke with nullptr", "[method]")
{
    type t_meth = type::get<method_test>();
    method meth = t_meth.get_method("method_with_ptr");
    method_test obj;

    variant var = meth.invoke(obj, nullptr);
    CHECK(var.is_valid() == true);
    CHECK(obj.method_with_ptr_called == true);
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("method - invoke with variant as argument", "[method]")
{
    type t_meth = type::get<method_test>();
    method meth = t_meth.get_method("variant_func");
    method_test obj;

    auto ret = meth.invoke(obj, variant(23));

    CHECK(ret.is_valid()    == true);
    CHECK(ret.to_bool()     == true);
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("method - invoke func with noexcept in signature", "[method]")
{
    method meth = type::get<method_test>().get_method("func_with_noexcept");
    method_test obj;

    auto ret = meth.invoke(obj, 23);

    CHECK(ret.is_valid() == true);
    CHECK(ret.to_int()   == 42);
    CHECK(obj.method_with_noexpcet_called == true);
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("method - invoke base method, which is not registerd", "[method]")
{
    type t_meth = type::get<derive_registered>();
    method meth = t_meth.get_method("some_method");
    derive_registered obj;

    auto ret = meth.invoke(obj);

    CHECK(ret.is_valid()    == true);
    CHECK(ret.to_bool()     == true);

    auto base_type = type::get<base_not_registered>();

    CHECK(t_meth.is_derived_from(base_type) == true);

    auto derived_type = type::get<derive_registered_with_base_class_list>();
    CHECK(derived_type.is_derived_from(base_type) == true);

    auto range = base_type.get_derived_classes();

    REQUIRE(range.size() == 2);
    CHECK(*range.begin() == t_meth);
    CHECK(*(++range.begin()) == derived_type);
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("Test method meta data", "[method]")
{
    method m8 = type::get<method_test_final>().get_method("method_8");
    variant value = m8.get_metadata(E_MetaData::SCRIPTABLE);
    REQUIRE(value.is_type<bool>() == true);
    REQUIRE(value.get_value<bool>() == true);
    // string meta data
    value = m8.get_metadata("TAG");
    REQUIRE(value.is_valid() == true);
    REQUIRE(value.get_value<int>() == 42);

    // no meta data
    method m7 = type::get<method_test_final>().get_method("method_7");
    REQUIRE(m7.is_valid() == true);
    REQUIRE(m7.get_metadata(E_MetaData::SCRIPTABLE).is_valid() == false);

    // not scriptable
    method m9 = type::get<method_test_final>().get_method("method_9");
    value = m9.get_metadata(E_MetaData::SCRIPTABLE);
    REQUIRE(value.is_valid() == true);
    REQUIRE(value.get_value<bool>() == false);
}

/////////////////////////////////////////////////////////////////////////////////////////
