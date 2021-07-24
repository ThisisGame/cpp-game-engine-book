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
 * @file tween.h
 * This file contains the core of tweeny: the main tween class.
 */

#ifndef TWEENY_TWEEN_H
#define TWEENY_TWEEN_H

#include <tuple>
#include <vector>
#include <functional>

#include "tweentraits.h"
#include "tweenpoint.h"

namespace tweeny {
    /**
     * @brief The tween class is the core class of tweeny. It controls the interpolation steps, easings and durations.
     *
     * It should not be constructed manually but rather from @p tweeny::from, to facilitate template argument
     * deduction (and also to keep your code clean).
     */
    template<typename T, typename... Ts>
    class tween {
        public:
            /**
             * @brief Instantiates a tween from a starting point.
             *
             * This is a static factory helper function to be used by @p tweeny::from. You should not use this directly.
             * @p t The first value in the point
             * @p vs The remaining values
             */
            static tween<T, Ts...> from(T t, Ts... vs);

        public:
            /**
             * @brief Default constructor for a tween
             *
             * This constructor is provided to facilitate the usage of containers of tweens (e.g, std::vector). It
             * should not be used manually as the tweening created by it is invalid.
             */
            tween();

            /**
             * @brief Adds a new point in this tweening.
             *
             * This will add a new tweening point with the specified values. Next calls to @p via and @p during
             * will refer to this point.
             *
             * **Example**
             *
             * @code
             * auto t = tweeny::from(0).to(100).to(200);
             * @endcode
             *
             * @param t, vs Point values
             * @returns *this
             */
            tween<T, Ts...> & to(T t, Ts... vs);

            /**
             * @brief Specifies the easing function for the last added point.
             *
             * This will specify the easing between the last tween point added by @p to and its previous step. You can
             * use any callable object. Additionally, you can use the easing objects specified in the class @p easing.
             *
             * If it is a multi-value point, you can either specify a single easing function that will be used for
             * every value or you can specify an easing function for each value. You can mix and match callable objects,
             * lambdas and bundled easing objects.
             *
             * **Example**:
             *
             * @code
             * // use bundled linear easing
             * auto tween1 = tweeny::from(0).to(100).via(tweeny::easing::linear);
             *
             * // use custom lambda easing
             * auto tween2 = tweeny::from(0).to(100).via([](float p, int a, int b) { return (b-a) * p + a; });
             * @endcode
             *
             * @param fs The functions
             * @returns *this
             * @see tweeny::easing
             */
            template<typename... Fs> tween<T, Ts...> & via(Fs... fs);


            /**
             * @brief Specifies the easing function for the last added point, accepting an enumeration.
             *
             * This will specify the easing between the last tween point added by @p to and its previous step. You can
             * use a value from the @p tweeny::easing::enumerated enum. You can then have an enumeration of your own
             * poiting to this enumerated enums, or use it directly. You can mix-and-match enumerated easings, functions
             * and easing names.
             *
             * **Example**:
             *
             * @code
             * auto tween1 = tweeny::from(0).to(100).via(tweeny::easing::enumerated::linear);
             * auto tween2 = tweeny::from(0.0f, 100.0f).to(100.0f, 0.0f).via(tweeny::easing::linear, "backOut");
             *
             * @param fs The functions
             * @returns *this
             * @see tweeny::easing
             */
            template<typename... Fs> tween<T, Ts...> & via(easing::enumerated enumerated, Fs... fs);

            /**
             * @brief Specifies the easing function for the last added point, accepting an easing name as a `std::string` value.
             *
             * This will specify the easing between the last tween point added by @p to and its previous step.
             * You can mix-and-match enumerated easings, functions and easing names.
             *
             * **Example**:
             *
             * @code
             * auto tween = tweeny::from(0.0f, 100.0f).to(100.0f, 0.0f).via(tweeny::easing::linear, "backOut");
             *
             * @param fs The functions
             * @returns *this
             * @see tweeny::easing
             */
            template<typename... Fs> tween<T, Ts...> & via(const std::string & easing, Fs... fs);

        /**
            * @brief Specifies the easing function for the last added point, accepting an easing name as a `const char *` value.
            *
            * This will specify the easing between the last tween point added by @p to and its previous step.
            * You can mix-and-match enumerated easings, functions and easing names.
            *
            * **Example**:
            *
            * @code
            * auto tween = tweeny::from(0.0f, 100.0f).to(100.0f, 0.0f).via(tweeny::easing::linear, "backOut");
            *
            * @param fs The functions
            * @returns *this
            * @see tweeny::easing
            */
            template<typename... Fs> tween<T, Ts...> & via(const char * easing, Fs... fs);

            /**
             * @brief Specifies the easing function for a specific point.
             *
             * Points starts at index 0. The index 0 refers to the first @p to call.
             * Using this function without adding a point with @p to leads to undefined
             * behaviour.
             *
             * @param index The tween point index
             * @param fs The functions
             * @returns *this
             * @see tweeny::easing
             */
            template<typename... Fs> tween<T, Ts...> & via(int index, Fs... fs);

            /**
             * @brief Specifies the duration, typically in milliseconds, for the tweening of values in last point.
             *
             * You can either specify a single duration for all values or give every value its own duration. Value types
             * must be convertible to the uint16_t type.
             *
             * **Example**:
             *
             * @code
             * // Specify that the first point will be reached in 100 milliseconds and the first value in the second
             * // point in 100, whereas the second value will be reached in 500.
             * auto tween = tweeny::from(0, 0).to(100, 200).during(100).to(200, 300).during(100, 500);
             * @endcode
             *
             * @param ds Duration values
             * @returns *this
             */
            template<typename... Ds> tween<T, Ts...> & during(Ds... ds);

            /**
             * @brief Steps the animation by the designated delta amount.
             *
             * You should call this every frame of your application, passing in the amount of delta time that
             * you want to animate.
             *
             * **Example**:
             *
             * @code
             * // tween duration is 100ms
             * auto tween = tweeny::from(0).to(100).during(100);
             *
             * // steps for 16ms
             * tween.step(16);
             * @endcode
             *
             * @param dt Delta duration
             * @param suppressCallbacks (Optional) Suppress callbacks registered with tween::onStep()
             * @returns std::tuple<Ts...> with the current tween values.
             */
            const typename detail::tweentraits<T, Ts...>::valuesType & step(int32_t dt, bool suppressCallbacks = false);

            /**
             * @brief Steps the animation by the designated delta amount.
             *
             * You should call this every frame of your application, passing in the amount of delta time that
             * you want to animate. This overload exists to match unsigned int arguments.
             *
             * @param dt Delta duration
             * @param suppressCallbacks (Optional) Suppress callbacks registered with tween::onStep()
             * @returns std::tuple<Ts...> with the current tween values.
             */
            const typename detail::tweentraits<T, Ts...>::valuesType & step(uint32_t dt, bool suppressCallbacks = false);

            /**
             * @brief Steps the animation by the designated percentage amount.
             *
             * You can use this function to step the tweening by a specified percentage delta.

             * **Example**:
             *
             * @code
             * // tween duration is 100ms
             * auto tween = tweeny::from(0).to(100).during(100);
             *
             * // steps for 16ms
             * tween.step(0.001f);
             * @endcode
             *
             * @param dp Delta percentage, between `0.0f` and `1.0f`
             * @param suppressCallbacks (Optional) Suppress callbacks registered with tween::onStep()
             * @returns std::tuple<Ts...> with the current tween values.
             */
            const typename detail::tweentraits<T, Ts...>::valuesType & step(float dp, bool suppressCallbacks = false);

            /**
             * @brief Seeks to a specified point in time based on the currentProgress.
             *
             * This function sets the current animation time and currentProgress. Callbacks set by @p call will be triggered.
             *
             * @param p The percentage to seek to, between 0.0f and 1.0f, inclusive.
             * @param suppressCallbacks (Optional) Suppress callbacks registered with tween::onSeek()
             * @returns std::tuple<Ts...> with the current tween values.
             */
            const typename detail::tweentraits<T, Ts...>::valuesType & seek(float p, bool suppressCallbacks = false);

            /**
             * @brief Seeks to a specified point in time.
             *
             * This function sets the current animation time and currentProgress. Callbacks set by @p call will be triggered.
             *
             * @param d The duration to seek to, between 0 and the total duration.
             * @param suppressCallbacks (Optional) Suppress callbacks registered with tween::onSeek()
             * @returns std::tuple<Ts...> with the current tween values.
             * @see duration
             */
            const typename detail::tweentraits<T, Ts...>::valuesType & seek(int32_t d, bool suppressCallbacks = false);

            /**
             * @brief Seeks to a specified point in time.
             *
             * This function sets the current animation time and currentProgress. Callbacks set by @p call will be triggered.
             *
             * @param d The duration to seek to, between 0 and the total duration.
             * @param suppressCallbacks (Optional) Suppress callbacks registered with tween::onSeek()
             * @returns std::tuple<Ts...> with the current tween values.
             * @see duration
             */
            const typename detail::tweentraits<T, Ts...>::valuesType & seek(uint32_t d, bool suppressCallbacks = false);

            /**
             * @brief Adds a callback that will be called when stepping occurs, accepting both the tween and
             * its values.
             *
             * You can add as many callbacks as you want. Its arguments types must be equal to the argument types
             * of a tween instance, preceded by a variable of the tween type. Callbacks can be of any callable type. It will only be called
             * via tween::step() functions. For seek callbacks, see tween::onSeek().
             *
             * Keep in mind that the function will be *copied* into an array, so any variable captured by value
             * will also be copied with it.
             *
             * If the callback returns false, it will be called next time. If it returns true, it will be removed from
             * the callback queue.
             *
             * **Example**:
             *
             * @code
             * auto t = tweeny:from(0).to(100).during(100);
             *
             * // pass a lambda
             * t.onStep([](tweeny::tween<int> & t, int v) { printf("%d ", v); return false; });
             *
             * // pass a functor instance
             * struct ftor { void operator()(tweeny::tween<int> & t, int v) { printf("%d ", v); return false; } };
             * t.onStep(ftor());
             * @endcode
             * @sa step
             * @sa seek
             * @sa onSeek
             * @param callback A callback in with the prototype `bool callback(tween<Ts...> & t, Ts...)`
             */
            tween<T, Ts...> & onStep(typename detail::tweentraits<T, Ts...>::callbackType callback);

            /**
             * @brief Adds a callback that will be called when stepping occurs, accepting only the tween.
             *
             * You can add as many callbacks as you want. It must receive the tween as an argument.
             * Callbacks can be of any callable type. It will only be called
             * via tween::step() functions. For seek callbacks, see tween::onSeek().
             *
             * Keep in mind that the function will be *copied* into an array, so any variable captured by value
             * will also be copied with it.
             *
             * If the callback returns false, it will be called next time. If it returns true, it will be removed from
             * the callback queue.
             *
             * **Example**:
             *
             * @code
             * auto t = tweeny:from(0).to(100).during(100);
             *
             * // pass a lambda
             * t.onStep([](tweeny::tween<int> & t) { printf("%d ", t.value()); return false; });
             *
             * // pass a functor instance
             * struct ftor { void operator()(tweeny::tween<int> & t) { printf("%d ", t.values()); return false; } };
             * t.onStep(ftor());
             * @endcode
             * @sa step
             * @sa seek
             * @sa onSeek
             * @param callback A callback in the form `bool f(tween<Ts...> & t)`
             */
            tween<T, Ts...> & onStep(typename detail::tweentraits<T, Ts...>::noValuesCallbackType callback);

            /**
             * @brief Adds a callback that will be called when stepping occurs, accepting only the tween values.
             *
             * You can add as many callbacks as you want. It must receive the tween values as an argument.
             * Callbacks can be of any callable type. It will only be called
             * via tween::step() functions. For seek callbacks, see tween::onSeek().
             *
             * Keep in mind that the function will be *copied* into an array, so any variable captured by value
             * will also be copied with it.
             *
             * If the callback returns false, it will be called next time. If it returns true, it will be removed from
             * the callback queue.
             *
             * **Example**:
             *
             * @code
             * auto t = tweeny:from(0).to(100).during(100);
             *
             * // pass a lambda
             * t.onStep([](int v) { printf("%d ", v); return false; });
             *
             * // pass a functor instance
             * struct ftor { void operator()(int x) { printf("%d ", x); return false; } };
             * t.onStep(ftor());
             * @endcode
             * @sa step
             * @sa seek
             * @sa onSeek
             * @param callback A callback in the form `bool f(Ts...)`
             */
            tween<T, Ts...> & onStep(typename detail::tweentraits<T, Ts...>::noTweenCallbackType callback);

            /**
             * @brief Adds a callback for that will be called when seeking occurs
             *
             * You can add as many callbacks as you want. Its arguments types must be equal to the argument types
             * of a tween instance, preceded by a variable of the tween typve. Callbacks can be of any callable type. It will be called
             * via tween::seek() functions. For step callbacks, see tween::onStep().
             *
             * Keep in mind that the function will be *copied* into an array, so any variable captured by value
             * will also be copied with it.
             *
             * If the callback returns false, it will be called next time. If it returns true, it will be removed from
             * the callback queue.
             *
             * **Example**:
             *
             * @code
             * auto t = t:from(0).to(100).during(100);
             *
             * // pass a lambda
             * t.onSeek([](tweeny::tween<int> & t, int v) { printf("%d ", v); });
             *
             * // pass a functor instance
             * struct ftor { void operator()(tweeny::tween<int> & t, int v) { printf("%d ", v); } };
             * t.onSeek(ftor());
             * @endcode
             * @param callback A callback in with the prototype `bool callback(tween<Ts...> & t, Ts...)`
             */
            tween<T, Ts...> & onSeek(typename detail::tweentraits<T, Ts...>::callbackType callback);

            /**
             * @brief Adds a callback for that will be called when seeking occurs, accepting only the tween values.
             *
             * You can add as many callbacks as you want. It must receive the tween as an argument.
             * Callbacks can be of any callable type. It will be called
             * via tween::seek() functions. For step callbacks, see tween::onStep().
             *
             * Keep in mind that the function will be *copied* into an array, so any variable captured by value
             * will also be copied again.
             *
             * If the callback returns false, it will be called next time. If it returns true, it will be removed from
             * the callback queue.
             *
             * **Example**:
             *
             * @code
             * auto t = t:from(0).to(100).during(100);
             *
             * // pass a lambda
             * t.onSeek([](int v) { printf("%d ", v); });
             *
             * // pass a functor instance
             * struct ftor { void operator()(int v) { printf("%d ", v); return false; } };
             * t.onSeek(ftor());
             * @endcode
             * @param callback A callback in the form `bool f(Ts...)`
             */
            tween<T, Ts...> & onSeek(typename detail::tweentraits<T, Ts...>::noTweenCallbackType callback);

            /**
             * @brief Adds a callback for that will be called when seeking occurs, accepting only the tween.
             *
             * You can add as many callbacks as you want. It must receive the tween as an argument.
             * Callbacks can be of any callable type. It will be called
             * via tween::seek() functions. For step callbacks, see tween::onStep().
             *
             * Keep in mind that the function will be *copied* into an array, so any variable captured by value
             * will also be copied again.
             *
             * If the callback returns false, it will be called next time. If it returns true, it will be removed from
             * the callback queue.
             *
             * **Example**:
             *
             * @code
             * auto t = t:from(0).to(100).during(100);
             *
             * // pass a lambda
             * t.onSeek([](tweeny::tween<int> & t) { printf("%d ", t.value()); return false; });
             *
             * // pass a functor instance
             * struct ftor { void operator()(tweeny::tween<int> & t) { printf("%d ",  t.value()); return false; } };
             * t.onSeek(ftor());
             * @endcode
             * @param callback A callback in the form `bool f(tween<Ts...> & t)`
             */
            tween<T, Ts...> & onSeek(typename detail::tweentraits<T, Ts...>::noValuesCallbackType callback);

            /**
             * @brief Returns the total duration of this tween
             *
             * @returns The duration of all the tween points.
             */
            uint32_t duration() const;

            /**
             * @brief Returns the current tween values
             *
             * This returns the current tween value as returned by the
             * tween::step() function, except that it does not perform a step.
             * @returns std::tuple<Ts...> with the current tween values.
             */
            const typename detail::tweentraits<T, Ts...>::valuesType & peek() const;

            /**
             * @brief Calculates and returns the tween values at a given progress
             *
             * This returns the tween value at the requested progress, without stepping
             * or seeking.
             * @returns std::tuple<Ts...> with the current tween values.
             */
             const typename detail::tweentraits<T, Ts...>::valuesType peek(float progress) const;


            /**
             * @brief Calculates and return the tween values at a given time
             *
             * This returns the tween values at the requested time, without stepping
             * or seeking.
             * @returns std::tuple<Ts...> with the calculated tween values.
             */
            const typename detail::tweentraits<T, Ts...>::valuesType peek(uint32_t time) const;

            /**
             * @brief Returns the current currentProgress of the interpolation.
             *
             * 0 means its at the values passed in the construction, 1 means the last step.
             * @returns the current currentProgress between 0 and 1 (inclusive)
             */
            float progress() const;

            /**
             * @brief Sets the direction of this tween forward.
             *
             * Note that this only affects tween::step() function.
             * @returns *this
             * @sa backward
             */
            tween<T, Ts...> & forward();

            /**
             * @brief Sets the direction of this tween backward.
             *
             * Note that this only affects tween::step() function.
             * @returns *this
             * @sa forward
             */
            tween<T, Ts...> & backward();

            /**
             * @brief Returns the current direction of this tween
             *
             * @returns -1 If it is mobin backwards in time, 1 if it is moving forward in time
             */
            int direction() const;

            /**
             * @brief Jumps to a specific tween point
             *
             * This will seek the tween to a percentage matching the beginning of that step.
             *
             * @param point The point to seek to. 0 means the point passed in tweeny::from
             * @param suppressCallbacks (optional) set to true to suppress seek() callbacks
             * @returns current values
             * @sa seek
             */
            const typename detail::tweentraits<T, Ts...>::valuesType & jump(size_t point, bool suppressCallbacks = false);

            /**
             * @brief Returns the current tween point
             *
             * @returns Current tween point
             */
            uint16_t point() const;

        private /* member types */:
            using traits = detail::tweentraits<T, Ts...>;

        private /* member variables */:
            uint32_t total = 0; // total runtime
            uint16_t currentPoint = 0; // current point
            float currentProgress = 0; // current progress
            std::vector<detail::tweenpoint<T, Ts...>> points;
            typename traits::valuesType current;
            std::vector<typename traits::callbackType> onStepCallbacks;
            std::vector<typename traits::callbackType> onSeekCallbacks;
            int8_t currentDirection = 1;

        private:
            /* member functions */
            tween(T t, Ts... vs);
            template<size_t I> void interpolate(float prog, unsigned point, typename traits::valuesType & values, detail::int2type<I>) const;
            void interpolate(float prog, unsigned point, typename traits::valuesType & values, detail::int2type<0>) const;
            void render(float p);
            void dispatch(std::vector<typename traits::callbackType> & cbVector);
            uint16_t pointAt(float progress) const;
    };

    /**
    * @brief Class specialization when a tween has a single value
    *
    * This class is preferred automatically by your compiler when your tween has only one value. It exists mainly
    * so that you dont need to use std::get<0> to obtain a single value when using tween::step, tween::seek or any other
    * value returning function. Other than that, you should look at the
    * tweeny::tween documentation.
    *
    * Except for this little detail, this class methods and behaviours are exactly the same.
    */
    template<typename T>
    class tween<T> {
        public:
            static tween<T> from(T t);

        public:
            tween(); ///< @sa tween::tween
            tween<T> & to(T t); ///< @sa tween::to
            template<typename... Fs> tween<T> & via(Fs... fs); ///< @sa tween::via
            template<typename... Fs> tween<T> & via(int index, Fs... fs); ///< @sa tween::via
            template<typename... Fs> tween<T> & via(tweeny::easing::enumerated enumerated, Fs... fs); ///< @sa tween::via
            template<typename... Fs> tween<T> & via(const std::string & easing, Fs... fs); ///< @sa tween::via
            template<typename... Fs> tween<T> & via(const char * easing, Fs... fs); ///< @sa tween::via
            template<typename... Ds> tween<T> & during(Ds... ds); ///< @sa tween::during
            const T & step(int32_t dt, bool suppressCallbacks = false); ///< @sa tween::step(int32_t dt, bool suppressCallbacks)
            const T & step(uint32_t dt, bool suppressCallbacks = false); ///< @sa tween::step(uint32_t dt, bool suppressCallbacks)
            const T & step(float dp, bool suppressCallbacks = false); ///< @sa tween::step(float dp, bool suppressCallbacks)
            const T & seek(float p, bool suppressCallbacks = false); ///< @sa tween::seek(float p, bool suppressCallbacks)
            const T & seek(int32_t d, bool suppressCallbacks = false); ///< @sa tween::seek(int32_t d, bool suppressCallbacks)
            const T & seek(uint32_t d, bool suppressCallbacks = false); ///< @sa tween::seek(uint32_t d, bool suppressCallbacks)
            tween<T> & onStep(typename detail::tweentraits<T>::callbackType callback); ///< @sa tween::onStep
            tween<T> & onStep(typename detail::tweentraits<T>::noValuesCallbackType callback); ///< @sa tween::onStep
            tween<T> & onStep(typename detail::tweentraits<T>::noTweenCallbackType callback); ///< @sa tween::onStep
            tween<T> & onSeek(typename detail::tweentraits<T>::callbackType callback); ///< @sa tween::onSeek
            tween<T> & onSeek(typename detail::tweentraits<T>::noValuesCallbackType callback); ///< @sa tween::onSeek
            tween<T> & onSeek(typename detail::tweentraits<T>::noTweenCallbackType callback); ///< @sa tween::onSeek
            const T & peek() const; ///< @sa tween::peek
            T peek(float progress) const; ///< @sa tween::peek
            T peek(uint32_t time) const; ///< @sa tween::peek
            uint32_t duration() const; ///< @sa tween::duration
            float progress() const; ///< @sa tween::progress
            tween<T> & forward(); ///< @sa tween::forward
            tween<T> & backward(); ///< @sa tween::backward
            int direction() const; ///< @sa tween::direction
            const T & jump(size_t point, bool suppressCallbacks = false); ///< @sa tween::jump
            uint16_t point() const; ///< @sa tween::point

        private /* member types */:
            using traits = detail::tweentraits<T>;

        private /* member variables */:
            uint32_t total = 0; // total runtime
            uint16_t currentPoint = 0; // current point
            float currentProgress = 0; // current progress
            std::vector<detail::tweenpoint<T>> points;
            T current;
            std::vector<typename traits::callbackType> onStepCallbacks;
            std::vector<typename traits::callbackType> onSeekCallbacks;
            int8_t currentDirection = 1;

        private:
            /* member functions */
            tween(T t);
            void interpolate(float prog, unsigned point, T & value) const;
            void render(float p);
            void dispatch(std::vector<typename traits::callbackType> & cbVector);
            uint16_t pointAt(float progress) const;
    };
}

#include "tween.tcc"
#include "tweenone.tcc"

#endif //TWEENY_TWEEN_H
