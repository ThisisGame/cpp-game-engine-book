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

#ifndef RTTR_LIBRARY_P_H_
#define RTTR_LIBRARY_P_H_

#include "rttr/detail/base/core_prerequisites.h"
#include "rttr/string_view.h"

#include "rttr/type.h"
#include "rttr/property.h"
#include "rttr/method.h"
#include "rttr/detail/registration/registration_state_saver.h"

#if RTTR_PLATFORM == RTTR_PLATFORM_WINDOWS
    #include <windows.h>
#endif

#include <atomic>

namespace rttr
{
namespace detail
{

/*!
 * This class contains the private implementation of the \ref library class.
 *
 */
class RTTR_LOCAL library_private
{
public:
    library_private(string_view file_name, string_view version)
    :   m_file_name(file_name),
        m_version(version),
#if RTTR_COMPILER == RTTR_COMPILER_MSVC && RTTR_COMP_VER <= 1800
        m_load_count(ATOMIC_VAR_INIT(0)),
#else
        m_load_count(0),
#endif
        m_handle(nullptr)
    {

    }

    ~library_private() = default;

    bool load()
    {
        if (m_handle)
        {
            ++m_load_count;
            return true;
        }
        m_state_saver.save_state_begin();

        auto result = load_native();
        if (result)
        {
            ++m_load_count;
            m_state_saver.save_state_end();
        }
        else
        {
            m_state_saver.reset();
        }

        return result;
    }

    bool unload()
    {
        if (!m_handle)
            return false;

        --m_load_count;

        if (m_load_count.load() == 0)
        {
            auto ret = unload_native();
            if (ret)
            {
                m_error_string.clear();
                m_state_saver.reset();
                m_handle = nullptr;
            }
        }

        return (m_handle == nullptr);
    }

    /////////////////////////////////////////////////////////////



    bool is_loaded() const RTTR_NOEXCEPT { return (m_handle != nullptr); }
    string_view get_error_string() const RTTR_NOEXCEPT { return m_error_string; }
    string_view get_file_name() const RTTR_NOEXCEPT { return m_file_name; }
    string_view get_qualified_filename() const RTTR_NOEXCEPT { return m_qualifed_file_name; }

    /////////////////////////////////////////////////////////////

    array_range<type> get_types() const RTTR_NOEXCEPT { return m_state_saver.get_types(); }

    array_range<property> get_global_properties() const RTTR_NOEXCEPT { return m_state_saver.get_global_properties(); }

    array_range<method> get_global_methods() const RTTR_NOEXCEPT { return m_state_saver.get_global_methods(); }

    int get_load_count() const RTTR_NOEXCEPT { return m_load_count.load(); }

    void set_load_count(int count) { m_load_count.store(count); }

private:
    bool load_native();
    bool unload_native();

private:
    std::string                 m_file_name;
    std::string                 m_version;
    std::string                 m_qualifed_file_name;
    std::string                 m_error_string;
    registration_state_saver    m_state_saver;

    std::atomic_int             m_load_count;

#if RTTR_PLATFORM == RTTR_PLATFORM_WINDOWS
    HMODULE
#else
    void*
#endif
    m_handle;
};

} // end namespace detail
} // end namespace rttr

#endif // RTTR_LIBRARY_P_H_
