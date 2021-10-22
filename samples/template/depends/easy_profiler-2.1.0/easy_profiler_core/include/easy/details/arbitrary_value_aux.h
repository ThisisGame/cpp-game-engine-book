/**
Lightweight profiler library for c++
Copyright(C) 2016-2019  Sergey Yagovtsev, Victor Zarubkin

Licensed under either of
    * MIT license (LICENSE.MIT or http://opensource.org/licenses/MIT)
    * Apache License, Version 2.0, (LICENSE.APACHE or http://www.apache.org/licenses/LICENSE-2.0)
at your option.

The MIT License
    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
    of the Software, and to permit persons to whom the Software is furnished
    to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
    INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
    PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
    LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
    TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE
    USE OR OTHER DEALINGS IN THE SOFTWARE.


The Apache License, Version 2.0 (the "License");
    You may not use this file except in compliance with the License.
    You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.

**/

#ifndef EASY_PROFILER_ARBITRARY_VALUE_AUX_H
#define EASY_PROFILER_ARBITRARY_VALUE_AUX_H

#include <stdint.h>
#include <type_traits>

#include <easy/details/easy_compiler_support.h>

struct ThreadStorage;

namespace profiler
{

    using vin_t = uint64_t;

    class ValueId EASY_FINAL
    {
        friend ::ThreadStorage;
        const void* m_id;

    public:

#if defined(_MSC_VER) && _MSC_VER <= 1800
        inline EASY_CONSTEXPR_FCN ValueId(const ValueId& _another) : m_id(_another.m_id) {}
        inline EASY_CONSTEXPR_FCN ValueId(ValueId&& _another) : m_id(_another.m_id) {}
#else
        inline EASY_CONSTEXPR_FCN ValueId(const ValueId&) = default;
        inline EASY_CONSTEXPR_FCN ValueId(ValueId&&) = default;
#endif

        explicit inline EASY_CONSTEXPR_FCN ValueId() : m_id(nullptr) {}
        explicit inline EASY_CONSTEXPR_FCN ValueId(const void* _member) : m_id(_member) {}

        template <class T>
        explicit inline EASY_CONSTEXPR_FCN ValueId(const T& _member) : m_id(&_member) {}

        template <class T, size_t N>
        explicit inline EASY_CONSTEXPR_FCN ValueId(const T (&_member)[N]) : m_id(_member) {}

        ValueId& operator = (const ValueId&) = delete;
        ValueId& operator = (ValueId&&) = delete;
    };

    namespace {
        template <class ... TArgs>
        inline EASY_CONSTEXPR_FCN bool subextract_value_id(TArgs...);

        template <>
        inline EASY_CONSTEXPR_FCN bool subextract_value_id<>() { return false; }

        template <class T>
        inline EASY_CONSTEXPR_FCN bool subextract_value_id(T) { return false; }

        inline EASY_CONSTEXPR_FCN ValueId subextract_value_id(ValueId _value) { return _value; }

        template <class ... TArgs>
        inline EASY_CONSTEXPR_FCN ValueId subextract_value_id(ValueId _value, TArgs...) { return _value; }

        template <class T, class ... TArgs>
        inline EASY_CONSTEXPR_FCN auto subextract_value_id(T, TArgs... _args) -> decltype(subextract_value_id(_args...)) {
            return subextract_value_id(_args...);
        }

        struct GetFirst {
            template <class T, class ... TArgs>
            static EASY_CONSTEXPR_FCN ValueId get(const T& _first, TArgs...) { return ValueId(_first); }

            template <class T, size_t N, class ... TArgs>
            static EASY_CONSTEXPR_FCN ValueId get(const T (&_first)[N], TArgs...) { return ValueId(_first); }
        };

        struct GetRest {
            template <class T, class ... TArgs>
            static EASY_CONSTEXPR_FCN ValueId get(const T&, TArgs... _args) { return subextract_value_id(_args...); }
        };
    } // end of noname namespace.

    template <class T, class ... TArgs>
    inline EASY_CONSTEXPR_FCN ValueId extract_value_id(const T& _first, TArgs... _args) {
        return ::std::conditional<::std::is_same<ValueId, decltype(subextract_value_id(_args...))>::value, GetRest, GetFirst>
            ::type::get(_first, _args...);
    }

    template <class T, size_t N, class ... TArgs>
    inline EASY_CONSTEXPR_FCN ValueId extract_value_id(const T (&_first)[N], TArgs... _args) {
        return ::std::conditional<::std::is_same<ValueId, decltype(subextract_value_id(_args...))>::value, GetRest, GetFirst>
            ::type::get(_first, _args...);
    }

} // end of namespace profiler.

#endif // EASY_PROFILER_ARBITRARY_VALUE_AUX_H
