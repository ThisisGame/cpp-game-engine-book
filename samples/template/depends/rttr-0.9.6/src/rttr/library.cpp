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

#include "rttr/library.h"
#include "rttr/detail/library/library_p.h"

#include <map>
#include <mutex>

namespace rttr
{

namespace detail
{

/////////////////////////////////////////////////////////////////////////////////////////

/*!
 * A simple manager class, to hold all created library_private pointer objects. A private singleton.
 * After application exit it will unload all libs without any reference.
 */
class library_manager
{
    public:
        static std::shared_ptr<library_private> create_or_find_library(string_view file_name, string_view version)
        {
            auto& manager = get_instance();
            std::lock_guard<std::mutex> lock(manager.m_library_mutex);

            auto file_as_string = file_name.to_string();
            auto itr = manager.m_library_map.find(file_as_string);
            if (itr != manager.m_library_map.end())
                return itr->second;

            auto lib = std::make_shared<library_private>(file_name, version);

            manager.m_library_map.emplace(std::move(file_as_string), lib);

            return lib;
        }

        ~library_manager()
        {
            clean_all_libs();
        }

        static void remove_item(const std::shared_ptr<library_private>& item)
        {
            auto& manager = get_instance();
            std::lock_guard<std::mutex> lock(manager.m_library_mutex);

            auto itr = manager.m_library_map.find(item->get_file_name().to_string()); // because we use string_view to find the item
            if (itr != manager.m_library_map.end())
                manager.m_library_map.erase(itr);
        }

    private:
        library_manager() {}

        static library_manager& get_instance()
        {
            static library_manager obj;
            return obj;
        }

        void clean_all_libs()
        {
            for (auto& item : m_library_map)
            {
                // only the manager holds library pointer, thus we can now unload
                if (item.second.use_count() == 1)
                {
                    item.second->set_load_count(1); // this will implicit force an unload of the library
                    item.second->unload();
                }
            }
        }

        // use std::less in order to use string_view for finding the item
        std::map<std::string, std::shared_ptr<library_private>> m_library_map;
        std::mutex m_library_mutex;
};
}

/////////////////////////////////////////////////////////////////////////////////////////

library::library(string_view file_name, string_view version)
:   m_pimpl(detail::library_manager::create_or_find_library(file_name, version)),
    m_is_loaded(false)
{

}

/////////////////////////////////////////////////////////////////////////////////////////

library::~library()
{
    // the library_manager holds an instance too, so the use_count is always >= 2
    // when 2 are left, it means there exist only one "library" instance
    if (m_pimpl.use_count() == 2 && m_pimpl->get_load_count() == 0)
        detail::library_manager::remove_item(m_pimpl);
}

/////////////////////////////////////////////////////////////////////////////////////////

bool library::load()
{
    if (m_is_loaded)
        return m_pimpl->is_loaded();

    m_is_loaded = true;
    return m_pimpl->load();
}

/////////////////////////////////////////////////////////////////////////////////////////

bool library::unload()
{
    if (m_is_loaded)
    {
        m_is_loaded = false;
        return m_pimpl->unload();
    }
    else
    {
        return false;
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

bool library::is_loaded() const RTTR_NOEXCEPT
{
    return m_pimpl->is_loaded();
}

/////////////////////////////////////////////////////////////////////////////////////////

string_view library::get_file_name() const RTTR_NOEXCEPT
{
    return m_pimpl->get_qualified_filename().empty() ? m_pimpl->get_file_name() :
                                                       m_pimpl->get_qualified_filename();
}

/////////////////////////////////////////////////////////////////////////////////////////

string_view library::get_error_string() const RTTR_NOEXCEPT
{
    return m_pimpl->get_error_string();
}

/////////////////////////////////////////////////////////////////////////////////////////

array_range<type> library::get_types() const RTTR_NOEXCEPT
{
    return m_pimpl->get_types();
}

/////////////////////////////////////////////////////////////////////////////////////////

array_range<property> library::get_global_properties() const RTTR_NOEXCEPT
{
    return m_pimpl->get_global_properties();
}

/////////////////////////////////////////////////////////////////////////////////////////

array_range<method> library::get_global_methods() const RTTR_NOEXCEPT
{
    return m_pimpl->get_global_methods();
}

/////////////////////////////////////////////////////////////////////////////////////////

} // end namespace rttr
