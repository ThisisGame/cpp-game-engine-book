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
 * The purpose of this file is to hold implementations for the tween.h file.
 */

#ifndef TWEENY_TWEEN_TCC
#define TWEENY_TWEEN_TCC

#include "tween.h"
#include "dispatcher.h"

namespace tweeny {

    namespace detail {
        template<typename T>
        T clip(const T & n, const T & lower, const T & upper) {
            return std::max(lower, std::min(n, upper));
        }
    }

    template<typename T, typename... Ts> inline tween<T, Ts...> tween<T, Ts...>::from(T t, Ts... vs) { return tween<T, Ts...>(t, vs...); }
    template<typename T, typename... Ts> inline tween<T, Ts...>::tween() { }
    template<typename T, typename... Ts> inline tween<T, Ts...>::tween(T t, Ts... vs) {
        points.emplace_back(t, vs...);
    }

    template<typename T, typename... Ts> inline tween<T, Ts...> & tween<T, Ts...>::to(T t, Ts... vs) {
        points.emplace_back(t, vs...);
        return *this;
    }

    template<typename T, typename... Ts>
    template<typename... Fs>
    inline tween<T, Ts...> & tween<T, Ts...>::via(Fs... vs) {
        points.at(points.size() - 2).via(vs...);
        return *this;
    }

    template<typename T, typename... Ts>
    template<typename... Fs>
    inline tween<T, Ts...> & tween<T, Ts...>::via(int index, Fs... vs) {
        points.at(static_cast<size_t>(index)).via(vs...);
        return *this;
    }

    template<typename T, typename... Ts>
    template<typename... Fs>
    tween<T, Ts...> & tween<T, Ts...>::via(easing::enumerated enumerated, Fs... vs) {
        switch (enumerated) {
            case easing::enumerated::def: return via(easing::def, vs...);
            case easing::enumerated::linear: return via(easing::linear, vs...);
            case easing::enumerated::stepped: return via(easing::stepped, vs...);
            case easing::enumerated::quadraticIn: return via(easing::quadraticIn, vs...);
            case easing::enumerated::quadraticOut: return via(easing::quadraticOut, vs...);
            case easing::enumerated::quadraticInOut: return via(easing::quadraticInOut, vs...);
            case easing::enumerated::cubicIn: return via(easing::cubicIn, vs...);
            case easing::enumerated::cubicOut: return via(easing::cubicOut, vs...);
            case easing::enumerated::cubicInOut: return via(easing::cubicInOut, vs...);
            case easing::enumerated::quarticIn: return via(easing::quarticIn, vs...);
            case easing::enumerated::quarticOut: return via(easing::quarticOut, vs...);
            case easing::enumerated::quarticInOut: return via(easing::quarticInOut, vs...);
            case easing::enumerated::quinticIn: return via(easing::quinticIn, vs...);
            case easing::enumerated::quinticOut: return via(easing::quinticOut, vs...);
            case easing::enumerated::quinticInOut: return via(easing::quinticInOut, vs...);
            case easing::enumerated::sinusoidalIn: return via(easing::sinusoidalIn, vs...);
            case easing::enumerated::sinusoidalOut: return via(easing::sinusoidalOut, vs...);
            case easing::enumerated::sinusoidalInOut: return via(easing::sinusoidalInOut, vs...);
            case easing::enumerated::exponentialIn: return via(easing::exponentialIn, vs...);
            case easing::enumerated::exponentialOut: return via(easing::exponentialOut, vs...);
            case easing::enumerated::exponentialInOut: return via(easing::exponentialInOut, vs...);
            case easing::enumerated::circularIn: return via(easing::circularIn, vs...);
            case easing::enumerated::circularOut: return via(easing::circularOut, vs...);
            case easing::enumerated::circularInOut: return via(easing::circularInOut, vs...);
            case easing::enumerated::bounceIn: return via(easing::bounceIn, vs...);
            case easing::enumerated::bounceOut: return via(easing::bounceOut, vs...);
            case easing::enumerated::bounceInOut: return via(easing::bounceInOut, vs...);
            case easing::enumerated::elasticIn: return via(easing::elasticIn, vs...);
            case easing::enumerated::elasticOut: return via(easing::elasticOut, vs...);
            case easing::enumerated::elasticInOut: return via(easing::elasticInOut, vs...);
            case easing::enumerated::backIn: return via(easing::backIn, vs...);
            case easing::enumerated::backOut: return via(easing::backOut, vs...);
            case easing::enumerated::backInOut: return via(easing::backInOut, vs...);
            default: return via(easing::def, vs...);
        }
    }

    template<typename T, typename... Ts>
    template<typename... Fs>
    tween<T, Ts...> & tween<T, Ts...>::via(const std::string & easing, Fs... vs) {
        if (easing == "stepped") return via(easing::stepped, vs...);
        if (easing == "linear") return via(easing::linear, vs...);
        if (easing == "quadraticIn") return via(easing::quadraticIn, vs...);
        if (easing == "quadraticOut") return via(easing::quadraticOut, vs...);
        if (easing == "quadraticInOut") return via(easing::quadraticInOut, vs...);
        if (easing == "cubicIn") return via(easing::cubicIn, vs...);
        if (easing == "cubicOut") return via(easing::cubicOut, vs...);
        if (easing == "cubicInOut") return via(easing::cubicInOut, vs...);
        if (easing == "quarticIn") return via(easing::quarticIn, vs...);
        if (easing == "quarticOut") return via(easing::quarticOut, vs...);
        if (easing == "quarticInOut") return via(easing::quarticInOut, vs...);
        if (easing == "quinticIn") return via(easing::quinticIn, vs...);
        if (easing == "quinticOut") return via(easing::quinticOut, vs...);
        if (easing == "quinticInOut") return via(easing::quinticInOut, vs...);
        if (easing == "sinusoidalIn") return via(easing::sinusoidalIn, vs...);
        if (easing == "sinusoidalOut") return via(easing::sinusoidalOut, vs...);
        if (easing == "sinusoidalInOut") return via(easing::sinusoidalInOut, vs...);
        if (easing == "exponentialIn") return via(easing::exponentialIn, vs...);
        if (easing == "exponentialOut") return via(easing::exponentialOut, vs...);
        if (easing == "exponentialInOut") return via(easing::exponentialInOut, vs...);
        if (easing == "circularIn") return via(easing::circularIn, vs...);
        if (easing == "circularOut") return via(easing::circularOut, vs...);
        if (easing == "circularInOut") return via(easing::circularInOut, vs...);
        if (easing == "bounceIn") return via(easing::bounceIn, vs...);
        if (easing == "bounceOut") return via(easing::bounceOut, vs...);
        if (easing == "bounceInOut") return via(easing::bounceInOut, vs...);
        if (easing == "elasticIn") return via(easing::elasticIn, vs...);
        if (easing == "elasticOut") return via(easing::elasticOut, vs...);
        if (easing == "elasticInOut") return via(easing::elasticInOut, vs...);
        if (easing == "backIn") return via(easing::backIn, vs...);
        if (easing == "backOut") return via(easing::backOut, vs...);
        if (easing == "backInOut") return via(easing::backInOut, vs...);
        return via(easing::def, vs...);
    }

    template<typename T, typename... Ts>
    template<typename... Fs>
    tween <T, Ts...> & tween<T, Ts...>::via(const char * easing, Fs... vs) {
        return via(std::string(easing));
    }

    template<typename T, typename... Ts>
    template<typename... Ds>
    inline tween<T, Ts...> & tween<T, Ts...>::during(Ds... ds) {
        total = 0;
        points.at(points.size() - 2).during(ds...);
        for (detail::tweenpoint<T, Ts...> & p : points) {
            total += p.duration();
            p.stacked = total;
        }
        return *this;
    }

    template<typename T, typename... Ts>
    inline const typename detail::tweentraits<T, Ts...>::valuesType & tween<T, Ts...>::step(int32_t dt, bool suppress) {
        return step(static_cast<float>(dt)/static_cast<float>(total), suppress);
    }

    template<typename T, typename... Ts>
    inline const typename detail::tweentraits<T, Ts...>::valuesType & tween<T, Ts...>::step(uint32_t dt, bool suppress) {
        return step(static_cast<int32_t>(dt), suppress);
    }

    template<typename T, typename... Ts>
    inline const typename detail::tweentraits<T, Ts...>::valuesType & tween<T, Ts...>::step(float dp, bool suppress) {
        dp *= currentDirection;
        seek(currentProgress + dp, true);
        if (!suppress) dispatch(onStepCallbacks);
        return current;
    }

    template<typename T, typename... Ts>
    inline const typename detail::tweentraits<T, Ts...>::valuesType & tween<T, Ts...>::seek(float p, bool suppress) {
        p = detail::clip(p, 0.0f, 1.0f);
        currentProgress = p;
        render(p);
        if (!suppress) dispatch(onSeekCallbacks);
        return current;
    }

    template<typename T, typename... Ts>
    inline const typename detail::tweentraits<T, Ts...>::valuesType & tween<T, Ts...>::seek(int32_t t, bool suppress) {
        return seek(static_cast<float>(t) / static_cast<float>(total), suppress);
    }

    template<typename T, typename... Ts>
    inline uint32_t tween<T, Ts...>::duration() const {
        return total;
    }

    template<typename T, typename... Ts>
    template<size_t I>
    inline void tween<T, Ts...>::interpolate(float prog, unsigned point, typename traits::valuesType & values, detail::int2type<I>) const {
        auto & p = points.at(point);
        auto pointDuration = uint32_t(p.duration() - (p.stacked - (prog * static_cast<float>(total))));
        float pointTotal = static_cast<float>(pointDuration) / static_cast<float>(p.duration(I));
        if (pointTotal > 1.0f) pointTotal = 1.0f;
        auto easing = std::get<I>(p.easings);
        std::get<I>(values) = easing(pointTotal, std::get<I>(p.values), std::get<I>(points.at(point+1).values));
        interpolate(prog, point, values, detail::int2type<I-1>{ });
    }

    template<typename T, typename... Ts>
    inline void tween<T, Ts...>::interpolate(float prog, unsigned point, typename traits::valuesType & values, detail::int2type<0>) const {
        auto & p = points.at(point);
        auto pointDuration = uint32_t(p.duration() - (p.stacked - (prog * static_cast<float>(total))));
        float pointTotal = static_cast<float>(pointDuration) / static_cast<float>(p.duration(0));
        if (pointTotal > 1.0f) pointTotal = 1.0f;
        auto easing = std::get<0>(p.easings);
        std::get<0>(values) = easing(pointTotal, std::get<0>(p.values), std::get<0>(points.at(point+1).values));
    }

    template<typename T, typename... Ts>
    inline void tween<T, Ts...>::render(float p) {
        currentPoint = pointAt(p);
        interpolate(p, currentPoint, current, detail::int2type<sizeof...(Ts) - 1 + 1 /* +1 for the T */>{ });
    }

    template<typename T, typename... Ts>
    tween<T, Ts...> & tween<T, Ts...>::onStep(typename detail::tweentraits<T, Ts...>::callbackType callback) {
        onStepCallbacks.push_back(callback);
        return *this;
    }

    template<typename T, typename... Ts>
    tween<T, Ts...> & tween<T, Ts...>::onStep(typename detail::tweentraits<T, Ts...>::noValuesCallbackType callback) {
        onStepCallbacks.push_back([callback](tween<T, Ts...> & t, T, Ts...) { return callback(t); });
        return *this;
    }

    template<typename T, typename... Ts>
    tween<T, Ts...> & tween<T, Ts...>::onStep(typename detail::tweentraits<T, Ts...>::noTweenCallbackType callback) {
        onStepCallbacks.push_back([callback](tween<T, Ts...> &, T t, Ts... vs) { return callback(t, vs...); });
        return *this;
    }

    template<typename T, typename... Ts>
    tween<T, Ts...> & tween<T, Ts...>::onSeek(typename detail::tweentraits<T, Ts...>::callbackType callback) {
        onSeekCallbacks.push_back(callback);
        return *this;
    }

    template<typename T, typename... Ts>
    tween<T, Ts...> & tween<T, Ts...>::onSeek(typename detail::tweentraits<T, Ts...>::noValuesCallbackType callback) {
        onSeekCallbacks.push_back([callback](tween<T, Ts...> & t, T, Ts...) { return callback(t); });
        return *this;
    }

    template<typename T, typename... Ts>
    tween<T, Ts...> & tween<T, Ts...>::onSeek(typename detail::tweentraits<T, Ts...>::noTweenCallbackType callback) {
        onSeekCallbacks.push_back([callback](tween<T, Ts...> &, T t, Ts... vs) { return callback(t, vs...); });
        return *this;
    }

    template<typename T, typename... Ts>
    void tween<T, Ts...>::dispatch(std::vector<typename traits::callbackType> & cbVector) {
        std::vector<size_t> dismissed;
        for (size_t i = 0; i < cbVector.size(); ++i) {
            auto && cb = cbVector[i];
            bool dismiss = detail::call<bool>(cb, std::tuple_cat(std::make_tuple(std::ref(*this)), current));
            if (dismiss) dismissed.push_back(i);
        }

        if (dismissed.size() > 0) {
            for (size_t i = 0; i < dismissed.size(); ++i) {
                size_t index = dismissed[i];
                cbVector[index] = cbVector.at(cbVector.size() - 1 - i);
            }
            cbVector.resize(cbVector.size() - dismissed.size());
        }
    }

    template<typename T, typename... Ts>
    const typename detail::tweentraits<T, Ts...>::valuesType & tween<T, Ts...>::peek() const {
      return current;
    }

    template<typename T, typename... Ts>
    const typename detail::tweentraits<T, Ts...>::valuesType tween<T, Ts...>::peek(float progress) const {
        typename detail::tweentraits<T, Ts...>::valuesType values;
        interpolate(progress, pointAt(progress), values, detail::int2type<sizeof...(Ts) - 1 + 1 /* +1 for the T */>{ });
        return values;
    }

    template<typename T, typename... Ts>
    const typename detail::tweentraits<T, Ts...>::valuesType tween<T, Ts...>::peek(uint32_t time) const {
        typename detail::tweentraits<T, Ts...>::valuesType values;
        float progress = static_cast<float>(time) / static_cast<float>(total);
        interpolate(progress, pointAt(progress), values, detail::int2type<sizeof...(Ts) - 1 + 1 /* +1 for the T */>{ });
        return values;
    }

  template<typename T, typename... Ts>
    float tween<T, Ts...>::progress() const {
        return currentProgress;
    }

    template<typename T, typename... Ts>
    tween<T, Ts...> & tween<T, Ts...>::forward() {
        currentDirection = 1;
        return *this;
    }

    template<typename T, typename... Ts>
    tween<T, Ts...> & tween<T, Ts...>::backward() {
        currentDirection = -1;
        return *this;
    }

    template<typename T, typename... Ts>
    int tween<T, Ts...>::direction() const {
        return currentDirection;
    }

    template<typename T, typename... Ts>
    inline const typename detail::tweentraits<T, Ts...>::valuesType & tween<T, Ts...>::jump(std::size_t p, bool suppress) {
        p = detail::clip(p, static_cast<size_t>(0), points.size() -1);
        return seek(static_cast<int32_t>(points.at(p).stacked), suppress);
    }

    template<typename T, typename... Ts> inline uint16_t tween<T, Ts...>::point() const {
        return currentPoint;
    }

    template<typename T, typename... Ts> inline uint16_t tween<T, Ts...>::pointAt(float progress) const {
        uint32_t t = static_cast<uint32_t>(progress * total);
        uint16_t point = 0;
        while (t > points.at(point).stacked) point++;
        if (point > 0 && t <= points.at(point - 1u).stacked) point--;
        return point;
    }
}

#endif //TWEENY_TWEEN_TCC
