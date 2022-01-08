/*
 This file is part of the Tweeny library.

 Copyright (c) 2016-2021 Leonardo Guilherme Lucena de Freitas
 Copyright (c) 2016 Guilherme R. Costa

 Permission is hereby granted, free of charge, to any person obtaining a copy of
 this software and associated documentation files (the "Software"), to deal in
 the Software without restriction, including without limitation the rights to
 use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 the Software, and to permit persons to whom the Software is furnished to do so,
 subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

/*
 * This file provides the easing resolution mechanism so that the library user can mix lambdas and the bundled
 * pre-defined easing functions. It shall not be used directly.
 * This file is private.
 */

#ifndef TWEENY_EASINGRESOLVE_H
#define TWEENY_EASINGRESOLVE_H

#include <tuple>
#include "easing.h"

namespace tweeny {
    namespace detail {
        using std::get;

        template<int I, typename TypeTuple, typename FunctionTuple, typename... Fs>
        struct easingresolve {
            static void impl(FunctionTuple &b, Fs... fs) {
                if (sizeof...(Fs) == 0) return;
                easingresolve<I, TypeTuple, FunctionTuple, Fs...>::impl(b, fs...);
            }
        };

        template<int I, typename TypeTuple, typename FunctionTuple, typename F1, typename... Fs>
        struct easingresolve<I, TypeTuple, FunctionTuple, F1, Fs...> {
            static void impl(FunctionTuple &b, F1 f1, Fs... fs) {
                get<I>(b) = f1;
                easingresolve<I + 1, TypeTuple, FunctionTuple, Fs...>::impl(b, fs...);
            }
        };

        template<int I, typename TypeTuple, typename FunctionTuple, typename... Fs>
        struct easingresolve<I, TypeTuple, FunctionTuple, easing::steppedEasing, Fs...> {
            typedef typename std::tuple_element<I, TypeTuple>::type ArgType;

            static void impl(FunctionTuple &b, easing::steppedEasing, Fs... fs) {
                get<I>(b) = easing::stepped.run<ArgType>;
                easingresolve<I + 1, TypeTuple, FunctionTuple, Fs...>::impl(b, fs...);
            }
        };

        template<int I, typename TypeTuple, typename FunctionTuple, typename... Fs>
        struct easingresolve<I, TypeTuple, FunctionTuple, easing::linearEasing, Fs...> {
            typedef typename std::tuple_element<I, TypeTuple>::type ArgType;

            static void impl(FunctionTuple &b, easing::linearEasing, Fs... fs) {
                get<I>(b) = easing::linear.run<ArgType>;
                easingresolve<I + 1, TypeTuple, FunctionTuple, Fs...>::impl(b, fs...);
            }
        };
        template<int I, typename TypeTuple, typename FunctionTuple, typename... Fs>
        struct easingresolve<I, TypeTuple, FunctionTuple, easing::defaultEasing, Fs...> {
            typedef typename std::tuple_element<I, TypeTuple>::type ArgType;

            static void impl(FunctionTuple &b, easing::defaultEasing, Fs... fs) {
                get<I>(b) = easing::def.run<ArgType>;
                easingresolve<I + 1, TypeTuple, FunctionTuple, Fs...>::impl(b, fs...);
            }
        };

        #define DECLARE_EASING_RESOLVE(__EASING_TYPE__) \
        template <int I, typename TypeTuple, typename FunctionTuple, typename... Fs> \
        struct easingresolve<I, TypeTuple, FunctionTuple, easing::__EASING_TYPE__ ## InEasing, Fs...> { \
          typedef typename std::tuple_element<I, TypeTuple>::type ArgType; \
          static void impl(FunctionTuple & b, decltype(easing::__EASING_TYPE__ ## In), Fs... fs) { \
            get<I>(b) = easing::__EASING_TYPE__ ## In.run<ArgType>; \
            easingresolve<I+1, TypeTuple, FunctionTuple, Fs...>::impl(b, fs...); \
          } \
        }; \
        \
        template <int I, typename TypeTuple, typename FunctionTuple, typename... Fs> \
          struct easingresolve<I, TypeTuple, FunctionTuple, easing::__EASING_TYPE__ ## OutEasing, Fs...> { \
          typedef typename std::tuple_element<I, TypeTuple>::type ArgType; \
          static void impl(FunctionTuple & b, decltype(easing::__EASING_TYPE__ ## Out), Fs... fs) { \
            get<I>(b) = easing::__EASING_TYPE__ ## Out.run<ArgType>; \
            easingresolve<I+1, TypeTuple, FunctionTuple, Fs...>::impl(b, fs...); \
          } \
        }; \
        \
        template <int I, typename TypeTuple, typename FunctionTuple, typename... Fs> \
          struct easingresolve<I, TypeTuple, FunctionTuple, easing::__EASING_TYPE__ ## InOutEasing, Fs...> { \
          typedef typename std::tuple_element<I, TypeTuple>::type ArgType; \
          static void impl(FunctionTuple & b, decltype(easing::__EASING_TYPE__ ## InOut), Fs... fs) { \
            get<I>(b) = easing::__EASING_TYPE__ ## InOut.run<ArgType>; \
            easingresolve<I+1, TypeTuple, FunctionTuple, Fs...>::impl(b, fs...); \
          } \
        }

        DECLARE_EASING_RESOLVE(quadratic);
        DECLARE_EASING_RESOLVE(cubic);
        DECLARE_EASING_RESOLVE(quartic);
        DECLARE_EASING_RESOLVE(quintic);
        DECLARE_EASING_RESOLVE(sinusoidal);
        DECLARE_EASING_RESOLVE(exponential);
        DECLARE_EASING_RESOLVE(circular);
        DECLARE_EASING_RESOLVE(bounce);
        DECLARE_EASING_RESOLVE(elastic);
        DECLARE_EASING_RESOLVE(back);
    }
}

#endif //TWEENY_EASINGRESOLVE_H
