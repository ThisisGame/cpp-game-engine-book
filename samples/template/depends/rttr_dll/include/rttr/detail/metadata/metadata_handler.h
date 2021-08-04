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

#ifndef RTTR_METADATA_HANDLER_H_
#define RTTR_METADATA_HANDLER_H_

#include "rttr/detail/base/core_prerequisites.h"
#include "rttr/detail/metadata/metadata.h"
#include "rttr/variant.h"

#include <array>

namespace rttr
{
namespace detail
{

/*!
 * This class holds an index to possible meta data.
 * This can be also a name or the declaring type of a property or method.
 */
template<std::size_t Metadata_Count>
class metadata_handler
{
    public:
        RTTR_FORCE_INLINE metadata_handler(std::array<metadata, Metadata_Count> new_data) : m_metadata_list(std::move(new_data)) { }

        RTTR_INLINE variant get_metadata(const variant& key) const
        {
            // I don't expect that many data, so we iterate from start to end
            for (const auto& item : m_metadata_list)
            {
                if (item.get_key() == key)
                    return item.get_value();
            }

            return variant();
        }

    private:
        std::array<metadata, Metadata_Count> m_metadata_list;
};

/*!
 * We use the C++ idiom of empty base class optimization.
 * So a wrapper with no metadata, will increase its sizeof(T).
 */
template<>
class metadata_handler<0>
{
    public:
        RTTR_FORCE_INLINE metadata_handler(std::array<metadata, 0>) {}

        RTTR_FORCE_INLINE void set_metadata(std::array<metadata, 0> new_data) { }

        RTTR_INLINE variant get_metadata(const variant& key) const  { return variant(); }
};

} // end namespace detail
} // end namespace rttr

#endif // RTTR_METADATA_HANDLER_H_
