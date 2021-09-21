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

#ifndef RTTR_METHOD_H_
#define RTTR_METHOD_H_

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
class instance;
class argument;
class method;

namespace detail
{
    class method_wrapper_base;
}

/*!
 * The \ref method class provides several meta information about a method and can be invoked.
 *
 * A instance of a method class can only be obtained from the \ref type class.
 * See \ref type::get_method() and \ref type::get_methods().
 *
 * For registration a method, nested inside a class, see \ref registration::class_<T>::method()
 * and for global methods see \ref registration::method().
 *
 * Meta Information
 * ----------------
 * A \ref method has a \ref get_name "name",  a \ref get_signature "signature", a \ref get_return_type "return type", a list of \ref get_parameter_infos "parameter information"
 * as well as attributes that specify its behavior: \ref is_static().
 * When the \ref method was declared inside a class, then \ref get_declaring_type() can be used to obtain the type of this class.
 *
 * The method can be invoked with \ref invoke(); When its not a \ref is_static "static method" you have to provide a valid class instance to invoke the method.
 * This instance can be the raw type on the stack; the current class hierarchy level doesn't matter. It can be also a raw pointer to the object or
 * a \ref variant which contains the instance, again as pointer or stack object.
 * When the method is declared as static you you still have to provide an empty instance object,
 * use therefore the default ctor of \ref instance::instance() "instance()", or as shortcut use simply `{}`.
 *
 * A method will be successfully invoked when the provided instance can be converted to the \ref get_declaring_type() "declared class" type.
 * When the method has \ref get_parameter_infos "parameters" defined, then the same number of arguments must be provided and the type itself must **100% match** the type of the registered function.
 * An automatically type conversion is **not** performed.
 *
 * The return type of \ref invoke() is \ef variant object.
 * This object contains not only the possible return value of a function, it also indicates whether the method was invoked or not.
 * A \ref variant::is_valid "valid" variant object means, that then the method was successfully invoked, otherwise not.
 * When the invoked method has no return type, i.e. is a `void` method, then a valid variant of type `void` is returned.
 *
 * While the \ref invoke() function can directly forward up to six arguments, it is sometime necessary to forward even more arguments.
 * Therefore the function \ref invoke_variadic() should be used; it allows to pack an unlimited amount of arguments into a std::vector and forward them to the function.
 *
 * Another way to invoke a method is to use the \ref type class through \ref type::invoke().
 *
 * Copying and Assignment
 * ----------------------
 * A \ref method object is lightweight and can be copied by value. However, each copy will refer to the same underlying method.
 *
 * Typical Usage
 * ----------------------
 *
 * \code{.cpp}
 *  using namespace rttr;
 *  struct MyStruct { int my_method(int param) { return param; } };
 *  //...
 *  variant obj = type::get_by_name("MyStruct").create({});
 *  method func = obj.get_type().get_method("my_method");
 *  if (func)
 *  {
 *     variant val = func.invoke(obj, 23);
 *     std::cout << val.get_value<int>(); // prints 23
 *     // you can also invoke the method with an object on the stack
 *     MyStruct inst;
 *     val = func.invoke(inst, 42);
 *     std::cout << val.get_value<int>(); // prints 42
 *     // or as pointer
 *     MyStruct* ptr = &inst;
 *     val = func.invoke(ptr, 7);
 *     std::cout << val.get_value<int>(); // prints 7
 *  }
 * \endcode
 *
 * \see property, enumeration, constructor and type
 */
class RTTR_API method
{
    public:
        /*!
         * \brief Returns true if this method is valid, otherwise false.
         *
         * \return True if this method is valid, otherwise false.
         */
        bool is_valid() const RTTR_NOEXCEPT;

        /*!
         * \brief Convenience function to check if this method is valid or not.
         *
         * \return True if this method is valid, otherwise false.
         */
        explicit operator bool() const RTTR_NOEXCEPT;

        /*!
         * \brief Returns the name of this method.
         *
         * \return Name of the method.
         */
        string_view get_name() const RTTR_NOEXCEPT;

        /*!
         * \brief Returns the access level with which this method was
         *        \ref registration::class_<T>::method() "registered".
         *
         * \remark When the method is not valid, this function will return level \ref access_levels::public_access.
         *
         * \return \ref access_levels of the method.
         */
        access_levels get_access_level() const RTTR_NOEXCEPT;

        /*!
         * \brief Returns true if this method is static method, otherwise false.
         *        A static method does not need an instance for performing an invoke.
         *
         * \remark When the method is not valid, this function will return false.
         *
         * \return True if this is a static method, otherwise false.
         */
        bool is_static() const RTTR_NOEXCEPT;

        /*!
         * Returns the type object of the return type.
         *
         * \return The type of the return type.
         */
        type get_return_type() const RTTR_NOEXCEPT;

        /*!
         * Returns the class that declares this method.
         *
         * \remark When this method does not belong to a class (i.e. is a global method) it will return an invalid type.
         *         When this method is not valid, this function will return an invalid type object (see \ref type::is_valid).
         *
         * \return \ref type "Type" of the declaring class/struct for this method.
         */
        type get_declaring_type() const RTTR_NOEXCEPT;

        /*!
         * \brief Returns an ordered range of \ref parameter_info objects, which matches the signature of the method.
         *
         * \return A range of parameter_info objects of the method signature.
         */
        array_range<parameter_info> get_parameter_infos() const RTTR_NOEXCEPT;

        /*!
         * \brief Returns the signature of this method as readable string.
         *
         * \return The signature as readable string.
         */
        string_view get_signature() const RTTR_NOEXCEPT;

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
         * \brief Invokes the method represented by the current instance \p object.
         *
         * \remark When it's a static method you still need to provide an instance object, use therefore the default ctor of \ref instance::instance() "instance()".
         *
         * \see get_parameter_infos()
         *
         * \return The possible return value of the method.
         */
        variant invoke(instance object) const;

        /*!
         * \brief Invokes the method represented by the current instance \p object, using the specified parameters.
         *
         * \remark The given argument type has to match **exactly** the type of the underling method parameter,
         *         otherwise the method cannot be invoked and an invalid \ref variant object (see \ref variant::is_valid)
         *         will be returned.
         *         When it's a static method you still need to provide an instance object,
         *         use therefore the default ctor of \ref instance::instance() "instance()".
         *
         * \return The possible return value of the method.
         *
         * \see get_parameter_infos()
         */
        variant invoke(instance object, argument arg1) const;

        /*!
         * \brief Invokes the method represented by the current instance \p object, using the specified parameters.
         *
         * \remark The given argument type has to match **exactly** the type of the underling method parameter,
         *         otherwise the method cannot be invoked and an invalid \ref variant object (see \ref variant::is_valid)
         *         will be returned.
         *         When it's a static method you still need to provide an instance object,
         *         use therefore the default ctor of \ref instance::instance() "instance()".
         *
         * \see get_parameter_infos()
         *
         * \return The possible return value of the method.
         */
        variant invoke(instance object, argument arg1, argument arg2) const;

        /*!
         * \brief Invokes the method represented by the current instance \p object, using the specified parameters.
         *
         * \remark The given argument type has to match **exactly** the type of the underling method parameter,
         *         otherwise the method cannot be invoked and an invalid \ref variant object (see \ref variant::is_valid)
         *         will be returned.
         *         When it's a static method you still need to provide an instance object,
         *         use therefore the default ctor of \ref instance::instance() "instance()".
         *
         * \see get_parameter_infos()
         *
         * \return The possible return value of the method.
         */
        variant invoke(instance object, argument arg1, argument arg2, argument arg3) const;

        /*!
         * \brief Invokes the method represented by the current instance \p object, using the specified parameters.
         *
         * \remark The given argument type has to match **exactly** the type of the underling method parameter,
         *         otherwise the method cannot be invoked and an invalid \ref variant object (see \ref variant::is_valid)
         *         will be returned.
         *         When it's a static method you still need to provide an instance object,
         *         use therefore the default ctor of \ref instance::instance() "instance()".
         *
         * \see get_parameter_infos()
         *
         * \return The possible return value of the method.
         */
        variant invoke(instance object, argument arg1, argument arg2, argument arg3, argument arg4) const;

        /*!
         * \brief Invokes the method represented by the current instance \p object, using the specified parameters.
         *
         * \remark The given argument type has to match **exactly** the type of the underling method parameter,
         *         otherwise the method cannot be invoked and an invalid \ref variant object (see \ref variant::is_valid)
         *         will be returned.
         *         When it's a static method you still need to provide an instance object,
         *         use therefore the default ctor of \ref instance::instance() "instance()".
         *
         * \see get_parameter_infos()
         *
         * \return The possible return value of the method.
         */
        variant invoke(instance object, argument arg1, argument arg2, argument arg3, argument arg4,
                       argument arg5) const;

        /*!
         * \brief Invokes the method represented by the current instance \p object, using the specified parameters.
         *
         * \remark The given argument type has to match **exactly** the type of the underling method parameter,
         *         otherwise the method cannot be invoked and an invalid \ref variant object (see \ref variant::is_valid)
         *         will be returned.
         *         When it's a static method you still need to provide an instance object,
         *         use therefore the default ctor of \ref instance::instance() "instance()".
         *
         * \see get_parameter_infos()
         *
         * \return The possible return value of the method.
         */
        variant invoke(instance object, argument arg1, argument arg2, argument arg3, argument arg4,
                       argument arg5, argument arg6) const;

        /*!
         * \brief Invokes the method represented by the current instance \p object, using the specified parameters.
         *        Use this method when the argument count is higher then six.
         *
         * \remark The given argument type has to match **exactly** the type of the underling method parameter,
         *         otherwise the method cannot be invoked and an invalid \ref variant object (see \ref variant::is_valid)
         *         will be returned.
         *         When it's a static method you still need to provide an instance object,
         *         use therefore the default ctor of \ref instance::instance() "instance()".
         *         Using this invoke function is slower, then specifying the arguments directly.
         *
         * \see get_parameter_infos()
         *
         * \return The possible return value of the method.
         */
        variant invoke_variadic(instance object, std::vector<argument> args) const;

        /*!
         * \brief Returns true if this method is the same like the \p other.
         *
         * \return True if both methods are equal, otherwise false.
         */
        bool operator==(const method& other) const RTTR_NOEXCEPT;

        /*!
         * Returns true if this method is the not the same like the \p other.
         *
         * \return True if both methods are different, otherwise false.
         */
        bool operator!=(const method& other) const RTTR_NOEXCEPT;

    private:
        method(const detail::method_wrapper_base* wrapper) RTTR_NOEXCEPT;

        template<typename T>
        friend T detail::create_item(const detail::class_item_to_wrapper_t<T>* wrapper);
        template<typename T>
        friend T detail::create_invalid_item();

    private:
        const detail::method_wrapper_base* m_wrapper;
};

} // end namespace rttr

#endif // RTTR_METHOD_H_
