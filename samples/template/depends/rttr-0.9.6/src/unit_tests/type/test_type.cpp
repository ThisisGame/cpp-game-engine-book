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

#include "unit_tests/type/test_type.h"

#include "unit_tests/test_classes.h"

#include <catch/catch.hpp>
#include <rttr/type>

#include <vector>
#include <map>
#include <string>
#include <memory>

using namespace rttr;

typedef std::map<int, std::string> IntToStringMap;
typedef std::map<int, int> IntToIntMap;


enum E_Alignment
{
    LEFT,
    RIGHT,
    BOTTOM,
    DOWN
};

template<typename...Args>
struct my_class_template {};

TEST_CASE("Test rttr::type - BasicTests", "[type]")
{
    using namespace rttr;

    SECTION("simple basic check")
    {
        int intVar = 23;
        const type intTypeInfo = type::get(intVar);
        CHECK(intTypeInfo.get_name() == "int");
        CHECK(intTypeInfo == type::get<int>());

        bool boolVar = true;
        const type boolTypeInfo = type::get(boolVar);
        CHECK(boolTypeInfo.get_name() == "bool");
        CHECK(boolTypeInfo == type::get<bool>());

        CHECK(boolTypeInfo != intTypeInfo);

        int ***intPtr = nullptr;
        CHECK(type::get<int***>() == type::get(intPtr));

    }

    SECTION("check pointer types")
    {
        int intVar = 23;
        int* intPtrVar = &intVar;
        const type intPtrTypeInfo = type::get(intPtrVar);
        CHECK(intPtrTypeInfo.get_name() == "int*");
        CHECK(intPtrTypeInfo == type::get<int*>());

        bool boolVar = true;
        bool* boolPtrVar = &boolVar;
        const type boolPtrTypeInfo = type::get(boolPtrVar);
        CHECK(boolPtrTypeInfo.get_name() == "bool*");
        CHECK(boolPtrTypeInfo == type::get<bool*>());

        CHECK(boolPtrTypeInfo != intPtrTypeInfo);
    }

    SECTION("check that cv was removed")
    {
        int intVar = 42;
        const int constIntVar = 42;
        CHECK(type::get(intVar) == type::get(constIntVar));
        CHECK(type::get<int>() == type::get(constIntVar));
        CHECK(type::get<int>() == type::get<const int>());
        CHECK(type::get<int>() == type::get<const int &>());

        CHECK(type::get<int*>() == type::get(&intVar));
        CHECK(type::get<int*>() == type::get<int *const>());
        CHECK(type::get<const int*>() == type::get(&constIntVar));
        CHECK(type::get<const int*>() == type::get<const int *const>());

        const int& intConstRef = intVar;
        CHECK(type::get<int>() == type::get(intConstRef));
        int*** ptr = nullptr;
        CHECK(type::get<int***>() == type::get(ptr));
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("Test rttr::type - ComplexerTypes", "[type]")
{
    std::vector<int> myList;
    std::vector<int> myList2;
    CHECK(type::get<std::vector<int> >() == type::get(myList));
    CHECK(type::get(myList) == type::get(myList2));

    IntToStringMap myMap;
    std::map<int, std::string> myMap2;

    CHECK((type::get<std::map<int, std::string> >() == type::get<IntToStringMap>()));
    CHECK((type::get<std::map<int, std::string> >() == type::get(myMap)));
    CHECK((type::get<IntToStringMap>() == type::get(myMap)));
    CHECK((type::get(myMap) == type::get(myMap2)));

    CHECK((type::get(myMap) != type::get<std::map<int, int> >()));

    // check typedef
    typedef std::map<int, std::string> MyMap;
    CHECK((type::get<MyMap>() == type::get<std::map<int, std::string>>()));
    MyMap obj;
    std::map<int, std::string> obj2;
    CHECK(type::get(obj) == type::get(obj2));
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("Test rttr::type - SingleClassInheritance", "[type]")
{

    {
        ClassSingle6A instance6A;
        ClassSingleBase& baseSingle = instance6A;

        // down cast cast
        CHECK(rttr_cast<ClassSingle6A*>(&baseSingle) != nullptr);
        CHECK(rttr_cast<ClassSingle3A*>(&baseSingle) != nullptr);
        CHECK(rttr_cast<ClassSingle6B*>(&baseSingle) == nullptr);

        // up cast cast
        CHECK(rttr_cast<ClassSingleBase*>(&instance6A) != nullptr);
        CHECK(rttr_cast<ClassSingle3A*>(&instance6A) != nullptr);
        CHECK(rttr_cast<ClassSingle1A*>(&instance6A) != nullptr);
    }

    {
        ClassSingle6E instance6E;
        ClassSingleBase& baseSingle = instance6E;

        // down cast cast
        CHECK(rttr_cast<ClassSingle6A*>(&baseSingle) == nullptr);
        CHECK(rttr_cast<ClassSingle3E*>(&baseSingle) != nullptr);
        CHECK(rttr_cast<ClassSingle6E*>(&baseSingle) != nullptr);

        // up cast cast
        CHECK(rttr_cast<ClassSingleBase*>(&instance6E) != nullptr);
        CHECK(rttr_cast<ClassSingle3E*>(&instance6E) != nullptr);
        CHECK(rttr_cast<ClassSingle1E*>(&instance6E) != nullptr);
        CHECK(rttr_cast<ClassSingle6E*>(&instance6E) != nullptr);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("Test rttr::type - MultipleClassInheritance", "[type]")
{
    {
        FinalClass final;
        ClassMultipleBaseA& baseMultiA = final;
        ClassMultipleBaseB& baseMultiB = final;
        ClassMultipleBaseC& baseMultiC = final;
        ClassMultipleBaseD& baseMultiD = final;
        ClassMultipleBaseE& baseMultiE = final;

        // down cast cast
        CHECK(rttr_cast<FinalClass*>(&baseMultiA) != nullptr);
        CHECK(rttr_cast<FinalClass*>(&baseMultiB) != nullptr);
        CHECK(rttr_cast<FinalClass*>(&baseMultiC) != nullptr);
        CHECK(rttr_cast<FinalClass*>(&baseMultiD) != nullptr);
        CHECK(rttr_cast<FinalClass*>(&baseMultiE) != nullptr);

        // up cast cast
        CHECK(rttr_cast<ClassMultipleBaseA*>(&final) != nullptr);
        CHECK(rttr_cast<ClassMultipleBaseB*>(&final) != nullptr);
        CHECK(rttr_cast<ClassMultipleBaseC*>(&final) != nullptr);
        CHECK(rttr_cast<ClassMultipleBaseD*>(&final) != nullptr);
        CHECK(rttr_cast<ClassMultipleBaseE*>(&final) != nullptr);

        // down cast cast to the middle
        CHECK(rttr_cast<ClassMultiple3A*>(&baseMultiA) != nullptr);
        CHECK(rttr_cast<ClassMultiple3B*>(&baseMultiB) != nullptr);
        CHECK(rttr_cast<ClassMultiple3C*>(&baseMultiC) != nullptr);
        CHECK(rttr_cast<ClassMultiple3D*>(&baseMultiD) != nullptr);
        CHECK(rttr_cast<ClassMultiple3E*>(&baseMultiE) != nullptr);
    }

    {
        FinalClass final;
        ClassMultiple6A classMulti6A;
        ClassMultiple3B classMulti3B;
        ClassMultiple1E classMulti1E;

        // check for invalid casts
        CHECK(rttr_cast<FinalClass*>(&classMulti6A) == nullptr);

        CHECK(rttr_cast<ClassMultiple4B*>(&classMulti3B) == nullptr);

        CHECK(rttr_cast<ClassMultiple2E*>(&classMulti1E) == nullptr);
        CHECK(rttr_cast<ClassMultiple6E*>(&classMulti1E) == nullptr);

        // check for valid casts
        CHECK(rttr_cast<ClassMultiple5A*>(&classMulti6A) != nullptr);
        CHECK(rttr_cast<ClassMultipleBaseA*>(&classMulti6A) != nullptr);

        CHECK(rttr_cast<ClassMultiple2B*>(&classMulti3B) != nullptr);
        CHECK(rttr_cast<ClassMultiple1B*>(&classMulti3B) != nullptr);

        CHECK(rttr_cast<ClassMultipleBaseE*>(&classMulti1E) != nullptr);
        CHECK(rttr_cast<ClassMultiple1E*>(&classMulti1E) != nullptr);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("Test rttr::type - Virtual Inheritance", "[type]")
{
    DiamondBottom diamond;

    DiamondTop* base    = &diamond;
    DiamondLeft* left   = &diamond;
    DiamondRight* right = &diamond;

    CHECK(rttr_cast<DiamondBottom*>(base)   == &diamond);   // top to bottom cast
    CHECK(rttr_cast<DiamondTop*>(&diamond)  == base);       // bottom to top
    CHECK(rttr_cast<DiamondLeft*>(base)     == left);       // base to one level up - left class
    CHECK(rttr_cast<DiamondRight*>(base)    == right);      // base to one level up - right class
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("type - get_base_classes()", "[type]")
{
    DiamondBottom d;
    const auto base_list_range = type::get(d).get_base_classes();
    REQUIRE(base_list_range.size() == 3);

    std::vector<type> base_list(base_list_range.cbegin(), base_list_range.cend());
    REQUIRE(base_list[0] == type::get<DiamondTop>());
    REQUIRE(base_list[1] == type::get<DiamondLeft>());
    REQUIRE(base_list[2] == type::get<DiamondRight>());
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("type - is_base_of()", "[type]")
{
    auto t_base = type::get<ClassSingleBase>();
    auto t_derived = type::get<ClassSingle6A>();

    CHECK(t_base.is_base_of(t_derived)          == true);
    CHECK(t_base.is_base_of<ClassSingle6A>()    == true);
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("type - is_derived_from()", "[type]")
{
    DiamondBottom d;

    REQUIRE(type::get(d).is_derived_from(type::get<DiamondTop>())   == true); // dynamic
    REQUIRE(type::get(d).is_derived_from<DiamondTop>()              == true); // static
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("Test rttr::type - TypeId/ClassInheritance", "[type]")
{
    ClassSingle6A instance6A;
    ClassSingleBase& baseSingle     = instance6A;
    ClassSingleBase* baseSinglePtr  = &instance6A;

    CHECK(type::get<ClassSingleBase*>() == type::get(baseSinglePtr));

    CHECK(type::get<ClassSingle6A>() == type::get(baseSingle));
    CHECK(type::get<ClassSingleBase*>() == type::get(&baseSingle));

    ClassSingle3A instance3A;
    CHECK(type::get<ClassSingle3A>() == type::get(instance3A));
    CHECK(type::get<ClassSingle6A>() != type::get(instance3A));
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("Test rttr::type - Check get_sizeof", "[type]")
{
    CHECK(type::get<bool>().get_sizeof()        == 1);
    CHECK(type::get<char>().get_sizeof()        == 1);
    CHECK(type::get<int>().get_sizeof()         == 4);
    CHECK(type::get<void>().get_sizeof()        == 0);
    CHECK(type::get<void(int)>().get_sizeof()   == 0);

#if RTTR_ARCH_TYPE == RTTR_ARCH_64
    CHECK(type::get<int*>().get_sizeof()         == 8);
#else
    CHECK(type::get<int*>().get_sizeof()         == 4);
#endif
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("Test rttr::type - Check is_arithmetic", "[type]")
{
    CHECK(type::get<int>().is_arithmetic()               == true);
    CHECK(type::get<float>().is_arithmetic()             == true);
    CHECK(type::get<double>().is_arithmetic()            == true);
    CHECK(type::get<char>().is_arithmetic()              == true);
    CHECK(type::get<bool>().is_arithmetic()              == true);

    CHECK(type::get<int8_t>().is_arithmetic()            == true);
    CHECK(type::get<int16_t>().is_arithmetic()           == true);
    CHECK(type::get<int32_t>().is_arithmetic()           == true);
    CHECK(type::get<int64_t>().is_arithmetic()           == true);

    CHECK(type::get<uint8_t>().is_arithmetic()           == true);
    CHECK(type::get<uint16_t>().is_arithmetic()          == true);
    CHECK(type::get<uint32_t>().is_arithmetic()          == true);
    CHECK(type::get<uint64_t>().is_arithmetic()          == true);

    CHECK(type::get<std::string>().is_arithmetic()       == false);
    CHECK(type::get<ClassSingle6A>().is_arithmetic()     == false);
    CHECK(type::get<ClassMultiple2B>().is_arithmetic()   == false);
    CHECK(type::get<FinalClass>().is_arithmetic()        == false);
    CHECK(type::get<E_Alignment>().is_arithmetic()       == false);
    CHECK(type::get<int*>().is_arithmetic()              == false);
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("Test rttr::type - Check is_class", "[type]")
{

    CHECK(type::get<std::string>().is_class()     == true);
    CHECK(type::get<ClassSingle6A>().is_class()   == true);
    CHECK(type::get<ClassMultiple2B>().is_class() == true);
    CHECK(type::get<FinalClass>().is_class()      == true);

    CHECK(type::get<int>().is_class()            == false);
    CHECK(type::get<float>().is_class()          == false);
    CHECK(type::get<double>().is_class()         == false);
    CHECK(type::get<char>().is_class()           == false);
    CHECK(type::get<bool>().is_class()           == false);
    CHECK(type::get<E_Alignment>().is_class()    == false);
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("Test rttr::type - Check is_enum", "[type]")
{
    CHECK(type::get<E_Alignment>().is_enumeration()     == true);

    CHECK(type::get<int>().is_enumeration()             == false);
    CHECK(type::get<float>().is_enumeration()           == false);
    CHECK(type::get<double>().is_enumeration()          == false);
    CHECK(type::get<char>().is_enumeration()            == false);
    CHECK(type::get<bool>().is_enumeration()            == false);
    CHECK(type::get<ClassSingle6A>().is_enumeration()   == false);
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("Test rttr::type - Check is_pointer", "[type]")
{
    CHECK(type::get<int*>().is_pointer()             == true);
    CHECK(type::get<ClassSingle6A*>().is_pointer()   == true);
    CHECK(type::get<int***>().is_pointer()           == true);
    CHECK(type::get<char*>().is_pointer()            == true);

    CHECK(type::get<int>().is_pointer()              == false);
    CHECK(type::get<float>().is_pointer()            == false);
    CHECK(type::get<double>().is_pointer()           == false);
    CHECK(type::get<char>().is_pointer()             == false);
    CHECK(type::get<bool>().is_pointer()             == false);
    CHECK(type::get<ClassSingle6A>().is_pointer()    == false);
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("Test rttr::type - Check is_array", "[type]")
{
    CHECK(type::get<int[10]>().is_array()        == true);
    CHECK(type::get<char[10]>().is_array()       == true);

    CHECK(type::get<char(*)[10]>().is_array()    == false);
    CHECK(type::get<int>().is_array()            == false);
    CHECK(type::get<float>().is_array()          == false);
    CHECK(type::get<int*>().is_array()           == false);
    CHECK(type::get<float*>().is_array()         == false);
    CHECK(type::get<double>().is_array()         == false);
    CHECK(type::get<char>().is_array()           == false);
    CHECK(type::get<bool>().is_array()           == false);
    CHECK(type::get<ClassSingle6A>().is_array()  == false);
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("Test rttr::type - is_associative_container", "[type]")
{
    CHECK(type::get<std::set<int>>().is_associative_container()                                 == true);
    CHECK((type::get<std::map<int, std::string>>().is_associative_container()                   == true));
    CHECK((type::get<std::multimap<int, std::string>>().is_associative_container()              == true));
    CHECK((type::get<std::multiset<int>>().is_associative_container()                           == true));

    CHECK(type::get<std::unordered_set<int>>().is_associative_container()                       == true);
    CHECK((type::get<std::unordered_map<int, std::string>>().is_associative_container()         == true));
    CHECK((type::get<std::unordered_multimap<int, std::string>>().is_associative_container()    == true));
    CHECK((type::get<std::unordered_multiset<int>>().is_associative_container()                 == true));

    CHECK(type::get<int>().is_associative_container()            == false);
    CHECK(type::get<float>().is_associative_container()          == false);
    CHECK(type::get<int*>().is_associative_container()           == false);
    CHECK(type::get<float*>().is_associative_container()         == false);
    CHECK(type::get<double>().is_associative_container()         == false);
    CHECK(type::get<char>().is_associative_container()           == false);
    CHECK(type::get<bool>().is_associative_container()           == false);
    CHECK(type::get<ClassSingle6A>().is_associative_container()  == false);
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("Test rttr::type - is_sequential_container", "[type]")
{
    CHECK(type::get<std::vector<int>>().is_sequential_container()       == true);

    CHECK(type::get<int>().is_sequential_container()            == false);
    CHECK(type::get<float>().is_sequential_container()          == false);
    CHECK(type::get<int*>().is_sequential_container()           == false);
    CHECK(type::get<float*>().is_sequential_container()         == false);
    CHECK(type::get<double>().is_sequential_container()         == false);
    CHECK(type::get<char>().is_sequential_container()           == false);
    CHECK(type::get<bool>().is_sequential_container()           == false);
    CHECK(type::get<ClassSingle6A>().is_sequential_container()  == false);
    CHECK((type::get<std::map<int, std::string>>().is_sequential_container()  == false));
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("Test rttr::type - Check is_wrapper", "[type]")
{
    CHECK(type::get<std::shared_ptr<int>>().is_wrapper()        == true);
    CHECK(type::get<std::reference_wrapper<int>>().is_wrapper() == true);
    CHECK(type::get<std::unique_ptr<int>>().is_wrapper()        == true);
    CHECK(type::get<std::weak_ptr<int>>().is_wrapper()          == true);

    std::shared_ptr<ClassSingle6A> sharedPtr = std::make_shared<ClassSingle6A>();
    CHECK(type::get(sharedPtr).is_wrapper()         == true);

    CHECK(type::get<int>().is_wrapper()             == false);
    CHECK(type::get<float>().is_wrapper()           == false);
    CHECK(type::get<int*>().is_wrapper()            == false);
    CHECK(type::get<float*>().is_wrapper()          == false);
    CHECK(type::get<double>().is_wrapper()          == false);
    CHECK(type::get<char>().is_wrapper()            == false);
    CHECK(type::get<bool>().is_wrapper()            == false);
    CHECK(type::get<ClassSingle6A*>().is_wrapper()  == false);
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("Test rttr::type - Check is_function_pointer", "[type]")
{
    using MyClass = ClassSingleBase;
    CHECK(type::get<void(*)()>().is_function_pointer()              == true);
    CHECK(type::get<void*(*)()>().is_function_pointer()             == true);

    CHECK(type::get<int>().is_function_pointer()                    == false);
    CHECK(type::get<int*>().is_function_pointer()                   == false);
    CHECK(type::get<void(void)>().is_function_pointer()             == false);
    CHECK(type::get<int(MyClass::*)>().is_function_pointer()        == false);
    CHECK(type::get<int(MyClass::*)(void)>().is_function_pointer()  == false);
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("Test rttr::type - Check is_member_function_pointer", "[type]")
{
    using MyClass = ClassSingleBase;
    CHECK(type::get<void(MyClass::*)()>().is_member_function_pointer()  == true);

    CHECK(type::get<void(void)>().is_member_function_pointer()          == false);
    CHECK(type::get<void*(*)()>().is_member_function_pointer()          == false);
    CHECK(type::get<int(MyClass::*)>().is_member_function_pointer()     == false);
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("Test rttr::type - Check is_member_object_pointer", "[type]")
{
    using MyClass = ClassSingleBase;
    CHECK(type::get<int(MyClass::*)>().is_member_object_pointer()   == true);

    CHECK(type::get<void(void)>().is_member_object_pointer()        == false);
    CHECK(type::get<void*(*)()>().is_member_object_pointer()        == false);
    CHECK(type::get<int(MyClass::*)()>().is_member_object_pointer() == false);
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("Test rttr::type - Check is_template_instantiation", "[type]")
{
    CHECK(type::get<std::string>().is_template_instantiation()            == true);
    CHECK(type::get<std::vector<int>>().is_template_instantiation()       == true);
    CHECK((type::get<std::array<int, 100>>().is_template_instantiation()  == true));
    CHECK((type::get<std::array<bool, 100>>().is_template_instantiation() == true));

    CHECK(type::get<int>().is_template_instantiation()                    == false);
    CHECK(type::get<ClassSingleBase>().is_template_instantiation()        == false);
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("Test rttr::type - get_template_arguments()", "[type]")
{
    SECTION("valid test - std::string")
    {
        auto type_list = type::get<std::string>().get_template_arguments();
        CHECK(type_list.size() == 3);
    }

    SECTION("valid test - std::array")
    {
        auto type_list = type::get<std::array<int, 100>>().get_template_arguments();
        CHECK(type_list.size() == 2);
        CHECK(*type_list.begin()     == type::get<int>());
        CHECK(*(++type_list.begin()) == type::get<std::size_t>());
    }

    SECTION("valid test - custom type")
    {
        CHECK((type::get<my_class_template<>>().get_template_arguments().size() == 0));
        CHECK((type::get<my_class_template<int, bool, char>>().get_template_arguments().size() == 3));
    }

    SECTION("invalid test")
    {
        CHECK(type::get<int>().get_template_arguments().size()                == 0);
        CHECK(type::get<ClassSingleBase>().get_template_arguments().size()    == 0);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("Test rttr::type - Check get_wrapped_type", "[type]")
{
    CHECK(type::get<std::shared_ptr<int>>().get_wrapped_type()          == type::get<int*>());
    CHECK(type::get<std::reference_wrapper<int>>().get_wrapped_type()   == type::get<int>());

    CHECK(type::get<std::shared_ptr<const int>>().get_wrapped_type()          == type::get<const int*>());
    CHECK(type::get<std::reference_wrapper<const int>>().get_wrapped_type()   == type::get<int>());

    std::shared_ptr<ClassSingle6A> sharedPtr = std::make_shared<ClassSingle6A>();
    CHECK(type::get(sharedPtr).get_wrapped_type()           == type::get<ClassSingle6A*>());

    // negative test
    CHECK(type::get<int>().get_wrapped_type().is_valid()    == false);
    CHECK(type::get<float>().get_wrapped_type().is_valid()  == false);
    CHECK(type::get<int*>().get_wrapped_type().is_valid()   == false);
    CHECK(type::get<float*>().get_wrapped_type().is_valid() == false);
    CHECK(type::get<ClassSingle6A>().get_wrapped_type().is_valid() == false);
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("Test rttr::type - Check custom wrapper type", "[type]")
{
    CHECK(type::get<custom_wrapper<int>>().get_wrapped_type()          == type::get<int>());

    CHECK(type::get<custom_wrapper<const int>>().get_wrapped_type()    == type::get<int>());
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("Test rttr::type - get_types()", "[type]")
{
    CHECK(type::get_types().size() > 1);
}

/////////////////////////////////////////////////////////////////////////////////////////
