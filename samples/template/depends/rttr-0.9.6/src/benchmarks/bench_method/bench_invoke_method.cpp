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

#include <rttr/type>

#include <nonius/nonius.h++>
#include <nonius/html_group_reporter.h>


/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

nonius::benchmark bench_native_invoke_method_void_return_value()
{
    return nonius::benchmark("native", [](nonius::chronometer meter)
    {
        ns_foo::method_class obj;
        meter.measure([&]()
        {
            obj.method_0();
        });
    });
}

/////////////////////////////////////////////////////////////////////////////////////////

nonius::benchmark bench_rttr_invoke_method_void_return_value()
{
    return nonius::benchmark("rttr", [](nonius::chronometer meter)
    {
        ns_foo::method_class obj;
        rttr::method m = rttr::type::get(obj).get_method("method_0");

        meter.measure([&]()
        {
            return m.invoke(obj);
        });
    });
}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

nonius::benchmark bench_native_invoke_method_int_return_value()
{
    return nonius::benchmark("native", [](nonius::chronometer meter)
    {
        ns_foo::method_class obj;

        meter.measure([&]()
        {
            return obj.method_0();
        });
    });
}

/////////////////////////////////////////////////////////////////////////////////////////

nonius::benchmark bench_rttr_invoke_method_int_return_value()
{
    return nonius::benchmark("rttr", [](nonius::chronometer meter)
    {
        ns_foo::method_class obj;
        rttr::method m = rttr::type::get(obj).get_method("int_method");

        meter.measure([&]()
        {
            return m.invoke(obj);
        });
    });
}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

nonius::benchmark bench_native_invoke_method_string_return_value()
{
    return nonius::benchmark("native", [](nonius::chronometer meter)
    {
        ns_foo::method_class obj;

        meter.measure([&]()
        {
            return obj.string_method();
        });
    });
}

/////////////////////////////////////////////////////////////////////////////////////////

nonius::benchmark bench_rttr_invoke_method_string_return_value()
{
    return nonius::benchmark("rttr", [](nonius::chronometer meter)
    {
        ns_foo::method_class obj;
        rttr::method m = rttr::type::get(obj).get_method("string_method");

        meter.measure([&]()
        {
            return m.invoke(obj);
        });
    });
}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

nonius::benchmark bench_native_invoke_method_arg_1()
{
    return nonius::benchmark("native", [](nonius::chronometer meter)
    {
        ns_foo::method_class obj;

        meter.measure([&]()
        {
            return obj.void_method_arg_1(23);
        });
    });
}

/////////////////////////////////////////////////////////////////////////////////////////

nonius::benchmark bench_rttr_invoke_method_arg_1()
{
    return nonius::benchmark("rttr", [](nonius::chronometer meter)
    {
        ns_foo::method_class obj;
        rttr::method m = rttr::type::get(obj).get_method("void_method_arg_1");

        meter.measure([&]()
        {
            return m.invoke(obj, 23);
        });
    });
}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

nonius::benchmark bench_native_invoke_method_arg_2()
{
    return nonius::benchmark("native", [](nonius::chronometer meter)
    {
        ns_foo::method_class obj;

        meter.measure([&]()
        {
            return obj.void_method_arg_2(23, 42);
        });
    });
}

/////////////////////////////////////////////////////////////////////////////////////////

nonius::benchmark bench_rttr_invoke_method_arg_2()
{
    return nonius::benchmark("rttr", [](nonius::chronometer meter)
    {
        ns_foo::method_class obj;
        rttr::method m = rttr::type::get(obj).get_method("void_method_arg_2");

        meter.measure([&]()
        {
            return m.invoke(obj, 23, 42);
        });
    });
}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

nonius::benchmark bench_native_invoke_method_arg_3()
{
    return nonius::benchmark("native", [](nonius::chronometer meter)
    {
        ns_foo::method_class obj;

        meter.measure([&]()
        {
            return obj.void_method_arg_3(23, 42, 5);
        });
    });
}

/////////////////////////////////////////////////////////////////////////////////////////

nonius::benchmark bench_rttr_invoke_method_arg_3()
{
    return nonius::benchmark("rttr", [](nonius::chronometer meter)
    {
        ns_foo::method_class obj;
        rttr::method m = rttr::type::get(obj).get_method("void_method_arg_3");

        meter.measure([&]()
        {
            return m.invoke(obj, 23, 42, 5);
        });
    });
}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

nonius::benchmark bench_native_invoke_method_arg_8()
{
    return nonius::benchmark("native", [](nonius::chronometer meter)
    {
        ns_foo::method_class obj;

        meter.measure([&]()
        {
            return obj.void_method_arg_8(1, 2, 3, 4, 5, 6, 7, 8);
        });
    });
}

/////////////////////////////////////////////////////////////////////////////////////////

nonius::benchmark bench_rttr_invoke_method_arg_8()
{
    return nonius::benchmark("rttr", [](nonius::chronometer meter)
    {
        ns_foo::method_class obj;
        rttr::method m = rttr::type::get(obj).get_method("void_method_arg_8");
        std::vector<rttr::argument> args = {1, 2, 3, 4, 5, 6, 7, 8};
        meter.measure([&]()
        {
            return m.invoke_variadic(obj, args);
        });
    });
}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

nonius::benchmark bench_native_invoke_method_ptr_arg()
{
    return nonius::benchmark("native", [](nonius::chronometer meter)
    {
        ns_foo::method_class obj;
       int* ptr = nullptr;
        meter.measure([&]()
        {
            return obj.method_ptr_arg(ptr);
        });
    });
}

/////////////////////////////////////////////////////////////////////////////////////////

nonius::benchmark bench_rttr_invoke_method_ptr_arg()
{
    return nonius::benchmark("rttr", [](nonius::chronometer meter)
    {
        ns_foo::method_class obj;
        rttr::method m = rttr::type::get(obj).get_method("method_ptr_arg");
        int* ptr = nullptr;
        meter.measure([&]()
        {
            return m.invoke(obj, ptr);
        });
    });
}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

void bench_invoke_method()
{
    nonius::configuration cfg;
    cfg.title = "invoke member method";

    nonius::html_group_reporter reporter;
    reporter.set_output_file("benchmark_invoke_method.html");

    /////////////////////////////////////

    reporter.set_current_group_name("void", "benchmark code:"
                                            "<pre>"
                                            "// Foo.h\n"
                                            "struct Foo {\n"
                                            "    void method();\n"
                                            "};\n"
                                            "// Foo.cpp\n"
                                            "void Foo::method() {}\n"
                                            "\n"
                                            "// Setup\n"
                                            "Foo obj;\n"
                                            "rttr::method m = rttr::type::get(obj).get_method(\"method\");\n"
                                            "// Benchmarking:\n\n"
                                            "// native approach\n"
                                            "obj.method();\n"
                                            "\n"
                                            "// rttr approach\n"
                                            "m.invoke(obj);\n"
                                            "</pre>");
    nonius::benchmark benchmarks_group_1[] = { bench_native_invoke_method_void_return_value(),
                                               bench_rttr_invoke_method_void_return_value()};
    // for unknown reason we have the run a pre-benchmark, otherwise we get wrong result (all samples are at: 0ns)
    auto dummy_benchmark = nonius::benchmark_registry{bench_rttr_invoke_method_void_return_value()};
    nonius::go(cfg, dummy_benchmark);
    nonius::go(cfg, std::begin(benchmarks_group_1), std::end(benchmarks_group_1), reporter);

    /////////////////////////////////////

    reporter.set_current_group_name("int", "benchmark code:"
                                            "<pre>"
                                            "// Foo.h\n"
                                            "struct Foo {\n"
                                            "    int method();\n"
                                            "};\n"
                                            "// Foo.cpp\n"
                                            "int Foo::method()\n"
                                            "{ return 42; }\n"
                                            "\n"
                                            "// Setup\n"
                                            "Foo obj;\n"
                                            "rttr::method m = rttr::type::get(obj).get_method(\"method\");\n"
                                            "// Benchmarking:\n\n"
                                            "// native approach\n"
                                            "obj.method();\n"
                                            "\n"
                                            "// rttr approach\n"
                                            "m.invoke(obj);\n"
                                            "</pre>");
    nonius::benchmark benchmarks_group_2[] = { bench_native_invoke_method_int_return_value(),
                                               bench_rttr_invoke_method_int_return_value()};

    nonius::go(cfg, std::begin(benchmarks_group_2), std::end(benchmarks_group_2), reporter);

    /////////////////////////////////////

    reporter.set_current_group_name("std::string", "benchmark code:"
                                                   "<pre>"
                                                   "// Foo.h\n"
                                                   "struct Foo {\n"
                                                   "    std::string method();\n"
                                                   "};\n"
                                                   "// Foo.cpp\n"
                                                   "std::string Foo::method()\n"
                                                   "{ return std::string(\"some text\"); }\n"
                                                   "\n"
                                                   "// Setup\n"
                                                   "Foo obj;\n"
                                                   "rttr::method m = rttr::type::get(obj).get_method(\"method\");\n"
                                                   "// Benchmarking:\n\n"
                                                   "// native approach\n"
                                                   "obj.method();\n"
                                                   "\n"
                                                   "// rttr approach\n"
                                                   "m.invoke(obj);\n"
                                                   "</pre>");
    nonius::benchmark benchmarks_group_3[] = { bench_native_invoke_method_string_return_value(),
                                               bench_rttr_invoke_method_string_return_value()};

    nonius::go(cfg, std::begin(benchmarks_group_3), std::end(benchmarks_group_3), reporter);

    /////////////////////////////////////

    reporter.set_current_group_name("arg 1", "benchmark code:"
                                              "<pre>"
                                              "// Foo.h\n"
                                              "struct Foo {\n"
                                              "    void method(int arg_1);\n"
                                              "int value_1;\n"
                                              "};\n"
                                              "// Foo.cpp\n"
                                              "void Foo::method(int arg_1)\n"
                                              "{ value_1 = arg_1; }\n"
                                              "\n"
                                              "// Setup\n"
                                              "Foo obj;\n"
                                              "rttr::method m = rttr::type::get(obj).get_method(\"method\");\n"
                                              "// Benchmarking:\n\n"
                                              "// native approach\n"
                                              "obj.method(42);\n"
                                              "\n"
                                              "// rttr approach\n"
                                              "m.invoke(obj, 42);\n"
                                              "</pre>");
    nonius::benchmark benchmarks_group_4[] = { bench_native_invoke_method_arg_1(),
                                               bench_rttr_invoke_method_arg_1()};

    nonius::go(cfg, std::begin(benchmarks_group_4), std::end(benchmarks_group_4), reporter);

    /////////////////////////////////////

    reporter.set_current_group_name("arg 2", "benchmark code:"
                                              "<pre>"
                                              "// Foo.h\n"
                                              "struct Foo {\n"
                                              "    void method(int arg_1, int arg_2);\n"
                                              "    int value_1; int value_2;\n"
                                              "};\n"
                                              "// Foo.cpp\n"
                                              "void Foo::method(int arg_1, int arg_2)\n"
                                              "{ value_1 = arg_1; value_2 = arg_2; }\n"
                                              "\n"
                                              "// Setup\n"
                                              "Foo obj;\n"
                                              "rttr::method m = rttr::type::get(obj).get_method(\"method\");\n"
                                              "// Benchmarking:\n\n"
                                              "// native approach\n"
                                              "obj.method(42, 23);\n"
                                              "\n"
                                              "// rttr approach\n"
                                              "m.invoke(obj, 42, 23);\n"
                                              "</pre>");
    nonius::benchmark benchmarks_group_5[] = { bench_native_invoke_method_arg_2(),
                                               bench_rttr_invoke_method_arg_2()};

    nonius::go(cfg, std::begin(benchmarks_group_5), std::end(benchmarks_group_5), reporter);

    /////////////////////////////////////

    reporter.set_current_group_name("arg 3", "benchmark code:"
                                              "<pre>"
                                              "// Foo.h\n"
                                              "struct Foo {\n"
                                              "    void method(int arg_1, int arg_2, int arg_3);\n"
                                              "    int value_1; int value_2; int value_3;\n"
                                              "};\n"
                                              "// Foo.cpp\n"
                                              "void Foo::method(int arg_1, int arg_2, int arg_3)\n"
                                              "{ value_1 = arg_1; value_2 = arg_2; value_3 = arg_3; }\n"
                                              "\n"
                                              "// Setup\n"
                                              "Foo obj;\n"
                                              "rttr::method m = rttr::type::get(obj).get_method(\"method\");\n"
                                              "// Benchmarking:\n\n"
                                              "// native approach\n"
                                              "obj.method(42, 23, 5);\n"
                                              "\n"
                                              "// rttr approach\n"
                                              "m.invoke(obj, 42, 23, 5);\n"
                                              "</pre>");
    nonius::benchmark benchmarks_group_6[] = { bench_native_invoke_method_arg_3(),
                                               bench_rttr_invoke_method_arg_3()};

    nonius::go(cfg, std::begin(benchmarks_group_6), std::end(benchmarks_group_6), reporter);

    /////////////////////////////////////

    reporter.set_current_group_name("arg 8", "benchmark code:"
                                              "<pre>"
                                              "// Foo.h\n"
                                              "struct Foo {\n"
                                              "    void method(int arg_1, int arg_2, int arg_3,\n"
                                              "                ..., int arg_8);\n"
                                              "    int value_1; int value_2; int value_3;\n"
                                              "     ...; int value_8;\n"
                                              "};\n"
                                              "// Foo.cpp\n"
                                              "void Foo::method(int arg_1, int arg_2, int arg_3,\n"
                                              "                 ..., int arg_8);\n"
                                              "{ value_1 = arg_1; value_2 = arg_2; value_3 = arg_3;\n"
                                              "   ... value_8 = arg_8;}\n"
                                              "\n"
                                              "// Setup\n"
                                              "Foo obj;\n"
                                              "rttr::method m = rttr::type::get(obj).get_method(\"method\");\n"
                                              "std::vector<argument> args = {1, 2, 3, 4, 5, 6, 7, 8}\n"
                                              "// Benchmarking:\n\n"
                                              "// native approach\n"
                                              "obj.method(1, 2, 3, 4, 5, 6, 7, 8);\n"
                                              "\n"
                                              "// rttr approach\n"
                                              "m.invoke_variadic(obj, args);\n"
                                              "</pre>");
    nonius::benchmark benchmarks_group_7[] = { bench_native_invoke_method_arg_8(),
                                               bench_rttr_invoke_method_arg_8()};

    nonius::go(cfg, std::begin(benchmarks_group_7), std::end(benchmarks_group_7), reporter);


    /////////////////////////////////////

    reporter.set_current_group_name("nullptr",  "benchmark code:"
                                                "<pre>"
                                                "// Foo.h\n"
                                                "struct Foo {\n"
                                                "    void method(int* ptr);\n"
                                                "};\n"
                                                "// Foo.cpp\n"
                                                "void Foo::method(int* ptr) {}\n"
                                                "\n"
                                                "// Setup\n"
                                                "Foo obj;\n"
                                                "rttr::method m = rttr::type::get(obj).get_method(\"method\");\n"
                                                "// Benchmarking:\n\n"
                                                "// native approach\n"
                                                "obj.method(nullptr);\n"
                                                "\n"
                                                "// rttr approach\n"
                                                "m.invoke(obj, nullptr);\n"
                                                "</pre>");
    nonius::benchmark benchmarks_group_8[] = { bench_native_invoke_method_ptr_arg(),
                                               bench_rttr_invoke_method_ptr_arg()};

    nonius::go(cfg, dummy_benchmark);
    nonius::go(cfg, std::begin(benchmarks_group_8), std::end(benchmarks_group_8), reporter);

    /////////////////////////////////////

    reporter.generate_report();
}
