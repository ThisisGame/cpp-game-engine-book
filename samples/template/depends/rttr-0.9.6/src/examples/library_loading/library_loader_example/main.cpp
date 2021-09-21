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

#include <cstdio>
#include <string>
#include <vector>
#include <array>
#include <map>
#include <iostream>

#include <rttr/type>

using namespace rttr;

#ifdef NDEBUG
    static string_view library_name("plugin_example");
#else
    static string_view library_name("plugin_example_d");
#endif

int main(int argc, char** argv)
{
    library lib(library_name); // load the actual plugin

    if (!lib.load())
    {
        std::cerr << lib.get_error_string() << std::endl;
        return -1;
    }

    {   // scope is important, because we have to free all data before we unload the library

        // print all classes contained in the library
        for (auto t : lib.get_types())
        {
            if (t.is_class() && !t.is_wrapper())
                std::cout << t.get_name() << std::endl;
        }
        // we cannot use the actual type, to get the type information,
        // thus we use string to retrieve it
        auto t = type::get_by_name("MyPluginClass");

        // iterate over all methods of the class
        for (auto meth : t.get_methods())
        {
            std::cout << meth.get_signature() << std::endl;
        }

        // work with the new type
        auto var = t.create();
        t.invoke("perform_calculation", var, {});
        std::cout << t.get_property_value("value", var).to_int() << std::endl; // prints "12"
    } // all data has to be remove before unload the library, otherwise we get UB

    if (!lib.unload())
    {
        std::cerr << lib.get_error_string() << std::endl;
        return -1;
    }

    auto t = type::get_by_name("MyPlugin");
    if (t.is_valid())
    {
        std::cerr << "the type: " << t.get_name() << " should not be valid!";
    }

    return 0;
}

