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

#ifndef RTTR_ENUMERATION_H_
#define RTTR_ENUMERATION_H_

#include "rttr/detail/base/core_prerequisites.h"
#include "rttr/type.h"
#include "rttr/string_view.h"
#include "rttr/detail/misc/class_item_mapper.h"

#include <memory>
#include <string>

namespace rttr
{
class type;
class variant;
class argument;

namespace detail
{
    class enumeration_wrapper_base;
}

/*!
 * The \ref enumeration class provides several meta information about an enum.
 *
 * A instance of an enumeration class can only be obtained from the \ref type class or the \ref property class.
 * See \ref type::get_enumeration() and \ref property::get_enumeration().
 *
 * For registration an enum, nested inside a class, see \ref registration::class_<T>::enumeration()
 * and for global enums see \ref registration::enumeration().
 *
 * Meta Information
 * ----------------
 * An \ref enumeration is described by it's declared name (\ref get_name()), it's enumerator names (\ref get_names())
 * and it's corresponding constant values (\ref get_values()).
 * The name is represented as \ref string_view and the values are stored as the underlying enum value.
 * When the \ref enumeration was declared inside a class, then \ref get_declaring_type() can be used to obtain the type of this class.
 *
 * The conversion functions \ref name_to_value(), \ref value_to_name() allow conversion between the value representation of an enumeration and its literal representation.
 *
 * Copying and Assignment
 * ----------------------
 * A \ref enumeration object is lightweight and can be copied by value. However, each copy will refer to the same underlying enumeration.
 *
 * Typical Usage
 * ----------------------
 *
\code{.cpp}
  using namespace rttr;
  enum class E_Alignment
  {
     AlignLeft       = 0x0001,
     AlignRight      = 0x0002,
     AlignHCenter    = 0x0004,
     AlignJustify    = 0x0008
  };

  RTTR_REGISTRATION
  {
    rttr::registration::enumeration<E_Alignment>("E_Alignment")
        (
            value("AlignLeft",      E_Alignment::AlignLeft),
            value("AlignRight",     E_Alignment::AlignRight),
            value("AlignHCenter",   E_Alignment::AlignHCenter),
            value("AlignJustify",   E_Alignment::AlignJustify)
        );
  }

  enumeration enum_align = type::get_by_name("E_Alignment").get_enumeration();
  if (enum_align)
  {
     E_Alignment enum_value = E_Alignment::AlignLeft;
     string_view name = enum_align.value_to_name(enum_value);
     std::cout << name; // prints "AlignLeft"

     variant var = enum_align.name_to_value("AlignJustify");
     enum_value = var.get_value<E_Alignment>(); // is now: "AlignJustify"
     std::cout << var.to_int();                 // prints "8";
     std::cout << var.to_string();              // prints "AlignJustify";
  }
\endcode
 *
 * \see method, property, constructor and type
 */
class RTTR_API enumeration
{
    public:
        /*!
         * \brief Returns true if this \ref enumeration is valid, otherwise false.
         *
         * \return True if this \ref enumeration is valid, otherwise false.
         */
        bool is_valid() const RTTR_NOEXCEPT;

        /*!
         * \brief Convenience function to check if this \ref enumeration is valid or not.
         *
         * \return True if this \ref enumeration is valid, otherwise false.
         */
        explicit operator bool() const RTTR_NOEXCEPT;

        /*!
         * \brief Returns the declared name of this \ref enumeration.
         *
         * \return Name of the \ref enumeration.
         */
        string_view get_name() const RTTR_NOEXCEPT;

        /*!
         * \brief Returns the underlying type (int, unsigned int, etc.) of this \ref enumeration.
         *
         * \return Data type of the \ref enumeration.
         */
        type get_underlying_type() const RTTR_NOEXCEPT;

        /*!
         * \brief Returns the type object of this \ref enumeration.
         *
         * \return Data type of the \ref enumeration.
         */
        type get_type() const RTTR_NOEXCEPT;

        /*!
         * \brief Returns the \ref type of the class or struct that declares this \ref enumeration.
         *
         * \remark When this enumeration does not belong to a class (i.e. is a global enumeration) it will return an invalid type object.
         *         When this enumeration is not valid, this function will return an invalid type object (see \ref type::is_valid).
         *
         * \return \ref type "Type" of the declaring class/struct for this enumeration.
         */
        type get_declaring_type() const RTTR_NOEXCEPT;

        /*!
         * \brief Returns the meta data for the given key \p key.
         *
         * \remark When no meta data is registered with the given \p key,
         *         an invalid \ref variant object is returned (see \ref variant::is_valid).
         *
         * \return A variant object, containing arbitrary data.
         */
        variant get_metadata(const variant& key) const;

        /*!
         * \brief Returns all enum names registered for this enumeration.
         *
         * \remark When the enumeration is invalid then an empty range is returned.
         *
         * \return A range of enumeration names.
         */
        array_range<string_view> get_names() const RTTR_NOEXCEPT;


         /*!
         * \brief Returns all enum values registered for this enumeration.
         *
         * \remark When the enumeration is invalid then an empty range is returned.
         *
         * \return A range of enumeration values.
         */
        array_range<variant> get_values() const RTTR_NOEXCEPT;

        /*!
         * \brief Returns the string_view that is used as the name of the given enumeration \p value,
         *        or an empty string_view if the \p value is not defined.
         *
         * \return A string_view object, containing the name for the given value.
         */
        string_view value_to_name(argument value) const;

        /*!
         * \brief Returns the value of the given enumeration \p name, or an empty variant if the name is not defined.
         *
         * \return A variant object, containing the value for the given \p name.
         */
        variant name_to_value(string_view name) const;

        /*!
         * \brief Returns true if this enumeration is the same like the \p other.
         *
         * \return True if both enumerations are equal, otherwise false.
         */
        bool operator==(const enumeration& other) const RTTR_NOEXCEPT;

        /*!
         * Returns true if this enumeration is the not the same like the \p other.
         *
         * \return True if both enumerations are different, otherwise false.
         */
        bool operator!=(const enumeration& other) const RTTR_NOEXCEPT;

    private:
        enumeration(const detail::enumeration_wrapper_base* wrapper) RTTR_NOEXCEPT;

        template<typename T>
        friend T detail::create_item(const detail::class_item_to_wrapper_t<T>* wrapper);
        template<typename T>
        friend T detail::create_invalid_item();
    private:
        const detail::enumeration_wrapper_base* m_wrapper;
};

} // end namespace rttr

#endif // RTTR_ENUMERATION_H_
