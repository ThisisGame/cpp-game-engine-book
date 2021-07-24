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
 * This file provides useful typedefs and traits for a tween.
 */

#ifndef TWEENY_TWEENTRAITS_H
#define TWEENY_TWEENTRAITS_H

#include <tuple>
#include <functional>
#include <type_traits>

namespace tweeny {
    template<typename T, typename... Ts> class tween;

    namespace detail {

      template<typename... Ts> struct equal {};
      template<typename T> struct equal<T> { enum { value = true }; };
      template <typename T, typename U, typename... Ts> struct equal<T, U, Ts...> {
        enum { value = std::is_same<T, U>::value && equal<T, Ts...>::value && equal<U, Ts...>::value };
      };

        template<typename T, typename...> struct first { typedef T type; };

        template<bool equal, typename... Ts>
        struct valuetype { };

        template<typename... Ts>
        struct valuetype<false, Ts...> {
            typedef std::tuple<Ts...> type;
        };

        template<typename... Ts>
        struct valuetype<true, Ts...> {
            typedef std::array<typename first<Ts...>::type, sizeof...(Ts)> type;
        };

        template<typename... Ts>
        struct tweentraits {
            typedef std::tuple<std::function<Ts(float, Ts, Ts)>...> easingCollection;
            typedef std::function<bool(tween<Ts...> &, Ts...)> callbackType;
            typedef std::function<bool(tween<Ts...> &)> noValuesCallbackType;
            typedef std::function<bool(Ts...)> noTweenCallbackType;
            typedef typename valuetype<equal<Ts...>::value, Ts...>::type valuesType;
            typedef std::array<uint16_t, sizeof...(Ts)> durationsArrayType;
            typedef tween<Ts...> type;
        };
    }
}

#endif //TWEENY_TWEENTRAITS_H
