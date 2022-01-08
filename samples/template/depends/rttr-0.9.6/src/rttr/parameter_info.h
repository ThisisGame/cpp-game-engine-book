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
#ifndef RTTR_PARAMETER_INFO_H_
#define RTTR_PARAMETER_INFO_H_

#include "rttr/detail/base/core_prerequisites.h"

#include "rttr/string_view.h"

#include <string>
#include <vector>
#include <cstdint>

namespace rttr
{

class variant;
class type;
class parameter_info;

namespace detail
{
    class parameter_info_wrapper_base;
    template<typename T>
    static parameter_info create_param_info(const T&);
}

/*!
 * The \ref parameter_info class provides several meta information about a parameter.
 *
 * A instance of a parameter_info class can be obtained from the \ref method or \ref constructor class.
 * See \ref method::get_parameter_infos() and \ref constructor::get_parameter_infos().
 *
 * For registration additional parameter infos, see \ref parameter_names().
 *
 * Meta Information
 * ----------------

 *
 * Copying and Assignment
 * ----------------------
 * A \ref parameter_info object is lightweight and can be copied by value. However, each copy will refer to the same underlying parameter_info.
 *
 * Typical Usage
 * ----------------------
 *
 * \code{.cpp}
 *   using namespace rttr;
 *
 *   void set_window_geometry(const char* name, int w, int h) {...}
 *
 *   RTTR_REGISTRATION
 *   {
 *        registration::method("set_window_geometry", &set_window_geometry)
 *        (
 *            parameter_names("window name", "width", "height")
 *        );
 *   }
 *
 *   //...
 *
 *   method meth = type::get_global_method("set_window_geometry");
 *   std::vector<parameter_info> param_list = meth.get_parameter_infos();
 *   for (const auto& info : param_list)
 *   {
 *       // print all names of the parameter types and its position in the paramter list
 *       std::cout << " name: '" << info.get_type().get_name() << "'\n"
 *                 << "index: " << info.get_index()
 *                              << std::endl;
 *   }
 * \endcode
 *
 * Output:
 *
 *      name: 'window name'
 *     index: 0
 *      name: 'width'
 *     index: 1
 *      name: 'height'
 *     index: 2
 *
 * \see method and constructor
 */
class RTTR_API parameter_info
{
    public:
        /*!
         * \brief Returns type of this parameter.
         *
         * \return Parameter type.
         */
        type get_type() const RTTR_NOEXCEPT;

        /*!
         * \brief Returns true when this parameter has a default value; otherwise false.
         *
         * \return Bool to indicate that this parameter has a default value.
         */
        bool has_default_value() const RTTR_NOEXCEPT;

        /*!
         * \brief Returns the default value as \ref variant for this parameter;
         *        or an \ref variant::is_valid() "invalid" variant to indicate that no default value is available.
         *
         * \return Default value as variant.
         */
        variant get_default_value() const;

        /*!
         * \brief Returns the name of this parameter.
         *        When no name was provided during registration via \ref parameter_names(),
         *        then an empty string is returned.
         *
         * \return The name of the parameter.
         */
        string_view get_name() const RTTR_NOEXCEPT;

        /*!
         * \brief Returns the zero-based position of the parameter in the formal parameter list.
         *
         * \return An integer representing the position this parameter occupies in the parameter list.
         */
        uint32_t get_index() const RTTR_NOEXCEPT;

        /*!
         * \brief Returns true if this property is the same like the \p other.
         *
         * \return True if both properties are equal, otherwise false.
         */
        bool operator==(const parameter_info& other) const RTTR_NOEXCEPT;

        /*!
         * Returns true if this property is the not the same like the \p other.
         *
         * \return True if both properties are different, otherwise false.
         */
        bool operator!=(const parameter_info& other) const RTTR_NOEXCEPT;

    private:
        template<typename T>
        friend parameter_info detail::create_param_info(const T&);

        parameter_info(const detail::parameter_info_wrapper_base* wrapper) RTTR_NOEXCEPT;
    private:
        const detail::parameter_info_wrapper_base* m_wrapper;
};

} // end namespace rttr

#endif // RTTR_PARAMETER_INFO_H_
