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

nonius::benchmark bench_native_find_method_level_1()
{
    return nonius::benchmark("native", [](nonius::chronometer meter)
    {
        ns_foo::method_class_d_1 obj_1;
        ns_foo::method_class& obj_base = obj_1;
        volatile std::size_t value = 0;
        meter.measure([&]()
        {
            if (dynamic_cast<ns_foo::method_class_d_1*>(&obj_base))
            {
                value = 1;
            }
            return value;
        });
    });
}

/////////////////////////////////////////////////////////////////////////////////////////

nonius::benchmark bench_rttr_find_method_level_1()
{
    return nonius::benchmark("rttr", [](nonius::chronometer meter)
    {
        ns_foo::method_class_d_1 obj_1;
        ns_foo::method_class& obj_base = obj_1;
        volatile std::size_t value = 0;
        rttr::method m = rttr::type::get(nullptr).get_method("");
        meter.measure([&]()
        {
            if ((m = rttr::type::get(obj_base).get_method("method_1")))
            {
                value = 1;
            }
            return value;
        });
    });
}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

nonius::benchmark bench_native_find_method_level_3()
{
    return nonius::benchmark("native", [](nonius::chronometer meter)
    {
        ns_foo::method_class_d_3 obj_3;
        ns_foo::method_class& obj_base = obj_3;
        volatile std::size_t value = 0;
        meter.measure([&]()
        {
            if (dynamic_cast<ns_foo::method_class_d_3*>(&obj_base))
            {
                value = 1;
            }
            return value;
        });
    });
}

/////////////////////////////////////////////////////////////////////////////////////////

nonius::benchmark bench_rttr_find_method_level_3()
{
    return nonius::benchmark("rttr", [](nonius::chronometer meter)
    {
        ns_foo::method_class_d_3 obj_3;
        ns_foo::method_class& obj_base = obj_3;
        volatile std::size_t value = 0;
        rttr::method m = rttr::type::get(nullptr).get_method("");
        meter.measure([&]()
        {
            if ((m = rttr::type::get(obj_base).get_method("method_3")))
            {
                value = 1;
            }
            return value;
        });
    });
}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

nonius::benchmark bench_native_find_method_level_6()
{
    return nonius::benchmark("native", [](nonius::chronometer meter)
    {
        ns_foo::method_class_d_6 obj_6;
        ns_foo::method_class& obj_base = obj_6;
        volatile std::size_t value = 0;
        meter.measure([&]()
        {
            if (dynamic_cast<ns_foo::method_class_d_6*>(&obj_base))
            {
                value = 1;
            }
            return value;
        });
    });
}

/////////////////////////////////////////////////////////////////////////////////////////

nonius::benchmark bench_rttr_find_method_level_6()
{
    return nonius::benchmark("rttr", [](nonius::chronometer meter)
    {
        ns_foo::method_class_d_6 obj_6;
        ns_foo::method_class& obj_base = obj_6;
        volatile std::size_t value = 0;
        rttr::method m = rttr::type::get(nullptr).get_method("");
        meter.measure([&]()
        {
            if ((m = rttr::type::get(obj_base).get_method("method_6")))
            {
                value = 1;
            }
            return value;
        });
    });
}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////


nonius::benchmark bench_native_find_method_neg_level_1()
{
    return nonius::benchmark("native", [](nonius::chronometer meter)
    {
        ns_foo::method_class_d_1 obj_1;
        ns_foo::method_class& obj_base = obj_1;
        volatile std::size_t value = 0;
        meter.measure([&]()
        {
            if (dynamic_cast<ns_foo::method_class_d_2*>(&obj_base))
            {
                value = 1;
            }
            return value;
        });
    });
}

/////////////////////////////////////////////////////////////////////////////////////////

nonius::benchmark bench_rttr_find_method_neg_level_1()
{
    return nonius::benchmark("rttr", [](nonius::chronometer meter)
    {
        ns_foo::method_class_d_1 obj_1;
        ns_foo::method_class& obj_base = obj_1;
        volatile std::size_t value = 0;
        rttr::method m = rttr::type::get(nullptr).get_method("");
        meter.measure([&]()
        {
            if ((m = rttr::type::get(obj_base).get_method("method_2")))
            {
                value = 1;
            }
            return value;
        });
    });
}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

nonius::benchmark bench_native_find_method_neg_level_3()
{
    return nonius::benchmark("native", [](nonius::chronometer meter)
    {
        ns_foo::method_class_d_3 obj_3;
        ns_foo::method_class& obj_base = obj_3;
        volatile std::size_t value = 0;
        meter.measure([&]()
        {
            if (dynamic_cast<ns_foo::method_class_d_4*>(&obj_base))
            {
                value = 1;
            }
            return value;
        });
    });
}

/////////////////////////////////////////////////////////////////////////////////////////

nonius::benchmark bench_rttr_find_method_neg_level_3()
{
    return nonius::benchmark("rttr", [](nonius::chronometer meter)
    {
        ns_foo::method_class_d_3 obj_3;
        ns_foo::method_class& obj_base = obj_3;
        volatile std::size_t value = 0;
        rttr::method m = rttr::type::get(nullptr).get_method("");
        meter.measure([&]()
        {
            if ((m = rttr::type::get(obj_base).get_method("method_4")))
            {
                value = 1;
            }
            return value;
        });
    });
}


/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

nonius::benchmark bench_native_find_method_neg_level_6()
{
    return nonius::benchmark("native", [](nonius::chronometer meter)
    {
        ns_foo::method_class_d_6 obj_6;
        ns_foo::method_class& obj_base = obj_6;
        volatile std::size_t value = 0;
        meter.measure([&]()
        {
            if (dynamic_cast<ns_foo::method_class_d_7*>(&obj_base))
            {
                value = 1;
            }
            return value;
        });
    });
}

/////////////////////////////////////////////////////////////////////////////////////////

nonius::benchmark bench_rttr_find_method_neg_level_6()
{
    return nonius::benchmark("rttr", [](nonius::chronometer meter)
    {
        ns_foo::method_class_d_6 obj_6;
        ns_foo::method_class& obj_base = obj_6;
        volatile std::size_t value = 0;
        rttr::method m = rttr::type::get(nullptr).get_method("");
        meter.measure([&]()
        {
            if ((m = rttr::type::get(obj_base).get_method("method_7")))
            {
                value = 1;
            }
            return value;
        });
    });
}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

nonius::benchmark bench_native_find_method_cross_cast_level_1()
{
    return nonius::benchmark("native", [](nonius::chronometer meter)
    {
        ns_foo::class_multiple_final obj_final;
        ns_foo::class_multiple_base_A& obj_base = obj_final;
        volatile std::size_t value = 0;
        meter.measure([&]()
        {
            if (dynamic_cast<ns_foo::class_multiple_1C*>(&obj_base))
            {
                value = 1;
            }
            return value;
        });
    });
}

/////////////////////////////////////////////////////////////////////////////////////////

nonius::benchmark bench_rttr_find_method_cross_cast_level_1()
{
    return nonius::benchmark("rttr", [](nonius::chronometer meter)
    {
        ns_foo::class_multiple_final obj_final;
        ns_foo::class_multiple_base_A& obj_base = obj_final;
        volatile std::size_t value = 0;
        rttr::method m = rttr::type::get(nullptr).get_method("");
        meter.measure([&]()
        {
            if ((m = rttr::type::get(obj_base).get_method("method_13"))) //method 13 is contained in 'class_multiple_1C'
            {
                value = 1;
            }
            return value;
        });
    });
}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

nonius::benchmark bench_native_find_method_cross_cast_level_3()
{
    return nonius::benchmark("native", [](nonius::chronometer meter)
    {
        ns_foo::class_multiple_final obj_final;
        ns_foo::class_multiple_base_A& obj_base = obj_final;
        volatile std::size_t value = 0;
        meter.measure([&]()
        {
            if (dynamic_cast<ns_foo::class_multiple_3C*>(&obj_base))
            {
                value = 1;
            }
            return value;
        });
    });
}

/////////////////////////////////////////////////////////////////////////////////////////

nonius::benchmark bench_rttr_find_method_cross_cast_level_3()
{
    return nonius::benchmark("rttr", [](nonius::chronometer meter)
    {
        ns_foo::class_multiple_final obj_final;
        ns_foo::class_multiple_base_A& obj_base = obj_final;
        volatile std::size_t value = 0;
        rttr::method m = rttr::type::get(nullptr).get_method("");
        meter.measure([&]()
        {
            if ((m = rttr::type::get(obj_base).get_method("method_15"))) //method 15 is contained in 'class_multiple_3C'
            {
                value = 1;
            }
            return value;
        });
    });
}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

nonius::benchmark bench_native_find_method_cross_cast_level_6()
{
    return nonius::benchmark("native", [](nonius::chronometer meter)
    {
        ns_foo::class_multiple_final obj_final;
        ns_foo::class_multiple_base_C& obj_base = obj_final;
        volatile std::size_t value = 0;
        meter.measure([&]()
        {
            if (dynamic_cast<ns_foo::class_multiple_final*>(&obj_base))
            {
                value = 1;
            }
            return value;
        });
    });
}

/////////////////////////////////////////////////////////////////////////////////////////

nonius::benchmark bench_rttr_find_method_cross_cast_level_6()
{
    return nonius::benchmark("rttr", [](nonius::chronometer meter)
    {
        ns_foo::class_multiple_final obj_final;
        ns_foo::class_multiple_base_C& obj_base = obj_final;
        volatile std::size_t value = 0;
        rttr::method m = rttr::type::get(nullptr).get_method("");
        meter.measure([&]()
        {
            if ((m = rttr::type::get(obj_base).get_method("method_18"))) //method 18 is contained in 'class_multiple_final'
            {
                value = 1;
            }
            return value;
        });
    });
}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////


nonius::benchmark bench_native_find_method_cross_cast_neg_level_1()
{
    return nonius::benchmark("native", [](nonius::chronometer meter)
    {
        ns_foo::class_multiple_2A obj_final;
        ns_foo::class_multiple_base_A& obj_base = obj_final;
        volatile std::size_t value = 0;
        meter.measure([&]()
        {
            if (dynamic_cast<ns_foo::class_multiple_1C*>(&obj_base))
            {
                value = 1;
            }
            return value;
        });
    });
}

/////////////////////////////////////////////////////////////////////////////////////////

nonius::benchmark bench_rttr_find_method_cross_cast_neg_level_1()
{
    return nonius::benchmark("rttr", [](nonius::chronometer meter)
    {
        ns_foo::class_multiple_2A obj_final;
        ns_foo::class_multiple_base_A& obj_base = obj_final;
        volatile std::size_t value = 0;
        rttr::method m = rttr::type::get(nullptr).get_method("");
        meter.measure([&]()
        {
            if ((m = rttr::type::get(obj_base).get_method("method_13"))) //method 13 is contained in 'class_multiple_1C'
            {
                value = 1;
            }
            return value;
        });
    });
}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

nonius::benchmark bench_native_find_method_cross_cast_neg_level_3()
{
    return nonius::benchmark("native", [](nonius::chronometer meter)
    {
        ns_foo::class_multiple_3A obj_final;
        ns_foo::class_multiple_base_A& obj_base = obj_final;
        volatile std::size_t value = 0;
        meter.measure([&]()
        {
            if (dynamic_cast<ns_foo::class_multiple_3C*>(&obj_base))
            {
                value = 1;
            }
            return value;
        });
    });
}

/////////////////////////////////////////////////////////////////////////////////////////

nonius::benchmark bench_rttr_find_method_cross_cast_neg_level_3()
{
    return nonius::benchmark("rttr", [](nonius::chronometer meter)
    {
        ns_foo::class_multiple_3A obj_final;
        ns_foo::class_multiple_base_A& obj_base = obj_final;
        volatile std::size_t value = 0;
        rttr::method m = rttr::type::get(nullptr).get_method("");
        meter.measure([&]()
        {
            if ((m = rttr::type::get(obj_base).get_method("method_15"))) //method 15 is contained in 'class_multiple_3C'
            {
                value = 1;
            }
            return value;
        });
    });
}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

nonius::benchmark bench_native_find_method_cross_cast_neg_level_6()
{
    return nonius::benchmark("native", [](nonius::chronometer meter)
    {
        ns_foo::class_multiple_final obj_final;
        ns_foo::class_multiple_base_C& obj_base = obj_final;
        volatile std::size_t value = 0;
        meter.measure([&]()
        {
            if (dynamic_cast<ns_foo::class_multiple_final_D*>(&obj_base))
            {
                value = 1;
            }
            return value;
        });
    });
}

/////////////////////////////////////////////////////////////////////////////////////////

nonius::benchmark bench_rttr_find_method_cross_cast_neg_level_6()
{
    return nonius::benchmark("rttr", [](nonius::chronometer meter)
    {
        ns_foo::class_multiple_final obj_final;
        ns_foo::class_multiple_base_C& obj_base = obj_final;
        volatile std::size_t value = 0;
        rttr::method m = rttr::type::get(nullptr).get_method("");
        meter.measure([&]()
        {
            if ((m = rttr::type::get(obj_base).get_method("method_19"))) //method 19 is contained in 'class_multiple_final_D'
            {
                value = 1;
            }
            return value;
        });
    });
}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

void bench_find_method()
{
    nonius::configuration cfg;
    cfg.title = "find member method";

    nonius::html_group_reporter reporter;
    reporter.set_output_file("benchmark_find_method.html");

    /////////////////////////////////////

    reporter.set_current_group_name("single hierarchy - Lvl. 1", "benchmark code:"
                                                                 "<pre>"
                                                                 "struct Base { virtual ~Base(){} };\n"
                                                                 "struct Derived_1 : Base { void method_1(){} };\n"
                                                                 "\n"
                                                                 "Derived_1 obj; Base& obj_base = obj;"
                                                                 "\n"
                                                                 "// native approach\n"
                                                                 "if (Derived_1* obj = dynamic_cast<Derived_1*>(&obj_base))\n"
                                                                 "    return true;\n"
                                                                 "\n"
                                                                 "// rttr approach\n"
                                                                 "if (rttr::type::get(obj_base).get_method(\"method_1\"))\n"
                                                                 "    return true;\n"
                                                                 "</pre>");
    nonius::benchmark benchmarks_group_1[] = { bench_native_find_method_level_1(),
                                               bench_rttr_find_method_level_1()};

    nonius::go(cfg, std::begin(benchmarks_group_1), std::end(benchmarks_group_1), reporter);

    /////////////////////////////////////

    reporter.set_current_group_name("single hierarchy - Lvl. 3", "benchmark code:"
                                                                 "<pre>"
                                                                 "struct Base { virtual ~Base(){} };\n"
                                                                 "...\n"
                                                                 "struct Derived_3 : Derived_2 { void method_3(){} };\n"
                                                                 "\n"
                                                                 "Derived_3 obj; Base& obj_base = obj;"
                                                                 "\n"
                                                                 "// native approach\n"
                                                                 "if (Derived_3* obj = dynamic_cast<Derived_3*>(&obj_base))\n"
                                                                 "    return true;\n"
                                                                 "\n"
                                                                 "// rttr approach\n"
                                                                 "if (rttr::type::get(obj_base).get_method(\"method_3\"))\n"
                                                                 "    return true;\n"
                                                                 "</pre>");
    nonius::benchmark benchmarks_group_2[] = { bench_native_find_method_level_3(),
                                               bench_rttr_find_method_level_3()};

    nonius::go(cfg, std::begin(benchmarks_group_2), std::end(benchmarks_group_2), reporter);

    /////////////////////////////////////

    reporter.set_current_group_name("single hierarchy - Lvl. 6", "benchmark code:"
                                                                  "<pre>"
                                                                  "struct Base { virtual ~Base(){} };\n"
                                                                  "...\n"
                                                                  "struct Derived_6 : Derived_5 { void method_6(){} };\n"
                                                                  "\n"
                                                                  "Derived_6 obj; Base& obj_base = obj;"
                                                                  "\n"
                                                                  "// native approach\n"
                                                                  "if (Derived_6* obj = dynamic_cast<Derived_6*>(&obj_base))\n"
                                                                  "    return true;\n"
                                                                  "\n"
                                                                  "// rttr approach\n"
                                                                  "if (rttr::type::get(obj_base).get_method(\"method_6\"))\n"
                                                                  "    return true;\n"
                                                                  "</pre>");
    nonius::benchmark benchmarks_group_3[] = { bench_native_find_method_level_6(),
                                               bench_rttr_find_method_level_6()};

    nonius::go(cfg, std::begin(benchmarks_group_3), std::end(benchmarks_group_3), reporter);

    /////////////////////////////////////
    /////////////////////////////////////

    reporter.set_current_group_name("across hierarchy - Lvl. 1", "benchmark code:"
                                                                  "<pre>"
                                                                  "struct Base_A { virtual ~Base_A(){} };\n"
                                                                  "struct Base_B { virtual ~Base_B(){} };\n"
                                                                  "struct Base_C { virtual ~Base_C(){} };\n"
                                                                  "...\n"
                                                                  "struct Derived_A_1 : Base { void method_1(){} };\n"
                                                                  "struct Derived_B_1 : Base { void method_2(){} };\n"
                                                                  "struct Derived_C_1 : Base { void method_3(){} };\n"
                                                                  "...\n"
                                                                  "struct Derived_A_5 : Base { void method_13(){} };\n"
                                                                  "struct Derived_B_5 : Base { void method_14(){} };\n"
                                                                  "struct Derived_C_5 : Base { void method_15(){} };\n"
                                                                  "...\n"
                                                                  "struct Final : Derived_A_5, Derived_B_5, Derived_C_5 { void method_16(){} };\n"
                                                                  "\n"
                                                                  "Final obj; Base_A& obj_base_a = obj;"
                                                                  "\n"
                                                                  "// native approach\n"
                                                                  "if (Derived_C_1* obj = dynamic_cast<Derived_C_1*>(&obj_base_a))\n"
                                                                  "    return true;\n"
                                                                  "\n"
                                                                  "// rttr approach\n"
                                                                  "if (rttr::type::get(obj_base_a).get_method(\"method_3\"))\n"
                                                                  "    return true;\n"
                                                                  "</pre>");

    nonius::benchmark benchmarks_group_4[] = { bench_native_find_method_cross_cast_level_1(),
                                               bench_rttr_find_method_cross_cast_level_1()};

    nonius::go(cfg, std::begin(benchmarks_group_4), std::end(benchmarks_group_4), reporter);

    /////////////////////////////////////

    reporter.set_current_group_name("across hierarchy - Lvl. 3", "benchmark code:"
                                                                  "<pre>"
                                                                  "struct Base_A { virtual ~Base_A(){} };\n"
                                                                  "struct Base_B { virtual ~Base_B(){} };\n"
                                                                  "struct Base_C { virtual ~Base_C(){} };\n"
                                                                  "...\n"
                                                                  "struct Derived_A_1 : Base { void method_1(){} };\n"
                                                                  "struct Derived_B_1 : Base { void method_2(){} };\n"
                                                                  "struct Derived_C_1 : Base { void method_3(){} };\n"
                                                                  "...\n"
                                                                  "struct Derived_A_5 : Base { void method_13(){} };\n"
                                                                  "struct Derived_B_5 : Base { void method_14(){} };\n"
                                                                  "struct Derived_C_5 : Base { void method_15(){} };\n"
                                                                  "...\n"
                                                                  "struct Final : Derived_A_5, Derived_B_5, Derived_C_5 { void method_16(){} };\n"
                                                                  "\n"
                                                                  "Final obj; Base_A& obj_base_a = obj;"
                                                                  "\n"
                                                                  "// native approach\n"
                                                                  "if (Derived_C_3* obj = dynamic_cast<Derived_C_3*>(&obj_base_a))\n"
                                                                  "    return true;\n"
                                                                  "\n"
                                                                  "// rttr approach\n"
                                                                  "if (rttr::type::get(obj_base_a).get_method(\"method_9\"))\n"
                                                                  "    return true;\n"
                                                                  "</pre>");

    nonius::benchmark benchmarks_group_5[] = { bench_native_find_method_cross_cast_level_3(),
                                               bench_rttr_find_method_cross_cast_level_3()};

    nonius::go(cfg, std::begin(benchmarks_group_5), std::end(benchmarks_group_5), reporter);

    /////////////////////////////////////

    reporter.set_current_group_name("across hierarchy - Lvl. 6", "benchmark code:"
                                                                  "<pre>"
                                                                  "struct Base_A { virtual ~Base_A(){} };\n"
                                                                  "struct Base_B { virtual ~Base_B(){} };\n"
                                                                  "struct Base_C { virtual ~Base_C(){} };\n"
                                                                  "...\n"
                                                                  "struct Derived_A_1 : Base { void method_1(){} };\n"
                                                                  "struct Derived_B_1 : Base { void method_2(){} };\n"
                                                                  "struct Derived_C_1 : Base { void method_3(){} };\n"
                                                                  "...\n"
                                                                  "struct Derived_A_5 : Base { void method_13(){} };\n"
                                                                  "struct Derived_B_5 : Base { void method_14(){} };\n"
                                                                  "struct Derived_C_5 : Base { void method_15(){} };\n"
                                                                  "...\n"
                                                                  "struct Final : Derived_A_5, Derived_B_5, Derived_C_5 { void method_16(){} };\n"
                                                                  "\n"
                                                                  "Final obj; Base_C& obj_base_c = obj;"
                                                                  "\n"
                                                                  "// native approach\n"
                                                                  "if (Final* obj = dynamic_cast<Final*>(&obj_base_c))\n"
                                                                  "    return true;\n"
                                                                  "\n"
                                                                  "// rttr approach\n"
                                                                  "if (rttr::type::get(obj_base_c).get_method(\"method_16\"))\n"
                                                                  "    return true;\n"
                                                                  "</pre>");

    nonius::benchmark benchmarks_group_6[] = { bench_native_find_method_cross_cast_level_6(),
                                               bench_rttr_find_method_cross_cast_level_6()};

    nonius::go(cfg, std::begin(benchmarks_group_6), std::end(benchmarks_group_6), reporter);

    /////////////////////////////////////////
    ////////////////////////////////////////
    // negative test - methods could not be found or cast was not successfull

    reporter.set_current_group_name("neg. single hierarchy - Lvl. 1", "benchmark code:"
                                                                      "<pre>"
                                                                      "struct Base { virtual ~Base(){} };\n"
                                                                      "struct Derived_1 : Base { void method_1(){} };\n"
                                                                      "struct Derived_2 : Base { void method_2(){} };\n"
                                                                      "\n"
                                                                      "Derived_1 obj; Base& obj_base = obj;"
                                                                      "\n"
                                                                      "// native approach; cast will not be successfull\n"
                                                                      "if (Derived_2* obj = dynamic_cast<Derived_2*>(&obj_base))\n"
                                                                      "    return true;\n"
                                                                      "\n"
                                                                      "// rttr approach; retrieve of method will not be successfull\n"
                                                                      "if (rttr::type::get(obj_base).get_method(\"method_2\"))\n"
                                                                      "    return true;\n"
                                                                      "</pre>");

    nonius::benchmark benchmarks_group_1_neg[] = { bench_native_find_method_neg_level_1(),
                                                   bench_rttr_find_method_neg_level_1()};

    nonius::go(cfg, std::begin(benchmarks_group_1_neg), std::end(benchmarks_group_1_neg), reporter);

    /////////////////////////////////////

    reporter.set_current_group_name("neg. single hierarchy - Lvl. 3", "benchmark code:"
                                                                      "<pre>"
                                                                      "struct Base { virtual ~Base(){} };\n"
                                                                      "struct Derived_1 : Base { void method_1(){} };\n"
                                                                      "...\n"
                                                                      "struct Derived_3 : Base { void method_3(){} };\n"
                                                                      "\n"
                                                                      "Derived_2 obj; Base& obj_base = obj;"
                                                                      "\n"
                                                                      "// native approach; cast will not be successfull\n"
                                                                      "if (Derived_3* obj = dynamic_cast<Derived_3*>(&obj_base))\n"
                                                                      "    return true;\n"
                                                                      "\n"
                                                                      "// rttr approach; retrieve of method will not be successfull\n"
                                                                      "if (rttr::type::get(obj_base).get_method(\"method_3\"))\n"
                                                                      "    return true;\n"
                                                                      "</pre>");
    nonius::benchmark benchmarks_group_2_neg[] = { bench_native_find_method_neg_level_3(),
                                                   bench_rttr_find_method_neg_level_3()};

    nonius::go(cfg, std::begin(benchmarks_group_2_neg), std::end(benchmarks_group_2_neg), reporter);

    /////////////////////////////////////

    reporter.set_current_group_name("neg. single hierarchy - Lvl. 6", "benchmark code:"
                                                                      "<pre>"
                                                                      "struct Base { virtual ~Base(){} };\n"
                                                                      "struct Derived_1 : Base { void method_1(){} };\n"
                                                                      "...\n"
                                                                      "struct Derived_7 : Base { void method_7(){} };\n"
                                                                      "\n"
                                                                      "Derived_6 obj; Base& obj_base = obj;"
                                                                      "\n"
                                                                      "// native approach; cast will not be successfull\n"
                                                                      "if (Derived_7* obj = dynamic_cast<Derived_7*>(&obj_base))\n"
                                                                      "    return true;\n"
                                                                      "\n"
                                                                      "// rttr approach; retrieve of method will not be successfull\n"
                                                                      "if (rttr::type::get(obj_base).get_method(\"method_7\"))\n"
                                                                      "    return true;\n"
                                                                      "</pre>");

    nonius::benchmark benchmarks_group_3_neg[] = { bench_native_find_method_neg_level_6(),
                                                   bench_rttr_find_method_neg_level_6()};

    nonius::go(cfg, std::begin(benchmarks_group_3_neg), std::end(benchmarks_group_3_neg), reporter);

    /////////////////////////////////////
    /////////////////////////////////////

    reporter.set_current_group_name("neg. across hierarchy - Lvl. 1", "benchmark code:"
                                                                      "<pre>"
                                                                      "struct Base_A { virtual ~Base_A(){} };\n"
                                                                      "struct Base_B { virtual ~Base_B(){} };\n"
                                                                      "struct Base_C { virtual ~Base_C(){} };\n"
                                                                      "...\n"
                                                                      "struct Derived_A_1 : Base { void method_1(){} };\n"
                                                                      "struct Derived_B_1 : Base { void method_2(){} };\n"
                                                                      "struct Derived_C_1 : Base { void method_3(){} };\n"
                                                                      "...\n"
                                                                      "struct Derived_A_5 : Base { void method_13(){} };\n"
                                                                      "struct Derived_B_5 : Base { void method_14(){} };\n"
                                                                      "struct Derived_C_5 : Base { void method_15(){} };\n"
                                                                      "...\n"
                                                                      "struct Final : Derived_A_5, Derived_B_5, Derived_C_5 { void method_16(){} };\n"
                                                                      "\n"
                                                                      "Derived_A_2 obj; Base_A& obj_base_a = obj;"
                                                                      "\n"
                                                                      "// native approach\n"
                                                                      "if (Derived_C_1* obj = dynamic_cast<Derived_C_1*>(&obj_base_a))\n"
                                                                      "    return true;\n"
                                                                      "\n"
                                                                      "// rttr approach\n"
                                                                      "if (rttr::type::get(obj_base_a).get_method(\"method_3\"))\n"
                                                                      "    return true;\n"
                                                                      "</pre>");

    nonius::benchmark benchmarks_group_4_neg[] = { bench_native_find_method_cross_cast_neg_level_1(),
                                                   bench_rttr_find_method_cross_cast_neg_level_1()};

    nonius::go(cfg, std::begin(benchmarks_group_4_neg), std::end(benchmarks_group_4_neg), reporter);

    /////////////////////////////////////

    reporter.set_current_group_name("neg. across hierarchy - Lvl. 3", "benchmark code:"
                                                                      "<pre>"
                                                                      "struct Base_A { virtual ~Base_A(){} };\n"
                                                                      "struct Base_B { virtual ~Base_B(){} };\n"
                                                                      "struct Base_C { virtual ~Base_C(){} };\n"
                                                                      "...\n"
                                                                      "struct Derived_A_1 : Base { void method_1(){} };\n"
                                                                      "struct Derived_B_1 : Base { void method_2(){} };\n"
                                                                      "struct Derived_C_1 : Base { void method_3(){} };\n"
                                                                      "...\n"
                                                                      "struct Derived_A_5 : Base { void method_13(){} };\n"
                                                                      "struct Derived_B_5 : Base { void method_14(){} };\n"
                                                                      "struct Derived_C_5 : Base { void method_15(){} };\n"
                                                                      "...\n"
                                                                      "struct Final : Derived_A_5, Derived_B_5, Derived_C_5 { void method_16(){} };\n"
                                                                      "\n"
                                                                      "Derived_A_4 obj; Base_A& obj_base_a = obj;"
                                                                      "\n"
                                                                      "// native approach\n"
                                                                      "if (Derived_C_3* obj = dynamic_cast<Derived_C_3*>(&obj_base_a))\n"
                                                                      "    return true;\n"
                                                                      "\n"
                                                                      "// rttr approach\n"
                                                                      "if (rttr::type::get(obj_base_a).get_method(\"method_9\"))\n"
                                                                      "    return true;\n"
                                                                      "</pre>");

    nonius::benchmark benchmarks_group_5_neg[] = { bench_native_find_method_cross_cast_neg_level_3(),
                                                   bench_rttr_find_method_cross_cast_neg_level_3()};

    nonius::go(cfg, std::begin(benchmarks_group_5_neg), std::end(benchmarks_group_5_neg), reporter);

    /////////////////////////////////////

    reporter.set_current_group_name("neg. across hierarchy - Lvl. 6", "benchmark code:"
                                                                     "<pre>"
                                                                     "struct Base_A { virtual ~Base_A(){} };\n"
                                                                     "struct Base_B { virtual ~Base_B(){} };\n"
                                                                     "struct Base_C { virtual ~Base_C(){} };\n"
                                                                     "...\n"
                                                                     "struct Derived_A_1 : Base { void method_1(){} };\n"
                                                                     "struct Derived_B_1 : Base { void method_2(){} };\n"
                                                                     "struct Derived_C_1 : Base { void method_3(){} };\n"
                                                                     "...\n"
                                                                     "struct Derived_A_5 : Base { void method_13(){} };\n"
                                                                     "struct Derived_B_5 : Base { void method_14(){} };\n"
                                                                     "struct Derived_C_5 : Base { void method_15(){} };\n"
                                                                     "...\n"
                                                                     "struct Final : Derived_A_5, Derived_B_5, Derived_C_5 { void method_16(){} };\n"
                                                                     "struct Final_D : Final { void method_17(){} };\n"
                                                                     "\n"
                                                                     "Final obj; Base_C& obj_base_c = obj;"
                                                                     "\n"
                                                                     "// native approach\n"
                                                                     "if (Final_D* obj = dynamic_cast<Final_D*>(&obj_base_c))\n"
                                                                     "    return true;\n"
                                                                     "\n"
                                                                     "// rttr approach\n"
                                                                     "if (rttr::type::get(obj_base_c).get_method(\"method_17\"))\n"
                                                                     "    return true;\n"
                                                                     "</pre>");

    nonius::benchmark benchmarks_group_6_neg[] = { bench_native_find_method_cross_cast_neg_level_6(),
                                                   bench_rttr_find_method_cross_cast_neg_level_6()};

    nonius::go(cfg, std::begin(benchmarks_group_6_neg), std::end(benchmarks_group_6_neg), reporter);

    reporter.generate_report();
}
