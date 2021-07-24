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
 * This file provides the declarations for a tween point utility class. A tweenpoint holds the tween values,
 * easings and durations.
 */


#ifndef TWEENY_TWEENPOINT_H
#define TWEENY_TWEENPOINT_H


#include <tuple>
#include <array>

#include "tweentraits.h"

namespace tweeny {
    namespace detail {
        /*
         * The tweenpoint class aids in the management of a tweening point by the tween class.
         * This class is private.
         */
        template<typename... Ts>
        struct tweenpoint {
            typedef detail::tweentraits<Ts...> traits;

            typename traits::valuesType values;
            typename traits::durationsArrayType durations;
            typename traits::easingCollection easings;
            typename traits::callbackType onEnterCallbacks;
            uint32_t stacked;

            /* Constructs a tweenpoint from a set of values, filling their durations and easings */
            tweenpoint(Ts... vs);

            /* Set the duration for all the values in this point */
            template<typename D> void during(D milis);

            /* Sets the duration for each value in this point */
            template<typename... Ds> void during(Ds... vs);

            /* Sets the easing functions of each value */
            template<typename... Fs> void via(Fs... fs);

            /* Sets the same easing function for all values */
            template<typename F> void via(F f);

            /* Returns the highest value in duration array */
            uint16_t duration() const;

            /* Returns the value of that specific value */
            uint16_t duration(size_t i) const;
        };
    }
}

#include "tweenpoint.tcc"

#endif //TWEENY_TWEENPOINT_H
