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

#ifndef RTTR_REGISTRATION_STATE_SAVER_H_
#define RTTR_REGISTRATION_STATE_SAVER_H_

#include "rttr/detail/base/core_prerequisites.h"
#include "rttr/array_range.h"

#include <vector>

namespace rttr
{
namespace detail
{

/*!
 * This class saves the current state of the registration of all possible items
 * and will undo the registration when the instance is destroyed.
 */
class RTTR_LOCAL registration_state_saver
{
    public:
        registration_state_saver() = default;

        /*!
         * This will not unregister any of the items.
         */
        ~registration_state_saver() = default;

        // no copy
        registration_state_saver(const registration_state_saver&) = delete;
        // no assign
        registration_state_saver& operator=(const registration_state_saver&) = delete;

        /*!
         * Begin storing the current registration state.
         * The first method you should call.
         */
        void save_state_begin();

        /*!
         * End storing the current registration state.
         * The last method you should call.
         */
        void save_state_end();

        /*!
         * Reset the state of all currently held member variables.
         */
        void reset();

        /*!
         * Returns the list of newly registered type in the current state.
         */
        array_range<type> get_types() const RTTR_NOEXCEPT;

        /*!
         * Returns the list of newly registered global properties in the current state.
         */
        array_range<property> get_global_properties() const RTTR_NOEXCEPT;

        /*!
         * Returns the list of newly registered global methods in the current state.
         */
        array_range<method> get_global_methods() const RTTR_NOEXCEPT;

    private:
        std::size_t                 m_old_type_size     = 0;
        std::size_t                 m_old_methods_size  = 0;
        std::size_t                 m_old_property_size = 0;
        std::vector<type>           m_types;
        std::vector<property>       m_global_properties;
        std::vector<method>         m_global_methods;
};

} // end namespace detail
} // end namespace rttr

#endif // RTTR_REGISTRATION_STATE_SAVER_H_
