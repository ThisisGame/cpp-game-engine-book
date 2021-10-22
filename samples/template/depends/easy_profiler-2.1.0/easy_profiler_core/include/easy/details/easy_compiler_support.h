/************************************************************************
* file name         : easy_compiler_support.h
* ----------------- :
* creation time     : 2016/09/22
* authors           : Victor Zarubkin, Sergey Yagovtsev
* emails            : v.s.zarubkin@gmail.com, yse.sey@gmail.com
* ----------------- :
* description       : This file contains auxiliary profiler macros for different compiler support.
* ----------------- :
* license           : Lightweight profiler library for c++
*                   : Copyright(C) 2016-2019  Sergey Yagovtsev, Victor Zarubkin
*                   :
*                   : Licensed under either of
*                   :     * MIT license (LICENSE.MIT or http://opensource.org/licenses/MIT)
*                   :     * Apache License, Version 2.0, (LICENSE.APACHE or http://www.apache.org/licenses/LICENSE-2.0)
*                   : at your option.
*                   :
*                   : The MIT License
*                   :
*                   : Permission is hereby granted, free of charge, to any person obtaining a copy
*                   : of this software and associated documentation files (the "Software"), to deal
*                   : in the Software without restriction, including without limitation the rights 
*                   : to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies 
*                   : of the Software, and to permit persons to whom the Software is furnished 
*                   : to do so, subject to the following conditions:
*                   : 
*                   : The above copyright notice and this permission notice shall be included in all 
*                   : copies or substantial portions of the Software.
*                   : 
*                   : THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
*                   : INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR 
*                   : PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE 
*                   : LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, 
*                   : TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE 
*                   : USE OR OTHER DEALINGS IN THE SOFTWARE.
*                   : 
*                   : The Apache License, Version 2.0 (the "License")
*                   :
*                   : You may not use this file except in compliance with the License.
*                   : You may obtain a copy of the License at
*                   :
*                   : http://www.apache.org/licenses/LICENSE-2.0
*                   :
*                   : Unless required by applicable law or agreed to in writing, software
*                   : distributed under the License is distributed on an "AS IS" BASIS,
*                   : WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*                   : See the License for the specific language governing permissions and
*                   : limitations under the License.
************************************************************************/

#ifndef EASY_PROFILER_COMPILER_SUPPORT_H
#define EASY_PROFILER_COMPILER_SUPPORT_H

#include <cstddef>

#if defined(_WIN32) && !defined(EASY_PROFILER_STATIC)
// Visual Studio and MinGW
# ifdef _BUILD_PROFILER
#  define PROFILER_API __declspec(dllexport)
# else
#  define PROFILER_API __declspec(dllimport)
# endif
#endif

#ifdef __cplusplus
#  if __cplusplus >= 201703L
#    define EASY_STD 17
#  elif __cplusplus >= 201402L
#    define EASY_STD 14
#  else
#    define EASY_STD 11
#  endif
#else
#  define EASY_STD 11
#endif


#if defined(_MSC_VER)
//////////////////////////////////////////////////////////////////////////
// Visual Studio

# if defined(EASY_OPTION_PRETTY_PRINT_FUNCTIONS) && EASY_OPTION_PRETTY_PRINT_FUNCTIONS != 0
#  define EASY_FUNC_NAME __FUNCSIG__
# else
#  define EASY_FUNC_NAME __FUNCTION__
# endif

# if _MSC_VER <= 1800
// There is no support for C++11 thread_local keyword prior to Visual Studio 2015. Use __declspec(thread) instead.
// There is also no support for C++11 magic statics feature :( So it becomes slightly harder to initialize static vars - additional "if" for each profiler block.
#  define EASY_THREAD_LOCAL __declspec(thread)
#  define EASY_LOCAL_STATIC_PTR(VarType, VarName, VarInitializer)\
                                            __declspec(thread) static VarType VarName = 0;\
                                            if (!VarName)\
                                                VarName = VarInitializer

// No constexpr support before Visual Studio 2015
#  define EASY_CONSTEXPR const
#  define EASY_STATIC_CONSTEXPR static const
#  define EASY_CONSTEXPR_FCN

// No noexcept support before Visual Studio 2015
#  define EASY_NOEXCEPT throw()

// No alignof support before Visual Studio 2015
#  define EASY_ALIGNOF(x) __alignof(x)
# endif

# if EASY_STD > 11 && _MSC_VER >= 1900
#  define EASY_LAMBDA_MOVE_CAPTURE
# endif

# define EASY_FORCE_INLINE __forceinline

#elif defined(__clang__)
//////////////////////////////////////////////////////////////////////////
// Clang Compiler

# define EASY_COMPILER_VERSION (__clang_major__ * 10 + __clang_minor__)

# if EASY_COMPILER_VERSION < 33 || (defined(__APPLE_CC__) && __APPLE_CC__ < 8000)
// There is no support for C++11 thread_local keyword prior to Clang v3.3 and Apple LLVM clang 8.0. Use __thread instead.
#  define EASY_THREAD_LOCAL __thread
# endif

# if EASY_COMPILER_VERSION < 31
// No constexpr support before Clang v3.1
#  define EASY_CONSTEXPR const
#  define EASY_STATIC_CONSTEXPR static const
#  define EASY_CONSTEXPR_FCN
# endif

# if EASY_COMPILER_VERSION < 29
// There is no support for C++11 magic statics feature prior to clang 2.9. It becomes slightly harder to initialize static vars - additional "if" for each profiler block.
#  define EASY_LOCAL_STATIC_PTR(VarType, VarName, VarInitializer)\
                                            EASY_THREAD_LOCAL static VarType VarName = 0;\
                                            if (!VarName)\
                                                VarName = VarInitializer

// There is no support for C++11 final keyword prior to Clang v2.9
#  define EASY_FINAL 
# endif

# if EASY_STD > 11 && EASY_COMPILER_VERSION >= 34
#  define EASY_LAMBDA_MOVE_CAPTURE
# endif

# define EASY_FORCE_INLINE inline __attribute__((always_inline))
# undef EASY_COMPILER_VERSION

#elif defined(__GNUC__)
//////////////////////////////////////////////////////////////////////////
// GNU Compiler

# define EASY_COMPILER_VERSION (__GNUC__ * 10 + __GNUC_MINOR__)

# if EASY_COMPILER_VERSION < 48
// There is no support for C++11 thread_local keyword prior to gcc 4.8. Use __thread instead.
#  define EASY_THREAD_LOCAL __thread
# endif

# if EASY_COMPILER_VERSION < 46
// No constexpr support before GCC v4.6
#  define EASY_CONSTEXPR const
#  define EASY_STATIC_CONSTEXPR static const
#  define EASY_CONSTEXPR_FCN

// No noexcept support before GCC v4.6
#  define EASY_NOEXCEPT throw()
# endif

# if EASY_COMPILER_VERSION < 43
// There is no support for C++11 magic statics feature prior to gcc 4.3. It becomes slightly harder to initialize static vars - additional "if" for each profiler block.
#  define EASY_LOCAL_STATIC_PTR(VarType, VarName, VarInitializer)\
                                            EASY_THREAD_LOCAL static VarType VarName = 0;\
                                            if (!VarName)\
                                                VarName = VarInitializer
# endif

# if EASY_COMPILER_VERSION < 47
// There is no support for C++11 final keyword prior to gcc 4.7
#  define EASY_FINAL 
# endif

# if EASY_STD > 11 && EASY_COMPILER_VERSION >= 49
#  define EASY_LAMBDA_MOVE_CAPTURE
# endif

# define EASY_FORCE_INLINE inline __attribute__((always_inline))
# undef EASY_COMPILER_VERSION

#else
//////////////////////////////////////////////////////////////////////////
// TODO: Add other compilers support

static_assert(false, "EasyProfiler is not configured for using your compiler type. Please, contact developers.");
#endif
// END
//////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////////
// Default values

#ifndef EASY_FUNC_NAME
# if defined(EASY_OPTION_PRETTY_PRINT_FUNCTIONS) && EASY_OPTION_PRETTY_PRINT_FUNCTIONS != 0
#  define EASY_FUNC_NAME __PRETTY_FUNCTION__
# else
#  define EASY_FUNC_NAME __func__
# endif
#endif

#ifndef EASY_THREAD_LOCAL
# define EASY_THREAD_LOCAL thread_local
# define EASY_CXX11_TLS_AVAILABLE
#endif

#ifndef EASY_LOCAL_STATIC_PTR
# define EASY_LOCAL_STATIC_PTR(VarType, VarName, VarInitializer) static VarType VarName = VarInitializer
# define EASY_MAGIC_STATIC_AVAILABLE
#endif

#ifndef EASY_FINAL
# define EASY_FINAL final
#endif

#ifndef EASY_FORCE_INLINE
# define EASY_FORCE_INLINE inline
#endif

#ifndef EASY_CONSTEXPR
# define EASY_CONSTEXPR constexpr
# define EASY_STATIC_CONSTEXPR static constexpr
# define EASY_CONSTEXPR_FCN constexpr
# define EASY_CONSTEXPR_AVAILABLE
#endif

#ifndef EASY_NOEXCEPT
# define EASY_NOEXCEPT noexcept
# define EASY_NOEXCEPT_AVAILABLE
#endif

#ifndef EASY_ALIGNOF
# define EASY_ALIGNOF(x) alignof(x)
#endif

#ifndef PROFILER_API
# define PROFILER_API
#endif

//////////////////////////////////////////////////////////////////////////

#endif // EASY_PROFILER_COMPILER_SUPPORT_H
