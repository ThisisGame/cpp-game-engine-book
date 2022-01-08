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
 * The purpose of this file is to hold implementations for the tween.h file, s
 * pecializing on the single value case.
 */
#ifndef TWEENY_TWEENONE_TCC
#define TWEENY_TWEENONE_TCC

#include "tween.h"
#include "dispatcher.h"

namespace tweeny {
    template<typename T> inline tween<T> tween<T>::from(T t) { return tween<T>(t); }
    template<typename T> inline tween<T>::tween() { }
    template<typename T> inline tween<T>::tween(T t) {
        points.emplace_back(t);
    }

    template<typename T> inline tween<T> & tween<T>::to(T t) {
        points.emplace_back(t);
        return *this;
    }

    template<typename T>
    template<typename... Fs>
    inline tween<T> & tween<T>::via(Fs... vs) {
        points.at(points.size() - 2).via(vs...);
        return *this;
    }

    template<typename T>
    template<typename... Fs>
    inline tween<T> & tween<T>::via(int index, Fs... vs) {
        points.at(static_cast<size_t>(index)).via(vs...);
        return *this;
    }

    template<typename T>
    template<typename... Fs>
    tween <T> & tween<T>::via(easing::enumerated enumerated, Fs... vs) {
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

    template<typename T>
    template<typename... Fs>
    tween <T> & tween<T>::via(const std::string & easing, Fs... vs) {
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

    template<typename T>
    template<typename... Fs>
    tween <T> & tween<T>::via(const char * easing, Fs... vs) {
        return via(std::string(easing));
    }

    template<typename T>
    template<typename... Ds>
    inline tween<T> & tween<T>::during(Ds... ds) {
        total = 0;
        points.at(points.size() - 2).during(ds...);
        for (detail::tweenpoint<T> & p : points) {
            total += p.duration();
            p.stacked = total;
        }
        return *this;
    }

    template<typename T>
    inline const T & tween<T>::step(int32_t dt, bool suppress) {
        return step(static_cast<float>(dt)/static_cast<float>(total), suppress);
    }

    template<typename T>
    inline const T & tween<T>::step(uint32_t dt, bool suppress) {
        return step(static_cast<int32_t>(dt), suppress);
    }

    template<typename T>
    inline const T & tween<T>::step(float dp, bool suppress) {
        dp *= currentDirection;
        seek(currentProgress + dp, true);
        if (!suppress) dispatch(onStepCallbacks);
        return current;
    }

    template<typename T>
    inline const T & tween<T>::seek(float p, bool suppress) {
        p = detail::clip(p, 0.0f, 1.0f);
        currentProgress = p;
        render(p);
        if (!suppress) dispatch(onSeekCallbacks);
        return current;
    }

    template<typename T>
    inline const T & tween<T>::seek(int32_t t, bool suppress) {
        return seek(static_cast<float>(t) / static_cast<float>(total), suppress);
    }

    template<typename T>
    inline const T & tween<T>::seek(uint32_t t, bool suppress) {
        return seek(static_cast<float>(t) / static_cast<float>(total), suppress);
    }

    template<typename T>
    inline uint32_t tween<T>::duration() const {
        return total;
    }

    template<typename T>
    inline void tween<T>::interpolate(float prog, unsigned point, T & value) const {
        auto & p = points.at(point);
        auto pointDuration = uint32_t(p.duration() - (p.stacked - (prog * static_cast<float>(total))));
        float pointTotal = static_cast<float>(pointDuration) / static_cast<float>(p.duration());
        if (pointTotal > 1.0f) pointTotal = 1.0f;
        auto easing = std::get<0>(p.easings);
        value = easing(pointTotal, std::get<0>(p.values), std::get<0>(points.at(point+1).values));
    }

    template<typename T>
    inline void tween<T>::render(float p) {
        currentPoint = pointAt(p);
        interpolate(p, currentPoint, current);
    }

    template<typename T>
    tween<T> & tween<T>::onStep(typename detail::tweentraits<T>::callbackType callback) {
        onStepCallbacks.push_back(callback);
        return *this;
    }

    template<typename T>
    tween<T> & tween<T>::onStep(typename detail::tweentraits<T>::noValuesCallbackType callback) {
        onStepCallbacks.push_back([callback](tween<T> & tween, T) { return callback(tween); });
        return *this;
    }

    template<typename T>
    tween<T> & tween<T>::onStep(typename detail::tweentraits<T>::noTweenCallbackType callback) {
        onStepCallbacks.push_back([callback](tween<T> &, T v) { return callback(v); });
        return *this;
    }

    template<typename T>
    tween<T> & tween<T>::onSeek(typename detail::tweentraits<T>::callbackType callback) {
        onSeekCallbacks.push_back(callback);
        return *this;
    }

    template<typename T>
    tween<T> & tween<T>::onSeek(typename detail::tweentraits<T>::noValuesCallbackType callback) {
        onSeekCallbacks.push_back([callback](tween<T> & t, T) { return callback(t); });
        return *this;
    }

    template<typename T>
    tween<T> & tween<T>::onSeek(typename detail::tweentraits<T>::noTweenCallbackType callback) {
        onSeekCallbacks.push_back([callback](tween<T> &, T v) { return callback(v); });
        return *this;
    }

    template<typename T>
    void tween<T>::dispatch(std::vector<typename traits::callbackType> & cbVector) {
        std::vector<size_t> dismissed;
        for (size_t i = 0; i < cbVector.size(); ++i) {
            auto && cb = cbVector[i];
            bool dismiss = cb(*this, current);
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

    template<typename T>
    const T & tween<T>::peek() const {
        return current;
    }


    template<typename T>
    T tween<T>::peek(float progress) const {
        T value;
        interpolate(progress, pointAt(progress), value);
        return value;
    }

    template<typename T>
    T tween<T>::peek(uint32_t time) const {
        T value;
        float progress = static_cast<float>(time) / static_cast<float>(total);
        interpolate(progress, pointAt(progress), value);
        return value;
    }


  template<typename T>
    float tween<T>::progress() const {
        return currentProgress;
    }

    template<typename T>
    tween<T> & tween<T>::forward() {
        currentDirection = 1;
        return *this;
    }

    template<typename T>
    tween<T> & tween<T>::backward() {
        currentDirection = -1;
        return *this;
    }

    template<typename T>
    int tween<T>::direction() const {
        return currentDirection;
    }

    template<typename T>
    inline const T & tween<T>::jump(size_t p, bool suppress) {
        p = detail::clip(p, static_cast<size_t>(0), points.size() -1);
        return seek(points.at(p).stacked, suppress);
    }

    template<typename T> inline uint16_t tween<T>::point() const {
        return currentPoint;
    }



    template<typename T> inline uint16_t tween<T>::pointAt(float progress) const {
        auto t = static_cast<uint32_t>(progress * total);
        uint16_t point = 0;
        while (t > points.at(point).stacked) point++;
        if (point > 0 && t <= points.at(point - 1u).stacked) point--;
        return point;
    }
}
#endif //TWEENY_TWEENONE_TCC
