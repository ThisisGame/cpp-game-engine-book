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

#ifndef RTTR_CORE_PREREQUISITES_H_
#define RTTR_CORE_PREREQUISITES_H_

#include "rttr/detail/base/version.h"

namespace rttr
{

#define RTTR_PLATFORM_WINDOWS 1
#define RTTR_PLATFORM_LINUX 2
#define RTTR_PLATFORM_APPLE 3

#define RTTR_COMPILER_MSVC 1
#define RTTR_COMPILER_GNUC 2
#define RTTR_COMPILER_CLANG 3
#define RTTR_COMPILER_APPLECLANG 4

#define RTTR_ENDIAN_LITTLE 1
#define RTTR_ENDIAN_BIG 2

#define RTTR_ARCH_32 1
#define RTTR_ARCH_64 2

/////////////////////////////////////////////////////////////////////////////////////////
// Platform
/////////////////////////////////////////////////////////////////////////////////////////
#if defined( __WIN32__ ) || defined( _WIN32 )
#   define RTTR_PLATFORM RTTR_PLATFORM_WINDOWS
#elif defined( __APPLE_CC__)
#   define RTTR_PLATFORM RTTR_PLATFORM_APPLE
#else
#   define RTTR_PLATFORM RTTR_PLATFORM_LINUX
#endif

/////////////////////////////////////////////////////////////////////////////////////////
// Compiler
/////////////////////////////////////////////////////////////////////////////////////////
#if defined( __clang__ )

#if defined __apple_build_version__
#   define RTTR_COMPILER RTTR_COMPILER_APPLECLANG
#else
#   define RTTR_COMPILER RTTR_COMPILER_CLANG
#endif
#   define RTTR_COMP_VER (((__clang_major__)*100) + \
                         (__clang_minor__*10) + \
                         __clang_patchlevel__)
#elif defined( __GNUC__ )
#   define RTTR_COMPILER RTTR_COMPILER_GNUC
#   define RTTR_COMP_VER (((__GNUC__)*100) + \
                         (__GNUC_MINOR__*10) + \
                          __GNUC_PATCHLEVEL__)
#elif defined( _MSC_VER )
#   define RTTR_COMPILER RTTR_COMPILER_MSVC
#   define RTTR_COMP_VER _MSC_VER
#else
#   error "No known compiler. Abort! Abort!"
#endif


/////////////////////////////////////////////////////////////////////////////////////////
// Architecture
/////////////////////////////////////////////////////////////////////////////////////////
#if defined(__x86_64__) || defined(_M_X64) || defined(__powerpc64__) || defined(__alpha__) ||\
    defined(__ia64__) || defined(__s390__) || defined(__s390x__)
#   define RTTR_ARCH_TYPE RTTR_ARCH_64
#else
#   define RTTR_ARCH_TYPE RTTR_ARCH_32
#endif

#if RTTR_COMPILER == RTTR_COMPILER_MSVC
#   define RTTR_INLINE          inline
#   define RTTR_FORCE_INLINE    __forceinline
#elif RTTR_COMPILER == RTTR_COMPILER_GNUC
#   define RTTR_INLINE          inline
#   define RTTR_FORCE_INLINE    inline  __attribute__((always_inline))
#elif RTTR_COMPILER == RTTR_COMPILER_CLANG || RTTR_COMPILER == RTTR_COMPILER_APPLECLANG
#   define RTTR_INLINE          inline
#   define RTTR_FORCE_INLINE    inline  __attribute__((always_inline))
#else
#   define RTTR_INLINE          inline
#   define RTTR_FORCE_INLINE    inline // no force inline for other platforms possible
#endif

/////////////////////////////////////////////////////////////////////////////////////////
// Compiler specific cmds for export and import code to DLL
/////////////////////////////////////////////////////////////////////////////////////////
#if RTTR_COMPILER == RTTR_COMPILER_MSVC || __MINGW32__ || __CYGWIN__
#     define RTTR_HELPER_DLL_IMPORT __declspec( dllimport )
#     define RTTR_HELPER_DLL_EXPORT __declspec( dllexport )
#     define RTTR_HELPER_DLL_LOCAL
#elif RTTR_COMPILER == RTTR_COMPILER_GNUC
#   if RTTR_COMP_VER >= 400
#       define RTTR_HELPER_DLL_IMPORT __attribute__ ((visibility ("default")))
#       define RTTR_HELPER_DLL_EXPORT __attribute__ ((visibility ("default")))
#       define RTTR_HELPER_DLL_LOCAL  __attribute__ ((visibility ("hidden")))
#   else
#       define RTTR_HELPER_DLL_IMPORT
#       define RTTR_HELPER_DLL_EXPORT
#       define RTTR_HELPER_DLL_LOCAL
#   endif
#elif RTTR_COMPILER == RTTR_COMPILER_CLANG || RTTR_COMPILER == RTTR_COMPILER_APPLECLANG
#       define RTTR_HELPER_DLL_IMPORT __attribute__ ((visibility ("default")))
#       define RTTR_HELPER_DLL_EXPORT __attribute__ ((visibility ("default")))
#       define RTTR_HELPER_DLL_LOCAL  __attribute__ ((visibility ("hidden")))
#else
#   error "Do not know how to export classes for this platform"
#endif

#ifdef RTTR_DLL // compiled as a DLL
#   ifdef RTTR_DLL_EXPORTS // defined if we are building the DLL
#       define RTTR_API RTTR_HELPER_DLL_EXPORT
#   else
#       define RTTR_API RTTR_HELPER_DLL_IMPORT
#   endif
#   define RTTR_LOCAL RTTR_HELPER_DLL_LOCAL
#else // it's a static lib.
#   define RTTR_API
#   define RTTR_LOCAL
#endif

/////////////////////////////////////////////////////////////////////////////////////////
// Macros for some C++11 features, which are not available on every compiler
/////////////////////////////////////////////////////////////////////////////////////////


#if RTTR_COMPILER == RTTR_COMPILER_MSVC
#   if RTTR_COMP_VER <= 1800
#       define RTTR_NO_CXX11_NOEXCEPT
#       define RTTR_NO_CXX17_NOEXCEPT_FUNC_TYPE
#   endif
#   if !defined(__cpp_constexpr) || (__cpp_constexpr < 201304)
#       define RTTR_NO_CXX11_CONSTEXPR
#       define RTTR_NO_CXX14_CONSTEXPR
#   endif
#   if !defined(_MSVC_LANG) || (_MSVC_LANG < 201703L)
#       define RTTR_NO_CXX17_NOEXCEPT_FUNC_TYPE
#   endif
#endif

#if RTTR_COMPILER == RTTR_COMPILER_GNUC ||  RTTR_COMPILER == RTTR_COMPILER_CLANG || RTTR_COMPILER == RTTR_COMPILER_APPLECLANG
#   if !defined(__cpp_constexpr) || (__cpp_constexpr < 201304)
#       define RTTR_NO_CXX14_CONSTEXPR
#   endif
#   if !defined(cpp_noexcept)
#       define RTTR_NO_CXX11_NOEXCEPT
#   endif
#   if !defined(__cpp_noexcept_function_type) || (__cpp_noexcept_function_type < 201510)
#       define RTTR_NO_CXX17_NOEXCEPT_FUNC_TYPE
#   endif
#endif

/////////////////////////////////////////////////////////////////////////////////////////

#if defined(RTTR_NO_CXX11_CONSTEXPR)
#   define RTTR_CONSTEXPR
#   define RTTR_CONSTEXPR_OR_CONST const
#else
#   define RTTR_CONSTEXPR constexpr
#   define RTTR_CONSTEXPR_OR_CONST constexpr
#endif


#if defined(RTTR_NO_CXX14_CONSTEXPR)
#   define RTTR_CXX14_CONSTEXPR
#else
#   define RTTR_CXX14_CONSTEXPR constexpr
#endif

#ifdef RTTR_NO_CXX11_NOEXCEPT
#   define RTTR_NOEXCEPT
#   define RTTR_NOEXCEPT_OR_NOTHROW throw()
#else
#   define RTTR_NOEXCEPT noexcept
#   define RTTR_NOEXCEPT_OR_NOTHROW noexcept
#endif


#define RTTR_STATIC_CONSTEXPR static RTTR_CONSTEXPR_OR_CONST

/////////////////////////////////////////////////////////////////////////////////////////
// deprecated macro
#if RTTR_COMPILER == RTTR_COMPILER_GNUC || RTTR_COMPILER == RTTR_COMPILER_CLANG || RTTR_COMPILER == RTTR_COMPILER_APPLECLANG
#   define RTTR_DEPRECATED __attribute__ ((deprecated))
#   define RTTR_DEPRECATED_WITH_MSG(msg) __attribute__ ((deprecated(msg)))
#elif RTTR_COMPILER == RTTR_COMPILER_MSVC
#   define RTTR_DEPRECATED __declspec(deprecated)
#   define RTTR_DEPRECATED_WITH_MSG(msg) __declspec(deprecated(msg))
#else
#   pragma message("WARNING: You need to implement DEPRECATED for this compiler")
#   define RTTR_DEPRECATED(func) func
#   define RTTR_DEPRECATED_MSG(func) func
#endif

/////////////////////////////////////////////////////////////////////////////////////////
// Disable some MSVC compile warnings
/////////////////////////////////////////////////////////////////////////////////////////
#if  RTTR_COMPILER == RTTR_COMPILER_MSVC
// Save warnings state
#   pragma warning (push)
// Turn off warnings generated by long std templates
// This warns about truncation to 255 characters in debug/browse info
#   pragma warning (disable : 4786)

// Turn off warnings generated by long std templates
// This warns about truncation to 255 characters in debug/browse info
#   pragma warning (disable : 4503)

// disable: "<type> needs to have dll-interface to be used by clients'
// Happens on STL member variables which are not public therefore is ok
#   pragma warning (disable : 4251)

// disable: "non dll-interface class used as base for dll-interface class"
// Happens when deriving from Singleton because bug in compiler ignores
// template export
#   pragma warning (disable : 4275)

// disable: "C++ Exception Specification ignored"
// This is because MSVC 6 did not implement all the C++ exception
// specifications in the ANSI C++ draft.
#   pragma warning( disable : 4290 )

// disable: "no suitable definition provided for explicit template
// instantiation request" Occurs in VC7 for no justifiable reason on all
// #includes of Singleton
#   pragma warning( disable: 4661)

// disable: "unreferenced formal parameter"
// Many versions of VC have bugs which generate this error in cases where they shouldn't
#   pragma warning (disable : 4100)
#endif

#if RTTR_COMPILER == RTTR_COMPILER_GNUC
#   define RTTR_BEGIN_DISABLE_DEPRECATED_WARNING        _Pragma ("GCC diagnostic push") \
                                                        _Pragma ("GCC diagnostic ignored \"-Wdeprecated-declarations\"")
#   define RTTR_END_DISABLE_DEPRECATED_WARNING          _Pragma ("GCC diagnostic pop")

#   define RTTR_BEGIN_DISABLE_CONDITIONAL_EXPR_WARNING
#   define RTTR_END_DISABLE_CONDITIONAL_EXPR_WARNING
#if RTTR_COMP_VER >= 700

    #define RTTR_BEGIN_DISABLE_EXCEPT_TYPE_WARNING      _Pragma ("GCC diagnostic push") \
                                                        _Pragma ("GCC diagnostic ignored \"-Wnoexcept-type\"")
    #define RTTR_END_DISABLE_EXCEPT_TYPE_WARNING        _Pragma ("GCC diagnostic pop")
#else

    #define RTTR_BEGIN_DISABLE_EXCEPT_TYPE_WARNING
    #define RTTR_END_DISABLE_EXCEPT_TYPE_WARNING

#endif

#   define RTTR_DECLARE_PLUGIN_CTOR       __attribute__((constructor))
#   define RTTR_DECLARE_PLUGIN_DTOR       __attribute__((destructor))

#elif RTTR_COMPILER == RTTR_COMPILER_CLANG || RTTR_COMPILER == RTTR_COMPILER_APPLECLANG

#if defined(__has_warning) && __has_warning("-Wdeprecated-declarations")
#   define RTTR_BEGIN_DISABLE_DEPRECATED_WARNING        _Pragma ("clang diagnostic push") \
                                                        _Pragma ("clang diagnostic ignored \"-Wdeprecated-declarations\"")
#   define RTTR_END_DISABLE_DEPRECATED_WARNING          _Pragma ("clang diagnostic pop")
#else
#   define RTTR_BEGIN_DISABLE_DEPRECATED_WARNING
#   define RTTR_END_DISABLE_DEPRECATED_WARNING
#endif

#   define RTTR_BEGIN_DISABLE_CONDITIONAL_EXPR_WARNING
#   define RTTR_END_DISABLE_CONDITIONAL_EXPR_WARNING

#if defined(__has_warning) && __has_warning("-Wnoexcept-type")
#       define RTTR_BEGIN_DISABLE_EXCEPT_TYPE_WARNING   _Pragma ("clang diagnostic push") \
                                                        _Pragma ("clang diagnostic ignored \"-Wnoexcept-type\"")
#       define RTTR_END_DISABLE_EXCEPT_TYPE_WARNING     _Pragma ("clang diagnostic pop")
#else
#       define RTTR_BEGIN_DISABLE_EXCEPT_TYPE_WARNING
#       define RTTR_END_DISABLE_EXCEPT_TYPE_WARNING
#endif

#   define RTTR_DECLARE_PLUGIN_CTOR        __attribute__((__constructor__))
#   define RTTR_DECLARE_PLUGIN_DTOR        __attribute__((__destructor__))

#elif RTTR_COMPILER == RTTR_COMPILER_MSVC
#   define RTTR_BEGIN_DISABLE_DEPRECATED_WARNING        __pragma( warning( push )) \
                                                        __pragma( warning( disable: 4996))
#   define RTTR_END_DISABLE_DEPRECATED_WARNING          __pragma( warning( pop ))


#   define RTTR_BEGIN_DISABLE_CONDITIONAL_EXPR_WARNING  __pragma( warning( push )) \
                                                        __pragma( warning( disable: 4127))
#   define RTTR_END_DISABLE_CONDITIONAL_EXPR_WARNING    __pragma( warning( pop ))

#   define RTTR_BEGIN_DISABLE_EXCEPT_TYPE_WARNING
#   define RTTR_END_DISABLE_EXCEPT_TYPE_WARNING
#   define RTTR_DECLARE_PLUGIN_CTOR
#   define RTTR_DECLARE_PLUGIN_DTOR

#else
#   pragma message("WARNING: unknown compiler, don't know how to disable deprecated warnings")
#endif

} // end namespace rttr

#endif // RTTR_CORE_PREREQUISITES_H_
