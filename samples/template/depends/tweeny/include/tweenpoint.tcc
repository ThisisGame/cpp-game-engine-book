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
 * This file implements the tweenpoint class
 */

#ifndef TWEENY_TWEENPOINT_TCC
#define TWEENY_TWEENPOINT_TCC

#include <algorithm>
#include <type_traits>

#include "tweenpoint.h"
#include "tweentraits.h"
#include "easing.h"
#include "easingresolve.h"
#include "int2type.h"

namespace tweeny {
    namespace detail {
        template<typename TypeTupleT, typename EasingCollectionT, typename EasingT, size_t I> void easingfill(EasingCollectionT & f, EasingT easing, int2type<I>) {
            easingresolve<I, TypeTupleT, EasingCollectionT, EasingT>::impl(f, easing);
            easingfill<TypeTupleT, EasingCollectionT, EasingT>(f, easing, int2type<I - 1>{ });
        }

        template<typename TypeTupleT, typename EasingCollectionT, typename EasingT> void easingfill(EasingCollectionT & f, EasingT easing, int2type<0>) {
            easingresolve<0, TypeTupleT, EasingCollectionT, EasingT>::impl(f, easing);
        }


        template <class ...T>
        struct are_same;

        template <class A, class B, class ...T>
        struct are_same<A, B, T...>
        {
            static const bool value = std::is_same<A, B>::value && are_same<B, T...>::value;
        };

        template <class A>
        struct are_same<A>
        {
            static const bool value = true;
        };


        template<typename... Ts>
        inline tweenpoint<Ts...>::tweenpoint(Ts... vs) : values{vs...} {
            during(static_cast<uint16_t>(0));
            via(easing::def);
        }

        template<typename... Ts>
        template<typename D>
        inline void tweenpoint<Ts...>::during(D milis) {
            for (uint16_t & t : durations) { t = static_cast<uint16_t>(milis); }
        }

        template<typename... Ts>
        template<typename... Ds>
        inline void tweenpoint<Ts...>::during(Ds... milis) {
            static_assert(sizeof...(Ds) == sizeof...(Ts),
                          "Amount of durations should be equal to the amount of values in a point");
            std::array<int, sizeof...(Ts)> list = {{ milis... }};
            std::copy(list.begin(), list.end(), durations.begin());
        }

        template<typename... Ts>
        template<typename... Fs>
        inline void tweenpoint<Ts...>::via(Fs... fs) {
            static_assert(sizeof...(Fs) == sizeof...(Ts),
                          "Number of functions passed to via() must be equal the number of values.");
            detail::easingresolve<0, std::tuple<Ts...>, typename traits::easingCollection, Fs...>::impl(easings, fs...);
        }

        template<typename... Ts>
        template<typename F>
        inline void tweenpoint<Ts...>::via(F f) {
            easingfill<typename traits::valuesType>(easings, f, int2type<sizeof...(Ts) - 1>{ });
        }

        template<typename... Ts>
        inline uint16_t tweenpoint<Ts...>::duration() const {
            return *std::max_element(durations.begin(), durations.end());
        }

        template<typename... Ts>
        inline uint16_t tweenpoint<Ts...>::duration(size_t i) const {
            return durations.at(i);
        }
    }
}
#endif //TWEENY_TWEENPOINT_TCC
