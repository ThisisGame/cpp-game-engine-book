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
#ifndef RTTR_CONSTRUCTOR_H_
#define RTTR_CONSTRUCTOR_H_

#include "rttr/detail/base/core_prerequisites.h"
#include "rttr/detail/misc/class_item_mapper.h"

#include "rttr/parameter_info.h"
#include "rttr/access_levels.h"
#include "rttr/array_range.h"
#include "rttr/string_view.h"

#include <string>
#include <vector>

namespace rttr
{

class variant;
class type;
class argument;

namespace detail
{
class constructor_wrapper_base;
}

/*!
 * The \ref constructor class provides several meta information about a constructor and can be invoked.
 *
 * A instance of a constructor class can only be obtained from the \ref type class.
 * See \ref type::get_constructor() and \ref type::get_constructors().
 *
 * For registration a constructor of a class, see \ref registration::class_<T>::constructor().
 *
 * Meta Information
 * ----------------
 * A \ref constructor has a signature (\ref get_signature()) and instantiate exactly one type (\ref get_instantiated_type()).
 * With \ref get_parameter_infos() you retrieve all information of the parameters for this constructor.
 * When the \ref constructor was declared inside a class, then \ref get_declaring_type() can be used to obtain the type of this class.
 *
 * The constructor can be invoked with \ref invoke() or via the \ref type class through \ref type::create().
 * The created object will be copied into a variant and returned. Depending on the used policy during the registration process,
 * it can be an object with automatic or dynamic storage.
 *
 * Copying and Assignment
 * ----------------------
 * A \ref constructor object is lightweight and can be copied by value. However, each copy will refer to the same underlying constructor.
 *
 * Typical Usage
 * ----------------------
 *
 * \code{.cpp}
 *      constructor ctor = type::get_by_name("std::string").get_constructor({type::get<std::string>()}); // retrieve the copy ctor
 *
 *      variant var = ctor.invoke(std::string("Hello World"));          // returns an object with automatic storage
 *
 *      std::cout << var.get_value<std::string>().c_str() << std::endl; // prints 'Hello World'
 * \endcode
 *
 * \see method, property, enumeration, destructor and type
 */
class RTTR_API constructor
{
    public:
        /*!
         * \brief Returns true if this constructor is valid, otherwise false.
         *
         * \return True if this constructor is valid, otherwise false.
         */
        bool is_valid() const RTTR_NOEXCEPT;

        /*!
         * \brief Convenience function to check if this constructor is valid or not.
         *
         * \return True if this constructor is valid, otherwise false.
         */
        explicit operator bool() const RTTR_NOEXCEPT;

        /*!
         * \brief Returns access level with which this constructor was
         *        \ref registration::class_<T>::constructor() "registered".
         *
         * \remark When the constructor is not valid, this function will return level \ref access_levels::public_access.
         *
         * \return \ref access_levels of the method.
         */
        access_levels get_access_level() const RTTR_NOEXCEPT;

        /*!
         * Returns the type object of the instantiated type.
         *
         * \return The instantiated type.
         */
        type get_instantiated_type() const RTTR_NOEXCEPT;

        /*!
         * \brief Returns the \ref type of the class or struct that declares this \ref constructor.
         *
         * \remark When this constructor does not belong to a class (i.e. is a primitive type) it will return an invalid type object.
         *         When this constructor is not valid, this function will return an invalid type object (see \ref type::is_valid).
         *
         * \return \ref type "Type" of the declaring class/struct for this enumeration.
         */
        type get_declaring_type() const RTTR_NOEXCEPT;

        /*!
         * \brief Returns the signature of this constructor as readable string.
         *
         * \return The signature as readable string.
         */
        string_view get_signature() const RTTR_NOEXCEPT;

        /*!
         * \brief Returns an ordered range of \ref parameter_info objects, which matches the signature of the constructor.
         *
         * \return A range of parameter_info objects of the constructor signature.
         */
        array_range<parameter_info> get_parameter_infos() const RTTR_NOEXCEPT;

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
         * \brief Invokes the constructor of type returned by \ref get_instantiated_type().
         *        The instance will always be created on the heap and will be returned as variant object.
         *
         * \remark Returns an invalid \ref variant object (see \ref variant::is_valid), when the arguments does
         *         not match the parameters of the underlying constructor.
         *
         * \see get_parameter_infos()
         *
         * \return An instance of the type \ref get_instantiated_type().
         */
        variant invoke() const;

        /*!
         * \brief Invokes the constructor of type returned by \ref get_instantiated_type().
         *        The instance will always be created on the heap and will be returned as variant object.
         *
         * \remark The given argument type has to match **exactly** the type of the underling constructor parameter,
         *         otherwise the constructor cannot be invoked and an invalid \ref variant object (see \ref variant::is_valid)
         *         will be returned.
         *
         * \see get_parameter_infos()
         *
         * \return An instance of the type \ref get_instantiated_type().
         */
        variant invoke(argument arg1) const;

        /*!
         * \brief Invokes the constructor of type returned by \ref get_instantiated_type().
         *        The instance will always be created on the heap and will be returned as variant object.
         *
         * \remark The given argument type has to match **exactly** the type of the underling constructor parameter,
         *         otherwise the constructor cannot be invoked and an invalid \ref variant object (see \ref variant::is_valid)
         *         will be returned.
         *
         * \see get_parameter_infos()
         *
         * \return An instance of the type \ref get_instantiated_type().
         */
        variant invoke(argument arg1, argument arg2) const;

        /*!
         * \brief Invokes the constructor of type returned by \ref get_instantiated_type().
         *        The instance will always be created on the heap and will be returned as variant object.
         *
         * \remark The given argument type has to match **exactly** the type of the underling constructor parameter,
         *         otherwise the constructor cannot be invoked and an invalid \ref variant object (see \ref variant::is_valid)
         *         will be returned.
         *
         * \see get_parameter_infos()
         *
         * \return An instance of the type \ref get_instantiated_type().
         */
        variant invoke(argument arg1, argument arg2, argument arg3) const;

        /*!
         * \brief Invokes the constructor of type returned by \ref get_instantiated_type().
         *        The instance will always be created on the heap and will be returned as variant object.
         *
         * \remark The given argument type has to match **exactly** the type of the underling constructor parameter,
         *         otherwise the constructor cannot be invoked and an invalid \ref variant object (see \ref variant::is_valid)
         *         will be returned.
         *
         * \see get_parameter_infos()
         *
         * \return An instance of the type \ref get_instantiated_type().
         */
        variant invoke(argument arg1, argument arg2, argument arg3, argument arg4) const;

        /*!
         * \brief Invokes the constructor of type returned by \ref get_instantiated_type().
         *        The instance will always be created on the heap and will be returned as variant object.
         *
         * \remark The given argument type has to match **exactly** the type of the underling constructor parameter,
         *         otherwise the constructor cannot be invoked and an invalid \ref variant object (see \ref variant::is_valid)
         *         will be returned.
         *
         * \see get_parameter_infos()
         *
         * \return An instance of the type \ref get_instantiated_type().
         */
        variant invoke(argument arg1, argument arg2, argument arg3, argument arg4,
                       argument arg5) const;

        /*!
         * \brief Invokes the constructor of type returned by \ref get_instantiated_type().
         *        The instance will always be created on the heap and will be returned as variant object.
         *
         * \remark The given argument type has to match **exactly** the type of the underling constructor parameter,
         *         otherwise the constructor cannot be invoked and an invalid \ref variant object (see \ref variant::is_valid)
         *         will be returned.
         *
         * \see get_parameter_infos()
         *
         * \return An instance of the type \ref get_instantiated_type().
         */
        variant invoke(argument arg1, argument arg2, argument arg3, argument arg4,
                       argument arg5, argument arg6) const;

        /*!
         * \brief Invokes the constructor of type returned by \ref get_instantiated_type().
         *        The instance will always be created on the heap and will be returned as variant object.
         *         Use this method when you need to instantiate a constructor with more then 6 parameters.
         *
         * \remark The given argument type has to match **exactly** the type of the underling constructor parameter,
         *         otherwise the constructor cannot be invoked and an invalid \ref variant object (see \ref variant::is_valid)
         *         will be returned.
         *         Using this invoke function is slower, then specifying the arguments directly.
         *
         * \see get_parameter_infos()
         *
         * \return An instance of the type \ref get_instantiated_type().
         */
        variant invoke_variadic(std::vector<argument> args) const;

        /*!
         * \brief Returns true if this constructor is the same like the \p other.
         *
         * \return True if both constructors are equal, otherwise false.
         */
        bool operator==(const constructor& other) const RTTR_NOEXCEPT;

        /*!
         * Returns true if this constructor is the not the same like the \p other.
         *
         * \return True if both constructors are different, otherwise false.
         */
        bool operator!=(const constructor& other) const RTTR_NOEXCEPT;

    private:
        constructor(const detail::constructor_wrapper_base* wrapper) RTTR_NOEXCEPT;

        template<typename T>
        friend T detail::create_item(const detail::class_item_to_wrapper_t<T>* wrapper);
        template<typename T>
        friend T detail::create_invalid_item();

    private:
        const detail::constructor_wrapper_base* m_wrapper;
};

} // end namespace rttr

#endif // RTTR_CONSTRUCTOR_H_
