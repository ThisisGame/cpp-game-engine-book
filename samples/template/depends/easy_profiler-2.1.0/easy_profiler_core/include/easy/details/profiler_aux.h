/************************************************************************
* file name         : profiler_aux.h
* ----------------- :
* creation time     : 2016/06/11
* author            : Victor Zarubkin
* email             : v.s.zarubkin@gmail.com
* ----------------- :
* description       : This file contains auxiliary profiler macros and funcitons.
* ----------------- :
* change log        : * 2016/06/11 Victor Zarubkin: Moved sources from profiler.h
*                   :
*                   : *
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

#ifndef EASY_PROFILER_AUX_H
#define EASY_PROFILER_AUX_H

#include <stdint.h>

#include <easy/details/easy_compiler_support.h>
#include <easy/details/profiler_colors.h>
#include <easy/details/profiler_in_use.h>

//////////////////////////////////////////////////////////////////////////

namespace profiler {

    enum EasyBlockStatus : uint8_t {
        OFF = 0, ///< The block is OFF
        ON = 1, ///< The block is ON (but if it's parent block is off recursively then this block will be off too)
        FORCE_ON = ON | 2, ///< The block is ALWAYS ON (even if it's parent has turned off all children)
        OFF_RECURSIVE = 4, ///< The block is OFF and all of it's children by call-stack are also OFF.
        ON_WITHOUT_CHILDREN = ON | OFF_RECURSIVE, ///< The block is ON but all of it's children are OFF.
        FORCE_ON_WITHOUT_CHILDREN = FORCE_ON | OFF_RECURSIVE, ///< The block is ALWAYS ON but all of it's children are OFF.
    };

}

//////////////////////////////////////////////////////////////////////////

# define EASY_STRINGIFY(a) #a
# define EASY_STRINGIFICATION(a) EASY_STRINGIFY(a)
# define EASY_TOKEN_JOIN(x, y) x ## y
# define EASY_TOKEN_CONCATENATE(x, y) EASY_TOKEN_JOIN(x, y)
# define EASY_UNIQUE_BLOCK(x) EASY_TOKEN_CONCATENATE(unique_profiler_mark_name_, x)
# define EASY_UNIQUE_FRAME_COUNTER(x) EASY_TOKEN_CONCATENATE(unique_profiler_frame_mark_name_, x)
# define EASY_UNIQUE_DESC(x) EASY_TOKEN_CONCATENATE(unique_profiler_descriptor_, x)

#ifdef USING_EASY_PROFILER

#include <string>
#include <type_traits>

namespace profiler
{
    template <const bool> struct NameSwitch;

    class ForceConstStr EASY_FINAL
    {
        friend NameSwitch<true>;
        friend NameSwitch<false>;

        const char* c_str;

    public:

        ForceConstStr() = delete;
        ForceConstStr(const ForceConstStr&) = delete;
        ForceConstStr(ForceConstStr&&) = delete;
        ForceConstStr& operator = (const ForceConstStr&) = delete;
        ForceConstStr& operator = (ForceConstStr&&) = delete;

        explicit EASY_CONSTEXPR_FCN ForceConstStr(const char* _str) : c_str(_str) {}
        explicit ForceConstStr(const ::std::string& _str) : c_str(_str.c_str()) {}
    };

    template <const bool IS_REF> struct NameSwitch EASY_FINAL
    {
        static const char* runtime_name(const char* name) { return name; }
        static const char* runtime_name(const ::std::string& name) { return name.c_str(); }
        static EASY_CONSTEXPR_FCN const char* runtime_name(const ForceConstStr&) { return ""; }

        template <class T>
        static EASY_CONSTEXPR_FCN const char* compiletime_name(const T&, const char* autoGeneratedName) { return autoGeneratedName; }
        static EASY_CONSTEXPR_FCN const char* compiletime_name(const char*, const char* autoGeneratedName) { return autoGeneratedName; }
        static EASY_CONSTEXPR_FCN const char* compiletime_name(const ForceConstStr& name, const char*) { return name.c_str; }
    };

    template <> struct NameSwitch<true> EASY_FINAL
    {
        static EASY_CONSTEXPR_FCN const char* runtime_name(const char*) { return ""; }
        static EASY_CONSTEXPR_FCN const char* runtime_name(const ForceConstStr&) { return ""; }
        static const char* runtime_name(const ::std::string& name) { return name.c_str(); }

        template <class T>
        static EASY_CONSTEXPR_FCN const char* compiletime_name(const T&, const char* autoGeneratedName) { return autoGeneratedName; }
        static EASY_CONSTEXPR_FCN const char* compiletime_name(const char* name, const char*) { return name; }
        static EASY_CONSTEXPR_FCN const char* compiletime_name(const ForceConstStr& name, const char*) { return name.c_str; }
    };

    //***********************************************

    template <class ... TArgs>
    inline EASY_CONSTEXPR_FCN color_t extract_color(TArgs...);

    template <>
    inline EASY_CONSTEXPR_FCN color_t extract_color<>() {
        return ::profiler::colors::Default;
    }

    template <class T>
    inline EASY_CONSTEXPR_FCN color_t extract_color(T) {
        return ::profiler::colors::Default;
    }

    template <>
    inline EASY_CONSTEXPR_FCN color_t extract_color<color_t>(color_t _color) {
        return _color;
    }

    template <class ... TArgs>
    inline EASY_CONSTEXPR_FCN color_t extract_color(color_t _color, TArgs...) {
        return _color;
    }

    template <class T, class ... TArgs>
    inline EASY_CONSTEXPR_FCN color_t extract_color(T, TArgs... _args) {
        return extract_color(_args...);
    }

    //***********************************************

    template <class ... TArgs>
    inline EASY_CONSTEXPR_FCN EasyBlockStatus extract_enable_flag(TArgs...);

    template <>
    inline EASY_CONSTEXPR_FCN EasyBlockStatus extract_enable_flag<>() {
        return ::profiler::ON;
    }

    template <class T>
    inline EASY_CONSTEXPR_FCN EasyBlockStatus extract_enable_flag(T) {
        return ::profiler::ON;
    }

    template <>
    inline EASY_CONSTEXPR_FCN EasyBlockStatus extract_enable_flag(EasyBlockStatus _flag) {
        return _flag;
    }

    template <class ... TArgs>
    inline EASY_CONSTEXPR_FCN EasyBlockStatus extract_enable_flag(EasyBlockStatus _flag, TArgs...) {
        return _flag;
    }

    template <class T, class ... TArgs>
    inline EASY_CONSTEXPR_FCN EasyBlockStatus extract_enable_flag(T, TArgs... _args) {
        return extract_enable_flag(_args...);
    }

    //***********************************************

} // END of namespace profiler.

# define EASY_UNIQUE_LINE_ID __FILE__ ":" EASY_STRINGIFICATION(__LINE__)
# define EASY_COMPILETIME_NAME(name) ::profiler::NameSwitch<::std::is_reference<decltype(name)>::value>::compiletime_name(name, EASY_UNIQUE_LINE_ID)
# define EASY_RUNTIME_NAME(name) ::profiler::NameSwitch<::std::is_reference<decltype(name)>::value>::runtime_name(name)
# define EASY_CONST_NAME(name) ::profiler::ForceConstStr(name)

#else

# define EASY_CONST_NAME(name) 

#endif // USING_EASY_PROFILER

//////////////////////////////////////////////////////////////////////////

#endif // EASY_PROFILER_AUX_H
