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

#if RTTR_PLATFORM == RTTR_PLATFORM_WINDOWS

#include "rttr/detail/library/library_p.h"
#include "rttr/detail/misc/utility.h"

#include <iostream>
#include <string>
#include <locale>


namespace
{

std::wstring convert_utf8_to_utf16(const std::string& source)
{
    if (source.empty())
        return std::wstring();

    const auto size_needed = MultiByteToWideChar(CP_UTF8, 0, &source[0], static_cast<int>(source.size()), NULL, 0);
    std::wstring result(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, &source[0], static_cast<int>(source.size()), &result[0], size_needed);
    return result;
}

/////////////////////////////////////////////////////////////////////////////////////////

std::string convert_utf16_to_utf8(const std::wstring& source)
{
    if (source.empty())
        return std::string();

    const auto size_needed = WideCharToMultiByte(CP_UTF8, 0, &source[0], static_cast<int>(source.size()), NULL, 0, NULL, NULL);
    std::string result( size_needed, 0 );
    WideCharToMultiByte(CP_UTF8, 0, &source[0], static_cast<int>(source.size()), &result[0], size_needed, NULL, NULL);
    return result;
}

/////////////////////////////////////////////////////////////////////////////////////////

std::string error_string(int error_code = -1)
{
    std::string result;
    rttr::string_view std_error_msg;

    if (error_code == -1)
    {
#if RTTR_PLATFORM == RTTR_PLATFORM_WINDOWS
        error_code = GetLastError();
#else
        error_code = errno;
#endif
    }

    switch (error_code)
    {
        case 0:
            break;
        case EACCES:
            std_error_msg = "Permission denied";
            break;
        case EMFILE:
            std_error_msg = "Too many open files";
            break;
        case ENOENT:
            std_error_msg = "No such file or directory";
            break;
        case ENOSPC:
            std_error_msg = "No space left on device";
            break;
        default:
        {
            wchar_t error_str[1024];
            const auto buffer_size =
                FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM,
                               NULL,
                               error_code,
                               MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                               (LPWSTR)&error_str,
                               sizeof(error_str) / sizeof(wchar_t),
                               NULL);
            result = convert_utf16_to_utf8(std::wstring(error_str, error_str + buffer_size));
            break;
        }
    }

    if (!std_error_msg.empty())
        result = std_error_msg.to_string();

    return result;
}

/////////////////////////////////////////////////////////////////////////////////////////

#define IS_SEP(x)   ((x) == L'/' || (x) == L'\\')
#define PREF        L'\\'
#define COLON       L':'

// returns end of prefix
size_t get_prefix_end(const std::wstring& path)
{
    if (path.size() > 2
        && IS_SEP(path[0])
        && IS_SEP(path[1])
        && !IS_SEP(path[2]))
    {   // network name => \\name
        std::size_t idx = 3;
        for (; idx < path.size() && !IS_SEP(path[idx]); ++idx)
            ;
        return idx;
    }
    else
    {   // no network name => drive:
        std::size_t idx = path.find(COLON, 0);
        if (idx == path.npos)
            idx = 0;
        else
            ++idx;
        return idx;
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

std::wstring get_root_name(const std::wstring& path)
{
    return path.substr(0, get_prefix_end(path));
}

/////////////////////////////////////////////////////////////////////////////////////////

std::wstring get_root_dir(const std::wstring& path)
{
    const auto idx = get_prefix_end(path);
    if (idx < path.size() && IS_SEP(path[idx]))
        return std::wstring(1, PREF);
    else
        return std::wstring();
}

/////////////////////////////////////////////////////////////////////////////////////////

bool has_root_name(const std::wstring& path)
{
    return !get_root_name(path).empty();
}

/////////////////////////////////////////////////////////////////////////////////////////

bool has_root_dir(const std::wstring& path)
{
    return !get_root_dir(path).empty();
}

/////////////////////////////////////////////////////////////////////////////////////////

bool is_absolute_path(const std::wstring& path)
{
    return (has_root_name(path) && has_root_dir(path));
}

} // end namespace anonymous


namespace rttr
{
namespace detail
{

/////////////////////////////////////////////////////////////////////////////////////////

bool library_private::load_native()
{
    std::wstring wfile_name = convert_utf8_to_utf16(m_file_name);

    std::vector<std::wstring> paths_to_try;

    std::vector<std::wstring> prefix_list = {L"lib"};
    std::vector<std::wstring> suffix_list = {L".dll"};

    if (is_absolute_path(wfile_name))
    {
        // that means, the suffix and prefix should be ignored at first try
        prefix_list.insert(suffix_list.begin(), std::wstring());
        suffix_list.insert(suffix_list.begin(), std::wstring());
    }
    else
    {
        prefix_list.insert(prefix_list.begin(), std::wstring());
        suffix_list.push_back(std::wstring());
    }

    std::wstring attempt;
    auto retry = true;

    for (auto prefix = 0u; retry && !m_handle && prefix < prefix_list.size(); ++prefix)
    {
        for (auto suffix = 0u; retry && !m_handle && suffix < suffix_list.size(); ++suffix)
        {
            // don't attach prefix or suffix when it is already there
            if (!prefix_list[prefix].empty() && starts_with(wfile_name, prefix_list[prefix]))
                continue;
            if (!suffix_list[suffix].empty() && ends_with(wfile_name, suffix_list[suffix]))
                continue;

            attempt = prefix_list[prefix] + wfile_name + suffix_list[suffix];

            m_handle = LoadLibraryW(attempt.c_str());
            if (::GetLastError() != ERROR_MOD_NOT_FOUND)
            {
                retry = false;
            }
        }
    }

    if (!m_handle)
    {
        m_error_string = "Cannot load library: '" + m_file_name + "' : " + error_string();
    }
    else
    {
        m_error_string.clear();
        m_qualifed_file_name = convert_utf16_to_utf8(attempt);
    }

    return (m_handle != nullptr);
}

/////////////////////////////////////////////////////////////////////////////////////////

bool library_private::unload_native()
{
    if (!FreeLibrary(m_handle))
    {
        m_error_string = "Cannot unload library: " + m_file_name + " : " + error_string();
        return false;
    }

    return true;
}

/////////////////////////////////////////////////////////////////////////////////////////

} // end namespace detail
} // end namespace rttr

#endif // RTTR_PLATFORM_WINDOWS
