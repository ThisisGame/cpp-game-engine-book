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

#ifndef RTTR_REGISTRATION_H_
#define RTTR_REGISTRATION_H_

#include "rttr/detail/base/core_prerequisites.h"
#include "rttr/policy.h"
#include "rttr/access_levels.h"
#include "rttr/detail/registration/bind_types.h"
#include "rttr/detail/registration/registration_executer.h"
#include "rttr/detail/default_arguments/default_arguments.h"
#include "rttr/detail/parameter_info/parameter_names.h"
#include "rttr/variant.h"

namespace rttr
{

namespace detail
{
    class metadata;
    template<typename Enum_Type>
    class enum_data;
    struct public_access    {};
    struct protected_access {};
    struct private_access   {};
    using access_levels_list = type_list<public_access, protected_access, private_access>;
}

/*!
 * The \ref registration class is the entry point for the manual registration of reflection information
 * to the type system. It is possible to register *constructors*, *properties*, *methods* and *enumerations*.
 *
 * See following example for a typical usage:
 *
 * Put the \ref RTTR_ENABLE() macro inside the class declaration, when you will inherit from this class,
 * otherwise you can omit the macro.
 * \code{.cpp}
 *  #include <rttr/type>
 *  struct Mesh
 *  {
 *      Mesh();
 *      Mesh(const std::string& name)
 *      Vector3d getPosition() const;
 *      void setPosition(const Vector3d& pos);
 *      enum E_TransformSpace
 *      {
 *          TS_LOCAL,
 *          TS_PARENT,
 *          TS_WORLD
 *      };
 *
 *      void setDirection(const Vector3 &vec, E_TransformSpace ts = TS_LOCAL);
 *      RTTR_ENABLE()
 *  private:
 *      Vector3d _pos;
 *  };
 * \endcode
 *
 * Then in a cpp file, register the constructors, properties and methods of the class \p Mesh.
 *
 * \code{.cpp}
 *  #include "Mesh.cpp"
 *  #include <rttr/registration>
 *  using namespace rttr;
 *
 *  // register the class Mesh before main is called
 *  RTTR_REGISTRATION
 *  {
 *    using namespace rttr;
 *    registration::class_<Mesh>("Mesh")
 *      .constructor<>()
 *      .constructor<const string&>()
 *      .enumeration<E_TransformSpace>("E_TransformSpace")
 *      (
 *          value("TS_LOCAL", TS_LOCAL),
 *          value("TS_PARENT", TS_PARENT),
 *          value("TS_WORLD", TS_WORLD),
 *
 *          metadata("GUI_DESCR", "This enum describes the transformation.")
 *      )
 *      .property("pos", &Mesh::getPosition, &Mesh::setPosition)
 *      (
 *          metadata("GUI_LABEL", "Position."),
 *          metadata("GUI_DESCR", "The position of the mesh."),
 *      )
 *      .method("setDirection", &Mesh::setDirection);
 *  }
 * \endcode
 *
 * \remark See the usage of `()` operator to add additional \ref rttr::metadata(variant, variant) "meta data", \ref policy "policies" or
 *         \ref rttr::value "enum values".
 *
 */
class RTTR_API registration
{
public:
    template<typename...T>
    class bind;

    /*!
     * The \ref class_ is used to register classes to RTTR.
     */
    template<typename Class_Type>
    class class_
    {
        public:
            /*!
             * \brief Construct a class_ object with the given name \p name.
             *
             * \param name The name of the class as string literal. Can be retrieved later via type::get_name().
             *
             */
            class_(string_view name);
            ~class_();


            /*!
             * \brief The bracket operator can be used to add additional meta data to the class type.
             *
             */
            template<typename...Args>
            class_<Class_Type>& operator()(Args&&...args);


            /*!
             * \brief Register a constructor for this class type with or without arguments.
             *
             * \param level  The access level of the constructor; default is: registration::public_access.<br>
             *               (can be also: registration::protected_access or registration::private_access)
             *
             * \remark The default constructor create policy is: \ref policy::ctor::as_std_shared_ptr.
             *
             * \see constructor, type::get_constructor(), type::create()
             *
             * \return A \ref bind object, in order to chain more calls.
             */
            template<typename... Args, typename acc_level = detail::public_access, typename Tp = typename std::enable_if<detail::contains<acc_level, detail::access_levels_list>::value>::type>
            bind<detail::ctor, Class_Type, acc_level, Args...> constructor(acc_level level = acc_level());

            /*!
             * \brief Register a constructor for this class type which uses a function \p F.
             *
             * \param func   A function which creates an instance of \p Class_Type;
             *               This can be a pointer to a function or a std::function.
             * \param level  The access level of the constructor; default is: registration::public_access.<br>
             *               (can be also: registration::protected_access or registration::private_access)
             *
             * \see constructor, type::get_constructor(), type::create()
             *
             * \return A \ref bind object, in order to chain more calls.
             */
            template<typename F, typename acc_level = detail::public_access, typename Tp = typename std::enable_if<!detail::contains<F, detail::access_levels_list>::value>::type>
            bind<detail::ctor_func, Class_Type, F, acc_level> constructor(F func, acc_level level = acc_level());


            /*!
             * \brief Register a property to this class.
             *
             * \param name  The name of the property.
             * \param acc   The accessor to the property; this can be a pointer to a member or a pointer to a variable.
             * \param level  The access level of the property; default is: registration::public_access.<br>
             *               (can be also: registration::protected_access or registration::private_access)
             *
             * \remark The name of the property has to be unique for this class, otherwise it will not be registered.
             *
             * \see property, type::get_property(), type::get_property_value(), type::set_property_value()
             *
             * \return A \ref bind object, in order to chain more calls.
             */
            template<typename A, typename acc_level = detail::public_access, typename Tp = typename std::enable_if<detail::contains<acc_level, detail::access_levels_list>::value>::type>
            bind<detail::prop, Class_Type, A, acc_level> property(string_view name, A acc, acc_level level = acc_level());

            /*!
             * \brief Register a read only property to this class.
             *
             * \param name  The name of the property.
             * \param acc   The accessor to the property; this can be a pointer to a member, a pointer to a variable,
             *              a pointer to a member function, a pointer to a function or a std::function.
             * \param level  The access level of the read only property; default is: registration::public_access.<br>
             *               (can be: registration::protected_access or registration::private_access)
             *
             * \remark The name of the property has to be unique for this class, otherwise it will not be registered.
             *
             * \see property, type::get_property(), type::get_property_value(), type::set_property_value()
             *
             * \return A \ref bind object, in order to chain more calls.
             */
            template<typename A, typename acc_level = detail::public_access, typename Tp = typename std::enable_if<detail::contains<acc_level, detail::access_levels_list>::value>::type>
            bind<detail::prop_readonly, Class_Type, A, acc_level> property_readonly(string_view name, A acc, acc_level level = acc_level());

            /*!
             * \brief Register a property to this class.
             *
             * \param name   The name of the property.
             * \param getter The getter accessor to the property; this can be a pointer to a member function,
             *               a pointer to a function or a std::function.
             * \param setter The setter accessor to the property; this can be a pointer to a member function,
             *               a pointer to a function or a std::function.
             * \param level  The access level of the property; default is: registration::public_access.<br>
             *               (can be also: registration::protected_access or registration::private_access)
             *
             * \remark The name of the property has to be unique for this class, otherwise it will not be registered.
             *
             * \see property, type::get_property(), type::get_property_value(), type::set_property_value()
             *
             * \return A \ref bind object, in order to chain more calls.
             */
            template<typename A1, typename A2, typename Tp = typename std::enable_if<!detail::contains<A2, detail::access_levels_list>::value>::type, typename acc_level = detail::public_access>
            bind<detail::prop, Class_Type, A1, A2, acc_level> property(string_view name, A1 getter, A2 setter, acc_level level = acc_level());


            /*!
             * \brief Register a method to this class.
             *
             * \param name      The name of the method.
             * \param function  The function accessor to this method; this can be a member function, a function or an std::function.
             * \param level     The access level of the method; default is: registration::public_access.<br>
             *                  (can be also: registration::protected_access or registration::private_access)
             *
             * \remark The method name does not have to be unique for this class.
             *
             * \see method, type::get_method(), type::invoke()
             *
             * \return A \ref bind object, in order to chain more calls.
             */
            template<typename F, typename acc_level = detail::public_access>
            bind<detail::meth, Class_Type, F, acc_level> method(string_view name, F f, acc_level level = acc_level());


            /*!
             * \brief Register a nested enumeration of type \p Enum_Type
             *
             * \param name      The name of the enumeration.
             *
             * \see enumeration, type::get_enumeration()
             *
             * \return A \ref bind object, in order to chain more calls.
             */
            template<typename Enum_Type>
            bind<detail::enum_, Class_Type, Enum_Type> enumeration(string_view name);
        private:
            class_(const std::shared_ptr<detail::registration_executer>& reg_exec);
            class_(const class_& other);
            class_& operator=(const class_& other);
        private:
            std::shared_ptr<detail::registration_executer> m_reg_exec;
            template<typename...T>
            friend class bind;
    };

    /*!
     * \brief Register a global property with read write access.
     *
     * \param name  The name of the property.
     * \param acc   The accessor to the property; a pointer to a variable.
     *
     * \remark The name of the property has to be unique, otherwise it will not be registered.
     *
     * \see property, type::get_global_property(),
     *                \ref type::get_property_value(string_view) "type::get_property_value()",
     *                \ref type::set_property_value(string_view, argument) "type::set_property_value"
     *
     * \return A \ref bind object, in order to chain more calls.
     */
    template<typename A>
    static bind<detail::prop, detail::invalid_type, A, detail::public_access> property(string_view name, A acc);

    /*!
     * \brief Register a global read only property.
     *
     * \param name  The name of the property.
     * \param acc   The accessor to the property; this can be a pointer to a variable,
     *              a pointer to a function or a std::function.
     *
     * \remark The name of the property has to be unique, otherwise it will not be registered.
     *
     * \see property, type::get_global_property(),
     *                \ref type::get_property_value(string_view) "type::get_property_value()",
     *                \ref type::set_property_value(string_view, argument) "type::set_property_value"
     *
     * \return A \ref bind object, in order to chain more calls.
     */
    template<typename A>
    static bind<detail::prop_readonly, detail::invalid_type, A, detail::public_access> property_readonly(string_view name, A acc);

    /*!
     * \brief Register a property to this class.
     *
     * \param name   The name of the property.
     * \param getter The getter accessor to the property; this can be a pointer to a function or a std::function.
     * \param setter The setter accessor to the property; this can be a pointer to a function or a std::function.
     *
     * \remark The name of the property has to be unique, otherwise it will not be registered.
     *
     * \see property, type::get_global_property(),
     *                \ref type::get_property_value(string_view) "type::get_property_value()",
     *                \ref type::set_property_value(string_view, argument) "type::set_property_value"
     *
     * \return A \ref bind object, in order to chain more calls.
     */
    template<typename A1, typename A2>
    static bind<detail::prop, detail::invalid_type, A1, A2, detail::public_access> property(string_view name, A1 getter, A2 setter);

    /*!
     * \brief Register a method to this class.
     *
     * \param name      The name of the method.
     * \param function  The function accessor to this method; this can be a member function, a function or a std::function.
     *
     * \remark The method name does *not* have to be unique.
     *
     * \see method, type::get_global_method(),
     *              \ref type::invoke(string_view, std::vector<argument>) "type::invoke()"
     *
     * \return A \ref bind object, in order to chain more calls.
     */
    template<typename F>
    static bind<detail::meth, detail::invalid_type, F, detail::public_access> method(string_view name, F f);

    /*!
     * \brief Register a global enumeration of type \p Enum_Type
     *
     * \param name The name of the enumeration.
     *
     * \remark The name of the enumeration has to be unique, otherwise it will not be registered.
     *
     * \see enumeration, type::get_enumeration()
     *
     * \return A \ref bind object, in order to chain more calls.
     */
    template<typename Enum_Type>
    static bind<detail::enum_, detail::invalid_type, Enum_Type> enumeration(string_view name);

    /////////////////////////////////////////////////////////////////////////////////////

    /*!
     * This variable can be used to specify during registration of a class member
     * the access level: `public`.
     *
     * See following example code:
     * \code{.cpp}
     *  using namespace rttr;
     *  struct Foo
     *  {
     *      void func() {}
     *  };
     *
     *  RTTR_REGISTRATION
     *  {
     *      registration::class_<Foo>("Foo")
     *          .method("func", &Foo::func, registration::public_access);
     *  }
     * \endcode
     *
     * \see access_levels
     */
    static const detail::public_access      public_access;

    /*!
     * This variable can be used to specify during registration of a class member
     * the access level: `protected`.
     *
     * See following example code:
     * \code{.cpp}
     *  using namespace rttr;
     *  struct Foo
     *  {
     *      protected:
     *          void func() {}
     *
     *      RTTR_REGISTRATION_FRIEND
     *  };
     *
     *  RTTR_REGISTRATION
     *  {
     *      registration::class_<Foo>("Foo")
     *          .method("func", &Foo::func, registration::protected_access);
     *  }
     * \endcode
     *
     * \see access_levels
     */
    static const detail::protected_access   protected_access;

    /*!
     * This variable can be used to specify during registration of a class member
     * the access level: `private`.
     *
     * See following example code:
     * \code{.cpp}
     *  using namespace rttr;
     *  struct Foo
     *  {
     *      private:
     *          void func() {}
     *
     *      RTTR_REGISTRATION_FRIEND
     *  };
     *
     *  RTTR_REGISTRATION
     *  {
     *      registration::class_<Foo>("Foo")
     *          .method("func", &Foo::func, registration::private_access);
     *  }
     * \endcode
     *
     * \see access_levels
     */
    static const detail::private_access     private_access;

private:
    registration() {}
    registration(const std::shared_ptr<detail::registration_executer>& reg_exec) : m_reg_exec(reg_exec) { }
    registration(const registration& other);
    registration& operator=(const registration& other);

private:
    std::shared_ptr<detail::registration_executer> m_reg_exec;
    template<typename...T>
    friend class bind;
};

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

/*!
 * \brief This is a helper function to register overloaded functions.
 *
 * Use it like following:
 * \code{.cpp}
 *
 * #include <rttr/registration>
 * #include <cmath>
 * using namespace rttr;
 *
 * RTTR_REGISTRATION
 * {
 *      registration::method("pow", select_overload<float(float, float)>(&pow));
 * }
 * \endcode
 *
 * \remark The method **cannot** be used with *MSVC x86* compiler, because of the different calling convention for global- and member-functions.
 * As workaround you have to explicitly cast to the function pointer:
 *
 * \code{.cpp}
 *
 * RTTR_REGISTRATION
 * {
 *      registration::method("pow", static_cast<float(*)(float, float)>(&pow));
 * }
 * \endcode
 *
 */
template<typename Signature>
Signature* select_overload(Signature* func)
{
  return func;
}


/*!
 * \brief This is a helper function to register overloaded member functions.
 *
 * Use it like following:
 * \code{.cpp}
 *
 * #include <rttr/registration>
 * #include <cmath>
 * using namespace rttr;
 *
 * struct Foo
 * {
 *   void func(int) {}
 *   void func(int, int) {}
 * };
 *
 * RTTR_REGISTRATION
 * {
 *      registration::class_<Foo>("Foo")
 *                   .method("func", select_overload<void(int)>(&Foo::func))
 *                   .method("func", select_overload<void(int, int)>(&Foo::func));
 * }
 * \endcode
 *
 * \remark The method **cannot** be used with *MSVC x86* compiler, because of the different calling convention for global- and member-functions.
 * As workaround you have to explicitly cast to the member function pointer:
 *
 * \code{.cpp}
 *
 * RTTR_REGISTRATION
 * {
 *      registration::class_<Foo>("Foo")
 *                   .method("func", static_cast<void(Foo::*)(int)>(&Foo::func))
 *                   .method("func", static_cast<void(Foo::*)(int, int)>(&Foo::func));
 * }
 * \endcode
 */
template<typename Signature, typename ClassType>
auto select_overload(Signature (ClassType::*func)) -> decltype(func)
{
    return func;
}


/*!
 * \brief This is a helper function to register overloaded const member functions.
 *
 * Use it like following:
 * \code{.cpp}
 *
 * #include <rttr/registration>
 * using namespace rttr;
 * struct Foo
 * {
 *   void func() {}
 *   void func() const {}
 * };
 *
 * RTTR_REGISTRATION
 * {
 *      registration::class_<Foo>("Foo")
 *                   .method("func", select_non_const(&Foo::func))
 *                   .method("func", select_const(&Foo::func));
 * }
 * \endcode
 *
 * \remark The method **cannot** be used with *MSVC x86* compiler, because of the different calling convention for global- and member-functions.
 * As workaround you have to explicitly cast to the member function pointer:
 *
 * \code{.cpp}
 *
 * RTTR_REGISTRATION
 * {
 *      registration::class_<Foo>("Foo")
 *                   .method("func", static_cast<void(Foo::*)()>(&Foo::func))
 *                   .method("func", static_cast<void(Foo::*)()const>(&Foo::func));
 * }
 * \endcode
 */
template<typename ClassType, typename ReturnType, typename... Args>
auto select_const(ReturnType (ClassType::*func)(Args...) const) -> decltype(func)
{
    return func;
}

#ifndef RTTR_NO_CXX17_NOEXCEPT_FUNC_TYPE
/*!
 * \brief Overload with `noexcept` function type.
 *
 * \see select_const
 */
template<typename ClassType, typename ReturnType, typename... Args>
auto select_const(ReturnType (ClassType::*func)(Args...) const noexcept) -> decltype(func)
{
    return func;
}
#endif

/*!
 * \brief This is a helper function to register overloaded const member functions.
 *
 * Use it like following:
 * \code{.cpp}
 *
 * #include <rttr/registration>
 * using namespace rttr;
 * struct Foo
 * {
 *   void func() {}
 *   void func() const {}
 * };
 *
 * RTTR_REGISTRATION
 * {
 *      registration::class<Foo>("Foo")
 *                   .method("func", select_non_const(&Foo::func))
 *                   .method("func", select_const(&Foo::func));
 * }
 * \endcode
 *
 * \remark The method **cannot** be used with *MSVC x86* compiler, because of the different calling convention for global- and member-functions.
 * As workaround you have to explicitly cast to the member function pointer:
 *
 * \code{.cpp}
 *
 * RTTR_REGISTRATION
 * {
 *      registration::class_<Foo>("Foo")
 *                   .method("func", static_cast<void(Foo::*)()>(&Foo::func))
 *                   .method("func", static_cast<void(Foo::*)()const>(&Foo::func));
 * }
 * \endcode
 *
 */
template<typename ClassType, typename ReturnType, typename... Args>
auto select_non_const(ReturnType(ClassType::*func)(Args...)) -> decltype(func)
{
    return func;
}

#ifndef RTTR_NO_CXX17_NOEXCEPT_FUNC_TYPE

/*!
 * \brief Overload with `noexcept` function type.
 *
 * \see select_const
 */
template<typename ClassType, typename ReturnType, typename... Args>
auto select_non_const(ReturnType(ClassType::*func)(Args...) noexcept) -> decltype(func)
{
    return func;
}
#endif

/*!
 * The \ref metadata function can be used to add additional meta data information during the
 * registration process of reflection information. Use it in the `()` operator of the returned
 * \ref bind object.
 */
RTTR_INLINE detail::metadata metadata(variant key, variant value);


/*!
 * The \ref value function should be used to add a mapping from enum `name` to `value`
 * during the registration process of reflection information. Use it in the `()` operator of the returned
 * \ref bind object.
 *
 * \see \ref registration::enumeration
 */
template<typename Enum_Type>
RTTR_INLINE detail::enum_data<Enum_Type> value(string_view, Enum_Type value);

/*!
 * The \ref default_arguments function should be used add default arguments,
 * for \ref constructor "constructors" or a \ref method "methods" during the registration process of reflection information.
 * Use it in the `()` operator of the returned \ref bind object.
 *
 * The given arguments must match the signature from the starting position to the right most argument.
 *
 * See following example code:
 * \code{.cpp}
 *
 * void my_function(int a, bool b, const std::string& text);
 *
 * RTTR_REGISTRATION
 * {
 *     using namespace rttr;
 *
 *     registration::method("my_function", &my_function)
 *     (
 *         default_arguments(true, std::string("default text"))
 *     );
 *
 *     // however setting default arguments like this, will raise a compile time error
 *     registration::method("my_function2", &my_function)
 *     (
 *          default_arguments(true) // does not work!!!; default value for the last argument is missing...
 *     );
 * }
 *
 * \endcode
 *
 */
template<typename...TArgs>
RTTR_INLINE detail::default_args<TArgs...> default_arguments(TArgs&&...args);

/*!
 * The \ref parameter_names function should be used add human-readable names of the parameters,
 * for \ref constructor "constructors" or a \ref method "methods" during the registration process of reflection information.
 * Use it in the `()` operator of the returned \ref bind object.
 *
 * The names must be provided as string literals (i.e. const char*) arguments.
 *
 * See following example code:
 * \code{.cpp}
 *   using namespace rttr;
 *
 *   void set_window_geometry(const char* name, int w, int h) {...}
 *
 *   RTTR_REGISTRATION
 *   {
 *        registration::method("set_window_geometry", &set_window_geometry)
 *        (
 *            parameter_names("window name", "width", "height")
 *        );
 *   }
 * \endcode
 *
 * \see \ref parameter_info
 *
 */
template<typename...TArgs>
RTTR_INLINE detail::parameter_names<detail::decay_t<TArgs>...> parameter_names(TArgs&&...args);


/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

#ifdef DOXYGEN


/*!
 * \brief Use this macro to automatically register your reflection information to RTTR before `main` is called.
 *
 * Use it in following way:
 * \code{.cpp}
 * RTTR_REGISTRATION
 * {
 *      rttr::registration::method("foo", &foo);
 * }
 * \endcode
 *
 * Just place the macro in global scope in a cpp file.
 *
 * \remark It is not possible to place the macro multiple times in one cpp file.
 *
 */
#define RTTR_REGISTRATION

/*!
 * \brief Use this macro to automatically register your reflection information inside a plugin to RTTR.
 *
 * Use it in following way:
 * \code{.cpp}
 *
 *   int some_method() { return 42; }
 *
 *   RTTR_PLUGIN_REGISTRATION
 *   {
 *       rttr::registration::method("some_method", &some_method);
 *   }
 * \endcode
 *
 * Just place the macro in global scope in a cpp file.
 *
 * \remark It is not possible to place the macro multiple times in one cpp file.
 *         When you compile your plugin with the `gcc` toolchain, make sure you use the compiler option: `-fno-gnu-unique`.
 *         otherwise the unregistration will not work properly.
 *
 * \see library
 */
#define RTTR_PLUGIN_REGISTRATION

/*!
 * \brief Place this macro inside a class, when you need to reflect properties,
 *        methods or constructors which are declared in `protected` or `private` scope of the class.
 *
\code{.cpp}
class Foo
{
    private:
        int m_value;

    RTTR_REGISTRATION_FRIEND
};
\endcode

\code{.cpp}
RTTR_REGISTRATION
{
    rttr::registration::class_<Foo>()
        .property("value", &Foo::m_value);
}
\endcode
 */
#define RTTR_REGISTRATION_FRIEND

/*!
 * The \ref bind class is used to chain registration calls.
 *
 * The class should not be instantiate directly, because it will be created automatically,
 * when a *constructor*, *property*, *method* or *enumeration* is registered.
 *
 * Additionally, the class has implemented the bracket operator `()` as variadic function template.
 * Use it to forward \ref rttr::metadata() "metadata()" or \ref policy "policies" to the previous registered item.
 *
 * \remark Do not instantiate this class directly!
*/
template<typename...T>
class registration::bind : public detail::base_class
{
    public:
        /*!
         * \brief The bracket operator can be used to add additional meta data or policies.
         */
        template<typename... Args>
        base_class operator()(Args&&... arg);
};

#endif

} // end namespace rttr

#include "rttr/detail/registration/registration_impl.h"

#endif // RTTR_REGISTRATION_H_
