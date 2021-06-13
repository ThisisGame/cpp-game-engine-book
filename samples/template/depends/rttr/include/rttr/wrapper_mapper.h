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

#ifndef RTTR_WRAPPER_MAPPER_H_
#define RTTR_WRAPPER_MAPPER_H_

#include "rttr/detail/base/core_prerequisites.h"

namespace rttr
{
//////////////////////////////////////////////////////////////////////////////////////

namespace detail { struct invalid_wrapper_type { }; }

/*!
 * The \ref wrapper_mapper class is a class template to access different wrapper types via one common interface.
 *
 * A wrapper type is a class which encapsulate an instance of another type.
 * This are for instance smart pointer classes, e.g. `std::shared_ptr<T>` or `std::unique_ptr<T>`.
 * Out of the box, RTTR recognize following wrapper types:
 * - \p `std::shared_ptr<T>`
 * - \p `std::reference_wrapper<T>`
 * - \p `std::weak_ptr<T>`
 * - \p `std::unique_ptr<T>`
 *
 * Custom wrapper types
 * --------------------
 * In order to work with custom wrapper types, its required to specialize the class
 * \ref rttr::wrapper_mapper<T> "wrapper_mapper<T>". Therefore you have to provide two nested type aliases:
 * 1. `using wrapped_type = typename T::encapsulated_type;`
 * 2. `using type = T`
 *
 * And three functions:
 * 1. `static wrapped_type get(const T& obj);`
 * 2. `static T create(wrapped_type& obj);` (Optional)
 * 3. `static T<U> convert(const type& source, bool& ok);` (Optional)
 *
 * \remark The \ref rttr::wrapper_mapper<T>::create(T& obj) "create()" function is optional. When no one is provided,
 *         then it will be not possible to convert from the wrapped type to the wrapper class from inside a variant.
 *         The \ref rttr::wrapper_mapper<T>::convert(const type& source, bool& ok) "convert()" function is also optional.
 *         When no one is provided, you cannot use the \ref rttr::type::register_wrapper_converter_for_base_classes<T>()
 *         function. For the wrapper classes: `std::shared_ptr<T>` and `std::reference_wrapper<T>`
 *         default conversion functions are included.
 *
 * \see variant::convert()
 *
 * Following code example illustrates how to add a specialization:
 * \code{.cpp}
 *
 *  // custom_type.h
 *
 *  template<typename T>
 *  class custom_type
 *  {
 *  public:
 *      custom_type(T& obj) : m_data(std::addressof(obj)) {}
 *      custom_type(T) : m_data(nullptr) {}
 *      T& get_data() { return *m_value; }
 *  private:
 *      T* m_data;
 *  };
 *
 *  // the best place for the specialization, is in the same header file like the type itself
 *  namespace rttr
 *  {
 *
 *  template<typename T>
 *  struct wrapper_mapper<custom_type<T>>
 *  {
 *      using wrapped_type  = decltype(std::declval<custom_type<T>>().get_data());
 *      using type          = custom_type<T>;
 *
 *      inline static wrapped_type get(const type& obj)
 *      {
 *         return obj.get_data();
 *      }
 *
 *      inline static type create(const wrapped_type& value)
 *      {
 *         return custom_type<T>(value);
 *      }
 *
 *      template<typename U>
 *      inline static custom_type<U> convert(const type& source, bool& ok)
 *      {
 *          if (auto obj = rttr_cast<typename custom_type<U>::wrapped_type*>(&source.get_data()))
 *          {
 *              ok = true;
 *              return custom_type<U>(*obj);
 *          }
 *          else
 *          {
 *              ok = false;
 *              return custom_type<U>();
 *          }
 *      }
 *  };
 *
 *  } // end namespace rttr
 * \endcode
 *
 * \remark
 * It is very important that the type alias for `wrapped_type` is the actual return type of the getter function.
 * Make also sure you put your specialization inside the namespace `rttr`.
 * The best place for this code, is below the declaration of your wrapper type.
 * When this is not possible, include your specialization code before registering your types to RTTR.
 */
template<typename T>
struct wrapper_mapper
{
#ifndef DOXYGEN
    using wrapped_type  = detail::invalid_wrapper_type;
    using type          = T;
#else
    using wrapped_type  = typename wrapper_type::encapsulated_type;
    using type          = T;

    inline static wrapped_type get(const type& obj)
    {
        return obj.get();
    }

    inline static type create(const wrapped_type& value)
    {
        return type(value);
    }

    template<typename U>
    inline static T<U> convert(const type& source, bool& ok)
    {
        return type(value);
    }
#endif
};

//////////////////////////////////////////////////////////////////////////////////////

} // end namespace rttr

#include "rttr/detail/impl/wrapper_mapper_impl.h"

#endif // RTTR_WRAPPER_MAPPER_H_
