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

/**
 * @file easing.h
 * The purpose of this file is to list all bundled easings. All easings are based on Robert Penner's easing
 * functions: http://robertpenner.com/easing/
 */

#ifndef TWEENY_EASING_H
#define TWEENY_EASING_H

#include <cmath>
#include <type_traits>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

/**
    * @defgroup easings Easings
    * @brief Bundled easing functions based on
    *        <a href="http://robertpenner.com/easing/">Robert Penner's Easing Functions</a>
    * @details You should plug these functions into @ref tweeny::tween::via function to specify the easing used in a tween.
    * @sa tweeny::easing
    * @{
    *//**
    *   @defgroup stepped Stepped
    *   @{
    *       @brief The value does not change. No interpolation is used.
    *   @}
    *//**
    *   @defgroup default Default
    *   @{
    *       @brief A default mode for arithmetic values it will change in constant speed, for non-arithmetic value will be constant.
    *   @}
    *//**
    *   @defgroup linear Linear
    *   @{
    *       @brief The most boring ever easing function. It has no acceleration and change values in constant speed.
    *   @}
    *//**
    *   @defgroup quadratic Quadratic
    *   @{
    *       @brief The most commonly used easing functions.
    *   @}
    *//**
    *   @defgroup cubic Cubic
    *   @{
    *       @brief A bit curvier than the quadratic easing.
    *   @}
    *//**
    *   @defgroup quartic Quartic
    *   @{
    *       @brief A steeper curve. Acceleration changes faster than Cubic.
    *   @}
    *//**
    *   @defgroup quintic Quintic
    *   @{
    *       @brief An even steeper curve. Acceleration changes really fast.
    *   @}
    *//**
    *   @defgroup sinuisodal Sinuisodal
    *   @{
    *       @brief A very gentle curve, gentlier than quadratic.
    *   @}
    *//**
    *   @defgroup exponential Exponential
    *   @{
    *       @brief A very steep curve, based on the `p(t) = 2^(10*(t-1))` equation.
    *   @}
    *//**
    *   @defgroup circular Circular
    *   @{
    *       @brief A smooth, circular slope that resembles the arc of an circle.
    *   @}
    *//**
    *   @defgroup back Back
    *   @{
    *       @brief An easing function that has a "cute" natural coming back effect.
    *   @}
    *//**
    *   @defgroup elastic Elastic
    *   @{
    *       @brief An elastic easing function. Values go a little past the maximum/minimum in an elastic effect.
    *   @}
    *//**
    *   @defgroup bounce Bounce
    *   @{
    *       @brief A bouncing easing function. Values "bounce" around the maximum/minumum.
    *   @}
    *//**
    * @}
    */

namespace tweeny {
    /**
     * @brief The easing class holds all the bundled easings.
     *
     * You should pass the easing function to the @p tweeny::tween::via method, to set the easing function that will
     * be used to interpolate values in a tween point.
     *
     * **Example**:
     *
     * @code
     * auto tween = tweeny::from(0).to(100).via(tweeny::easing::linear);
     * @endcode
     */
    class easing {
        public:
            /**
             * @brief Enumerates all easings to aid in runtime when adding easins to a tween using tween::via
             *
             * The aim of this enum is to help in situations where the easing doesn't come straight from the C++
             * code but rather from a configuration file or some sort of external paramenter.
             */
            enum class enumerated {
                def,
                linear,
                stepped,
                quadraticIn,
                quadraticOut,
                quadraticInOut,
                cubicIn,
                cubicOut,
                cubicInOut,
                quarticIn,
                quarticOut,
                quarticInOut,
                quinticIn,
                quinticOut,
                quinticInOut,
                sinusoidalIn,
                sinusoidalOut,
                sinusoidalInOut,
                exponentialIn,
                exponentialOut,
                exponentialInOut,
                circularIn,
                circularOut,
                circularInOut,
                bounceIn,
                bounceOut,
                bounceInOut,
                elasticIn,
                elasticOut,
                elasticInOut,
                backIn,
                backOut,
                backInOut
            };

            /**
             * @ingroup stepped
             * @brief Value is constant.
             */
            static constexpr struct steppedEasing {
                template<typename T>
                static T run(float position, T start, T end) {
                    return start;
                }
            } stepped = steppedEasing{};
      
            /**
             * @ingroup default
             * @brief Values change with constant speed for arithmetic type only. The non-arithmetic it will be constant.
             */
            static constexpr struct defaultEasing {
                template<class...> struct voidify { using type = void; };
                template<class... Ts> using void_t = typename voidify<Ts...>::type;

                template<class T, class = void>
                struct supports_arithmetic_operations : std::false_type {};

                template<class T>
                struct supports_arithmetic_operations<T, void_t<
                    decltype(std::declval<T>() + std::declval<T>()),
                    decltype(std::declval<T>() - std::declval<T>()),
                    decltype(std::declval<T>() * std::declval<T>()),
                    decltype(std::declval<T>() * std::declval<float>()),
                    decltype(std::declval<float>() * std::declval<T>())
                    >> : std::true_type{};


                template<typename T>
                static typename std::enable_if<std::is_integral<T>::value, T>::type run(float position, T start, T end) {
                    return static_cast<T>(roundf((end - start) * position + start));
                }

                template<typename T>
                static typename std::enable_if<supports_arithmetic_operations<T>::value && !std::is_integral<T>::value, T>::type run(float position, T start, T end) {
                    return static_cast<T>((end - start) * position + start);
                }

                template<typename T>
                static typename std::enable_if<!supports_arithmetic_operations<T>::value, T>::type run(float position, T start, T end) {
                    return start;
                }
            } def = defaultEasing{};

            /**
             * @ingroup linear
             * @brief Values change with constant speed.
             */
            static constexpr struct linearEasing {
                template<typename T>
                static typename std::enable_if<std::is_integral<T>::value, T>::type run(float position, T start, T end) {
                    return static_cast<T>(roundf((end - start) * position + start));
                }

                template<typename T>
                static typename std::enable_if<!std::is_integral<T>::value, T>::type run(float position, T start, T end) {
                    return static_cast<T>((end - start) * position + start);
                }
            } linear = linearEasing{};

             /**
              * @ingroup quadratic
              * @brief Accelerate initial values with a quadratic equation.
              */
            static constexpr struct quadraticInEasing {
                template<typename T>
                static T run(float position, T start, T end) {
                    return static_cast<T>((end - start) * position * position + start);
                }
            } quadraticIn = quadraticInEasing{};

            /**
              * @ingroup quadratic
              * @brief Deaccelerate ending values with a quadratic equation.
              */
            static constexpr struct quadraticOutEasing {
                template<typename T>
                static T run(float position, T start, T end) {
                    return static_cast<T>((-(end - start)) * position * (position - 2) + start);
                }
            } quadraticOut = quadraticOutEasing{};

            /**
              * @ingroup quadratic
              * @brief Acceelerate initial and deaccelerate ending values with a quadratic equation.
              */
            static constexpr struct quadraticInOutEasing {
                template<typename T>
                static T run(float position, T start, T end) {
                    position *= 2;
                    if (position < 1) {
                        return static_cast<T>(((end - start) / 2) * position * position + start);
                    }

                    --position;
                    return static_cast<T>((-(end - start) / 2) * (position * (position - 2) - 1) + start);
                }
            } quadraticInOut = quadraticInOutEasing{};

            /**
              * @ingroup cubic
              * @brief Aaccelerate initial values with a cubic equation.
              */
            static constexpr struct cubicInEasing {
                template<typename T>
                static T run(float position, T start, T end) {
                    return static_cast<T>((end - start) * position * position * position + start);
                }
            } cubicIn = cubicInEasing{};

            /**
              * @ingroup cubic
              * @brief Deaccelerate ending values with a cubic equation.
              */
            static constexpr struct cubicOutEasing {
                template<typename T>
                static T run(float position, T start, T end) {
                    --position;
                    return static_cast<T>((end - start) * (position * position * position + 1) + start);
                }
            } cubicOut = cubicOutEasing{};

            /**
              * @ingroup cubic
              * @brief Acceelerate initial and deaccelerate ending values with a cubic equation.
              */
            static constexpr struct cubicInOutEasing {
                template<typename T>
                static T run(float position, T start, T end) {
                    position *= 2;
                    if (position < 1) {
                        return static_cast<T>(((end - start) / 2) * position * position * position + start);
                    }
                    position -= 2;
                    return static_cast<T>(((end - start) / 2) * (position * position * position + 2) + start);
                }
            } cubicInOut = cubicInOutEasing{};

            /**
              * @ingroup quartic
              * @brief Acceelerate initial values with a quartic equation.
              */
            static constexpr struct quarticInEasing {
                template<typename T>
                static T run(float position, T start, T end) {
                    return static_cast<T>((end - start) * position * position * position * position + start);
                }
            } quarticIn = quarticInEasing{};

            /**
              * @ingroup quartic
              * @brief Deaccelerate ending values with a quartic equation.
              */
            static constexpr struct quarticOutEasing {
                template<typename T>
                static T run(float position, T start, T end) {
                    --position;
                    return static_cast<T>( -(end - start) * (position * position * position * position - 1) + start);
                }
            } quarticOut = quarticOutEasing{};

            /**
              * @ingroup quartic
              * @brief Acceelerate initial and deaccelerate ending values with a quartic equation.
              */
            static constexpr struct quarticInOutEasing {
                template<typename T>
                static T run(float position, T start, T end) {
                    position *= 2;
                    if (position < 1) {
                        return static_cast<T>(((end - start) / 2) * (position * position * position * position) +
                                              start);
                    }
                    position -= 2;
                    return static_cast<T>((-(end - start) / 2) * (position * position * position * position - 2) +
                                          start);
                }
            } quarticInOut = quarticInOutEasing{};

            /**
              * @ingroup quintic
              * @brief Acceelerate initial values with a quintic equation.
              */
            static constexpr struct quinticInEasing {
                template<typename T>
                static T run(float position, T start, T end) {
                    return static_cast<T>((end - start) * position * position * position * position * position + start);
                }
            } quinticIn = quinticInEasing{};

            /**
              * @ingroup quintic
              * @brief Deaccelerate ending values with a quintic equation.
              */
            static constexpr struct quinticOutEasing {
                template<typename T>
                static T run(float position, T start, T end) {
                    position--;
                    return static_cast<T>((end - start) * (position * position * position * position * position + 1) +
                                          start);
                }
            } quinticOut = quinticOutEasing{};

            /**
              * @ingroup quintic
              * @brief Acceelerate initial and deaccelerate ending values with a quintic equation.
              */
            static constexpr struct quinticInOutEasing {
                template<typename T>
                static T run(float position, T start, T end) {
                    position *= 2;
                    if (position < 1) {
                        return static_cast<T>(
                            ((end - start) / 2) * (position * position * position * position * position) +
                            start);
                    }
                    position -= 2;
                    return static_cast<T>(
                        ((end - start) / 2) * (position * position * position * position * position + 2) +
                        start);
                }
            } quinticInOut = quinticInOutEasing{};

            /**
              * @ingroup sinusoidal
              * @brief Acceelerate initial values with a sinusoidal equation.
              */
            static constexpr struct sinusoidalInEasing {
                template<typename T>
                static T run(float position, T start, T end) {
                    return static_cast<T>(-(end - start) * cosf(position * static_cast<float>(M_PI) / 2) + (end - start) + start);
                }
            } sinusoidalIn = sinusoidalInEasing{};

            /**
              * @ingroup sinusoidal
              * @brief Deaccelerate ending values with a sinusoidal equation.
              */
            static constexpr struct sinusoidalOutEasing {
                template<typename T>
                static T run(float position, T start, T end) {
                    return static_cast<T>((end - start) * sinf(position * static_cast<float>(M_PI) / 2) + start);
                }
            } sinusoidalOut = sinusoidalOutEasing{};

            /**
              * @ingroup sinusoidal
              * @brief Acceelerate initial and deaccelerate ending values with a sinusoidal equation.
              */
            static constexpr struct sinusoidalInOutEasing {
                template<typename T>
                static T run(float position, T start, T end) {
                    return static_cast<T>((-(end - start) / 2) * (cosf(position * static_cast<float>(M_PI)) - 1) + start);
                }
            } sinusoidalInOut = sinusoidalInOutEasing{};

            /**
              * @ingroup exponential
              * @brief Acceelerate initial values with an exponential equation.
              */
            static constexpr struct exponentialInEasing {
                template<typename T>
                static T run(float position, T start, T end) {
                    return static_cast<T>((end - start) * powf(2, 10 * (position - 1)) + start);
                }
            } exponentialIn = exponentialInEasing{};

            /**
              * @ingroup exponential
              * @brief Deaccelerate ending values with an exponential equation.
              */
            static constexpr struct exponentialOutEasing {
                template<typename T>
                static T run(float position, T start, T end) {
                    return static_cast<T>((end - start) * (-powf(2, -10 * position) + 1) + start);
                }
            } exponentialOut = exponentialOutEasing{};

            /**
              * @ingroup exponential
              * @brief Acceelerate initial and deaccelerate ending values with an exponential equation.
              */
            static constexpr struct exponentialInOutEasing {
                template<typename T>
                static T run(float position, T start, T end) {
                    position *= 2;
                    if (position < 1) {
                        return static_cast<T>(((end - start) / 2) * powf(2, 10 * (position - 1)) + start);
                    }
                    --position;
                    return static_cast<T>(((end - start) / 2) * (-powf(2, -10 * position) + 2) + start);
                }
            } exponentialInOut = exponentialInOutEasing{};

            /**
              * @ingroup circular
              * @brief Acceelerate initial values with a circular equation.
              */
            static constexpr struct circularInEasing {
                template<typename T>
                static T run(float position, T start, T end) {
                    return static_cast<T>( -(end - start) * (sqrtf(1 - position * position) - 1) + start );
                }
            } circularIn = circularInEasing{};

            /**
              * @ingroup circular
              * @brief Deaccelerate ending values with a circular equation.
              */
            static constexpr struct circularOutEasing {
                template<typename T>
                static T run(float position, T start, T end) {
                    --position;
                    return static_cast<T>((end - start) * (sqrtf(1 - position * position)) + start);
                }
            } circularOut = circularOutEasing{};

            /**
              * @ingroup circular
              * @brief Acceelerate initial and deaccelerate ending values with a circular equation.
              */
            static constexpr struct circularInOutEasing {
                template<typename T>
                static T run(float position, T start, T end) {
                    position *= 2;
                    if (position < 1) {
                        return static_cast<T>((-(end - start) / 2) * (sqrtf(1 - position * position) - 1) + start);
                    }

                    position -= 2;
                    return static_cast<T>(((end - start) / 2) * (sqrtf(1 - position * position) + 1) + start);
                }
            } circularInOut = circularInOutEasing{};

            /**
              * @ingroup bounce
              * @brief Acceelerate initial values with a "bounce" equation.
              */
            static constexpr struct bounceInEasing {
                template<typename T>
                static T run(float position, T start, T end) {
                    return (end - start) - bounceOut.run((1 - position), T(), end) + start;
                }
            } bounceIn = bounceInEasing{};

            /**
              * @ingroup bounce
              * @brief Deaccelerate ending values with a "bounce" equation.
              */
            static constexpr struct bounceOutEasing {
                template<typename T>
                static T run(float position, T start, T end) {
                    T c = end - start;
                    if (position < (1 / 2.75f)) {
                        return static_cast<T>(c * (7.5625f * position * position) + start);
                    } else if (position < (2.0f / 2.75f)) {
                        float postFix = position -= (1.5f / 2.75f);
                        return static_cast<T>(c * (7.5625f * (postFix) * position + .75f) + start);
                    } else if (position < (2.5f / 2.75f)) {
                        float postFix = position -= (2.25f / 2.75f);
                        return static_cast<T>(c * (7.5625f * (postFix) * position + .9375f) + start);
                    } else {
                        float postFix = position -= (2.625f / 2.75f);
                        return static_cast<T>(c * (7.5625f * (postFix) * position + .984375f) + start);
                    }
                }
            } bounceOut = bounceOutEasing{};

            /**
            * @ingroup bounce
            * @brief Acceelerate initial and deaccelerate ending values with a "bounce" equation.
            */
            static constexpr struct bounceInOutEasing {
                template<typename T>
                static T run(float position, T start, T end) {
                    if (position < 0.5f) return static_cast<T>(bounceIn.run(position * 2, T(), end) * .5f + start);
                    else return static_cast<T>(bounceOut.run((position * 2 - 1), T(), end) * .5f + (end - start) * .5f + start);
                }
            } bounceInOut = bounceInOutEasing{};

            /**
              * @ingroup elastic
              * @brief Acceelerate initial values with an "elastic" equation.
              */
            static constexpr struct elasticInEasing {
                template<typename T>
                static T run(float position, T start, T end) {
                    if (position <= 0.00001f) return start;
                    if (position >= 0.999f) return end;
                    float p = .3f;
                    auto a = end - start;
                    float s = p / 4;
                    float postFix =
                        a * powf(2, 10 * (position -= 1)); // this is a fix, again, with post-increment operators
                    return static_cast<T>(-(postFix * sinf((position - s) * (2 * static_cast<float>(M_PI)) / p)) + start);
                }
            } elasticIn = elasticInEasing{};

            /**
              * @ingroup elastic
              * @brief Deaccelerate ending values with an "elastic" equation.
              */
            static constexpr struct elasticOutEasing {
                template<typename T>
                static T run(float position, T start, T end) {
                    if (position <= 0.00001f) return start;
                    if (position >= 0.999f) return end;
                    float p = .3f;
                    auto a = end - start;
                    float s = p / 4;
                    return static_cast<T>(a * powf(2, -10 * position) * sinf((position - s) * (2 * static_cast<float>(M_PI)) / p) + end);
                }
            } elasticOut = elasticOutEasing{};

            /**
            * @ingroup elastic
            * @brief Acceelerate initial and deaccelerate ending values with an "elastic" equation.
            */
            static constexpr struct elasticInOutEasing {
                template<typename T>
                static T run(float position, T start, T end) {
                    if (position <= 0.00001f) return start;
                    if (position >= 0.999f) return end;
                    position *= 2;
                    float p = (.3f * 1.5f);
                    auto a = end - start;
                    float s = p / 4;
                    float postFix;

                    if (position < 1) {
                        postFix = a * powf(2, 10 * (position -= 1)); // postIncrement is evil
                        return static_cast<T>(-0.5f * (postFix * sinf((position - s) * (2 * static_cast<float>(M_PI)) / p)) + start);
                    }
                    postFix = a * powf(2, -10 * (position -= 1)); // postIncrement is evil
                    return static_cast<T>(postFix * sinf((position - s) * (2 * static_cast<float>(M_PI)) / p) * .5f + end);
                }
            } elasticInOut = elasticInOutEasing{};

            /**
              * @ingroup back
              * @brief Acceelerate initial values with a "back" equation.
              */
            static constexpr struct backInEasing {
                template<typename T>
                static T run(float position, T start, T end) {
                    float s = 1.70158f;
                    float postFix = position;
                    return static_cast<T>((end - start) * (postFix) * position * ((s + 1) * position - s) + start);
                }
            } backIn = backInEasing{};

            /**
              * @ingroup back
              * @brief Deaccelerate ending values with a "back" equation.
              */
            static constexpr struct backOutEasing {
                template<typename T>
                static T run(float position, T start, T end) {
                    float s = 1.70158f;
                    position -= 1;
                    return static_cast<T>((end - start) * ((position) * position * ((s + 1) * position + s) + 1) + start);
                }
            } backOut = backOutEasing{};

            /**
            * @ingroup back
            * @brief Acceelerate initial and deaccelerate ending values with a "back" equation.
            */
            static constexpr struct backInOutEasing {
                template<typename T>
                static T run(float position, T start, T end) {
                    float s = 1.70158f;
                    float t = position;
                    auto b = start;
                    auto c = end - start;
                    float d = 1;
                    s *= (1.525f);
                    if ((t /= d / 2) < 1) return static_cast<T>(c / 2 * (t * t * (((s) + 1) * t - s)) + b);
                    float postFix = t -= 2;
                    return static_cast<T>(c / 2 * ((postFix) * t * (((s) + 1) * t + s) + 2) + b);
                }
            } backInOut = backInOutEasing{};
    };
}
#endif //TWEENY_EASING_H
