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

#include <catch/catch.hpp>

#include <rttr/type>
#include <iostream>

using namespace rttr;

/////////////////////////////////////////////////////////////////////////////////////////

#ifdef NDEBUG
    static string_view library_name("unit_test_plugin");
#else
    static string_view library_name("unit_test_plugin_d");
#endif

/////////////////////////////////////////////////////////////////////////////////////////

bool does_plugin_type_exist()
{
    return type::get_by_name("test_plugin_class").is_valid() &&
           type::get_by_name("test_container_plugin").is_valid();
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("library - load", "[library]")
{
    SECTION("positive test")
    {
        library lib(library_name);

        CHECK(does_plugin_type_exist() == false);
        CHECK(lib.load() == true);
        REQUIRE(lib.get_error_string().empty() == true);
        CHECK(does_plugin_type_exist() == true);

        CHECK(lib.get_types().size()             > 1);
        CHECK(lib.get_global_properties().size() == 0);
        CHECK(lib.get_global_methods().size()    == 0);

        CHECK(lib.unload() == true);
        CHECK(lib.get_error_string().empty()     == true);

        CHECK(lib.get_types().size()             == 0);
        CHECK(lib.get_global_properties().size() == 0);
        CHECK(lib.get_global_methods().size()    == 0);
        CHECK(does_plugin_type_exist()           == false);
    }

    SECTION("negative test")
    {
        library lib("SomeNonExisitingLibrary");

        CHECK(lib.is_loaded()                    == false);
        CHECK(lib.get_types().size()             == 0);
        CHECK(lib.get_global_properties().size() == 0);
        CHECK(lib.get_global_methods().size()    == 0);
        CHECK(lib.get_error_string().empty()     == true);

        CHECK(lib.load()                         == false);

        CHECK(lib.is_loaded()                    == false);
        CHECK(lib.get_error_string().empty()     == false);
        CHECK(lib.unload()                       == false);
    }

    SECTION("load with suffix")
    {
#if RTTR_PLATFORM == RTTR_PLATFORM_WINDOWS
        library lib(library_name + std::string(".dll"));
#elif RTTR_PLATFORM == RTTR_PLATFORM_LINUX
        library lib(library_name + std::string(".so"));
#elif RTTR_PLATFORM == RTTR_PLATFORM_APPLE
        library lib(library_name + std::string(".dylib"));
#else
    #error "Don't know library suffix on this plattform!"
#endif
        CHECK(does_plugin_type_exist()          == false);
        CHECK(lib.load()                        == true);
        REQUIRE(lib.get_error_string().empty()  == true);
        CHECK(does_plugin_type_exist()          == true);

        CHECK(lib.get_error_string().empty()    == true);
        CHECK(lib.unload()                      == true);

        CHECK(does_plugin_type_exist()          == false);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("library - multi load", "[library]")
{
    SECTION("load multiple times")
    {
        library lib(library_name);

        CHECK(does_plugin_type_exist()              == false);
        CHECK(lib.load()                            == true);
        CHECK(lib.load()                            == true);

        REQUIRE(lib.get_error_string().empty()      == true);
        CHECK(does_plugin_type_exist()              == true);

        CHECK(lib.get_types().size()                > 1);
        CHECK(lib.get_global_properties().size()    == 0);
        CHECK(lib.get_global_methods().size()       == 0);

        CHECK(lib.unload() == true);
        CHECK(does_plugin_type_exist() == false);
    }

    SECTION("load the same lib from different instance multiple times")
    {
        library lib1(library_name);
        library lib2(library_name);

        CHECK(does_plugin_type_exist() == false);

        CHECK(lib1.load() == true);
        CHECK(lib2.load() == true);

        CHECK(does_plugin_type_exist() == true);

        CHECK(lib1.unload() == false);
        CHECK(lib1.unload() == false);

        CHECK(does_plugin_type_exist() == true);

        CHECK(lib2.unload() == true);

        CHECK(does_plugin_type_exist() == false);
    }

    SECTION("load the same lib multiple times 2xload() and 2xunload()")
    {
        library lib(library_name);

        CHECK(does_plugin_type_exist() == false);

        CHECK(lib.load() == true);
        CHECK(does_plugin_type_exist() == true);

        CHECK(lib.unload() == true);
        CHECK(does_plugin_type_exist() == false);

        CHECK(lib.load() == true);
        CHECK(does_plugin_type_exist() == true);

        CHECK(lib.unload() == true);
        CHECK(does_plugin_type_exist() == false);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("library - using types", "[library]")
{
    SECTION("property invoke")
    {
        library lib(library_name);

        CHECK(lib.load() == true);

        {
            auto t = type::get_by_name("test_plugin_class");
            variant var_obj = t.create();
            CHECK(var_obj.get_type() == t);

            auto prop = t.get_property("point");
            CHECK(prop.is_valid() == true);

            variant point = prop.get_value(var_obj);
            REQUIRE(point.is_valid() == true);

            auto point_x = point.get_type().get_property("x");
            auto point_y = point.get_type().get_property("y");

            point_x.set_value(point, 23);
            point_y.set_value(point, 42);

            CHECK(point_x.get_value(point).to_int() == 23);
            CHECK(point_y.get_value(point).to_int() == 42);

            CHECK(prop.set_value(var_obj, point) == true);
        }

        CHECK(lib.unload() == true);
    }

    SECTION("sequential view")
    {
        library lib(library_name);

        CHECK(lib.load() == true);

        {
            auto t = type::get_by_name("test_container_plugin");
            variant var_obj = t.create();
            CHECK(var_obj.is_valid() == true);

            auto prop = t.get_property("my_array");
            CHECK(prop.is_valid() == true);

            variant var_point_list = prop.get_value(var_obj);
            CHECK(var_point_list.is_valid() == true);

            CHECK(var_point_list.is_sequential_container() == true);

            auto view = var_point_list.create_sequential_view();

            CHECK(view.is_valid() == true);
            CHECK(view.get_size() == 100);
        }

        CHECK(lib.unload() == true);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////
