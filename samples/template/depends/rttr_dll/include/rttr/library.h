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

#ifndef RTTR_LIBRARY_H_
#define RTTR_LIBRARY_H_

#include "rttr/detail/base/core_prerequisites.h"

#include "rttr/type.h"

#include <memory>

namespace rttr
{
namespace detail
{
class library_private;
} // end namespace detail

/*!
 * The \ref library class provides a cross platform way of explicit loading shared objects
 * (`.so` on Unix based system and `.DLL` on windows).
 *
 * With a call to \ref load() the library will be loaded and with \ref unload() unloaded.
 * The explicit call to \ref unload() is not necessary. An internal ref count will trigger the unload automatically
 * on application exit. So you are free the let the \ref library instance go out of scope after loading,
 * the type data will still be available.
 *
 * After loading the library, the types of the plugin will be registered to the type system of RTTR.
 * Use therefore the macro: \ref RTTR_PLUGIN_REGISTRATION.
 * The \ref type "types" or global \ref property "properties" or  \ref method "methods"
 * can be retrieved directly from the library class via getters.
 *
 * Because the types are registered via RTTR, it is not necessary to additionally mark your
 * types for export (e.g. using `__declspec( dllexport )` on windows).
 * Furthermore, with using RTTR, it is possible to export overloaded methods (same name but different signature).
 *
 * Copying and Assignment
 * ----------------------
 * A \ref library object cannot be copied or assigned.
 *
 * Typical Usage
 * ----------------------
 * A typical usage example is the following:
 * Some cpp file in your plugin called: "MyPlugin":
 *  \code{.cpp}
 *   #include <rttr/registration>
 *   struct Foo
 *   {
 *      void set_value(int v) { value = v; }
 *      int get_value() const { return value; }
 *      int value = 0;
 *   };
 *
 *   RTTR_PLUGIN_REGISTRATION
 *   {
 *     rttr::registration::class_<Foo>("Foo")
 *             .constructor<>()
 *             .property("value", &Foo::set_value, &Foo::get_value);
 *   }
 *
 *  \endcode
 *
 * Now in your application, which loads the plugin:
 *  \code{.cpp}
 *   library lib("MyPlugin"); // file suffix is not needed, will be automatically appended
 *   lib.load();
 *   auto t = type::get_by_name("Foo");
 *   std::cout << t.get_name() << std::endl; // prints "Foo"
 *   variant var = t.create();
 *   auto value = t.set_property_value("value", var, 12);
 *   std::cout << t.get_property_value("value", var).to_string() << std::endl; // prints "12"
 *  \endcode
 */
class RTTR_API library
{
public:

    /*!
     * \brief Constructs a library instance that will load the given library \p `file_name` and
     *        an optional version number \p version.
     *        The file name is expected to be encoded in UTF-8 format.
     *
     * It is recommend to omit the file suffix, the library class will automatically look for a file
     * with the native library suffix/prefix ( e.g. `lib`, `.so` on Unix, `.dylib` on macOS and iOS, and `.dll` on Windows)
     */
    library(string_view file_name, string_view version = string_view());

     /*!
      * \brief Destroys the library instance.
      *
      * \remark This will not \ref unload() "unload" the library.
      *         However, on application exit, the library will be unloaded automatically.
      *
      * \see unload()
      */
    ~library();

    library(const library&) = delete;
    library& operator=(const library&) = delete;

     /*!
      * \brief Loads the library and returns `true`; otherwise `false.`
      *        When the library could not be loaded, check the \ref get_error_string() "error string".
      *
      * \return `true` when the library was successfully loaded; otherwise `false`.
      *
      * \see unload(), get_error_string()
      */
     bool load();

     /*!
      * \brief Unloads the library.
      *        On application exit this happens automatically, so usually you don't need to call this function.
      *        When the same library is loaded multiple times, this call will not succeed before every library instance was unloaded.
      *
      * \remark When you unload the library, make sure you don't hold any data (methods, properties or variants etc...) anymore,
      *         which was created by this library. Otherwise undefined behavior may occur (crash!).
      *
      * \return `true` when the library was successfully unloaded, otherwise `false`.
      *
      * \see load(), get_error_string()
      */
     bool unload();

     /*!
      * \brief Returns true if the library is loaded; otherwise returns false
      *
      * \return `true` when the library is loaded; otherwise `false`.
      *
      * \see load(), unload()
      */
     bool is_loaded() const RTTR_NOEXCEPT;

     /*!
      * \brief When the library was not yet \ref load() "loaded", the file name given in the constructor will be returned.
      *        After a successful call to \ref load(), \ref get_file_name() returns the fully-qualified file name of the library,
      *        including the absolute path to the library if one was given in the constructor.
      *        e.g.
      *  \code{.cpp}
      *   library lib("MyPlugin"); // loading a windows DLL relative (actual name is MyPlugin.dll)
      *   lib.load();
      *   lib.get_file_name();     // returns "MyPlugin.dll"
      *  \endcode
      *
      * \return `true` when the library is loaded; otherwise `false`.
      *
      * \see load(), unload()
      */
     string_view get_file_name() const RTTR_NOEXCEPT;

     /*!
      * \brief Returns a text string with the description of the last error that occurred.
      *        The error string will be only set when the \ref load() "loading" or \ref unload() "unloading" fails.
      *
      * \return An error string. Empty when no error occurred.
      *
      * \see load(), unload()
      */
     string_view get_error_string() const RTTR_NOEXCEPT;

     /*!
      * \brief A range of all registered type in this library.
      *
      * \return A range of all loaded types. Or empty if no \ref type "types" were loaded.
      *
      * \see type
      */
     array_range<type> get_types() const RTTR_NOEXCEPT;

     /*!
      * \brief A range of all registered global \ref property "properties" in this library.
      *
      * \return A range of all loaded properties. Or empty if no \ref property "properties" were loaded.
      *
      * \see property
      */
     array_range<property> get_global_properties() const RTTR_NOEXCEPT;

     /*!
      * \brief A range of all registered global \ref method "methods" in this library.
      *
      * \return A range of all loaded methods. Or empty if no \ref method "methods" were loaded.
      *
      * \see method
      */
     array_range<method> get_global_methods() const RTTR_NOEXCEPT;

private:
    std::shared_ptr<detail::library_private> m_pimpl;
    bool                                     m_is_loaded;
};

} // end namespace rttr

#endif  // RTTR_LIBRARY_H_
