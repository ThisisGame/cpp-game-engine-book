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

#include "benchmarks/bench_rttr_cast/test_classes.h"

#include <rttr/type>

#include <nonius/nonius.h++>
#include <nonius/html_group_reporter.h>

#include <iostream>
#include <memory>

#include <string>
#include <vector>
using namespace rttr;

#define ITEM_COUNT 10

template<typename T>
struct bench_data
{
    std::vector<std::shared_ptr<T> > m_vec;
};

using bench_data_single_base = bench_data<ClassSingleBase>;

/////////////////////////////////////////////////////////////////////////////////////////

bench_data_single_base setup_level_1()
{
    std::vector<std::shared_ptr<ClassSingleBase> > vec;
    vec.reserve(ITEM_COUNT * 5);
    for (int i = 0; i < ITEM_COUNT; ++i)
    {
        vec.push_back(std::make_shared<ClassSingle1A>());
        vec.push_back(std::make_shared<ClassSingle1B>());
        vec.push_back(std::make_shared<ClassSingle1C>());
        vec.push_back(std::make_shared<ClassSingle1D>());
        vec.push_back(std::make_shared<ClassSingle1E>());
    }

    return bench_data_single_base{std::move(vec)};
}

/////////////////////////////////////////////////////////////////////////////////////////

bench_data_single_base setup_level_3()
{
    std::vector<std::shared_ptr<ClassSingleBase> > vec;
    vec.reserve(ITEM_COUNT * 5);
    for (int i = 0; i < ITEM_COUNT; ++i)
    {
        vec.push_back(detail::make_unique<ClassSingle3A>());
        vec.push_back(detail::make_unique<ClassSingle3B>());
        vec.push_back(detail::make_unique<ClassSingle3C>());
        vec.push_back(detail::make_unique<ClassSingle3D>());
        vec.push_back(detail::make_unique<ClassSingle3E>());
    }

    return bench_data_single_base{std::move(vec)};
}

/////////////////////////////////////////////////////////////////////////////////////////

bench_data_single_base setup_level_6()
{
    std::vector<std::shared_ptr<ClassSingleBase> > vec;
    vec.reserve(ITEM_COUNT * 5);
    for (int i = 0; i < ITEM_COUNT; ++i)
    {
        vec.push_back(detail::make_unique<ClassSingle6A>());
        vec.push_back(detail::make_unique<ClassSingle6B>());
        vec.push_back(detail::make_unique<ClassSingle6C>());
        vec.push_back(detail::make_unique<ClassSingle6D>());
        vec.push_back(detail::make_unique<ClassSingle6E>());
    }

    return bench_data_single_base{std::move(vec)};
}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

using bench_data_virtual = bench_data<ClassDiamondTop>;

/////////////////////////////////////////////////////////////////////////////////////////

bench_data_virtual setup_virtual_inheritance_level_1()
{
    std::vector<std::shared_ptr<ClassDiamondTop> > vec;
    vec.reserve(ITEM_COUNT * 3);
    for (int i = 0; i < ITEM_COUNT; ++i)
    {
        vec.push_back(detail::make_unique<ClassDiamondLeft1>());
        vec.push_back(detail::make_unique<ClassDiamondMiddle1>());
        vec.push_back(detail::make_unique<ClassDiamondRight1>());
    }

    return bench_data_virtual{std::move(vec)};
}

/////////////////////////////////////////////////////////////////////////////////////////

bench_data_virtual setup_virtual_inheritance_level_3()
{
    std::vector<std::shared_ptr<ClassDiamondTop> > vec;
    vec.reserve(ITEM_COUNT * 3);
    for (int i = 0; i < ITEM_COUNT; ++i)
    {
        vec.push_back(detail::make_unique<ClassDiamondLeft3>());
        vec.push_back(detail::make_unique<ClassDiamondMiddle3>());
        vec.push_back(detail::make_unique<ClassDiamondRight3>());
    }

    return bench_data_virtual{std::move(vec)};
}

/////////////////////////////////////////////////////////////////////////////////////////

bench_data_virtual setup_virtual_inheritance_level_6()
{
    std::vector<std::shared_ptr<ClassDiamondTop> > vec;
    vec.reserve(ITEM_COUNT * 3);
    for (int i = 0; i < ITEM_COUNT; ++i)
    {
        vec.push_back(detail::make_unique<ClassDiamondLeft6>());
        vec.push_back(detail::make_unique<ClassDiamondMiddle6>());
        vec.push_back(detail::make_unique<ClassDiamondRight6>());
    }

    return bench_data_virtual{std::move(vec)};
}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

bench_data<ClassMultipleBaseC> setup_multiple_class_hierachy()
{
    std::vector<std::shared_ptr<ClassMultipleBaseC> > vec;
    vec.reserve(ITEM_COUNT * 7);
    for (int i = 0; i < ITEM_COUNT; ++i)
    {
        vec.push_back(detail::make_unique<FinalClass>());
        vec.push_back(detail::make_unique<ClassMultiple5C>());
        vec.push_back(detail::make_unique<FinalClass>());
        vec.push_back(detail::make_unique<ClassMultiple4C>());
        vec.push_back(detail::make_unique<FinalClass>());
        vec.push_back(detail::make_unique<ClassMultiple5C>());
        vec.push_back(detail::make_unique<FinalClass>());
    }

    return bench_data<ClassMultipleBaseC>{std::move(vec)};
}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

bench_data<ClassMultiple3A> setup_cross_cast_class_hierachy_1()
{
    std::vector<std::shared_ptr<ClassMultiple3A> > vec;
    vec.reserve(ITEM_COUNT * 2);
    for (int i = 0; i < ITEM_COUNT; ++i)
    {
        vec.push_back(detail::make_unique<FinalClass>());
        vec.push_back(detail::make_unique<ClassMultiple6A>());
    }

    return bench_data<ClassMultiple3A>{std::move(vec)};
}

/////////////////////////////////////////////////////////////////////////////////////////

bench_data<ClassMultiple6A> setup_cross_cast_class_hierachy_2()
{
    std::vector<std::shared_ptr<ClassMultiple6A> > vec;
    vec.reserve(ITEM_COUNT * 2);
    for (int i = 0; i < ITEM_COUNT; ++i)
    {
        vec.push_back(detail::make_unique<FinalClass>());
        vec.push_back(detail::make_unique<ClassMultiple6A>());
    }

    return bench_data<ClassMultiple6A>{std::move(vec)};
}


/////////////////////////////////////////////////////////////////////////////////////////


nonius::benchmark bench_level_1_dynamic_cast()
{
    return nonius::benchmark("dynamic_cast", [](nonius::chronometer meter)
    {
        const bench_data_single_base bench_data = setup_level_1();
        volatile std::size_t value = 0;
        meter.measure([&]()
        {
            for (std::size_t i = 0; i < bench_data.m_vec.size(); ++i)
            {
                if (dynamic_cast<ClassSingle1A*>(bench_data.m_vec[i].get()))
                {
                    value += 1;
                }
                else if (dynamic_cast<ClassSingle1B*>(bench_data.m_vec[i].get()))
                {
                    value += 1;
                }
                else if (dynamic_cast<ClassSingle1C*>(bench_data.m_vec[i].get()))
                {
                    value += 1;
                }
                else if (dynamic_cast<ClassSingle1D*>(bench_data.m_vec[i].get()))
                {
                    value += 1;
                }
                else if (dynamic_cast<ClassSingle1E*>(bench_data.m_vec[i].get()))
                {
                    value += 1;
                }
            }

            return value;
        });
    });
}

/////////////////////////////////////////////////////////////////////////////////////////

nonius::benchmark bench_level_1_rttr_cast()
{
    return nonius::benchmark("rttr_cast", [](nonius::chronometer meter)
    {
        bench_data_single_base bench_data = setup_level_1();
        volatile std::size_t value = 0;
        meter.measure([&]()
        {
            for (std::size_t i = 0; i < bench_data.m_vec.size(); ++i)
            {
                if (rttr_cast<ClassSingle1A*>(bench_data.m_vec[i].get()))
                {
                    value += 1;
                }
                else if (rttr_cast<ClassSingle1B*>(bench_data.m_vec[i].get()))
                {
                    value += 1;
                }
                else if (rttr_cast<ClassSingle1C*>(bench_data.m_vec[i].get()))
                {
                    value += 1;
                }
                else if (rttr_cast<ClassSingle1D*>(bench_data.m_vec[i].get()))
                {
                    value += 1;
                }
                else if (rttr_cast<ClassSingle1E*>(bench_data.m_vec[i].get()))
                {
                    value += 1;
                }
            }

            return value;
        });
    });
}

/////////////////////////////////////////////////////////////////////////////////////////

nonius::benchmark bench_level_3_dynamic_cast()
{
    return nonius::benchmark("dynamic_cast", [](nonius::chronometer meter)
    {
        bench_data_single_base bench_data = setup_level_3();
        volatile std::size_t value = 0;
        meter.measure([&]()
        {
            for (std::size_t i = 0; i < bench_data.m_vec.size(); ++i)
            {
                if (dynamic_cast<ClassSingle3A*>(bench_data.m_vec[i].get()))
                {
                    value += 1;
                }
                else if (dynamic_cast<ClassSingle3B*>(bench_data.m_vec[i].get()))
                {
                    value += 1;
                }
                else if (dynamic_cast<ClassSingle3C*>(bench_data.m_vec[i].get()))
                {
                    value += 1;
                }
                else if (dynamic_cast<ClassSingle3D*>(bench_data.m_vec[i].get()))
                {
                    value += 1;
                }
                else if (dynamic_cast<ClassSingle3E*>(bench_data.m_vec[i].get()))
                {
                    value += 1;
                }
            }

            return value;
        });
    });
}

/////////////////////////////////////////////////////////////////////////////////////////

nonius::benchmark bench_level_3_rttr_cast()
{
    return nonius::benchmark("rttr_cast", [](nonius::chronometer meter)
    {
        bench_data_single_base bench_data = setup_level_3();
        volatile std::size_t value = 0;
        meter.measure([&]()
        {
            for (std::size_t i = 0; i < bench_data.m_vec.size(); ++i)
            {
                if (rttr_cast<ClassSingle3A*>(bench_data.m_vec[i].get()))
                {
                    value += 1;
                }
                else if (rttr_cast<ClassSingle3B*>(bench_data.m_vec[i].get()))
                {
                    value += 1;
                }
                else if (rttr_cast<ClassSingle3C*>(bench_data.m_vec[i].get()))
                {
                    value += 1;
                }
                else if (rttr_cast<ClassSingle3D*>(bench_data.m_vec[i].get()))
                {
                    value += 1;
                }
                else if (rttr_cast<ClassSingle3E*>(bench_data.m_vec[i].get()))
                {
                    value += 1;
                }
            }

            return value;
        });
    });
}

/////////////////////////////////////////////////////////////////////////////////////////

nonius::benchmark bench_level_6_dynamic_cast()
{
    return nonius::benchmark("dynamic_cast", [](nonius::chronometer meter)
    {
        bench_data_single_base bench_data = setup_level_6();
        volatile std::size_t value = 0;
        meter.measure([&]()
        {
            for (std::size_t i = 0; i < bench_data.m_vec.size(); ++i)
            {
                if (dynamic_cast<ClassSingle6A*>(bench_data.m_vec[i].get()))
                {
                    value += 1;
                }
                else if (dynamic_cast<ClassSingle6B*>(bench_data.m_vec[i].get()))
                {
                    value += 1;
                }
                else if (dynamic_cast<ClassSingle6C*>(bench_data.m_vec[i].get()))
                {
                    value += 1;
                }
                else if (dynamic_cast<ClassSingle6D*>(bench_data.m_vec[i].get()))
                {
                    value += 1;
                }
                else if (dynamic_cast<ClassSingle6E*>(bench_data.m_vec[i].get()))
                {
                    value += 1;
                }
            }

            return value;
        });
    });
}

/////////////////////////////////////////////////////////////////////////////////////////

nonius::benchmark bench_level_6_rttr_cast()
{
    return nonius::benchmark("rttr_cast", [](nonius::chronometer meter)
    {
        bench_data_single_base bench_data = setup_level_6();
        volatile std::size_t value = 0;
        meter.measure([&]()
        {
            for (std::size_t i = 0; i < bench_data.m_vec.size(); ++i)
            {
                if (rttr_cast<ClassSingle6A*>(bench_data.m_vec[i].get()))
                {
                    value += 1;
                }
                else if (rttr_cast<ClassSingle6B*>(bench_data.m_vec[i].get()))
                {
                    value += 1;
                }
                else if (rttr_cast<ClassSingle6C*>(bench_data.m_vec[i].get()))
                {
                    value += 1;
                }
                else if (rttr_cast<ClassSingle6D*>(bench_data.m_vec[i].get()))
                {
                    value += 1;
                }
                else if (rttr_cast<ClassSingle6E*>(bench_data.m_vec[i].get()))
                {
                    value += 1;
                }
            }

            return value;
        });
    });
}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

nonius::benchmark bench_virtual_inheritance_level_1_dynamic_cast()
{
    return nonius::benchmark("dynamic_cast", [](nonius::chronometer meter)
    {
        bench_data_virtual bench_data = setup_virtual_inheritance_level_1();
        volatile std::size_t value = 0;
        meter.measure([&]()
        {
            for (std::size_t i = 0; i < bench_data.m_vec.size(); ++i)
            {
                if (dynamic_cast<ClassDiamondLeft1*>(bench_data.m_vec[i].get()))
                {
                    value += 1;
                }
                else if (dynamic_cast<ClassDiamondMiddle1*>(bench_data.m_vec[i].get()))
                {
                    value += 1;
                }
                else if (dynamic_cast<ClassDiamondRight1*>(bench_data.m_vec[i].get()))
                {
                    value += 1;
                }
            }

            return value;
        });
    });
}

/////////////////////////////////////////////////////////////////////////////////////////

nonius::benchmark bench_virtual_inheritance_level_1_rttr_cast()
{
    return nonius::benchmark("rttr_cast", [](nonius::chronometer meter)
    {
        bench_data_virtual bench_data = setup_virtual_inheritance_level_1();
        volatile std::size_t value = 0;
        meter.measure([&]()
        {
            for (std::size_t i = 0; i < bench_data.m_vec.size(); ++i)
            {
                if (rttr_cast<ClassDiamondLeft1*>(bench_data.m_vec[i].get()))
                {
                    value += 1;
                }
                else if (rttr_cast<ClassDiamondMiddle1*>(bench_data.m_vec[i].get()))
                {
                    value += 1;
                }
                else if (rttr_cast<ClassDiamondRight1*>(bench_data.m_vec[i].get()))
                {
                    value += 1;
                }
            }

            return value;
        });
    });
}

/////////////////////////////////////////////////////////////////////////////////////////

nonius::benchmark bench_virtual_inheritance_level_3_dynamic_cast()
{
    return nonius::benchmark("dynamic_cast", [](nonius::chronometer meter)
    {
        bench_data_virtual bench_data = setup_virtual_inheritance_level_3();
        volatile std::size_t value = 0;
        meter.measure([&]()
        {
            for (std::size_t i = 0; i < bench_data.m_vec.size(); ++i)
            {
                if (dynamic_cast<ClassDiamondLeft3*>(bench_data.m_vec[i].get()))
                {
                    value += 1;
                }
                else if (dynamic_cast<ClassDiamondMiddle3*>(bench_data.m_vec[i].get()))
                {
                    value += 1;
                }
                else if (dynamic_cast<ClassDiamondRight3*>(bench_data.m_vec[i].get()))
                {
                    value += 1;
                }
            }

            return value;
        });
    });
}

/////////////////////////////////////////////////////////////////////////////////////////

nonius::benchmark bench_virtual_inheritance_level_3_rttr_cast()
{
    return nonius::benchmark("rttr_cast", [](nonius::chronometer meter)
    {
        bench_data_virtual bench_data = setup_virtual_inheritance_level_3();
        volatile std::size_t value = 0;
        meter.measure([&]()
        {
            for (std::size_t i = 0; i < bench_data.m_vec.size(); ++i)
            {
                if (rttr_cast<ClassDiamondLeft3*>(bench_data.m_vec[i].get()))
                {
                    value += 1;
                }
                else if (rttr_cast<ClassDiamondMiddle3*>(bench_data.m_vec[i].get()))
                {
                    value += 1;
                }
                else if (rttr_cast<ClassDiamondRight3*>(bench_data.m_vec[i].get()))
                {
                    value += 1;
                }
            }

            return value;
        });
    });
}


/////////////////////////////////////////////////////////////////////////////////////////

nonius::benchmark bench_virtual_inheritance_level_6_dynamic_cast()
{
    return nonius::benchmark("dynamic_cast", [](nonius::chronometer meter)
    {
        bench_data_virtual bench_data = setup_virtual_inheritance_level_6();
        volatile std::size_t value = 0;
        meter.measure([&]()
        {
            for (std::size_t i = 0; i < bench_data.m_vec.size(); ++i)
            {
                if (dynamic_cast<ClassDiamondLeft6*>(bench_data.m_vec[i].get()))
                {
                    value += 1;
                }
                else if (dynamic_cast<ClassDiamondMiddle6*>(bench_data.m_vec[i].get()))
                {
                    value += 1;
                }
                else if (dynamic_cast<ClassDiamondRight6*>(bench_data.m_vec[i].get()))
                {
                    value += 1;
                }
            }

            return value;
        });
    });
}

/////////////////////////////////////////////////////////////////////////////////////////

nonius::benchmark bench_virtual_inheritance_level_6_rttr_cast()
{
    return nonius::benchmark("rttr_cast", [](nonius::chronometer meter)
    {
        bench_data_virtual bench_data = setup_virtual_inheritance_level_6();
        volatile std::size_t value = 0;
        meter.measure([&]()
        {
            for (std::size_t i = 0; i < bench_data.m_vec.size(); ++i)
            {
                if (rttr_cast<ClassDiamondLeft6*>(bench_data.m_vec[i].get()))
                {
                    value += 1;
                }
                else if (rttr_cast<ClassDiamondMiddle6*>(bench_data.m_vec[i].get()))
                {
                    value += 1;
                }
                else if (rttr_cast<ClassDiamondRight6*>(bench_data.m_vec[i].get()))
                {
                    value += 1;
                }
            }

            return value;
        });
    });
}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

nonius::benchmark bench_multiple_inheritance_dynamic_cast()
{
    return nonius::benchmark("dynamic_cast", [](nonius::chronometer meter)
    {
        auto bench_data = setup_multiple_class_hierachy();
        volatile std::size_t value = 0;
        meter.measure([&]()
        {
            for (std::size_t i = 0; i < bench_data.m_vec.size(); ++i)
            {
                if (dynamic_cast<FinalClass*>(bench_data.m_vec[i].get()))
                {
                    value += 1;
                }
            }

            return value;
        });
    });
}

/////////////////////////////////////////////////////////////////////////////////////////

nonius::benchmark bench_multiple_inheritance_rttr_cast()
{
    return nonius::benchmark("rttr_cast", [](nonius::chronometer meter)
    {
        auto bench_data = setup_multiple_class_hierachy();
        volatile std::size_t value = 0;
        meter.measure([&]()
        {
            for (std::size_t i = 0; i < bench_data.m_vec.size(); ++i)
            {
                if (rttr_cast<FinalClass*>(bench_data.m_vec[i].get()))
                {
                    value += 1;
                }
            }

            return value;
        });
    });
}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

nonius::benchmark bench_cross_cast_dynamic_cast_1()
{
    return nonius::benchmark("dynamic_cast", [](nonius::chronometer meter)
    {
        auto bench_data = setup_cross_cast_class_hierachy_1();
        volatile std::size_t value = 0;
        meter.measure([&]()
        {
            for (std::size_t i = 0; i < bench_data.m_vec.size(); ++i)
            {
                if (dynamic_cast<ClassMultiple3C*>(bench_data.m_vec[i].get()))
                {
                    value += 1;
                }
            }

            return value;
        });
    });
}

/////////////////////////////////////////////////////////////////////////////////////////

nonius::benchmark bench_cross_cast_rttr_cast_1()
{
    return nonius::benchmark("rttr_cast", [](nonius::chronometer meter)
    {
        auto bench_data = setup_cross_cast_class_hierachy_1();
        volatile std::size_t value = 0;
        meter.measure([&]()
        {
            for (std::size_t i = 0; i < bench_data.m_vec.size(); ++i)
            {
                if (rttr_cast<ClassMultiple3C*>(bench_data.m_vec[i].get()))
                {
                    value += 1;
                }
            }

            return value;
        });
    });
}

/////////////////////////////////////////////////////////////////////////////////////////

nonius::benchmark bench_cross_cast_dynamic_cast_2()
{
    return nonius::benchmark("dynamic_cast", [](nonius::chronometer meter)
    {
        auto bench_data = setup_cross_cast_class_hierachy_2();
        volatile std::size_t value = 0;
        meter.measure([&]()
        {
            for (std::size_t i = 0; i < bench_data.m_vec.size(); ++i)
            {
                if (dynamic_cast<ClassMultiple6E*>(bench_data.m_vec[i].get()))
                {
                    value += 1;
                }
            }

            return value;
        });
    });
}

/////////////////////////////////////////////////////////////////////////////////////////

nonius::benchmark bench_cross_cast_rttr_cast_2()
{
    return nonius::benchmark("rttr_cast", [](nonius::chronometer meter)
    {
        auto bench_data = setup_cross_cast_class_hierachy_2();
        volatile std::size_t value = 0;
        meter.measure([&]()
        {
            for (std::size_t i = 0; i < bench_data.m_vec.size(); ++i)
            {
                if (rttr_cast<ClassMultiple6E*>(bench_data.m_vec[i].get()))
                {
                    value += 1;
                }
            }

            return value;
        });
    });
}


/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

nonius::benchmark bench_level_6_typeid()
{
    return nonius::benchmark("typeid", [](nonius::chronometer meter)
    {
        bench_data_single_base bench_data = setup_level_6();
        volatile std::size_t value = 0;
        meter.measure([&]()
        {
            for (std::size_t i = 0; i < bench_data.m_vec.size(); ++i)
            {
                if ( dynamic_cast<ClassSingle1A*>(bench_data.m_vec[i].get()))
                {
                    value += 1;
                }
                else if (dynamic_cast<ClassSingle1B*>(bench_data.m_vec[i].get()))
                {
                    value += 1;
                }
                else if (dynamic_cast<ClassSingle1C*>(bench_data.m_vec[i].get()))
                {
                    value += 1;
                }
                else if (dynamic_cast<ClassSingle1D*>(bench_data.m_vec[i].get()))
                {
                    value += 1;
                }
                else if (dynamic_cast<ClassSingle1E*>(bench_data.m_vec[i].get()))
                {
                    value += 1;
                }
            }

            return value;
        });
    });
}

/////////////////////////////////////////////////////////////////////////////////////////

nonius::benchmark bench_level_6_type()
{
    return nonius::benchmark("type", [](nonius::chronometer meter)
    {
        bench_data_single_base bench_data = setup_level_6();
        volatile std::size_t value = 0;
        meter.measure([&]()
        {
            for (std::size_t i = 0; i < bench_data.m_vec.size(); ++i)
            {
                if (rttr_cast<ClassSingle1A*>(bench_data.m_vec[i].get()))
                {
                    value += 1;
                }
                else if (rttr_cast<ClassSingle1B*>(bench_data.m_vec[i].get()))
                {
                    value += 1;
                }
                else if (rttr_cast<ClassSingle1C*>(bench_data.m_vec[i].get()))
                {
                    value += 1;
                }
                else if (rttr_cast<ClassSingle1D*>(bench_data.m_vec[i].get()))
                {
                    value += 1;
                }
                else if (rttr_cast<ClassSingle1E*>(bench_data.m_vec[i].get()))
                {
                    value += 1;
                }
            }

            return value;
        });
    });
}

/////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

void start_rttr_cast_benchmark()
{
    nonius::configuration cfg;
    cfg.title = "dynamic_cast vs. rttr_cast";

    nonius::html_group_reporter reporter;
    reporter.set_output_file("benchmark_dynamic_cast_vs_rttr_cast.html");

    reporter.set_current_group_name("downcast to level 1", "A cast from the base class one hierarchy level down.");
    nonius::benchmark benchmarks_group_1[] = { bench_level_1_dynamic_cast(), bench_level_1_rttr_cast() };
    nonius::go(cfg, std::begin(benchmarks_group_1), std::end(benchmarks_group_1), reporter);

    reporter.set_current_group_name("downcast to level 3", "A cast from the base class three hierarchy levels down.");
    nonius::benchmark benchmarks_group_2[] = { bench_level_3_dynamic_cast(), bench_level_3_rttr_cast() };
    nonius::go(cfg, std::begin(benchmarks_group_2), std::end(benchmarks_group_2), reporter);

    reporter.set_current_group_name("downcast to level 6", "A cast from the base class six hierarchy levels down.");
    nonius::benchmark benchmarks_group_3[] = { bench_level_6_dynamic_cast(), bench_level_6_rttr_cast() };
    nonius::go(cfg, std::begin(benchmarks_group_3), std::end(benchmarks_group_3), reporter);

    ///////////////////////////////////////////////////////////////////////////////

    reporter.set_current_group_name("virtual inheritance level 1", "A cast from virtual base class one hierarchy levels down.");
    nonius::benchmark benchmarks_group_4[] = { bench_virtual_inheritance_level_1_dynamic_cast(), bench_virtual_inheritance_level_1_rttr_cast() };
    nonius::go(cfg, std::begin(benchmarks_group_4), std::end(benchmarks_group_4), reporter);

    reporter.set_current_group_name("virtual inheritance level 3", "A cast from virtual base class three hierarchy levels down.");
    nonius::benchmark benchmarks_group_5[] = { bench_virtual_inheritance_level_3_dynamic_cast(), bench_virtual_inheritance_level_3_rttr_cast() };
    nonius::go(cfg, std::begin(benchmarks_group_5), std::end(benchmarks_group_5), reporter);

    reporter.set_current_group_name("virtual inheritance level 6", "A cast from virtual base class six hierarchy levels down.");
    nonius::benchmark benchmarks_group_6[] = { bench_virtual_inheritance_level_6_dynamic_cast(), bench_virtual_inheritance_level_6_rttr_cast() };
    nonius::go(cfg, std::begin(benchmarks_group_6), std::end(benchmarks_group_6), reporter);

    ///////////////////////////////////////////////////////////////////////////////

    reporter.set_current_group_name("multiple inheritance", "A cast from virtual base class to the most derived class,<br>which itself is derived from 5 other classes (all 7 hierarchy levels deep).");
    nonius::benchmark benchmarks_group_7[] = { bench_multiple_inheritance_dynamic_cast(), bench_multiple_inheritance_rttr_cast() };
    nonius::go(cfg, std::begin(benchmarks_group_7), std::end(benchmarks_group_7), reporter);

    ///////////////////////////////////////////////////////////////////////////////

    reporter.set_current_group_name("cross cast 1", "A cross cast in the 4th hierarchy level (out of 8)<br>to the third (out of 5 class wide hierarchy).");
    nonius::benchmark benchmarks_group_8[] = { bench_cross_cast_dynamic_cast_1(), bench_cross_cast_rttr_cast_1() };
    nonius::go(cfg, std::begin(benchmarks_group_8), std::end(benchmarks_group_8), reporter);


    reporter.set_current_group_name("cross cast 2", "A cross cast in the 7th hierarchy level (out of 8)<br>to the fifth (out of 5 class wide hierarchy).");
    nonius::benchmark benchmarks_group_9[] = { bench_cross_cast_dynamic_cast_2(), bench_cross_cast_rttr_cast_2() };
    nonius::go(cfg, std::begin(benchmarks_group_9), std::end(benchmarks_group_9), reporter);

    reporter.generate_report();
}



/////////////////////////////////////////////////////////////////////////////////////////
#if 0

void bench_type_get_vs_typeid()
{
    vector<unique_ptr<ClassSingleBase> > vec;

    vec.reserve(ITEM_COUNT * 5);
    for (int i = 0; i < ITEM_COUNT; ++i)
    {
        vec.push_back(detail::make_unique<ClassSingle6A>());
        vec.push_back(detail::make_unique<ClassSingle6B>());
        vec.push_back(detail::make_unique<ClassSingle6C>());
        vec.push_back(detail::make_unique<ClassSingle6D>());
        vec.push_back(detail::make_unique<ClassSingle6E>());
    }

    chrono::high_resolution_clock::duration elapsedTime_1;
    vector<void*> result_reference;
    result_reference.resize(vec.size());
    for (int count = 0; count < RUN_COUNT; ++count)
    {
        const auto startTime = chrono::high_resolution_clock::now();
        for (std::size_t i = 0; i < vec.size(); ++i)
        {
            if (typeid(ClassSingle6A) == typeid(*vec[i]))
            {
                result_reference[i] = vec[i]->get_ptr();
            }
            else if (typeid(ClassSingle6B) == typeid(*vec[i]))
            {
                result_reference[i] = vec[i]->get_ptr();
            }
            else if (typeid(ClassSingle6C) == typeid(*vec[i]))
            {
                result_reference[i] = vec[i]->get_ptr();
            }
            else if (typeid(ClassSingle6D) == typeid(*vec[i]))
            {
                result_reference[i] = vec[i]->get_ptr();
            }
            else if (typeid(ClassSingle6E) == typeid(*vec[i]))
            {
                result_reference[i] = vec[i]->get_ptr();
            }
        }
        elapsedTime_1 += chrono::high_resolution_clock::now() - startTime;
    }
    elapsedTime_1 /= RUN_COUNT;

    chrono::high_resolution_clock::duration elapsedTime_2;
    vector<void*> result;
    result.resize(vec.size());
    for (int count = 0; count < RUN_COUNT; ++count)
    {
        const auto startTime = chrono::high_resolution_clock::now();
        for (std::size_t i = 0; i < vec.size(); ++i)
        {
            if (type::get<ClassSingle6A>() == type::get(*vec[i]))
            {
                result[i] = vec[i]->get_ptr();
            }
            else if (type::get<ClassSingle6B>() == type::get(*vec[i]))
            {
                result[i] = vec[i]->get_ptr();
            }
            else if (type::get<ClassSingle6C>() == type::get(*vec[i]))
            {
                result[i] = vec[i]->get_ptr();
            }
            else if (type::get<ClassSingle6D>() == type::get(*vec[i]))
            {
                result[i] = vec[i]->get_ptr();
            }
            else if (type::get<ClassSingle6E>() == type::get(*vec[i]))
            {
                result[i] = vec[i]->get_ptr();
            }
        }
        elapsedTime_2 += chrono::high_resolution_clock::now() - startTime;
    }

    elapsedTime_2 /= RUN_COUNT;

    if (result != result_reference)
        std::cout << "ERROR - Invalid rttr_cast - bench_type_get_vs_typeid" << std::endl;

    print_stats("BaseClass to Level 6 [typeid]", elapsedTime_1, elapsedTime_2);
}

/////////////////////////////////////////////////////////////////////////////////////////

void start_rttr_cast_benchmark()
{
    std::cout << center("dynamic_cast vs. rttr_cast", TABLE_WIDTH) << std::endl;
    std::cout << std::string(TABLE_WIDTH, '-') << "\n";
    std::cout << left("", COLUMN_0_WIDTH) << " | "
              << center("[dynamic_cast]", COLUMN_1_WIDTH)    << " | "
              << center("[rttr_cast]", COLUMN_2_WIDTH)       << " | "
              << "\n";

    std::cout << std::string(TABLE_WIDTH, '-') << "\n";

    bench_single_inheritance_level_1();
    bench_single_inheritance_level_3();
    bench_single_inheritance_level_6();
    std::cout << std::string(TABLE_WIDTH, '-') << "\n";
    bench_virtual_inheritance_level_1();
    bench_virtual_inheritance_level_3();
    bench_virtual_inheritance_level_6();
    std::cout << std::string(TABLE_WIDTH, '-') << "\n";
    bench_multiple_class_hierachy_end();
    std::cout << std::string(TABLE_WIDTH, '-') << "\n";
    bench_cross_cast_1();
    bench_cross_cast_2();
    std::cout << std::string(TABLE_WIDTH, '-') << "\n";
    bench_type_get_vs_typeid();
    std::cout << std::string(TABLE_WIDTH, '-') << "\n";
}
#endif