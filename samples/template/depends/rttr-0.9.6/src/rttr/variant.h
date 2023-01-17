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

#ifndef RTTR_VARIANT_H_
#define RTTR_VARIANT_H_

#include "rttr/detail/base/core_prerequisites.h"
#include "rttr/detail/misc/misc_type_traits.h"
#include "rttr/detail/variant/variant_data.h"
#include "rttr/detail/misc/argument_wrapper.h"
#include "rttr/detail/variant/variant_compare.h"

#include <type_traits>
#include <cstddef>
#include <cstdint>
#include <algorithm>
#include <string>

namespace rttr
{

class variant_associative_view;
class variant_sequential_view;
class type;
class variant;
class argument;
class instance;

namespace detail
{
    template<class T>
    RTTR_INLINE T* unsafe_variant_cast(variant* operand) RTTR_NOEXCEPT;
    template<class T>
    RTTR_INLINE const T* unsafe_variant_cast(const variant* operand) RTTR_NOEXCEPT;

    struct data_address_container;
    template<typename T>
    struct empty_type_converter;

    template<typename T, typename Tp, typename Converter = empty_type_converter<T>>
    struct variant_data_base_policy;
    struct variant_data_policy_nullptr_t;

    enum class variant_policy_operation : uint8_t;

    template<typename T, typename Decayed = decay_except_array_t<T>>
    using decay_variant_t = enable_if_t<!std::is_same<Decayed, variant>::value, Decayed>;

    using variant_policy_func = bool (*)(variant_policy_operation, const variant_data&, argument_wrapper);
}

/*!
 * The \ref variant class allows to store data of any type and convert between these types transparently.
 *
 * This class serves as container for any given single \ref get_type() "type". It can hold one value at a time
 * (using containers you can hold multiple types e.g. `std::vector<int>`). Remark that the content is copied
 * into the variant class. Even raw arrays (e.g. `int[10]`) are copied. However, the internal implementation of variant
 * has an optimization for storing small types, which avoid heap allocation.
 *
 * The main purpose of this class is to be the return value for \ref property::get_value() "property" and
 * \ref method::invoke() "method" invokes or as container for storing meta data.
 *
 * Copying and Assignment
 * ----------------------
 * A \ref variant object can be copied and assigned, however each copy will perform a copy of the contained value.
 *
 * Typical Usage
 * -------------
 *
 * \code{.cpp}
 *  variant var;
 *  var = 23;                               // copy integer
 *  int x = var.to_int();                   // x = 23
 *
 *  var = "Hello World";                    // var contains now a std::string (implicit conversion of string literals to std::string)
 *  int y = var.to_int();                   // y = 0, because invalid conversion
 *
 *  var = "42";                             // contains a std::string
 *  std::cout << var.to_int();              // convert std::string to integer and prints "42"
 *
 *  int my_array[100];
 *  var = my_array;                         // copies the content of my_array into var
 *  auto& arr = var.get_value<int[100]>();  // extracts the content of var by reference
 * \endcode
 *
 * Extract Value
 * -------------
 * For extracting a value out of a variant you can use the \ref get_value() function.
 * This will return a *const reference* to the contained value.
 * However, you must instantiated this function with the exact type of the stored value,
 * otherwise undefined behaviour will occur.
 * Therefore you **should** check it's \ref get_type() "type" before extracting with \ref is_type<T>().
 *
 * See following example:
 * \code{.cpp}
 *  struct custom_type
 *  {
 *     //...
 *  };
 *
 *  variant var = custom_type{};
 *  if (var.is_type<custom_type>())                             // yields to true
 *    const custom_type& value = var.get_value<custom_type>();  // extracts the value by reference
 * \endcode
 *
 * Conversion
 * ----------
 * The \ref variant class offers three possibilities to convert to a new type.
 * - \ref convert(const type& target_type) "convert(const type& target_type)" - convert the variant internally to a new type
 * - \ref variant::convert(bool* ok) "convert\<T\>(bool *ok)" - convert the contained value to an internally default created value of type \p T and returns this new value
 * - \ref variant::convert(T& value) "convert\<T\>(T& value)" - convert the contained value to a given \p value of type \p T
 *
 * See following example code:
 * \code{.cpp}
 *  variant var = 23;                           // var contains an int
 *  if(var.can_convert<std::string>())          // check whether conversion is possible
 *  {
 *    var.convert(type::get<std::string>());    // var contains now a std::string, with value => "23"
 *    var.is_type<std::string>();               // yields to 'true'
 *  }
 *
 *  bool ok = false;
 *  int int_value = var.convert<int>(&ok);      // int_value == 23, ok yields to 'true'
 *  // or
 *  uint8_t small_int = 0;
 *  bool result = var.convert<uint8_t>(small_int); // result == false, because small_int cannot hold the value '500'
 *
 * \endcode
 *
 * \remark
 * It is possible that \ref can_convert() will return `true`, but \ref convert() will actually fail and return `false`.
 * The reason for this is \ref can_convert() will return the general ability of converting between types given suitable data;
 * when no suitable data is given, the conversion cannot be performed.
 *
 * A good example is the conversion from `std::string` to `int`.
 * \code{.cpp}
 *  variant var = "Answer: 42";
 *  var.can_convert<int>();             // yields to 'true'
 *  int number;
 *  bool result = var.convert(number);  // yields to 'false', because the string contains non-numeric characters
 *
 * \endcode
 *
 * Hence, it is important to have both functions return true for a successful conversion.
 *
 * Custom Converter
 * ----------------
 * The variant class allows to convert from and to user-defined types,
 * therefore you have to register a conversion function.
 *
 * See following example code:
 * \code{.cpp}
 *  std::string converter_func(const custom_type& value, bool& ok)
 *  {
 *      ok = true;
 *      // convert value to std::string
 *      return std::string(...);
 *  }
 *
 *  //...
 *  variant var = custom_type(...);
 *  var.can_convert<std::string>();     // return 'false'
 *  // register the conversion function
 *  type::register_converter_func(converter_func);
 *
 *  var.can_convert<std::string>();     // return 'true'
 *  var.to_string();                    // converts from 'custom_type' to 'std::string'
 * \endcode
 *
 * For more information see \ref type::register_converter_func()
 *
 * \see variant_array_view
 */
class RTTR_API variant
{
    public:
        /*!
         * \brief Constructs an invalid variant. That means a valid which contains no data.
         *
         * \see is_valid()
         */
        RTTR_INLINE variant();

        /*!
         * \brief Constructs a new variant with the new value \p val.
         *        The value will be copied or moved into the variant.
         */
        template<typename T, typename Tp = detail::decay_variant_t<T>>
        variant(T&& val);

        /*!
         * \brief Constructs a new variant object from the given variant \p other.
         */
        variant(const variant& other);

        /*!
         * \brief Constructs a new variant via move constructor.
         */
        variant(variant&& other);

        /*!
         * \brief Destroys the variant and the contained object.
         */
        RTTR_INLINE ~variant();

        /*!
         * Assigns the value of the \p other object to this variant.
         *
         * \return A reference to the variant with the new data.
         */
        template<typename T, typename Tp = detail::decay_variant_t<T>>
        variant& operator=(T&& other);

        /*!
         * \brief Assigns the value of the \a other variant to this variant.
         *
         * \return A reference to the variant with the new data.
         */
        variant& operator=(variant&& other);

        /*!
         * \brief Assigns the value of the \a other variant to this variant.
         *
         * \return A reference to the variant with the new data.
         */
        variant& operator=(const variant& other);

        /*!
         * \brief Compares this variant with \p other and returns `true` if they are equal; otherwise returns `false`.
         *
         * The variant uses the equality operator of the containing \ref get_type() "type" to check for equality.
         * When \p other is not of the same type as the containing type, it will try to convert to it and do then the equality check.
         *
         * \remark In order to use this function with template types, like `std::tuple<int, std::string>`,
         *         you need to register the comparison operator to the type system with \ref type::register_comparators<T>().
         *         The reason for that is, template types might define the `==` operator, but not the contained template type.
         *
         * \note Comparability might not be available for the type stored in this variant or in \p other.
         *
         * \see \ref variant::operator!=(const variant&) const "operator!="
         *
         * \return A boolean with value `true`, that indicates both variant's are equal, otherwise `false`.
         */
        RTTR_INLINE bool operator==(const variant& other) const;

        /*!
         * \brief Compares this variant with \p other and returns `true` if they are **not** equal; otherwise returns `false`.
         *
         * \remark In order to use this function with template types, like `std::tuple<int, std::string>`,
         *         you need to register the comparison operator to the type system with \ref type::register_comparators<T>().
         *         The reason for that is, template types might define the `!=` operator, but not the contained template type.
         *
         * \note Comparability might not be available for the type stored in this variant or in \p other.
         *
         * \see \ref variant::operator==(const variant&) const "operator=="
         *
         * \return A boolean with value `true`, that indicates both variant's are **not** equal, otherwise `false`.
         */
        RTTR_INLINE bool operator!=(const variant& other) const;

        /*!
         * \brief Compares this variant with \p other and returns `true` if this is *less than* \p other, otherwise returns `false`.
         *
         * The variant uses the *less than* operator of the containing \ref get_type() "type".
         * When \p other is not of the same type as the containing type, it will try to convert to it and do then the *less than* check.
         *
         * \remark In order to use this function with template types, like `std::tuple<int, std::string>`,
         *         you need to register the comparison operator to the type system with \ref type::register_comparators<T>().
         *         The reason for that is, template types might define the `<` operator, but not the contained template type.
         *
         * \note Comparability might not be available for the type stored in this variant or in \p other.
         *
         * \see \ref variant::operator>(const variant&) const "operator\>"
         *
         * \return A boolean with value `true`, that indicates that this variant is *less than* \p other, otherwise `false`.
         */
        RTTR_INLINE bool operator<(const variant& other) const;


        /*!
         * \brief Compares this variant with \p other and returns `true` if this is *less* or *equal* than \p other, otherwise returns `false`.
         *
         * The variant uses the *less than* and equality operator of the containing \ref get_type() "type"
         * to get the result of the comparision.
         *
         * \note Comparability might not be available for the type stored in this variant or in \p other.
         *
         * \see \ref variant::operator<(const variant&) const "operator\<",
         *      \ref variant::operator==(const variant&) const "operator\=="
         *
         * \return A boolean with value `true`, that indicates that this variant is *less* or *equal* than \p other, otherwise `false`.
         */
        RTTR_INLINE bool operator<=(const variant& other) const;

        /*!
         * \brief Compares this variant with \p other and returns `true` if this is *greater* or *equal* then \p other, otherwise returns `false`.
         *
         * The variant uses the *greater than* and equality operator of the containing \ref get_type() "type"
         * to get the result of the comparision.
         *
         * \note Comparability might not be available for the type stored in this variant or in \p other.
         *
         * \see \ref variant::operator>(const variant&) const "operator\>",
         *      \ref variant::operator==(const variant&) const "operator\=="
         *
         * \return A boolean with value `true`, that indicates that this variant is *greater* or *equal* than \p other, otherwise `false`.
         */
        RTTR_INLINE bool operator>=(const variant& other) const;

        /*!
         * \brief Compares this variant with \p other and returns `true` if this is *greater than* \p other, otherwise returns `false`.
         *
         * The variant uses the *less than* and equality operator of the containing \ref get_type() "type"
         * to get the result of the comparision.
         *
         * \note Comparability might not be available for the type stored in this variant or in \p other.
         *
         * \see \ref variant::operator<(const variant&) const "operator\<"
         *
         * \return A boolean with value `true`, that indicates that this variant is *greater than* \p other, otherwise `false`.
         */
        RTTR_INLINE bool operator>(const variant& other) const;

        /*!
         * \brief When the variant contains a value, then this function will clear the content.
         *
         * \remark After calling this function \ref is_valid() will return `false`.
         */
        void clear();

        /*!
         * \brief Swaps the content of this variant with \p other variant.
         */
        void swap(variant& other);

        /*!
         * \brief Returns `true` if the containing variant data is of the given template type `T`.
         *
         * \return True if variant data is of type `T`, otherwise false.
         */
        template<typename T>
        bool is_type() const;

        /*!
         * \brief Returns the \ref type object of underlying data.
         *
         * \remark When the variant has not stored any data, then an invalid \ref type object is returned.
         *
         * \return \ref type of the underlying data type.
         */
        type get_type() const;

        /*!
         * \brief Returns true if this variant is valid, that means the variant is holding some data.
         *
         * When the variant doesn't hold any data it will return false.
         *
         * \remark A variant can also hold `void` data, this is used to indicate that a method call,
         *         which has no return value, was successfully. In this case, there is no data actually stored,
         *         but this function will return true.
         *
         * \return `True` if this variant is valid, otherwise `false`.
         */
        bool is_valid() const;

        /*!
         * \brief Convenience function to check if this \ref variant is valid or not.
         *
         * \see is_valid()
         *
         * \return `True` if this \ref variant is valid, otherwise `false`.
         */
        explicit operator bool() const;

        /*!
         * \brief Returns true, when for the underlying or the \ref type::get_wrapped_type() "wrapped type"
         *        an associative_mapper exists.
         *
         * \return True if the containing value is an associative container; otherwise false.
         */
        bool is_associative_container() const;

        /*!
         * \brief Returns true, when for the underlying or the \ref type::get_wrapped_type() "wrapped type"
         *        an sequential_mapper exists.
         *
         * \return True if the containing value is an sequentail container; otherwise false.
         */
        bool is_sequential_container() const;

        /*!
         * \brief Returns a reference to the containing value as type \p T.
         *
         * \code{.cpp}
         *  struct custom_type
         *  {
         *     //...
         *  };
         *
         *  variant var = custom_type{};
         *  if (var.is_type<custom_type>())                         // yields to true
         *      custom_type& value = var.get_value<custom_type>();  // extracts the value by reference
         * \endcode
         *
         * \remark Only call this method when it is possible to return the containing value as the given type \p T.
         *         Use therefore the method \ref is_type().
         *         Otherwise the call leads to undefined behaviour.
         *         Also make sure you don't clean this variant, when you still hold a reference to the containing value.
         *
         * \see is_type(), variant_cast<T>
         *
         * \return A reference to the stored value.
         */
        template<typename T>
        T& get_value();

        /*!
         * \brief Returns a reference to the containing value as type \p T.
         *
         * \code{.cpp}
         *  struct custom_type
         *  {
         *     //...
         *  };
         *
         *  variant var = custom_type{};
         *  if (var.is_type<custom_type>())                             // yields to true
         *    const custom_type& value = var.get_value<custom_type>();  // extracts the value by reference
         * \endcode
         *
         * \remark Only call this method when it is possible to return the containing value as the given type \p T.
         *         Use therefore the method \ref is_type().
         *         Otherwise the call leads to undefined behaviour.
         *         Also make sure you don't clean this variant, when you still hold a reference to the containing value.
         *
         * \see is_type(), variant_cast<T>
         *
         * \return A reference to the stored value.
         */
        template<typename T>
        const T& get_value() const;

        /*!
         * \brief Returns a reference to the contained wrapped value as type \p T.
         *
         * \code{.cpp}
         *  int value = 23;
         *  variant var = std::ref(value);
         *
         *  if (var.get_type().get_wrapped_type() == type::get<int>())  // yields to true
         *    const int& ref_value = var.get_wrapped_value<int>();  // extracts the value by reference
         * \endcode
         *
         * \remark Only call this method when it is possible to return the containing value as the given type \p T.
         *         Use therefore the method \ref rttr::type::get_wrapped_type() "get_wrapped_type()".
         *         Otherwise the call leads to undefined behaviour.
         *
         * \see rttr::type::get_wrapped_type()
         *
         * \return A reference to the stored wrapped value.
         */
        template<typename T>
        const T& get_wrapped_value() const;

        /*!
         * \brief Extracts the wrapped value and copies its content into a new variant.
         *
         * \code{.cpp}
         *  int value1 = 23;
         *  variant var1 = std::ref(value1);
         *
         *  if (var1.get_type().get_wrapped_type() == type::get<int>())  // yields to true
         *  {
         *     variant var2 = var1.extract_wrapped_value(); // value will be copied into "var2"
         *     var2.get_type() == type::get<int>(); // yields to true
         *     const int& value2 = var2.get_value<int>();
         *     std::cout << value2 << std::endl;    // prints "23"
         *  }
         * \endcode
         *
         * \remark Calling this method works only for wrapped types which are copiable.
         *         When you work with custom types, which are not copyable, the variant will be \ref is_valid "invalid"
         *
         * \return A variant with the wrapped value.
         *
         * \see type::is_wrapper()
         */
        variant extract_wrapped_value() const;

        /*!
         * \brief Returns `true` if the contained value can be converted to the given type \p T.
         *        Otherwise `false`.
         *
         * \return `True` if this variant can be converted to `T`; otherwise `false`.
         *
         * \see convert(), type::register_converter_func()
         */
        template<typename T>
        bool can_convert() const;

        /*!
         * \brief Returns `true` if the contained value can be converted to the given type \p target_type;
         *        otherwise `false`.
         *
         * The returned value indicates that a conversion is in general possible.
         * However a conversion might still fail when doing the actual conversion with \ref convert().
         * An example is the conversion from a string to a number.
         * When the string does not contain non-numeric characters, the conversion will not succeed.
         *
         * \return `True` if this variant can be converted to \p target_type; otherwise `false`.
         *
         * \see convert(), type::register_converter_func()
         */
        bool can_convert(const type& target_type) const;

        /*!
         * \brief Converts the containing variant internally to the given type \p target_type.
         *        When the conversion was successfully the function will return `true`.
         *        When the conversion fails, then the containing variant value stays the same and the function will return `false`.
         *
         * There are already certain standard type conversions implemented:
         * 1. Conversion of all arithmetic types (e.g. `double` to `int`, 'std::size_t' to 'int8_t' and so on)
         * 2. Conversion of all arithmetic types to and from `std::string`.
         * 3. Conversion of enum types to `std::string`, its underlying arithmetic types and vice versa.
         * 4. Conversion of \ref rttr::wrapper_mapper<T> "wrapper classes" to wrapped types and vice versa (e.g, `std::shared_ptr<int>` to `int*`)
         * 5. Conversion of raw pointers to its derived types, if a \ref rttr_cast to the type described by \p target_type would succeed.
         *
         * See therefore following example code:
         * \code{.cpp}
         *  struct base { virtual ~base(){} RTTR_ENABLE() };
         *  struct derived : base { RTTR_ENABLE(base) };
         *  derived d;
         *  variant var = static_cast<base*>(&d);           // var contains a '*base' ptr
         *  bool ret = var.convert(type::get<derived*>());  // yields to 'true'
         *  var.is_type<derived*>();                        // yields to 'true'
         * \endcode
         *
         * Additionally, it is possible to add custom conversion function, which has to be registered
         * via \ref type::register_converter_func().
         *
         * \return `True` if this variant can be converted to \p target_type; otherwise `false`.
         *
         * \see can_convert(), type::register_converter_func()
         */
        bool convert(const type& target_type);

        /*!
         * \brief Converts the containing data to a *new value* of type \p T and return this *value*.
         *        If \p ok is non-null: \p *ok is set to `true` when the value was successfully converted to \p T; otherwise \p *ok is set to `false`.
         *        The type \p T must meet the requirement to be *default constructible*.
         *
         * \code{.cpp}
         *  variant val = 12;
         *  if (val.can_convert<float>())
         *  {
         *    bool ok = false;
         *    float result = val.convert<float>(&ok);
         *    if (ok)
         *    // ...
         *  }
         * \endcode
         *
         * In order to enable a custom type conversion, a conversion function has to be registered
         * via \ref type::register_converter_func().
         *
         * \remark Before doing the conversion you should check whether it is in general possible to convert to type \p T
         *         with the function \ref can_convert(). When the conversion fails, a default constructed value of type \p T is returned.
         *
         * \return The converted value as type \p T.
         *
         * \see can_convert(), type::register_converter_func()
         */
        template<typename T>
        T convert(bool* ok = nullptr) const;

        /*!
         * \brief Converts the containing data to the given value \p value and returns a `bool` flag that indicated whether the conversion
         *        was successful or not.
         *
         * When you need to convert to a type which cannot be default constructed use this function.
         *
         * See following example code:
         * \code{.cpp}
         *  variant val = 12;
         *  if (val.can_convert<custom_type>())
         *  {
         *    custom_type obj(42); // non-default ctor type
         *    bool result = val.convert(obj);
         *    if (result)
         *    // ...
         *  }
         * \endcode
         *
         * In order to enable a custom type conversion, a conversion function has to be registered
         * via \ref type::register_converter_func().
         *
         * \remark Before doing the conversion you should check whether it is in general possible to convert to type \p T
         *         with the function \ref can_convert()
         *
         * \return `True` if the contained data could be converted to \p value; otherwise `false`.
         *
         * \see can_convert(), type::register_converter_func()
         */
        template<typename T>
        bool convert(T& value) const;

        /*!
         * \brief Creates a \ref variant_associative_view from the containing value,
         *        when the \ref variant::get_type "type" or its \ref type::get_raw_type() "raw type"
         *        or the \ref type::get_wrapped_type() "wrapped type" is an \ref type::is_associative_container() "associative container".
         *        Otherwise a default constructed variant_associative_view will be returned.
         *
         * A typical example is the following:
         *
         * \code{.cpp}
         *   std::map<int, std::string> my_map;
         *   my_map.insert({1, "One"});
         *   my_map.insert({2, "Two"});
         *   my_map.insert({3, "Three"});
         *
         *   variant var = my_map;                                          // copies the content of my_map into var
         *   variant_associative_view view = var.create_associative_view(); // creates a view of the hold container in the variant (data is not copied!)
         *   std::size_t x = view.get_size();                               // return number of elements x = 3
         *   for (auto& item : view)                                        // iterates over all items stored in the container
         *   {
         *      auto key = item.first.extract_wrapped_value().to_string();
         *      auto value = item.second.extract_wrapped_value().to_string();
         *      std::cout << "key: " << key << " value: " << value << std::endl;
         *   }
         * \endcode
         *
         * \remark This function will return an \ref variant_associative_view::is_valid() "invalid" object, when the \ref variant::get_type "type" is no associative container.
         *
         * \return A variant_associative_view object.
         *
         * \see can_convert(), convert()
         */
        variant_associative_view create_associative_view() const;

        /*!
         * \brief Creates a \ref variant_sequential_view from the containing value,
         *        when the \ref variant::get_type "type" or its \ref type::get_raw_type() "raw type"
         *        or the \ref type::get_wrapped_type() "wrapped type" is an \ref type::is_sequential_container() "sequential container".
         *        Otherwise a default constructed variant_sequential_view will be returned.
         *
         * A typical example is the following:
         *
         * \code{.cpp}
         *  std::vector<int> my_vec = { 1, 2, 3, 4, 5};
         *  variant var = my_vec; // copies data into variant
         *  if (var.is_sequential_container())
         *  {
         *      variant_sequential_view view = var.create_sequential_view();  // performs no copy of the underlying vector
         *      std::cout << view.get_size() << std::endl;  // prints: '5'
         *      for (const auto& item : view) // iterates over all elements of the std::vector<T>
         *      {
         *          // remark that the value is stored inside a 'std::reference_wrapper', however there is an automatic conversion for wrapper classes implemented.
         *          std::cout << "data: " << item.to_string() << " ";
         *      }
         *  }
         * \endcode
         *
         * \remark This function will return an \ref variant_sequential_view::is_valid() "invalid" object, when the \ref variant::get_type "type" is no sequential container.
         *
         * \return A variant_sequential_view object.
         *
         * \see can_convert(), convert()
         */
        variant_sequential_view create_sequential_view() const;

        /*!
         * \brief Returns the variant as a `bool` if this variant is of \ref is_type() "type" `bool`.
         *
         * Returns `true` if the variant contains an \ref type::is_arithmetic() "arithmetic type" which value is non-zero
         * or if the variant contains a `std::string` and its lower-case content is not one of the following:
         * `""` (empty), `"0"` or `"false"`; otherwise returns `false`.
         *
         * Also any user-defined \ref type::register_converter_func() "conversion function" from the
         * \ref is_type() "source type" to `bool` will be executed when necessary.
         *
         * \see can_convert(), is_type()
         *
         * \return A `bool` value.
         */
        bool to_bool() const;

        /*!
         * \brief Returns the containing variant as an `int` when the \ref is_type() "type" is an `integer`.
         *
         * When the variant contains an \ref type::is_arithmetic() "arithmetic type" or an `std::string` then a conversion to `int`
         * will be tried. Also any user-defined \ref type::register_converter_func() "conversion function" from the \ref is_type() "source type" to `int`
         * will be executed when necessary.
         *
         * If \p ok is non-null: \p *ok is set to `true` if the value could be converted to an `int`; otherwise \p *ok is set to `false`.
         *
         * \remark A value overflow is not allowed, so if the internal value is larger than `int`, the conversion will fail.
         *         Precision loss, such as in conversion from floating-point to `int` on platforms where they differ in size is allowed.
         *         A conversion from `std::string` which contains non-numeric characters will fail.
         *
         * \see can_convert(), is_type()
         *
         * \return An `int` value.
         */
        int to_int(bool *ok = nullptr) const;

        /*!
         * \brief Returns the containing variant as a `float` when the \ref is_type() "type" is a `float`.
         *
         * When the variant contains an \ref type::is_arithmetic() "arithmetic type" or an `std::string` then a conversion to `float`
         * will be tried. Also any user-defined \ref type::register_converter_func() "conversion function" from the \ref is_type() "source type" to `float`
         * will be executed when necessary.
         *
         * If \p ok is non-null: \p *ok is set to `true` if the value could be converted to an `float`; otherwise \p *ok is set to `false`.
         *
         * \remark A value overflow is not allowed, so if the internal value is larger than `float`, the conversion will fail.
         *         Precision loss, such as in conversion from `double` to `float` on platforms where they differ in size is allowed.
         *         A conversion from `std::string` which contains non-numeric characters will fail.
         *
         * \see can_convert(), is_type()
         *
         * \return A `float` value.
         */
        float to_float(bool* ok = nullptr) const;

        /*!
         * \brief Returns the containing variant as a `double` when the \ref is_type() "type" is a `double`.
         *
         * When the variant contains an \ref type::is_arithmetic() "arithmetic type" or an `std::string` then a conversion to `double`
         * will be tried. Also any user-defined \ref type::register_converter_func() "conversion function" from the \ref is_type() "source type" to `double`
         * will be executed when necessary.
         *
         * If \p ok is non-null: \p *ok is set to `true` if the value could be converted to an `double`; otherwise \p *ok is set to `false`.
         *
         * \remark A value overflow is not allowed, so if the internal value is larger than `double`, the conversion will fail.
         *         Precision loss, such as in conversion from `double` to `float` on platforms where they differ in size is allowed.
         *         A conversion from `std::string` which contains non-numeric characters will fail.
         *
         * \see can_convert(), is_type()
         *
         * \return A `double` value.
         */
        double to_double(bool* ok = nullptr) const;

        /*!
         * \brief Returns the containing variant as a `std::string` when the \ref is_type() "type" is a `std::string`.
         *
         * When the variant contains an \ref type::is_arithmetic() "arithmetic type" then a conversion to `std::string` will be done.
         * Also any user-defined \ref type::register_converter_func() "conversion function" from the \ref is_type() "source type" to `std::string`
         * will be executed when necessary.
         *
         * If \p ok is non-null: \p *ok is set to `true` if the value could be converted to an `std::string`; otherwise \p *ok is set to `false`.
         *
         * \see can_convert(), is_type()
         *
         * \return A `std::string` value.
         */
        std::string to_string(bool *ok = nullptr) const;

        /*!
         * \brief Returns the containing variant as an `int8_t` when the \ref is_type() "type" is an `int8_t`.
         *
         * When the variant contains an \ref type::is_arithmetic() "arithmetic type" or an `std::string` then a conversion to `int8_t`
         * will be tried. Also any user-defined \ref type::register_converter_func() "conversion function" from the \ref is_type() "source type" to `int8_t`
         * will be executed when necessary.
         *
         * If \p ok is non-null: \p *ok is set to `true` if the value could be converted to an `int8_t`; otherwise \p *ok is set to `false`.
         *
         * \remark A value overflow is not allowed, so if the internal value is larger than `int8_t`, the conversion will fail.
         *         Precision loss, such as in conversion from floating-point to `int8_t` on platforms where they differ in size is allowed.
         *         A conversion from `std::string` which contains non-numeric characters will fail.
         *
         * \see can_convert(), is_type()
         *
         * \return A `int8_t` value.
         */
        int8_t to_int8(bool *ok = nullptr) const;

        /*!
         * \brief Returns the containing variant as an `int16_t` when the \ref is_type() "type" is an `int16_t`.
         *
         * When the variant contains an \ref type::is_arithmetic() "arithmetic type" or an `std::string` then a conversion to `int16_t`
         * will be tried. Also any user-defined \ref type::register_converter_func() "conversion function" from the \ref is_type() "source type" to `int16_t`
         * will be executed when necessary.
         *
         * If \p ok is non-null: \p *ok is set to `true` if the value could be converted to an `int16_t`; otherwise \p *ok is set to `false`.
         *
         * \remark A value overflow is not allowed, so if the internal value is larger than `int16_t`, the conversion will fail.
         *         Precision loss, such as in conversion from floating-point to `int16_t` on platforms where they differ in size is allowed.
         *         A conversion from `std::string` which contains non-numeric characters will fail.
         *
         * \see can_convert(), is_type()
         *
         * \return A `int16_t` value.
         */
        int16_t to_int16(bool *ok = nullptr) const;

        /*!
         * \brief Returns the containing variant as an `int32_t` when the \ref is_type() "type" is an `int32_t`.
         *
         * When the variant contains an \ref type::is_arithmetic() "arithmetic type" or an `std::string` then a conversion to `int32_t`
         * will be tried. Also any user-defined \ref type::register_converter_func() "conversion function" from the \ref is_type() "source type" to `int32_t`
         * will be executed when necessary.
         *
         * If \p ok is non-null: \p *ok is set to `true` if the value could be converted to an `int32_t`; otherwise \p *ok is set to `false`.
         *
         * \remark A value overflow is not allowed, so if the internal value is larger than `int32_t`, the conversion will fail.
         *         Precision loss, such as in conversion from floating-point to `int32_t` on platforms where they differ in size is allowed.
         *         A conversion from `std::string` which contains non-numeric characters will fail.
         *
         * \see can_convert(), is_type()
         *
         * \return A `int32_t` value.
         */
        int32_t to_int32(bool *ok = nullptr) const;

        /*!
         * \brief Returns the containing variant as an `int64_t` when the \ref is_type() "type" is an `int64_t`.
         *
         * When the variant contains an \ref type::is_arithmetic() "arithmetic type" or an `std::string` then a conversion to `int64_t`
         * will be tried. Also any user-defined \ref type::register_converter_func() "conversion function" from the \ref is_type() "source type" to `int64_t`
         * will be executed when necessary.
         *
         * If \p ok is non-null: \p *ok is set to `true` if the value could be converted to an `int64_t`; otherwise \p *ok is set to `false`.
         *
         * \remark A value overflow is not allowed, so if the internal value is larger than `int64_t`, the conversion will fail.
         *         Precision loss, such as in conversion from floating-point to `int64_t` on platforms where they differ in size is allowed.
         *         A conversion from `std::string` which contains non-numeric characters will fail.
         *
         * \see can_convert(), is_type()
         *
         * \return A `int64_t` value.
         */
        int64_t to_int64(bool *ok = nullptr) const;

        /*!
         * \brief Returns the containing variant as an `uint8_t` when the \ref is_type() "type" is an `uint8_t`.
         *
         * When the variant contains an \ref type::is_arithmetic() "arithmetic type" or an `std::string` then a conversion to `uint8_t`
         * will be tried. Also any user-defined \ref type::register_converter_func() "conversion function" from the \ref is_type() "source type" to `uint8_t`
         * will be executed when necessary.
         *
         * If \p ok is non-null: \p *ok is set to `true` if the value could be converted to an `uint8_t`; otherwise \p *ok is set to `false`.
         *
         * \remark A value overflow is not allowed, so if the internal value is larger than `uint8_t`, the conversion will fail.
         *         Also a loss of signedness is not allowed, that means a negative signed integer cannot be converted to `uint8_t`.
         *         Precision loss, such as in conversion from floating-point to `uint8_t` on platforms where they differ in size is allowed.
         *         A conversion from `std::string` which contains non-numeric characters will fail.
         *
         * \see can_convert(), is_type()
         *
         * \return A `uint8_t` value.
         */
        uint8_t to_uint8(bool *ok = nullptr) const;

        /*!
         * \brief Returns the containing variant as an `uint16_t` when the \ref is_type() "type" is an `uint16_t`.
         *
         * When the variant contains an \ref type::is_arithmetic() "arithmetic type" or an `std::string` then a conversion to `uint16_t`
         * will be tried. Also any user-defined \ref type::register_converter_func() "conversion function" from the \ref is_type() "source type" to `uint16_t`
         * will be executed when necessary.
         *
         * If \p ok is non-null: \p *ok is set to `true` if the value could be converted to an `uint16_t`; otherwise \p *ok is set to `false`.
         *
         * \remark A value overflow is not allowed, so if the internal value is larger than `uint16_t`, the conversion will fail.
         *         Also a loss of signedness is not allowed, that means a negative signed integer cannot be converted to `uint16_t`.
         *         Precision loss, such as in conversion from floating-point to `uint16_t` on platforms where they differ in size is allowed.
         *         A conversion from `std::string` which contains non-numeric characters will fail.
         *
         * \see can_convert(), is_type()
         *
         * \return A `uint16_t` value.
         */
        uint16_t to_uint16(bool *ok = nullptr) const;

        /*!
         * \brief Returns the containing variant as an `uint32_t` when the \ref is_type() "type" is an `uint32_t`.
         *
         * When the variant contains an \ref type::is_arithmetic() "arithmetic type" or an `std::string` then a conversion to `uint32_t`
         * will be tried. Also any user-defined \ref type::register_converter_func() "conversion function" from the \ref is_type() "source type" to `uint32_t`
         * will be executed when necessary.
         *
         * If \p ok is non-null: \p *ok is set to `true` if the value could be converted to an `uint32_t`; otherwise \p *ok is set to `false`.
         *
         * \remark A value overflow is not allowed, so if the internal value is larger than `uint32_t`, the conversion will fail.
         *         Also a loss of signedness is not allowed, that means a negative signed integer cannot be converted to `uint32_t`.
         *         Precision loss, such as in conversion from floating-point to `uint32_t` on platforms where they differ in size is allowed.
         *         A conversion from `std::string` which contains non-numeric characters will fail.
         *
         * \see can_convert(), is_type()
         *
         * \return A `uint32_t` value.
         */
        uint32_t to_uint32(bool *ok = nullptr) const;

        /*!
         * \brief Returns the containing variant as an `uint64_t` when the \ref is_type() "type" is an `uint64_t`.
         *
         * When the variant contains an \ref type::is_arithmetic() "arithmetic type" or an `std::string` then a conversion to `uint64_t`
         * will be tried. Also any user-defined \ref type::register_converter_func() "conversion function" from the \ref is_type() "source type" to `uint64_t`
         * will be executed when necessary.
         *
         * If \p ok is non-null: \p *ok is set to `true` if the value could be converted to an `uint8_t`; otherwise \p *ok is set to `false`.
         *
         * \remark A value overflow is not allowed, so if the internal value is larger than `uint64_t`, the conversion will fail.
         *         Also a loss of signedness is not allowed, that means a negative signed integer cannot be converted to `uint64_t`.
         *         Precision loss, such as in conversion from floating-point to `uint64_t` on platforms where they differ in size is allowed.
         *         A conversion from `std::string` which contains non-numeric characters will fail.
         *
         * \see can_convert(), is_type()
         *
         * \return A `uint64_t` value.
         */
        uint64_t to_uint64(bool *ok = nullptr) const;

    private:
        /////////////////////////////////////////////////////////////////////////////////


        /*!
         * \brief Returns a pointer to the underlying data
         *
         * \remark You do not have to use this method directly.
         *
         * \return
         */
        RTTR_INLINE void* get_ptr() const;

         /*!
         * \brief Returns the type object of the underlying data
         *
         * \remark You do not have to use this method directly.
         *
         * \return Type object.
         */
        RTTR_INLINE type get_raw_type() const;

        /*!
         * \brief Returns a pointer to the underlying data.
         *        This will return
         *
         * \remark You do not have to use this method directly.
         *
         * \return Raw void pointer.
         */
        RTTR_INLINE void* get_raw_ptr() const;

        //! Helper function to initialize all arithmetic types
        template<typename T>
        detail::enable_if_t<std::is_arithmetic<T>::value, T> convert_impl(bool* ok = nullptr) const;

        template<typename T>
        detail::enable_if_t<!std::is_arithmetic<T>::value && !std::is_enum<T>::value, T> convert_impl(bool* ok = nullptr) const;

        template<typename T>
        detail::enable_if_t<std::is_enum<T>::value, T> convert_impl(bool* ok = nullptr) const;

        /*!
         * \brief Returns a pointer to the underlying object pointer wrapped in a smart_ptr.
         *
         * \remark You do not have to use this method directly.
         *
         * \return Type object of the wrapped pointer object.
         */
        RTTR_INLINE detail::data_address_container get_data_address_container() const;

        bool convert(const type& target_type, variant& var) const;

        /*!
         * \brief Tries to convert the internal type to the given type \p to.
         *
         * \return `True`, when the conversion was successful, otherwise `false`.
         */
        template<typename T>
        bool try_basic_type_conversion(T& to) const;

        /*!
         * \brief Tries to convert the internal pointer type to the given pointer type \p T.
         *
         * \return `True`, when the conversion was successful, otherwise `false`.
         */
        template<typename T>
        typename std::enable_if<detail::pointer_count<T>::value == 1, bool>::type
        try_pointer_conversion(T& to, const type& source_type, const type& target_type) const;

        /*!
         * \brief A dummy method which does in fact always return `falsev.
         *
         * \return `False`.
         */
        template<typename T>
        typename std::enable_if<detail::pointer_count<T>::value != 1, bool>::type
        try_pointer_conversion(T& to, const type& source_type, const type& target_type) const;

        /*!
         * \brief Compares the containing and the given variant \p other for equality.
         *
         * \param ok \p ok is set to `true` if the value could be compared; otherwise \p ok is set to `false`.
         *
         * \return A boolean with value `true`, that indicates both variant's are equal, otherwise `false`.
         */
        bool compare_equal(const variant& other, bool& ok) const;

        /*!
         * \brief Compares the containing and the given variant \p other for less than.
         *
         * \param ok \p ok is set to `true` if the value could be compared; otherwise \p ok is set to `false`.
         *
         * \return A boolean with value `true`, that indicates this variant is less than the \p other, otherwise `false`.
         */
        bool compare_less(const variant& other, bool& ok) const;

        /*!
         * \brief A function to check whether the contained pointer type is a `nullptr` or not.
         *
         * \return A boolean with value `true`, when the contained value type is equal to `nullptr`; otherwise false.
         */
        RTTR_INLINE bool is_nullptr() const;


        variant create_wrapped_value(const type& wrapped_type) const;

    private:
        friend class argument;
        friend class instance;

        template<typename T, typename Tp, typename Converter>
        friend struct detail::variant_data_base_policy;
        friend struct detail::variant_data_policy_nullptr_t;
        friend RTTR_API bool detail::variant_compare_less(const variant&, const type&, const variant&, const type&, bool& ok);
        template<class T>
        friend RTTR_INLINE T* detail::unsafe_variant_cast(variant* operand) RTTR_NOEXCEPT;


        detail::variant_data            m_data;
        detail::variant_policy_func     m_policy;
};

/////////////////////////////////////////////////////////////////////////////////////////

/*!
 * \brief Returns a reference to the containing value as type \p T.
 *
 * \code{.cpp}
 *
 *  variant var = std::string("hello world");
 *  std:string& value_ref = variant_cast<std::string&>(var);  // extracts the value by reference
 *  std:string value = variant_cast<std::string>(var);        // copies the value
 *
 * \endcode
 *
 * \remark Extracting a value type, which is not stored in the variant, leads to undefined behaviour.
 *         No exception or error code will be returned!
 */
template<class T>
T variant_cast(const variant& operand);

/*!
 * \brief Returns a reference to the containing value as type \p T.
 *
 * \code{.cpp}
 *
 *  variant var = std::string("hello world");
 *  std::string& value_ref = variant_cast<std::string&>(var);  // extracts the value by reference
 *  std::string value = variant_cast<std::string>(var);        // copies the value
 *
 * \endcode
 *
 * \remark Extracting a value type, which is not stored in the variant, leads to undefined behaviour.
 *         No exception or error code will be returned!
 */
template<class T>
T variant_cast(variant& operand);

/*!
 * \brief Move the containing value from the variant into a type \p T.
 *
 * \code{.cpp}
 *
 *  variant var = std::string("hello world");
 *  std::string& a = variant_cast<std::string&>(var);
 *  std::string b = variant_cast<std::string>(std::move(var)); // move the value to 'b'
 *  std::cout << "a: " << a << std::endl; // is now empty (nothing to print)
 *  std::cout << "b: " << b << std::endl; // prints "hello world"
 *
 * \endcode
 *
 * \remark Extracting a value type, which is not stored in the variant, leads to undefined behaviour.
 *         No exception or error code will be returned!
 */
template<class T>
T variant_cast(variant&& operand);

/*!
 * \brief Returns a pointer to the containing value with type \p T.
 *        When the containing value is of type \p T, a valid pointer to the type will be returned.
 *        Otherwise a `nullptr` is returned.
 *
 * \code{.cpp}
 *
 *  variant var = std::string("hello world");
 *  std:string* a = variant_cast<std::string>(&var);  // performs an internal type check and returns the value by reference
 *  int* b        = variant_cast<int>(&var);
 *  std::cout << "a valid: " << a != nullptr << std::endl;  // prints "1"
 *  std::cout << "b valid: " << b != nullptr << std::endl;  // prints "0"
 * \endcode
 *
 * \return A valid pointer, when the containing type is of type \p T; otherwise a `nullptr`.
 */
template<class T>
const T* variant_cast(const variant* operand) RTTR_NOEXCEPT;

/*!
 * \brief Returns a pointer to the containing value with type \p T.
 *        When the containing value is of type \p T, a valid pointer to the type will be returned.
 *        Otherwise a `nullptr` is returned.
 *
 * \code{.cpp}
 *
 *  variant var = std::string("hello world");
 *  std:string* a = variant_cast<std::string>(&var);  // performs an internal type check and returns the value by reference
 *  int* b        = variant_cast<int>(&var);
 *  std::cout << "a valid: " << a != nullptr << std::endl;  // prints "1"
 *  std::cout << "b valid: " << b != nullptr << std::endl;  // prints "0"
 * \endcode
 *
 * \return A valid pointer, when the containing type is of type \p T; otherwise a `nullptr`.
 */
template<class T>
T* variant_cast(variant* operand) RTTR_NOEXCEPT;

} // end namespace rttr

#include "rttr/detail/variant/variant_impl.h"

#endif // RTTR_VARIANT_H_
