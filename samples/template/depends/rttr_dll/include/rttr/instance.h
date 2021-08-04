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

#ifndef RTTR_INSTANCE_H_
#define RTTR_INSTANCE_H_

#include "rttr/detail/base/core_prerequisites.h"
#include "rttr/detail/misc/misc_type_traits.h"
#include "rttr/detail/misc/data_address_container.h"

namespace rttr
{
class variant;
class type;
class argument;

/*!
 * The \ref instance class is used for forwarding the instance of an object to invoke a \ref property or \ref method.
 *
 * \remark The \ref instance class will internally hold a reference to the object. It will not perform any copy operation on the data itself.
 *         Make sure you don't hold an \ref instance, while the underlying object is already destroyed. Otherwise it will lead to undefined behavior.
 */
class RTTR_API instance
{
    template<typename T, typename Tp = detail::decay_t<T>>
    using decay_instance_t = detail::enable_if_t<!std::is_same<instance, Tp>::value &&
                                                 !std::is_same<variant, Tp>::value, T>;

public:
    /*!
     * \brief Creates an \ref instance::is_valid() "invalid" instance object.
     *
     * Use this constructor, when you need to invoke a property or method where no instance is required.
     */
    RTTR_INLINE instance() RTTR_NOEXCEPT;

    /*!
     * \brief Creates an instance object from a \ref variant object.
     */
    RTTR_INLINE instance(const variant& var) RTTR_NOEXCEPT;

    /*!
     * \brief Copy constructor for an instance.
     */
    RTTR_INLINE instance(const instance& other) RTTR_NOEXCEPT;

    /*!
     * \brief Creates an instance object from type \p T.
     *
     * \remark Internally, the instance class will hold a reference to the address of the given object \p data.
     */
    template<typename T, typename Tp = decay_instance_t<T>>
    RTTR_INLINE instance(T& data) RTTR_NOEXCEPT;

    /*!
     * \brief This function will try to convert the underlying instance to the given type \p Target_Type*.
     *        When the conversion succeeds, a valid pointer will be returned. Otherwise a nullptr.
     *
     * \return A pointer to the instance of \p Target_Type, when the conversion succeeds, otherwise a nullptr.
     */
    template<typename Target_Type>
    RTTR_INLINE Target_Type* try_convert() const RTTR_NOEXCEPT;

    /*!
     * \brief Returns true when the instance class contains a reference to an object. Otherwise false.
     *
     * \return True when a reference is stored, otherwise false.
     */
    RTTR_INLINE bool is_valid() const RTTR_NOEXCEPT;

    /*!
     * \brief Returns true when the instance class contains a reference to an object. Otherwise false.
     *
     * \return True when a reference is stored, otherwise false.
     */
    explicit operator bool() const RTTR_NOEXCEPT;

    /*!
     * \brief Returns the type of the internally hold instance.
     *
     * \remark When no reference is stored, an \ref type::is_valid() "invalid"  \ref type object will be returned.
     *
     * \return Type object of stored reference.
     */
    RTTR_INLINE type get_type() const RTTR_NOEXCEPT;

    /*!
     * \brief Returns an \ref instance object for the wrapped instance.
     *
     * See following example code:
     * \code{.cpp}
     *   std::shared_ptr<foo> f;
     *   instance obj = f;
     *
     *   obj.get_type() == type::get<std::shared_ptr<foo>>();       // yields to true
     *   obj.get_wrapped_instance().get_type() == type::get<foo>(); // yields to true
     * \endcode
     *
     * \remark When the current instance is not a \ref type::is_wrapper() "wrapper type",
     *         an \ref instance::is_valid() "invalid" instance will be returned.
     *
     * \return An \ref instance object from the wrapped type.
     */
    RTTR_INLINE instance get_wrapped_instance() const RTTR_NOEXCEPT;

    /*!
     * \brief Returns the most derived type of the hold instance.
     *
     * See following example code:
     * \code{.cpp}
     *   struct base { RTTR_ENABLE() };
     *   struct middle : base { RTTR_ENABLE(base) };
     *   struct derived : middle { RTTR_ENABLE(middle) };
     *   //...
     *   derived d;
     *   base& b = d;
     *   instance obj = b;
     *
     *   obj.get_type() == type::get<base>();               // yields to true
     *   obj.get_derived_type() == type::get<derived>();    // yields to true
     * \endcode
     */
    RTTR_INLINE type get_derived_type() const RTTR_NOEXCEPT;

private:
    instance& operator=(const instance& other) RTTR_NOEXCEPT;

    detail::data_address_container m_data_container;
};

} // end namespace rttr

#include "rttr/detail/impl/instance_impl.h"

#endif // RTTR_INSTANCE_H_
