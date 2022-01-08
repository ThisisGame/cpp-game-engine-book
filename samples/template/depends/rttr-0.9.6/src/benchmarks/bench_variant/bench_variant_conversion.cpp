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

#include <rttr/type>
#include <rttr/registration>

#include <nonius/nonius.h++>
#include <nonius/html_group_reporter.h>

#include <locale>

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

static int setup_integer()
{
    return 12345;
}

/////////////////////////////////////////////////////////////////////////////////////////

static float setup_float()
{
    return 123.12345f;
}

/////////////////////////////////////////////////////////////////////////////////////////

static double setup_double()
{
    return 123456.123456;
}

/////////////////////////////////////////////////////////////////////////////////////////

static bool setup_bool()
{
    return true;
}

/////////////////////////////////////////////////////////////////////////////////////////

static std::string setup_string_integer()
{
    return std::string("12345");
}

/////////////////////////////////////////////////////////////////////////////////////////

static std::string setup_string_float()
{
    return std::string("123.12345");
}

/////////////////////////////////////////////////////////////////////////////////////////

static std::string setup_string_double()
{
    return std::string("123456.123456");
}

/////////////////////////////////////////////////////////////////////////////////////////

static std::string setup_string_bool()
{
    return std::string("1");
}


/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

nonius::benchmark bench_native_int_to_string()
{
    return nonius::benchmark("native", [](nonius::chronometer meter)
    {
        int var = setup_integer();
        std::string result;
        meter.measure([&]()
        {
            result = std::to_string(var);
        });
    });
}

/////////////////////////////////////////////////////////////////////////////////////////

nonius::benchmark bench_variant_int_to_string()
{
    return nonius::benchmark("rttr::variant", [](nonius::chronometer meter)
    {
        rttr::variant var = setup_integer();
        std::string result;
        meter.measure([&]()
        {
            result = var.to_string();
        });
    });
}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

nonius::benchmark bench_native_string_to_int()
{
    return nonius::benchmark("native", [](nonius::chronometer meter)
    {
        std::string number_string = setup_string_integer();
        int result = 0;
        meter.measure([&]()
        {
            std::size_t pos = 0;
            result = std::stoi(number_string, &pos);
        });
    });
}

/////////////////////////////////////////////////////////////////////////////////////////

nonius::benchmark bench_variant_string_to_int()
{
    return nonius::benchmark("rttr::variant", [](nonius::chronometer meter)
    {
        rttr::variant var = setup_string_integer();
        int result = 0;
        meter.measure([&]()
        {
            result = var.to_int();
        });
    });
}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

nonius::benchmark bench_native_float_to_string()
{
    return nonius::benchmark("native", [](nonius::chronometer meter)
    {
        float var = setup_float();
        std::string result;
        meter.measure([&]()
        {
            result = std::to_string(var);
        });
    });
}

/////////////////////////////////////////////////////////////////////////////////////////

nonius::benchmark bench_variant_float_to_string()
{
    return nonius::benchmark("rttr::variant", [](nonius::chronometer meter)
    {
        rttr::variant var = setup_float();
        std::string result;
        meter.measure([&]()
        {
            result = var.to_string();
        });
    });
}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

nonius::benchmark bench_native_string_to_float()
{
    return nonius::benchmark("native", [](nonius::chronometer meter)
    {
        std::string number_string = setup_string_float();
        float result;
        meter.measure([&]()
        {
            std::size_t pos = 0;
            result = std::stof(number_string, &pos);
        });
    });
}

/////////////////////////////////////////////////////////////////////////////////////////

nonius::benchmark bench_variant_string_to_float()
{
    return nonius::benchmark("rttr::variant", [](nonius::chronometer meter)
    {
        rttr::variant var = setup_string_float();
        float result = 0.0f;
        meter.measure([&]()
        {
            result = var.to_float();
        });
    });
}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

nonius::benchmark bench_native_double_to_string()
{
    return nonius::benchmark("native", [](nonius::chronometer meter)
    {
        double var = setup_double();
        std::string result;
        meter.measure([&]()
        {
            result = std::to_string(var);
        });
    });
}

/////////////////////////////////////////////////////////////////////////////////////////

nonius::benchmark bench_variant_double_to_string()
{
    return nonius::benchmark("rttr::variant", [](nonius::chronometer meter)
    {
        rttr::variant var = setup_double();
        std::string result;
        meter.measure([&]()
        {
            result = var.to_string();
        });
    });
}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

nonius::benchmark bench_native_string_to_double()
{
    return nonius::benchmark("native", [](nonius::chronometer meter)
    {
        std::string number_string = setup_string_double();
        double result;
        meter.measure([&]()
        {
            std::size_t pos = 0;
            result = std::stod(number_string, &pos);
        });
    });
}

/////////////////////////////////////////////////////////////////////////////////////////

nonius::benchmark bench_variant_string_to_double()
{
    return nonius::benchmark("rttr::variant", [](nonius::chronometer meter)
    {
        rttr::variant var = setup_string_double();
        double result = 0.0;
        meter.measure([&]()
        {
            result = var.to_double();
        });
    });
}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

nonius::benchmark bench_native_bool_to_string()
{
    return nonius::benchmark("native", [](nonius::chronometer meter)
    {
        bool value = setup_bool();
        std::string result;
        meter.measure([&]()
        {
            result = value ? std::string("true") : std::string("false");
        });
    });
}

/////////////////////////////////////////////////////////////////////////////////////////

nonius::benchmark bench_variant_bool_to_string()
{
    return nonius::benchmark("rttr::variant", [](nonius::chronometer meter)
    {
        rttr::variant var = setup_bool();
        std::string result;
        meter.measure([&]()
        {
            result = var.to_string();
        });
    });
}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

static bool string_to_bool(std::string text)
{
    std::transform(text.cbegin(), text.cend(), text.begin(), ::tolower);
    text.erase( std::remove_if( text.begin(), text.end(), []( char ch ) { return std::isspace<char>( ch, std::locale::classic() ); } ), text.end() );

    return !(text == "false" || text == "0" || text.empty());
}

/////////////////////////////////////////////////////////////////////////////////////////

nonius::benchmark bench_native_string_to_bool()
{
    return nonius::benchmark("native", [](nonius::chronometer meter)
    {
        std::string bool_string = setup_string_bool();
        bool result = false;
        meter.measure([&]()
        {
            result = string_to_bool(bool_string);
        });
    });
}

/////////////////////////////////////////////////////////////////////////////////////////

nonius::benchmark bench_variant_string_to_bool()
{
    return nonius::benchmark("rttr::variant", [](nonius::chronometer meter)
    {
        rttr::variant var = setup_string_bool();
        bool result = false;
        meter.measure([&]()
        {
            result = var.to_bool();
        });
    });
}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

void bench_variant_conversion()
{
    nonius::configuration cfg;
    cfg.title = "rttr::variant conversion";

    nonius::html_group_reporter reporter;
    reporter.set_output_file("benchmark_variant_conversion.html");

    //////////////////////////////////

    reporter.set_current_group_name("int to string", "Converts an integer number to a <code>std::string</code>:<br><pre>rttr::variant var = 12345;\nvar.to_string();</pre>");

    nonius::benchmark benchmarks_group_1[] = { bench_native_int_to_string(),
                                               bench_variant_int_to_string()
                                              };

    nonius::go(cfg, std::begin(benchmarks_group_1), std::end(benchmarks_group_1), reporter);

    //////////////////////////////////

    reporter.set_current_group_name("string to int", "Converts a <code>std::string</code> to an integer:<br><pre>rttr::variant var = std::string(\"12345\");\nvar.to_int()</pre>");

    nonius::benchmark benchmarks_group_2[] = { bench_native_string_to_int(),
                                               bench_variant_string_to_int()
                                              };

    nonius::go(cfg, std::begin(benchmarks_group_2), std::end(benchmarks_group_2), reporter);

    //////////////////////////////////

    reporter.set_current_group_name("float to string", "Converts a floating point number to a <code>std::string</code>:<br><pre>rttr::variant var = 123.12345f;\nvar.to_string();</pre>");

    nonius::benchmark benchmarks_group_3[] = { bench_native_string_to_float(),
                                               bench_variant_string_to_float()
                                              };

    nonius::go(cfg, std::begin(benchmarks_group_3), std::end(benchmarks_group_3), reporter);

    //////////////////////////////////

    reporter.set_current_group_name("string to float", "Converts a <code>std::string</code> to a floating point number:<br><pre>rttr::variant var = std::string(\"123.12345\");\nvar.to_float().");

    nonius::benchmark benchmarks_group_4[] = { bench_native_float_to_string(),
                                               bench_variant_float_to_string()
                                              };

    nonius::go(cfg, std::begin(benchmarks_group_4), std::end(benchmarks_group_4), reporter);

    //////////////////////////////////

    reporter.set_current_group_name("double to string", "Converts a double number to a <code>std::string</code>:<br><pre>rttr::variant var = 123456.123456;\nvar.to_string();</pre>");

    nonius::benchmark benchmarks_group_5[] = { bench_native_string_to_double(),
                                               bench_variant_string_to_double()
                                              };

    nonius::go(cfg, std::begin(benchmarks_group_5), std::end(benchmarks_group_5), reporter);

    //////////////////////////////////

    reporter.set_current_group_name("string to double", "Converts a <code>std::string</code> to a double number:<br><pre>rttr::variant var = std::string(\"123456.123456\");\nvar.to_double();</pre>");

    nonius::benchmark benchmarks_group_6[] = { bench_native_double_to_string(),
                                               bench_variant_double_to_string()
                                              };

    nonius::go(cfg, std::begin(benchmarks_group_6), std::end(benchmarks_group_6), reporter);

    //////////////////////////////////

    reporter.set_current_group_name("bool to string", "Converts a bool number to a <code>std::string</code>:<br><pre>rttr::variant var = 123456.123456;\nvar.to_string();</pre>");

    nonius::benchmark benchmarks_group_7[] = { bench_native_string_to_bool(),
                                               bench_variant_string_to_bool()
                                              };

    nonius::go(cfg, std::begin(benchmarks_group_7), std::end(benchmarks_group_7), reporter);

    //////////////////////////////////

    reporter.set_current_group_name("string to bool", "Converts a <code>std::string</code> to a bool number:<br><pre>rttr::variant var = std::string(\"123456.123456\");\nvar.to_bool();</pre>");

    nonius::benchmark benchmarks_group_8[] = { bench_native_bool_to_string(),
                                               bench_variant_bool_to_string()
                                              };

    nonius::go(cfg, std::begin(benchmarks_group_8), std::end(benchmarks_group_8), reporter);

    //////////////////////////////////

    reporter.generate_report();
}

/////////////////////////////////////////////////////////////////////////////////////////
