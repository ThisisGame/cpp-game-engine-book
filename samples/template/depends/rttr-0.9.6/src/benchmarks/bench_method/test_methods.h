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

#ifndef RTTR_TEST_METHODS_H_
#define RTTR_TEST_METHODS_H_

#include <rttr/type>

#if RTTR_COMPILER == RTTR_COMPILER_CLANG || RTTR_COMPILER == RTTR_COMPILER_APPLECLANG
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wunused-private-field"
#endif

namespace ns_foo
{

#define CLASS(CLASS_NAME, NUMBER) struct CLASS_NAME { virtual ~CLASS_NAME(){} void RTTR_CAT(method_,NUMBER)() { } virtual void method_v() { } RTTR_ENABLE() private: double dummy_double_value; std::string dummy_string_value; };
#define CLASS_INHERIT(CLASS1, CLASS2, NUMBER) struct CLASS1 : CLASS2 { void RTTR_CAT(method_,NUMBER)() { } virtual void method_v() { } RTTR_ENABLE(CLASS2) private: double dummy_double_value; std::string dummy_string_value; };

struct method_class
{
    method_class();
    virtual ~method_class();

    void method_0();

    virtual void method_v();

    void void_method();
    int int_method();
    std::string string_method();

    void void_method_arg_1(int arg_1);
    void void_method_arg_2(int arg_1, int arg_2);
    void void_method_arg_3(int arg_1, int arg_2, int arg_3);
    void void_method_arg_8(int arg_1, int arg_2, int arg_3,
                           int arg_4, int arg_5, int arg_6,
                           int arg_7, int arg_8);

    void method_ptr_arg(int* ptr);

    bool method_void_called = false;
    bool method_int_called = false;
    bool method_arg_1_called = false;
    bool method_arg_2_called = false;
    bool method_arg_3_called = false;

    int value_1 = 0;
    int value_2 = 0;
    int value_3 = 0;
    int value_4 = 0;
    int value_5 = 0;
    int value_6 = 0;
    int value_7 = 0;
    int value_8 = 0;

    RTTR_ENABLE()
};

struct method_class_d_1 : method_class
{

    void method_1()                  { }
    virtual void method_v()          { }

    RTTR_ENABLE(method_class)
};


CLASS_INHERIT(method_class_d_2, method_class_d_1, 2)

struct method_class_d_3 : method_class_d_2
{

    void method_3()                  { }
    virtual void method_v()          { }

    RTTR_ENABLE(method_class_d_2)
};

CLASS_INHERIT(method_class_d_4, method_class_d_3, 4)
CLASS_INHERIT(method_class_d_5, method_class_d_4, 5)


struct method_class_d_6 : method_class_d_5
{

    void method_6()                  { }
    virtual void method_v()          { }

    RTTR_ENABLE(method_class_d_5)
};

struct method_class_d_7 : method_class_d_6
{

    void method_7()                  { }
    virtual void method_v()          { }

    RTTR_ENABLE(method_class_d_6)
};


CLASS(class_multiple_base_A, 0)
CLASS_INHERIT(class_multiple_1A, class_multiple_base_A, 1)
CLASS_INHERIT(class_multiple_2A, class_multiple_1A, 2)
CLASS_INHERIT(class_multiple_3A, class_multiple_2A, 3)
CLASS_INHERIT(class_multiple_4A, class_multiple_3A, 4)
CLASS_INHERIT(class_multiple_5A, class_multiple_4A, 5)


CLASS(class_multiple_base_B, 6)
CLASS_INHERIT(class_multiple_1B, class_multiple_base_B, 7)
CLASS_INHERIT(class_multiple_2B, class_multiple_1B, 8)
CLASS_INHERIT(class_multiple_3B, class_multiple_2B, 9)
CLASS_INHERIT(class_multiple_4B, class_multiple_3B, 10)
CLASS_INHERIT(class_multiple_5B, class_multiple_4B, 11)


CLASS(class_multiple_base_C, 12)
CLASS_INHERIT(class_multiple_1C, class_multiple_base_C, 13)
CLASS_INHERIT(class_multiple_2C, class_multiple_1C, 14)
CLASS_INHERIT(class_multiple_3C, class_multiple_2C, 15)
CLASS_INHERIT(class_multiple_4C, class_multiple_3C, 16)
CLASS_INHERIT(class_multiple_5C, class_multiple_4C, 17)


struct class_multiple_final : class_multiple_5A, class_multiple_5B, class_multiple_5C
{
    void method_18()        { }
    virtual void method_v() { }

    RTTR_ENABLE(class_multiple_5A, class_multiple_5B, class_multiple_5C)
};

struct class_multiple_final_D : class_multiple_final
{
    void method_19()        { }
    virtual void method_v() { }

    RTTR_ENABLE(class_multiple_final)
};

} // end namespace ns_foo

#define CLASS_REGISTER(CLASS_NAME, NUMBER) \
    registration::class_<ns_foo::CLASS_NAME>("ns_foo::" #CLASS_NAME) \
        .constructor<>() \
        .method("method_" #NUMBER, &ns_foo::CLASS_NAME::RTTR_CAT(method_,NUMBER)) \
        ;

#if RTTR_COMPILER == RTTR_COMPILER_CLANG || RTTR_COMPILER == RTTR_COMPILER_APPLECLANG
#   pragma GCC diagnostic pop
#endif

#endif // RTTR_TEST_METHODS_H_
