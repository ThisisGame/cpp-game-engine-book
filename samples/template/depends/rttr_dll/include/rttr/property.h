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

#ifndef RTTR_PROPERTY_H_
#define RTTR_PROPERTY_H_

#include "rttr/detail/base/core_prerequisites.h"
#include "rttr/detail/misc/class_item_mapper.h"
#include "rttr/parameter_info.h"
#include "rttr/access_levels.h"
#include "rttr/string_view.h"

#include <string>

namespace rttr
{
class variant;
class type;
class enumeration;
class instance;
class argument;
class property;

namespace detail
{
    class property_wrapper_base;
}

/*!
 * The \ref property class provides several meta information about a property and gives read/write access to its value.
 *
 * A instance of a property class can only be obtained from the \ref type class.
 * See \ref type::get_property() and \ref type::get_properties().
 *
 * For registration a property, nested inside a class, see \ref registration::class_<T>::property()
 * and for global properties see \ref registration::property().
 *
 * Meta Information
 * ----------------
 * A \ref property has a \ref get_name() "name", and a \ref get_type() "type" as well as attributes that specify its behavior:
 * \ref is_readonly(), \ref is_static(), \ref is_enumeration(), \ref is_array().
 * When the \ref property was declared inside a class, then \ref get_declaring_type() can be used to obtain the type of this class.
 *
 * The property's values are set and retrieved with \ref set_value() and \ref get_value();
 * When its not a \ref is_static "static property" you have to provide a class instance to set/get the property value.
 * This instance can be the raw type on the stack; the current hierarchy level doesn't matter. It can be also a raw pointer to the object or
 * a \ref variant which contains the instance, again as pointer or stack object.
 * When the property is declared as \ref is_static "static" you you still have to provide an empty instance object,
 * use therefore the default ctor of \ref instance::instance() "instance()", or as shortcut use simply `{}`.
 *
 * A property will be successfully \ref set_value "set" when the provided instance can be converted to the \ref get_declaring_type() "declared class" type.
 * The new forwarded property value must 100% match the type of the registered property. An automatically type conversion is **not** performed.
 *
 * The return type of \ref get_value() is \ref variant object.
 * This object contains not only the value of the property, it also indicates whether the property value could be retrieved or not.
 * A \ref variant::is_valid "valid" variant object means, that the property was successfully retrieved, otherwise not.
 *
 * Another way to get access a property is through \ref type "type's" set and get functions. See \ref type::set_property_value() and type::get_property_value() for details.
 *
 * Copying and Assignment
 * ----------------------
 * A \ref property object is lightweight and can be copied by value. However, each copy will refer to the same underlying property.
 *
 * Typical Usage
 * ----------------------
 *
\code{.cpp}
  using namespace rttr;
  struct MyStruct { int value = 23; };
  //...
  variant obj = type::get_by_name("MyStruct").create({});
  property prop = type::get_by_name("MyStruct").get_property("value");
  if (prop)
  {
     variant val = prop.get_value(obj);
     std::cout << val.get_value<int>(); // prints 23
     MyStruct inst;
     val = prop.set_value(inst, 42);
     std::cout << inst.value; // prints 42
     // or as pointer
     MyStruct* ptr = &inst;
     val = prop.set_value(ptr, 7);
     std::cout << ptr->value; // prints 7
     // or do it all in one call
     type::get(inst).set_propert_value("value", inst, 1024);
     std::cout << inst.value; // prints 1024
  }
\endcode
 *
 * \see method, enumeration, constructor and type
 */
class RTTR_API property
{
    public:
        /*!
         * \brief Returns true if this property is valid, otherwise false.
         *
         * \return True if this property is valid, otherwise false.
         */
        bool is_valid() const RTTR_NOEXCEPT;

        /*!
         * \brief Convenience function to check if this property is valid or not.
         *
         * \return True if this property is valid, otherwise false.
         */
        explicit operator bool() const RTTR_NOEXCEPT;

        /*!
         * \brief Returns the access level with which this property was
         *        \ref registration::class_<T>::property() "registered".
         *
         * \remark When the property is not valid, this function will return level \ref access_levels::public_access.
         *
         * \return \ref access_levels of the property.
         */
        access_levels get_access_level() const RTTR_NOEXCEPT;

        /*!
         * \brief Returns true if this property is read only, otherwise false.
         *
         * \remark When the property is not valid, this function will return false.
         *
         * \return True if this is a read only property, otherwise false.
         */
        bool is_readonly() const RTTR_NOEXCEPT;

        /*!
         * \brief Returns true if this property is static property, otherwise false.
         *        A static property does not need an instance for performing set_value/get_value.
         *
         * \remark When the property is not valid, this function will return false.
         *
         * \return True if this is a static property, otherwise false.
         */
        bool is_static() const RTTR_NOEXCEPT;

        /*!
         * \brief Returns true if the underlying property is an \ref enumeration.
         *
         * \remark When the property is not valid, this function will return false.
         *
         * \return True if this is a \ref enumeration type, otherwise false.
         */
        bool is_enumeration() const RTTR_NOEXCEPT;

       /*!
         * \brief Returns the enumerator if this property is an enum type;
         *        otherwise the returned value is \ref enumeration::is_valid "not valid".
         *
         * \see is_enumeration()
         *
         * \return An enumeration object.
         */
        enumeration get_enumeration() const RTTR_NOEXCEPT;

        /*!
         * \brief Returns the name of this property.
         *
         * \remark When the property is not valid, this function will return an empty string_view.
         *
         * \return Name of the property.
         */
        string_view get_name() const RTTR_NOEXCEPT;

        /*!
         * \brief Returns the underlying \ref type object of this property.
         *
         * \remark When the property is not valid, this function will return an invalid type object.
         *
         * \return \ref type "Type" of the underlying property.
         */
        type get_type() const RTTR_NOEXCEPT;

        /*!
         * \brief Returns the \ref type of the class or struct that declares this property.
         *
         * \remark When this property does not belong to a class (i.e. is a global property) it will return an invalid type object.
         *         When this property is not valid, this function will return an invalid type object (see \ref type::is_valid).
         *
         * \return \ref type "Type" of the declaring class/struct for this property.
         */
        type get_declaring_type() const RTTR_NOEXCEPT;

        /*!
         * \brief Set the property of the given instance \p object to the given value \p arg.
         *
         * \remark  When the property is declared as \ref is_readonly "read only" this function will return false.
         *          When you have a static property just pass an empty instance as object argument.
         *          When the property is not valid, this function will return false.
         *
         * \see get_value().
         *
         * \return The return value indicates whether the operation was successful or not.
         */
        bool set_value(instance object, argument arg) const;

        /*!
         * \brief Returns the current property value of the given instance \p object.
         *
         * \remark When the property is static, you can forward an empty instance.
         *
         * \see set_value().
         *
         * \return The property value of the given instance \p object.
         */
        variant get_value(instance object) const;

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
         * \brief Returns true if this property is the same like the \p other.
         *
         * \return True if both properties are equal, otherwise false.
         */
        bool operator==(const property& other) const RTTR_NOEXCEPT;

        /*!
         * Returns true if this property is the not the same like the \p other.
         *
         * \return True if both properties are different, otherwise false.
         */
        bool operator!=(const property& other) const RTTR_NOEXCEPT;

    private:
        //! Constructs a property from a property_wrapper_base.
        property(const detail::property_wrapper_base* wrapper) RTTR_NOEXCEPT;

        template<typename T>
        friend T detail::create_item(const detail::class_item_to_wrapper_t<T>* wrapper);
        template<typename T>
        friend T detail::create_invalid_item();

    private:
        const detail::property_wrapper_base* m_wrapper;
};

} // end namespace rttr

#endif // RTTR_PROPERTY_H_
