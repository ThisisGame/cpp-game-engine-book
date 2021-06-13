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

#ifndef RTTR_TYPE_H_
#define RTTR_TYPE_H_

#include "rttr/detail/base/core_prerequisites.h"
#include "rttr/string_view.h"
#include "rttr/array_range.h"
#include "rttr/filter_item.h"

#include <type_traits>
#include <vector>
#include <string>
#include <memory>
#include <cstdint>

namespace rttr
{

class variant;
class constructor;
class destructor;
class method;
class property;
class enumeration;
class type;
class instance;
class argument;

template<typename Target_Type, typename Source_Type>
Target_Type rttr_cast(Source_Type object) RTTR_NOEXCEPT;

namespace detail
{
struct derived_info;
struct base_class_info;
struct type_converter_base;
class type_register;
class type_register_private;

static type get_invalid_type() RTTR_NOEXCEPT;
struct invalid_type{};
struct type_data;
class destructor_wrapper_base;
class property_wrapper_base;
RTTR_LOCAL RTTR_INLINE type create_type(type_data*) RTTR_NOEXCEPT;

template<typename T>
RTTR_LOCAL std::unique_ptr<type_data> make_type_data();

template<typename T, typename Tp, typename Converter>
struct variant_data_base_policy;

struct type_comparator_base;

RTTR_API bool compare_types_less_than(const void*, const void*, const type&, int&);
RTTR_API bool compare_types_equal(const void*, const void*, const type&, bool&);
} // end namespace detail

/*!
 * The \ref type class holds the type information for any arbitrary object.
 *
 * Every class or primitive data type can have an unique type object.
 * With the help of this object you can compare unknown types for equality at runtime or introspect the type
 * for its \ref property "properties", \ref method "methods", \ref enumeration "enumerations",
 * \ref constructor "constructors" and \ref destructor "destructor".
 *
 * Retrieve %type
 * ------------------
 * A type object **cannot** be created. It is only possible to retrieve a type object via three static template member functions:
 *
 * ### type::get<T>() ###
 *
 * This function just expects one template argument. Use it to check against a known type.
 *
 * \code{.cpp}
 *      type::get<int>() == type::get<int>();  // yields to true
 *      type::get<int>() == type::get<bool>(); // yields to false
 * \endcode
 *
 * ### type::get_by_name(string_view) ###
 *
 * This function just expects the name of the type. This is useful when you know only the name of the type and cannot include the type itself into the source code.
 *
 * \code{.cpp}
 *      type::get_by_name("int")  == type::get<int>(); // yields to true
 *      type::get_by_name("bool") == type::get<int>(); // yields to false
 *      type::get_by_name("MyNameSpace::MyStruct") == type::get<MyNameSpace::MyStruct>();  // yields to true
 * \endcode
 *
 * \remark Before using the function \ref type::get_by_name(), you have to use one time the function via \ref type::get<T>(), otherwise the type is not registered in the type system.
 *
 * ### type::get<T>(T&& obj) ###
 *
 * This function takes a universal reference and returns from every given object the corresponding type object.
 *
 * \code{.cpp}
 *      int int_obj;
 *      int* int_obj_ptr = &int_obj;
 *      const int* c_int_obj_ptr = int_obj_ptr;
 *
 *      type::get<int>()         == type::get(int_obj);        // yields to true
 *      type::get<int*>()        == type::get(int_obj_ptr);    // yields to true
 *      type::get<const int*>()  == type::get(c_int_obj_ptr);  // yields to true
 * \endcode
 *
 * When this function is called for a glvalue expression whose type is a polymorphic class type,
 * then the result refers to a \ref type object representing the type of the most derived object.
 *
 * \code{.cpp}
 *      struct Base { RTTR_ENABLE() };
 *      struct Derived : Base { RTTR_ENABLE(Base) };
 *      //...
 *      Derived d;
 *      Base& base = d;
 *      type::get<Derived>()   == type::get(base);      // yields to true
 *      type::get<Base>()      == type::get(base);      // yields to false
 *
 *      // remark, when called with pointers:
 *      Base* base_ptr = &d;
 *      type::get<Derived>()   == type::get(base_ptr);  // yields to false
 *      type::get<Base*>()     == type::get(base_ptr);  // yields to true
 * \endcode
 *
 * \remark If the type of the expression is a cv-qualified type, the result of the rttr::type::get expression refers to a rttr::type object representing the cv-unqualified type.
 *
 * \code{.cpp}
 *      class D { ... };
 *      D d1;
 *      const D d2;
 *      type::get(d1)  == type::get(d2);         // yields true
 *      type::get<D>() == type::get<const D>();  // yields true
 *      type::get<D>() == type::get(d2);         // yields true
 *      type::get<D>() == type::get<const D&>(); // yields true
 *      type::get<D>() == type::get<const D*>(); // yields false
 * \endcode
 * Any `top level` cv-qualifier of the given type `T` will be removed.
 *
 *
 * Copying and Assignment
 * ----------------------
 * A \ref type object is lightweight and can be copied by value. However, each copy will refer to the same underlying type.
 *
 */
class RTTR_API type
{
    public:
        typedef uintptr_t type_id;

        /*!
         * \brief Assigns a type to another one.
         *
         */
        RTTR_INLINE type(const type& other) RTTR_NOEXCEPT;

        /*!
         * \brief Assigns a type to another one.
         *
         * \return A type object.
         */
        RTTR_INLINE type& operator=(const type& other) RTTR_NOEXCEPT;

        /*!
         * \brief Comparison operator for sorting the type data according to some internal criterion.
         *
         * \return True if this type is less than the \a other.
         */
        RTTR_INLINE bool operator<(const type& other) const RTTR_NOEXCEPT;

        /*!
         * \brief Comparison operator for sorting the type data according to some internal criterion.
         *
         * \return True if this type is greater than the \a other.
         */
        RTTR_INLINE bool operator>(const type& other) const RTTR_NOEXCEPT;

        /*!
         * \brief Comparison operator for sorting the type data according to some internal criterion.
         *
         * \return True if this type is greater than or equal to \a other.
         */
        RTTR_INLINE bool operator>=(const type& other) const RTTR_NOEXCEPT;

        /*!
         * \brief Comparison operator for sorting the type data according to some internal criterion.
         *
         * \return True if this type is less than or equal to \a other.
         */
        RTTR_INLINE bool operator<=(const type& other) const RTTR_NOEXCEPT;

        /*!
         * \brief Compares this type with the \a other type and returns true
         *        if both describe the same type, otherwise returns false.
         *
         * \return True if both type are equal, otherwise false.
         */
        RTTR_INLINE bool operator==(const type& other) const RTTR_NOEXCEPT;

        /*!
         * \brief Compares this type with the \a other type and returns true
         *        if both describe different types, otherwise returns false.
         *
         * \return True if both type are \b not equal, otherwise false.
         */
        RTTR_INLINE bool operator!=(const type& other) const RTTR_NOEXCEPT;

        /*!
         * \brief Returns the id of this type.
         *
         * \note This id is unique at process runtime,
         *       but the id can be changed every time the process is executed.
         *
         * \return The type id.
         */
        RTTR_INLINE type_id get_id() const RTTR_NOEXCEPT;

        /*!
         * \brief Returns the unique and human-readable name of the type.
         *
         * \remark The content of this string is compiler depended.
         *
         * \return The type name.
         */
        RTTR_INLINE string_view get_name() const RTTR_NOEXCEPT;

        /*!
         * \brief Returns true if this type is valid, that means the type holds valid data to a type.
         *
         * \return True if this type is valid, otherwise false.
         */
        RTTR_INLINE bool is_valid() const RTTR_NOEXCEPT;

        /*!
         * \brief Convenience function to check if this \ref type is valid or not.
         *
         * \return True if this \ref type is valid, otherwise false.
         */
         RTTR_INLINE explicit operator bool() const RTTR_NOEXCEPT;

        /*!
         * \brief Returns a type object which represent the raw type.
         *        A raw type, is a type type without any qualifiers (const and volatile) nor any pointer.
         *
         * \remark When the current type is already the raw type, it will return an copy from itself.
         *
         * \return The corresponding raw type object.
         */
        RTTR_INLINE type get_raw_type() const RTTR_NOEXCEPT;

        /*!
         * \brief Returns a type object which represent the wrapped type.
         *        A wrapper type is a class which encapsulate an instance of another type.
         *        This encapsulate type is also called *wrapped type*.
         *
         * See following example code:
         * \code{.cpp}
         *   type wrapped_type = type::get<std::shared_ptr<int>>().get_wrapped_type();
         *   wrapped_type == type::get<int*>(); // yields to true
         *
         *   wrapped_type = type::get<std::reference_wrapper<int>>().get_wrapped_type();
         *   wrapped_type == type::get<int>(); // yields to true
         * \endcode
         *
         * \remark When the current type is not a wrapper type, this function will return an \ref type::is_valid "invalid type".
         *
         * \see \ref wrapper_mapper "wrapper_mapper<T>"
         *
         * \return The type object of the wrapped type.
         */
        RTTR_INLINE type get_wrapped_type() const RTTR_NOEXCEPT;

        /*!
         * \brief Returns a type object for the given template type \a T.
         *
         * \return type for the template type \a T.
         */
        template<typename T>
        RTTR_LOCAL static type get() RTTR_NOEXCEPT;

        /*!
         * \brief Returns a type object for the given instance \a object.
         *
         * \remark If the type of the expression is a cv-qualified type, the result of the type::get() expression refers to a
         *         type object representing the cv-unqualified type.
         *         When type::get() is applied to a glvalue expression whose type is a polymorphic class type,
         *         the result refers to a type object representing the type of the most derived object.
         *
         * \return type for an \a object of type \a T.
         */
        template<typename T>
        RTTR_LOCAL static type get(T&& object) RTTR_NOEXCEPT;

        /*!
         * \brief Returns the type object with the given name \p name.
         *
         * \remark The search for the type is case sensitive. White spaces will be ignored.
         *         The name of the type corresponds to the name which was used during \ref RTTR_REGISTRATION.
         *         Only after the registration process was executed, then the type can be retrieved with this function.
         *         Otherwise and invalid type will be returned.
         *
         * \return \ref type object with the name \p name.
         */
        static type get_by_name(string_view name) RTTR_NOEXCEPT;

        /*!
         * \brief Returns a range of all registered type objects.
         *
         * \remark The order of the type object is unspecified.
         *
         * \return A range of type objects.
         */
        static array_range<type> get_types() RTTR_NOEXCEPT;

        /*!
         * \brief Returns the size in bytes of the object representation of the current type (i.e. `sizeof(T)`).
         *
         * \return The size of the type in bytes.
         */
        RTTR_INLINE std::size_t get_sizeof() const RTTR_NOEXCEPT;

        /*!
         * \brief Returns true whether the given type is class; that is not an atomic type or a method.
         *
         * \return True if the type is a class, otherwise false.
         */
        RTTR_INLINE bool is_class() const RTTR_NOEXCEPT;

         /*!
         * \brief Returns true whether the given type is an instantiation of a class template.
         *
         * See following example code:
         * \code{.cpp}
         * template<typename T>
         * struct foo { }; // class template
         *
         * struct bar { }; // NO class template
         *
         * type::get<foo<int>>().is_template_instantiation(); // yield to 'true'
         * type::get<bar>().is_template_instantiation();      // yield to 'false'
         * \endcode
         *
         * \return `true` if the type is a class template, otherwise `false`.
         *
         * \see get_template_arguments()
         */
        RTTR_INLINE bool is_template_instantiation() const RTTR_NOEXCEPT;

        /*!
         * \brief Returns a list of type objects that represents the template arguments.
         *        An empty list is returned when this type is not an instantiation of a template
         *        or contains no template arguments at all.
         *
         * See following example code:
         * \code{.cpp}
         * template<typename...Args>
         * struct my_class { };
         *
         * auto type_list = type::get<my_class<int, bool, char>>().get_template_arguments();
         * for (const auto& t : type_list)
         * {
         *    std::cout << t.get_name() << " " << std::endl; // will print 'int bool char'
         * }
         * \endcode
         *
         * \return A list of nested types.
         *
         * \see is_template_instantiation()
         */
        array_range<type> get_template_arguments() const RTTR_NOEXCEPT;

        /*!
         * \brief Returns true whether the given type represents an enumeration.
         *
         * \return True if the type is an enumeration, otherwise false.
         */
        RTTR_INLINE bool is_enumeration() const RTTR_NOEXCEPT;

        /*!
         * \brief Returns the enumerator if this type is an enum type;
         *        otherwise the returned value is \ref enumeration::is_valid "not valid".
         *
         * \see is_enumeration()
         *
         * \return A enumeration object.
         */
        enumeration get_enumeration() const RTTR_NOEXCEPT;

        /*!
         * \brief Returns true whether the given type represents a wrapper type.
         *        A wrapper type is a class which encapsulate an instance of another type.
         *        RTTR recognize automatically following wrapper types:
         *        - \p `std::shared_ptr<T>`
         *        - \p `std::reference_wrapper<T>`
         *        - \p `std::weak_ptr<T>`
         *        - \p `std::unique_ptr<T>`
         *
         *        In order to work with custom wrapper types, its required to specialize the class \ref wrapper_mapper "wrapper_mapper<T>"
         *        and implement a getter function to retrieve the encapsulate type.
         *
         * \see \ref wrapper_mapper "wrapper_mapper<T>"
         *
         * \return True if the type is an wrapper, otherwise false.
         *
         */
        RTTR_INLINE bool is_wrapper() const RTTR_NOEXCEPT;

        /*!
         * \brief Returns `true` whether the given type represents an array.
         *        An array is always also a sequential container.
         *        The check will return `true` only for raw C-Style arrays:
         * \code{.cpp}
         *
         *  type::get<int[10]>().is_array();            // true
         *  type::get<int>().is_array();                // false
         *  type::get<std::array<int,10>>().is_array(); // false
         * \endcode
         *
         * \return `true` if the type is an array, otherwise `false`.
         *
         * \see is_sequential_container()
         */
        RTTR_INLINE bool is_array() const RTTR_NOEXCEPT;

        /*!
         * \brief Returns true whether the given type represents an
         *        <a target="_blank" href=https://en.wikipedia.org/wiki/Associative_containers>associative container</a>.
         *
         * \return True if the type is an associative container, otherwise false.
         *
         * \see \ref associative_container_mapper "associative_container_mapper<T>"
         */
        RTTR_INLINE bool is_associative_container() const RTTR_NOEXCEPT;

        /*!
         * \brief Returns true whether the given type represents an
         *        <a target="_blank" href=https://en.wikipedia.org/wiki/Sequence_container_(C%2B%2B)>sequence container</a>.
         *
         * \return True if the type is an sequential container, otherwise false.
         *
         * \see \ref sequential_container_mapper "sequential_container_mapper<T>"
         */
        RTTR_INLINE bool is_sequential_container() const RTTR_NOEXCEPT;

        /*!
         * \brief Returns true whether the given type represents a pointer.
         *        e.g. `int*`, or `bool*`
         *
         * \return True if the type is a pointer, otherwise false.
         */
        RTTR_INLINE bool is_pointer() const RTTR_NOEXCEPT;

        /*!
         * \brief Returns true whether the given type represents an arithmetic type.
         *        An arithmetic type is a integral type or a floating point type.
         *        e.g. `bool`, `int`, `float`, etc...
         *
         * \return True if the type is a arithmetic type, otherwise false.
         */
        RTTR_INLINE bool is_arithmetic() const RTTR_NOEXCEPT;

        /*!
         * \brief Returns true whether the given type represents a pointer to a function
         *        e.g. `void (*)(void)`
         *
         * \return True if the type is a function pointer, otherwise false.
         */
        RTTR_INLINE bool is_function_pointer() const RTTR_NOEXCEPT;

        /*!
         * \brief Returns true whether the given type represents a pointer to a member object.
         *        e.g. `int (MyClass::*)`
         *
         * \return True if the type is a member object pointer, otherwise false.
         */
        RTTR_INLINE bool is_member_object_pointer() const RTTR_NOEXCEPT;

        /*!
         * \brief Returns true whether the given type represents a pointer to a member function.
         *        e.g. `void (MyClass::*)(void)`
         *
         * \return True if the type is a member function pointer type, otherwise false.
         */
        RTTR_INLINE bool is_member_function_pointer() const RTTR_NOEXCEPT;

        /*!
         * \brief Returns true if this type is derived from the given type \p other, otherwise false.
         *
         * \remark Make sure that the complete class hierarchy has the macro RTTR_ENABLE
         *         inside the class declaration, otherwise the returned information of this function
         *         is **not correct**.
         *
         * \return Returns true if this type is a derived type from \p other, otherwise false.
         */
        bool is_derived_from(const type& other) const RTTR_NOEXCEPT;

        /*!
         * \brief Returns true if this type is derived from the given type \a T, otherwise false.
         *
         * \remark Make sure that the complete class hierarchy has the macro RTTR_ENABLE
         *         inside the class declaration, otherwise the returned information of this function
         *         is **not correct**.
         *
         * \return Returns true if this type is a derived type from \a T, otherwise false.
         */
        template<typename T>
        bool is_derived_from() const RTTR_NOEXCEPT;

        /*!
         * \brief Returns true if this type is the base class from the given type \p other, otherwise false.
         *
         * \remark Make sure that the complete class hierarchy has the macro RTTR_ENABLE
         *         inside the class declaration, otherwise the returned information of this function
         *         is **not correct**.
         *
         * \return Returns true if this type is a base class type from \p other, otherwise false.
         */
        bool is_base_of(const type& other) const RTTR_NOEXCEPT;

         /*!
         * \brief Returns true if this type is the base class from the given type \a T, otherwise false.
         *
         * \remark Make sure that the complete class hierarchy has the macro RTTR_ENABLE
         *         inside the class declaration, otherwise the returned information of this function
         *         is **not correct**.
         *
         * \return Returns true if this type is a base class type from \a T, otherwise false.
         */
        template<typename T>
        bool is_base_of() const RTTR_NOEXCEPT;

        /*!
         * \brief Returns a range of all base classes of this type.
         *
         * \remark Make sure that the complete class hierarchy has the macro RTTR_ENABLE
         *         inside the class declaration, otherwise the returned information of this function
         *         is **not correct**. The order of this list depends on the declaration order of classes
         *         inside RTTR_ENABLE. E.g. RTTR_ENABLE(A1, A2) => A1 will be for A2 in the list.
         *         Accordingly the root (or parent or base) class is always the first type in the list.
         *
         * \return A range of types.
         */
        array_range<type> get_base_classes() const RTTR_NOEXCEPT;

        /*!
         * \brief Returns a range of all derived classes of this type.
         *
         * \remark Make sure that the complete class hierarchy has the macro RTTR_ENABLE
         *         inside the class declaration, otherwise the returned information of this function
         *         is **not correct**. The order of this list depends on the declaration order of classes
         *         inside RTTR_ENABLE. E.g. RTTR_ENABLE(A1, A2) => A1 will be for A2 in the list.
         *
         * \return A range of type objects.
         */
        array_range<type> get_derived_classes() const RTTR_NOEXCEPT;

        /////////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////////

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
         * \brief Returns a public constructor whose parameters match the types in the specified list.
         *
         * \remark When no parameter list is given, it will be searched for the default constructor.
         *
         * \return A valid constructor will be returned when the parameter matches the registered constructor;
         *         otherwise an invalid constructor.
         */
        constructor get_constructor(const std::vector<type>& params = std::vector<type>() ) const RTTR_NOEXCEPT;

        /*!
         * \brief Returns a range of all registered *public* constructors for this type.
         *
         * \remark In order to retrieve *private* constructors, use \ref type::get_constructors(filter_items) const
         *         with following filter combination `filter_item::instance_item | filter_item::static_item | filter_item::non_public_access`
         *         The constructors are sorted after its order of registration.
         *
         * \return A range of constructors.
         */
        array_range<constructor> get_constructors() const RTTR_NOEXCEPT;

        /*!
         * \brief Returns a range of all registered constructors for this type,
         *        based on the given \p filter. The base classes are **not** included in the search.
         *
         * Combine the enum values inside \ref filter_item with the OR operator to return a certain range of constructors.
         *
         * See following example code:
         * \code{.cpp}
         * #include <rttr/registration>
         *
         * struct my_struct { my_struct() {} my_struct(int) {} my_struct(bool) {} RTTR_ENABLE() };
         *
         * RTTR_REGISTRATION
         * {
         *     rttr::registration::class_<my_struct>("my_struct")
         *         .constructor<>() ( policy::ctor::as_object )
         *         .constructor<bool>(registration::protected_access) ( policy::ctor::as_object )
         *         .constructor<int>() ( policy::ctor::as_object );
         * }
         *
         * int main()
         * {
         *     type t = rttr::type::get<my_struct>();
         *     for (auto& ctor : t.get_constructors())
         *         std::cout << ctor.get_signature() << ", "; // prints "my_struct( ), my_struct( int ),"
         *
         *     std::cout << std::endl;
         *
         *     for (auto& ctor : t.get_constructors(filter_item::instance_item | filter_item::non_public_access))
         *         std::cout << ctor.get_signature() << ", "; // prints "my_struct( bool ),"
         *
         *     std::cout << std::endl;
         *
         *     for (auto& ctor : t.get_constructors(filter_item::static_item | filter_item::non_public_access))
         *         std::cout << ctor.get_signature() << ", "; // prints "my_struct( bool ),"
         *
         *     std::cout << std::endl;
         *
         *     for (auto& ctor : t.get_constructors(filter_item::instance_item | filter_item::public_access | filter_item::declared_only))
         *         std::cout << ctor.get_signature() << std::endl; // prints "my_struct( ), my_struct( int ),"
         *
         *     return 0;
         * }
         * \endcode
         *
         * \remark The properties are sorted after its order of registration.
         *
         * \return A range of properties.
         */
        array_range<constructor> get_constructors(filter_items filter) const RTTR_NOEXCEPT;

        /*!
         * \brief Creates an instance of the current type, with the given arguments \p args for the constructor.
         *
         * \remark When the argument types does not match the parameter list of the constructor then the he will not be invoked.
         *         Constructors with registered \ref default_arguments will be honored.
         *
         * \return Returns an instance of the given type.
         */
        variant create(std::vector<argument> args = std::vector<argument>()) const;

        /*!
         * \brief Returns the corresponding destructor for this type.
         *
         * \remark When there is no constructor registered for this type, then also the destructor is not available.
         *        A destructor will always been automatically registered.
         *
         * \return Returns the destructor for this type.
         */
        destructor get_destructor() const RTTR_NOEXCEPT;

        /*!
         * \brief Destroys the contained object in the variant \p obj.
         *
         * \remark When the \p obj could be destroyed the given \p obj is invalid after calling this method;
         *         Otherwise it is still valid.
         *
         * \return True if the destructor of the object could be invoked, otherwise false.
         */
        bool destroy(variant& obj) const RTTR_NOEXCEPT;


        /*!
         * \brief Returns a property with the name \p name.
         *
         * \remark When there exists no property with the name \p name, then an invalid property is returned.
         *
         * \return A property with name \p name.
         */
        property get_property(string_view name) const RTTR_NOEXCEPT;

        /*!
         * \brief Returns a range of all registered *public* properties for this type and
         *        all its base classes.
         *
         * \remark In order to retrieve *private* properties, use \ref type::get_properties(filter_items) const
         *         with following filter combination `filter_item::instance_item | filter_item::static_item | filter_item::non_public_access`
         *         The properties are sorted after its order of registration.
         *
         * \return A range of properties.
         */
        array_range<property> get_properties() const RTTR_NOEXCEPT;

        /*!
         * \brief Returns a range of all registered properties for this type,
         *        based on the given \p filter. The base classes are included in the search.
         *
         * Combine the enum values inside \ref filter_item with the OR operator to return a certain range of properties.
         *
         * See following example code:
         * \code{.cpp}
         * #include <rttr/registration>
         *
         * struct base { int p1, p2; RTTR_ENABLE() };
         * struct derived : base { int p3; static const int p4 = 23; RTTR_ENABLE(base) };
         *
         * RTTR_REGISTRATION
         * {
         *     rttr::registration::class_<base>("base")
         *         .property("p1", &base::p1)
         *         .property("p2", &base::p2, rttr::registration::private_access);
         *
         *     rttr::registration::class_<derived>("derived")
         *         .property("p3", &derived::p3)
         *         .property_readonly("p4", &derived::p4, rttr::registration::private_access);
         * }
         *
         * int main()
         * {
         *     type t = rttr::type::get<derived>();
         *     for (auto& prop : t.get_properties())
         *         std::cout << prop.get_name() << ", "; // prints "p1, p3,"
         *
         *     std::cout << std::endl;
         *
         *     for (auto& prop : t.get_properties(filter_item::instance_item | filter_item::non_public_access))
         *         std::cout << meth.get_name() << ", "; // prints "p2,"
         *
         *     std::cout << std::endl;
         *
         *     for (auto& prop : t.get_properties(filter_item::static_item | filter_item::non_public_access))
         *         std::cout << meth.get_name() << ", "; // prints "p4,"
         *
         *     std::cout << std::endl;
         *
         *     for (auto& prop : t.get_properties(filter_item::instance_item | filter_item::public_access | filter_item::declared_only))
         *         std::cout << meth.get_name() << ", "; // prints "p3"
         *
         *     return 0;
         * }
         * \endcode
         *
         * \remark The properties are sorted after its order of registration.
         *
         * \return A range of properties.
         */
        array_range<property> get_properties(filter_items filter) const RTTR_NOEXCEPT;

        /*!
         * \brief Returns a global property with the name \p name.
         *
         * \remark When there exists no property with the name \p name, then an invalid property is returned.
         *
         * \return A property with name \p name.
         */
        static property get_global_property(string_view name) RTTR_NOEXCEPT;

        /*!
         * \brief Returns a range of all registered global properties.
         *
         * \remark The order of the properties in the range is undefined.
         *         Client code should **not** depend on the order in which properties are returned,
         *         because that order varies.
         *
         * \return A range of properties.
         */
        static array_range<property> get_global_properties() RTTR_NOEXCEPT;


        /*!
         * \brief Returns the property value of property named \p name from the instance \p obj.
         *
         * \remark When the given instance is empty, the value of global property will be tryed to returned.
         *
         * \return A variant containing the value of the property.
         */
        variant get_property_value(string_view name, instance obj) const;

        /*!
         * \brief Returns the property value of property named \p name.
         *
         * \return A variant containing the value of the property.
         */
        static variant get_property_value(string_view name);

        /*!
         * \brief This function will set the given value \p arg to a property named \p name to the instance \p obj.
         *
         * \remark When the given instance is empty, the value of a global property with name \p name will be tryed to set.
         *
         * \return A bool value, which is true, when the value could be set, otherwise false.
         */
        bool set_property_value(string_view name, instance obj, argument arg) const;

        /*!
         * \brief This function will set the given value \p arg to a property named \p name.
         *
         * \return A bool value, which is true, when the value could be set, otherwise false.
         */
        static bool set_property_value(string_view name, argument arg);


        /*!
         * \brief Returns a method with the name \p name.
         *
         * \remark When there exists no method with the name \p name, then an invalid method is returned.
         *
         * \return A method with name \p name.
         */
        method get_method(string_view name) const RTTR_NOEXCEPT;

        /*!
         * \brief Returns a method with the name \p name which match the given parameter type list \p type_list.
         *
         * \remark When there exists no method with the name \p name and matching parameter type list \p type_list,
         *         then an invalid method is returned.
         *
         * \return A method with name \p name.
         */
        method get_method(string_view name, const std::vector<type>& type_list) const RTTR_NOEXCEPT;

        /*!
         * \brief Returns a range of all registered *public* methods for this type and
         *        all its base classes.
         *
         * \remark In order to retrieve *private* methods, use \ref type::get_methods(filter_items) const
         *         with following filter combination `filter_item::instance_item | filter_item::static_item | filter_item::non_public_access`
         *         The methods are sorted after its order of registration.
         *
         * \return A range of methods.
         */
        array_range<method> get_methods() const RTTR_NOEXCEPT;

        /*!
         * \brief Returns a range of all registered methods for this type,
         *        based on the given \p filter. The base classes are included in the search.
         *
         * Combine the enum values inside \ref filter_item with the OR operator to return a certain range of methods.
         *
         * See following example code:
         * \code{.cpp}
         * #include <rttr/registration>
         *
         * struct base { void func_1() {} void func_2() {} RTTR_ENABLE() };
         * struct derived : base { void func_3() {} static void func_4() {} RTTR_ENABLE(base) };
         *
         * RTTR_REGISTRATION
         * {
         *     rttr::registration::class_<base>("base")
         *         .method("func_1", &base::func_1)
         *         .method("func_2", &base::func_2, rttr::registration::private_access);
         *
         *     rttr::registration::class_<derived>("derived")
         *         .method("func_3", &derived::func_3)
         *         .method("func_4", &derived::func_4, rttr::registration::private_access);
         * }
         *
         * int main()
         * {
         *     type t = rttr::type::get<derived>();
         *     for (auto& meth : t.get_methods())
         *         std::cout << meth.get_name() << ", "; // prints "func_1, func_3,"
         *
         *     std::cout << std::endl;
         *
         *     for (auto& meth : t.get_methods(filter_item::instance_item | filter_item::non_public_access))
         *         std::cout << meth.get_name() << ", "; // prints "func_2,"
         *
         *     std::cout << std::endl;
         *
         *     for (auto& meth : t.get_methods(filter_item::static_item | filter_item::non_public_access))
         *         std::cout << meth.get_name() << ", "; // prints "func_4,"
         *
         *     std::cout << std::endl;
         *
         *     for (auto& meth : t.get_methods(filter_item::instance_item | filter_item::public_access | filter_item::declared_only))
         *         std::cout << meth.get_name() << ", "; // prints "func_3,"
         *
         *     return 0;
         * }
         * \endcode
         *
         * \remark The methods are sorted after its order of registration.
         *
         * \return A range of methods.
         */
        array_range<method> get_methods(filter_items filter) const RTTR_NOEXCEPT;

        /*!
         * \brief Returns a global method with the name \p name.
         *
         * \remark When there exists no method with the name \p name, and invalid method is returned.
         *
         * \return A method with name \p name.
         */
        static method get_global_method(string_view name) RTTR_NOEXCEPT;

        /*!
         * \brief Returns a global method with the name \p name which match the given parameter list \p params.
         *
         * \remark When there exists no method with the name \p name and matching parameter list \p params,
         *         then an invalid method is returned.
         *
         * \return A method with name \p name and parameter signature \p params.
         */
        static method get_global_method(string_view name, const std::vector<type>& params) RTTR_NOEXCEPT;

        /*!
         * \brief Returns a range of all registered global methods.
         *
         * \remark The order of the methods in the range is undefined.
         *         Client code should **not** depend on the order in which methods are returned,
         *         because that order varies.
         *
         * \return A range of methods.
         */
        static array_range<method> get_global_methods() RTTR_NOEXCEPT;


        /*!
         * \brief Invokes the method represented by the current instance \p object.
         *
         * \remark When it's a static method you still need to provide an instance object,
         *         use therefore the default ctor of \ref instance::instance() "instance()",
         *         or as shortcut use simply `{}`.
         *         Methods with registered \ref default_arguments will be honored.
         *
         * \return A variant object containing the possible return value,
         *         otherwise when it is a void function an empty but valid variant object.
         */
        variant invoke(string_view name, instance obj, std::vector<argument> args) const;

        /*!
         * \brief Invokes a global method named \p name with the specified argument \p args.
         *
         * \return A variant object containing the possible return value,
         *         otherwise when it is a void function an empty but valid variant object.
         *         Methods with registered \ref default_arguments will be honored.
         */
        static variant invoke(string_view name, std::vector<argument> args);

        /*!
         * \brief Register a converter func `F`, which will be used internally by the
         *        \ref variant class to convert between types.
         *
         *  This function converts a *source* type to a *target* type.
         *  The signature of this function has to be the following: `TargetType (SourceType, bool& ok)`
         *
         * See following example code:
         *  \code{.cpp}
         *   std::string conv_func(int value, bool& ok)
         *   {
         *      std::string result = std::to_string(value);
         *      ok = true;
         *      return result;
         *   }
         *   // register the conversion function
         *   type::register_converter_func(conv_func);
         *  \endcode
         *
         * \see variant::convert()
         */
        template<typename F>
        static void register_converter_func(F func);

        /*!
         * \brief Register for all base classes of the giving type \p T
         *        wrapper converter functions.
         *        The converters are registered in both directions respectively.
         *        From derived to base class and vice versa.
         *
         *
         * See following example code:
         *  \code{.cpp}
         *   struct base { virtual ~base() {}; RTTR_ENABLE() };
         *   struct derived : base { virtual ~derived() {}; RTTR_ENABLE(base) };
         *
         *   variant var = std::make_shared<derived>();
         *   var.convert(type::get<std::shared_ptr<base>>());    // yields to `false`
         *
         *   // register the conversion functions
         *   type::register_wrapper_converter_for_base_classes<std::shared_ptr<derived>>();
         *
         *   var.convert(type::get<std::shared_ptr<base>>());    // yields to `true`, derived to base conversion
         *   var.convert(type::get<std::shared_ptr<derived>>()); // yields to `true`, base to derived conversion
         *  \endcode
         *
         * \see variant::convert(), \ref wrapper_mapper "wrapper_mapper<T>"
         */
        template<typename T>
        static void register_wrapper_converter_for_base_classes();

        /*!
         * \brief Register comparison operators for template type \p T.
         *        This requires a valid `operator==` and `operator<` for type \p T.
         *
         * The registered comparison operators will be used in the \ref variant class.
         *
         * See following example code:
         *  \code{.cpp}
         *  // two variants, same content
         *   variant a = std::make_tuple<int, std::string>(12, std::string("some text"));
         *   variant b = std::make_tuple<int, std::string>(12, std::string("some text"));
         *
         *   a == b;                // yields to false
         *
         *   // register comparators for type 'std::tuple<std::string, int>'
         *   type::register_comparators<std::tuple<std::string, int>>();
         *
         *   a == b;                // yields to true
         *  \endcode
         *
         * \see variant::operator==(), variant::operator<()
         */
        template<typename T>
        static void register_comparators();

        /*!
         * \brief Register the equal comparison operators for template type \p T.
         *        This requires a valid `operator==` for type \p T.
         *
         * The registered comparison operators will be used in the \ref variant class.
         *
         * See following example code:
         *  \code{.cpp}
         *  // two variants, same content
         *   variant a = std::make_tuple<int, int>(42, 42);
         *   variant b = std::make_tuple<int, int>(42, 42);
         *
         *   a == b;                // yields to false
         *
         *   // register comparators for type 'std::tuple<int, int>'
         *   type::register_comparators<std::tuple<int, int>>();
         *
         *   a == b;                // yields to true
         *  \endcode
         *
         * \see variant::operator==()
         */
        template<typename T>
        static void register_equal_comparator();

        /*!
         * \brief Register the less than comparison operators for template type \p T.
         *        This requires a valid `operator<` for type \p T.
         *
         * The registered comparison operators will be used in the \ref variant class.
         *
         * See following example code:
         *  \code{.cpp}
         *  struct my_id
         *  {
         *     my_id(int i) : id(i) {}
         *     bool operator<(const foo& rhs) const { return (id < rhs.id); }
         *
         *     int id;
         *  };
         *  // two variants, same content
         *   variant a = my_id(23);
         *   variant b = my_id(42);
         *
         *   a < b;                // yields to false
         *
         *   // register comparators for type 'my_id'
         *   type::register_less_than_comparator<my_id>();
         *
         *   a < b;                // yields to true
         *  \endcode
         *
         * \see variant::operator<()
         */
        template<typename T>
        static void register_less_than_comparator();

    private:

        /*!
         * Constructs an empty and invalid type object.
         */
        type() RTTR_NOEXCEPT;

        /*!
         * \brief Constructs a valid type object.
         *
         * \param id The unique id of the data type.
         */
        RTTR_INLINE explicit type(detail::type_data* data) RTTR_NOEXCEPT;

        /*!
         * \brief This function try to convert the given pointer \p ptr from the type \p source_type
         *        to the target type \p target_type.
         *
         * \remark The returned pointer is always the raw type of \p target_type. You do not have to use this function by your own.
         *
         * \return Returns the converted pointer; when the conversion fails is a null pointer is returned.
         */
        static void* apply_offset(void* ptr, const type& source_type, const type& target_type) RTTR_NOEXCEPT;

        /*!
         * \brief This function returns the most derived type for the given object \p ptr of type \p source_type.
         *
         * \return Returns the most derived type for the given instance \p ptr.
         */
        static type get_derived_type(void* ptr, const type& source_type) RTTR_NOEXCEPT;

        /*!
         * \brief When for the current type instance a converter function to type \p target_type was registered,
         *        then this function returns a valid pointer to a type_converter_base object.
         *        Otherwise this function returns a `nullptr`.
         *
         * \see register_converter_func()
         */
        const detail::type_converter_base* get_type_converter(const type& target_type) const RTTR_NOEXCEPT;

        /*!
         * \brief When for the current type instance a equal comparator function was registered,
         *        then this function returns a valid pointer to a `type_comparator_base` object.
         *        Otherwise this function returns a `nullptr`.
         *
         * \see register_equal_comparator()
         */
        const detail::type_comparator_base* get_equal_comparator() const RTTR_NOEXCEPT;

        /*!
         * \brief When for the current type instance a less-than comparator function was registered,
         *        then this function returns a valid pointer to a `type_comparator_base` object.
         *        Otherwise this function returns a `nullptr`.
         *
         * \see register_less_than_comparator()
         */
        const detail::type_comparator_base* get_less_than_comparator() const RTTR_NOEXCEPT;

        /*!
         * \brief Returns the level of indirection for this this type. A.k.a pointer count.
         *        E.g. (`int` will return `0`; `int*` will return `1`; `int**` will return `2`; etc...)
         *
         * \return The pointer dimension.
         */
        RTTR_INLINE std::size_t get_pointer_dimension() const RTTR_NOEXCEPT;

        /*!
         * \brief Returns the raw type of an array
         *        E.g.  for type `int[100]` it will return the type object of `int`;
         *              `const int[10][25]` will return also `int`; etc...
         *
         * \return The raw array type.
         */
        RTTR_INLINE type get_raw_array_type() const RTTR_NOEXCEPT;

        /*!
         * \brief Returns the compiler depended name of the type.
         *
         * \return The full type name.
         */
        RTTR_INLINE string_view get_full_name() const RTTR_NOEXCEPT;

        /*!
         * \brief Creates a wrapped value from the given argument \p arg and moves it into the
         *        the given variant \p var.
         */
        void create_wrapped_value(const argument& arg, variant& var) const;


        /////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////

        //! Creates a variant from the given argument data.
        RTTR_INLINE variant create_variant(const argument& data) const;

        friend class variant;
        template<typename Target_Type, typename Source_Type>
        friend Target_Type rttr_cast(Source_Type object) RTTR_NOEXCEPT;

        friend class instance;
        friend class detail::type_register;
        friend class detail::type_register_private;

        friend type detail::create_type(detail::type_data*) RTTR_NOEXCEPT;

        template<typename T>
        friend std::unique_ptr<detail::type_data> detail::make_type_data();

        template<typename T, typename Tp, typename Converter>
        friend struct detail::variant_data_base_policy;

        friend RTTR_API bool detail::compare_types_less_than(const void*, const void*, const type&, int&);
        friend RTTR_API bool detail::compare_types_equal(const void*, const void*, const type&, bool&);

    private:
        detail::type_data* m_type_data;
};

} // end namespace rttr

#include "rttr/detail/type/type_impl.h"

#endif // RTTR_TYPE_H_
