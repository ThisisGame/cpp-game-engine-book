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

#ifndef RTTR_POLICY_H_
#define RTTR_POLICY_H_

#include "rttr/detail/base/core_prerequisites.h"
#include "rttr/detail/misc/misc_type_traits.h"
#include "rttr/detail/policies/ctor_policies.h"
#include "rttr/detail/policies/meth_policies.h"
#include "rttr/detail/policies/prop_policies.h"

namespace rttr
{

/*!
 * The \ref policy class contains all policies that can be used during the registration of reflection information.
 *
 * For easier usage, the policies are subdivided into following groups:
 * - for methods: \ref policy::meth
 * - for properties: \ref policy::prop
 * - for constructors: \ref policy::ctor
 *
 */
struct RTTR_API policy
{
    /*!
     * The \ref meth class groups all policies that can be used during registration of methods.
     *
     * \endcode
     */
    struct RTTR_API meth
    {
        /*!
         * This policy can be used when a method return a reference to an object
         * and the caller should be able to access this object via the returned variant.
         * Reference cannot be copied directly in a variant, therefore it is possible transform the reference
         * to a pointer.
         *
         * See following example code:
         * \code{.cpp}
         *
         * std::string& get_text() { static std:string text("hello world"); return text; }
         *
         * RTTR_REGISTRATION
         * {
         *      registration::method("get_text", &get_text)
         *                    (
         *                       policy::meth::return_ref_as_ptr
         *                    );
         * }
         * int main()
         * {
         *   method meth = type::get_global_method("get_text");
         *   std::cout << meth.get_return_type().get_name();    // prints "std::string*"
         *   variant var = meth.invoke(instance());
         *   std::cout << var.is_type<std::string*>();          // prints "true"
         *   return 0;
         * }
         * \endcode
         */
        static const detail::return_as_ptr      return_ref_as_ptr;

       /*!
         * This policy should be used when the return value of a method should not be forwarded to the caller.
         * For the caller it looks like the method has no return value, the return type will be *void*.
         *
         * See following example code:
         * \code{.cpp}
         *
         * using namespace rttr;
         * int my_func() { return 42; }
         *
         * RTTR_REGISTRATION
         * {
         *      registration::method("my_func", &my_func)
         *                    (
         *                       policy::meth::discard_return
         *                    );
         * }
         *
         * int main()
         * {
         *   method meth = type::get_global_method("my_func");
         *   std::cout << meth.get_return_type().get_name();    // prints "void"
         *   variant var = meth.invoke(instance());
         *   std::cout << var.is_type<void>();                  // prints "true"
         *   return 0;
         * }
         * \endcode
         */
        static const detail::discard_return     discard_return;
    };

    /*!
     * The \ref prop class groups all policies that can be used during registration of properties.
     *
     */
    struct RTTR_API prop
    {
        /*!
         * The \ref bind_as_ptr policy will bind a member object as *pointer* type.
         *
         * This can be useful when binding big data types, like arrays, to avoid copies during get/set of the property.
         *
         * See following example code:
         * \code{.cpp}
         * using namespace rttr;
         * struct Foo
         * {
         *   std::vector<int> vec;
         * };
         *
         * RTTR_REGISTRATION
         * {
         *      registration::class_<Foo>("Foo")
         *                   .property("vec", &Foo::vec)
         *                   (
         *                       policy::prop::bind_as_ptr
         *                   );
         * }
         *
         * int main()
         * {
         *   Foo obj;
         *   property prop = type::get<Foo>().get_property("vec");
         *   variant var = prop.get_value(obj);
         *   std::cout << var.is_type<std::vector<int>*>(); // prints "true"
         *   prop.set_value(obj, var);                      // not really necessary, but remark that now a std::vector<int>* is expected
         *   return 0;
         * }
         * \endcode
         */
        static const detail::bind_as_ptr        bind_as_ptr;

        /*!
         * The \ref as_reference_wrapper policy will bind a member object as *std::reference_wrapper* type.
         *
         * This can be useful when binding big data types, like arrays, to avoid copies during get/set of the property.
         *
         * See following example code:
         * \code{.cpp}
         * using namespace rttr;
         * struct Foo
         * {
         *   std::vector<int> vec;
         * };
         *
         * RTTR_REGISTRATION
         * {
         *      registration::class_<Foo>("Foo")
         *                   .property("vec", &Foo::vec)
         *                   (
         *                       policy::prop::as_reference_wrapper
         *                   );
         * }
         *
         * int main()
         * {
         *   Foo obj;
         *   property prop = type::get<Foo>().get_property("vec");
         *   variant var = prop.get_value(obj);
         *   std::cout << var.is_type<std::reference_wrapper<std::vector<int>>>(); // prints "true"
         *   prop.set_value(obj, var);      // not really necessary, but remark that now a std::reference_wrapper<std::vector<int>> is expected
         *   return 0;
         * }
         * \endcode
         */
        static const detail::as_reference_wrapper        as_reference_wrapper;
    };

    /*!
     * The \ref ctor class groups all policies that can be used during registration of constructors.
     *
     */
    struct RTTR_API ctor
    {
        /*!
         * The \ref as_raw_ptr policy will create an instance of a class as raw pointer.
         *
         * That means the object is created with a *new*-expression and its lifetime lasts
         * until it is destroyed using a *delete*-expression.
         * In order to invoke the delete expression use the corresponding \ref destructor.
         *
         * See following example code:
         * \code{.cpp}
         * using namespace rttr;
         * struct Foo
         * {
         * };
         *
         * RTTR_REGISTRATION
         * {
         *      registration::class_<Foo>("Foo")
         *                   .constructor<>()
         *                   (
         *                       policy::ctor::as_raw_ptr
         *                   );
         * }
         *
         * int main()
         * {
         *   variant var = type::get<Foo>().create();
         *   std::cout << var.is_type<Foo*>();          // prints "true"
         *   var.get_type().destroy(var);               // free's the memory with 'delete'
         *   std::cout << var.is_valid();               // prints "false"
         *   return 0;
         * }
         * \endcode
         */
        static const detail::as_raw_pointer         as_raw_ptr;

         /*!
         * The \ref as_std_shared_ptr policy will create an instance of a class through *std::make_shared<T>*.
         *
         * That means the object is \ref type::is_wrapper() "wrapped" into a *std::shared_ptr<T>*.
         * The wrapped object is destroyed and its memory deallocated when either of the following happens:
         * - the last remaining variant owning the *shared_ptr* is destroyed
         * - the last remaining variant owning the *shared_ptr* is assigned another data or variant
         *
         * The object is destroyed using the default deleter of *std::shared_ptr*.
         *
         * See following example code:
         * \code{.cpp}
         * using namespace rttr;
         * struct Foo
         * {
         * };
         *
         * RTTR_REGISTRATION
         * {
         *      registration::class_<Foo>("Foo")
         *                   .constructor<>()
         *                    (
         *                        policy::ctor::as_std_shared_ptr
         *                    );
         * }
         *
         * int main()
         * {
         *   variant var = type::get<Foo>().create();
         *   std::cout << var.is_type<std::shared_ptr<Foo>>();  // prints "true"
         *   return 0;                                          // the memory for contained 'Foo' instance is freed automatically,
         * }                                                    // because the var object is gone out of scope
         * \endcode
         */
        static const detail::as_std_shared_ptr      as_std_shared_ptr;

        /*!
         * The \ref as_object policy will create an instance of a class with automatic storage.
         *
         * Objects with automatic storage duration are automatically destroyed when the block in which they are created exits.
         * Which is in our case the \ref variant.
         * However, that means also you don't have to deal with pointers or wrappers. In order to use this creation policy,
         * the object must be *copy constructible*.
         *
         * See following example code:
         * \code{.cpp}
         * using namespace rttr;
         * struct Foo
         * {
         * };
         *
         * RTTR_REGISTRATION
         * {
         *      registration::class_<Foo>("Foo")
         *                   .constructor<>()
         *                    (
         *                        policy::ctor::as_object
         *                    );
         * }
         *
         * int main()
         * {
         *   variant var = type::get<Foo>().create();   // creates a new instance of 'Foo' and moves the content into variant 'var'
         *   std::cout << var.is_type<Foo>();           // prints "true"
         *   variant var2 = var;                        // creates a new instance of 'Foo', through copy construction
         *   return 0;                                  // the memory of the two 'Foo' instances is freed automatically
         * }
         * \endcode
         */
        static const detail::as_object              as_object;
    };
};

} // end namespace rttr

#endif // RTTR_POLICY_H_
