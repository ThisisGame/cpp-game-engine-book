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

#ifndef RTTR_DESTRUCTOR_H_
#define RTTR_DESTRUCTOR_H_

#include "rttr/detail/base/core_prerequisites.h"
#include "rttr/detail/misc/class_item_mapper.h"

namespace rttr
{

class variant;
class type;

namespace detail
{
class destructor_wrapper_base;
template<>
RTTR_API destructor create_invalid_item();
}

/*!
 * The \ref destructor class provides a destructor for registered types.
 *
 * A instance of a destructor class can only be obtained from the \ref type class.
 * See \ref type::get_destructor().
 *
 * Copying and Assignment
 * ----------------------
 * A \ref destructor object is lightweight and can be copied by value. However, each copy will refer to the same underlying destructor.
 *
 * Typical Usage
 * ----------------------
 *
 * \code{.cpp}
 *     constructor string_ctor == type::get_by_name("std::string").get_constructor({type::get<const char*>()});
 *
 *     variant my_string = string_ctor.invoke("Hello World"); // returns an ptr to the object on the heap
 *
 *     type::get("std::string").get_destructor().invoke(my_string);
 *
 *     my_string.is_valid(); // yield to false
 * \endcode
 *
 * \see method, property, enumeration, constructor and type
 */
class RTTR_API destructor
{
    public:
        /*!
         * \brief Returns true whether this destructor object is valid; otherwise false.
         *
         * \return Returns true when the destructor is valid; otherwise false.
         */
        bool is_valid() const RTTR_NOEXCEPT;

        /*!
         * \brief Convenience function to check if this destructor is valid or not.
         *
         * \return True if this destructor is valid, otherwise false.
         */
        explicit operator bool() const RTTR_NOEXCEPT;

        /*!
         * Returns the class that declares this destructor.
         *
         * \remark When this destructor is not valid, this function will return an invalid type object (see \ref type::is_valid).
         *
         * \return \ref type "Type" of the declaring class/struct for this destructor.
         */
        type get_declaring_type() const RTTR_NOEXCEPT;

        /*!
         * \brief Returns the rttr::type for which this destructor can delete objects.
         *
         * \return The type of this destructor.
         */
        type get_destructed_type() const RTTR_NOEXCEPT;

        /*!
         * \brief Destroys the contained object in the variant \p obj.
         *
         * \remark When the \p obj could be destroyed the given \p obj is invalid after calling this method;
         *         Otherwise it is still valid.
         *
         * \return True if the destructor of the object could be invoked, otherwise false.
         */
        bool invoke(variant& obj) const RTTR_NOEXCEPT;

        /*!
         * \brief Returns true if this destructor is the same like the \p other.
         *
         * \return True if both destructors are equal, otherwise false.
         */
        bool operator==(const destructor& other) const RTTR_NOEXCEPT;

        /*!
         * Returns true if this destructor is the not the same like the \p other.
         *
         * \return True if both destructors are different, otherwise false.
         */
        bool operator!=(const destructor& other) const RTTR_NOEXCEPT;

    private:
        destructor(const detail::destructor_wrapper_base* wrapper) RTTR_NOEXCEPT;

        template<typename T>
        friend T detail::create_item(const detail::class_item_to_wrapper_t<T>* wrapper);
        template<typename T>
        friend T detail::create_invalid_item();

    private:
        const detail::destructor_wrapper_base* m_wrapper;
};

} // end namespace rttr

#endif // RTTR_DESTRUCTOR_H_
