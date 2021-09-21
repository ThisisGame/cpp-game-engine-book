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

#include "rttr/detail/base/core_prerequisites.h"

#if RTTR_PLATFORM != RTTR_PLATFORM_WINDOWS

#include "rttr/detail/library/library_p.h"

#include "rttr/detail/misc/utility.h"

#include <vector>
#include <dlfcn.h>
#include <cstdio>

namespace
{

bool is_absolute_path(const std::string& file_name)
{
    return (!file_name.empty() && file_name[0] == '/');
}

std::vector<std::string> get_suffixes_sys(const std::string& version)
{
    std::vector<std::string> result;

#if RTTR_PLATFORM == RTTR_PLATFORM_LINUX

    if (!version.empty())
        result.emplace_back(".so." + version);
    else
        result.emplace_back(".so");
#endif


#if RTTR_PLATFORM == RTTR_PLATFORM_APPLE

    if (!version.empty())
    {
        result.emplace_back("." + version + ".bundle");
        result.emplace_back("." + version + ".dylib");
    }
    else
    {
        result.emplace_back(".bundle");
        result.emplace_back(".dylib");
    }
#endif

    return result;
}

bool file_exist(const std::string& fileName)
{
    if (auto file = fopen(fileName.c_str(), "r"))
    {
        fclose(file);
        return true;
    } else
    {
        return false;
    }
}

static rttr::string_view get_error()
{
    using namespace rttr;
    auto err = dlerror();
    return err ? string_view(err): string_view();
}

} // end namespace anonymous

namespace rttr
{
namespace detail
{

/////////////////////////////////////////////////////////////////////////////////////////

bool library_private::load_native()
{
    std::vector<std::string> prefix_list = {"lib"};
    std::vector<std::string> suffix_list = get_suffixes_sys(m_version);

    if (is_absolute_path(m_file_name))
    {
        suffix_list.insert(suffix_list.begin(), std::string());
        prefix_list.insert(suffix_list.begin(), std::string());
    }
    else
    {
        suffix_list.push_back(std::string());
        prefix_list.push_back(std::string());
    }

    int dl_flags = RTLD_NOW;
    auto retry = true;
    std::string attempt;

    for (auto prefix = 0u; retry && !m_handle && prefix < prefix_list.size(); ++prefix)
    {
        for (auto suffix = 0u; retry && !m_handle && suffix < suffix_list.size(); ++suffix)
        {
            // don't attach prefix or suffix when it is already there
            if (!prefix_list[prefix].empty() && starts_with(m_file_name, prefix_list[prefix]))
                continue;
            if (!suffix_list[suffix].empty() && ends_with(m_file_name, suffix_list[suffix]))
                continue;

            attempt = prefix_list[prefix] + m_file_name + suffix_list[suffix];

            m_handle = dlopen(attempt.c_str(), dl_flags);
            if (!m_handle && is_absolute_path(m_file_name) && file_exist(attempt))
            {
                retry = false;
            }
        }

    }

    if (!m_handle)
    {
        m_error_string = "Cannot load library " + m_file_name + " " + get_error();
    }
    else
    {
        m_qualifed_file_name = attempt;
        m_error_string.clear();
    }

    return (m_handle != nullptr);
}

/////////////////////////////////////////////////////////////////////////////////////////

bool library_private::unload_native()
{
    if (dlclose(m_handle))
    {
         m_error_string = "Cannot unload library: '" + m_file_name + "'" +  get_error();
         return false;
    }

    return true;
}

/////////////////////////////////////////////////////////////////////////////////////////


} // end namespace detail
} // end namespace rttr

#endif // NOT RTTR_PLATFORM_WINDOWS

