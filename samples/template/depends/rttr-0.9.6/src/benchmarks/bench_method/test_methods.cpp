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

#include "benchmarks/bench_method/test_methods.h"

#include <rttr/registration>


RTTR_REGISTRATION
{
    using namespace rttr;

    registration::class_<ns_foo::method_class>("ns_foo::method_class")
        .constructor<>()
        .method("method_0",             &ns_foo::method_class::method_0)
        .method("method_v",             &ns_foo::method_class::method_v)
        .method("void_method",          &ns_foo::method_class::void_method)
        .method("int_method",           &ns_foo::method_class::int_method)
        .method("string_method",        &ns_foo::method_class::string_method)
        .method("void_method_arg_1",    &ns_foo::method_class::void_method_arg_1)
        .method("void_method_arg_2",    &ns_foo::method_class::void_method_arg_2)
        .method("void_method_arg_3",    &ns_foo::method_class::void_method_arg_3)
        .method("void_method_arg_8",    &ns_foo::method_class::void_method_arg_8)
        .method("method_ptr_arg",       &ns_foo::method_class::method_ptr_arg)
        ;


    CLASS_REGISTER(method_class_d_1, 1)
    CLASS_REGISTER(method_class_d_2, 2)
    CLASS_REGISTER(method_class_d_3, 3)
    CLASS_REGISTER(method_class_d_4, 4)
    CLASS_REGISTER(method_class_d_5, 5)
    CLASS_REGISTER(method_class_d_6, 6)
    CLASS_REGISTER(method_class_d_7, 7)


    CLASS_REGISTER(class_multiple_base_A, 0)
    CLASS_REGISTER(class_multiple_1A, 1)
    CLASS_REGISTER(class_multiple_2A, 2)
    CLASS_REGISTER(class_multiple_3A, 3)
    CLASS_REGISTER(class_multiple_4A, 4)
    CLASS_REGISTER(class_multiple_5A, 5)

    CLASS_REGISTER(class_multiple_base_B, 6)
    CLASS_REGISTER(class_multiple_1B, 7)
    CLASS_REGISTER(class_multiple_2B, 8)
    CLASS_REGISTER(class_multiple_3B, 9)
    CLASS_REGISTER(class_multiple_4B, 10)
    CLASS_REGISTER(class_multiple_5B, 11)

    CLASS_REGISTER(class_multiple_base_C, 12)
    CLASS_REGISTER(class_multiple_1C, 13)
    CLASS_REGISTER(class_multiple_2C, 14)
    CLASS_REGISTER(class_multiple_3C, 15)
    CLASS_REGISTER(class_multiple_4C, 16)
    CLASS_REGISTER(class_multiple_5C, 17)


    CLASS_REGISTER(class_multiple_final, 18)
    CLASS_REGISTER(class_multiple_final_D, 19)

}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

namespace ns_foo
{

/////////////////////////////////////////////////////////////////////////////////////////

method_class::method_class()
{

}

/////////////////////////////////////////////////////////////////////////////////////////

method_class::~method_class()
{
}

/////////////////////////////////////////////////////////////////////////////////////////

void method_class::method_0()
{
}

/////////////////////////////////////////////////////////////////////////////////////////

void method_class::method_v()
{
}

/////////////////////////////////////////////////////////////////////////////////////////

void method_class::void_method()
{
    method_void_called = true;
}

/////////////////////////////////////////////////////////////////////////////////////////

int method_class::int_method()
{
    return 42;
}

/////////////////////////////////////////////////////////////////////////////////////////

std::string method_class::string_method()
{
    return std::string("some text");
}

/////////////////////////////////////////////////////////////////////////////////////////

void method_class::void_method_arg_1(int arg_1)
{
    value_1 = arg_1;
}

/////////////////////////////////////////////////////////////////////////////////////////

void method_class::void_method_arg_2(int arg_1, int arg_2)
{
    value_1 = arg_1;
    value_2 = arg_2;
}

/////////////////////////////////////////////////////////////////////////////////////////

void method_class::void_method_arg_3(int arg_1, int arg_2, int arg_3)
{
    value_1 = arg_1;
    value_2 = arg_2;
    value_3 = arg_3;
}

/////////////////////////////////////////////////////////////////////////////////////////

void method_class::void_method_arg_8(int arg_1, int arg_2, int arg_3,
                                     int arg_4, int arg_5, int arg_6,
                                     int arg_7, int arg_8)
{
    value_1 = arg_1;
    value_2 = arg_2;
    value_3 = arg_3;
    value_4 = arg_4;
    value_5 = arg_5;
    value_6 = arg_6;
    value_7 = arg_7;
    value_8 = arg_8;
}

/////////////////////////////////////////////////////////////////////////////////////////

void method_class::method_ptr_arg(int* ptr)
{
}

/////////////////////////////////////////////////////////////////////////////////////////

}
