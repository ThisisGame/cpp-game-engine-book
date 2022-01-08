/************************************************************************
* file name         : common_functions.h
* ----------------- :
* creation time     : 2017/12/06
* author            : Victor Zarubkin
* email             : v.s.zarubkin@gmail.com
* ----------------- :
* description       : The file contains common functions used by different UI widgets.
* ----------------- :
* change log        : * 2017/12/06 Victor Zarubkin: Initial commit. Moved sources from common_types.h
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

#ifndef EASY_PROFILER_GUI_COMMON_FUNCTIONS_H
#define EASY_PROFILER_GUI_COMMON_FUNCTIONS_H

#include <sstream>
#include <stdlib.h>
#include <type_traits>

#include <QFont>
#include <QRgb>
#include <QString>
#include <QStyle>
#include <QWidget>

#include "common_types.h"

class QTreeWidgetItem;

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

#define PROF_MICROSECONDS(timestamp) (qreal(timestamp) * 1e-3)
//#define PROF_MICROSECONDS(timestamp) (timestamp)

#define PROF_FROM_MICROSECONDS(to_timestamp) static_cast<profiler::timestamp_t>((to_timestamp) * 1e3)
//#define PROF_FROM_MICROSECONDS(to_timestamp) (to_timestamp)

#define PROF_MILLISECONDS(timestamp) ((timestamp) * 1e-6)
//#define PROF_MILLISECONDS(timestamp) ((timestamp) * 1e-3)

#define PROF_FROM_MILLISECONDS(to_timestamp) ((to_timestamp) * 1e6)
//#define PROF_FROM_MILLISECONDS(to_timestamp) ((to_timestamp) * 1e3)

#define PROF_NANOSECONDS(timestamp) static_cast<profiler::timestamp_t>(timestamp)
//#define PROF_NANOSECONDS(timestamp) ((timestamp) * 1000)

//////////////////////////////////////////////////////////////////////////

EASY_FORCE_INLINE qreal units2microseconds(qreal _value) {
    return _value;
    //return _value * 1e3;
}

EASY_FORCE_INLINE qreal microseconds2units(qreal _value) {
    return _value;
    //return _value * 1e-3;
}

#ifdef EASY_CONSTEXPR_AVAILABLE
template <class TEnum>
EASY_FORCE_INLINE EASY_CONSTEXPR_FCN typename ::std::underlying_type<TEnum>::type int_cast(TEnum _enumValue) {
    return static_cast<typename ::std::underlying_type<TEnum>::type>(_enumValue);
}
#else
# define int_cast(_enumValue) static_cast<::std::underlying_type<decltype(_enumValue)>::type>(_enumValue)
#endif

//////////////////////////////////////////////////////////////////////////

namespace profiler_gui {

//////////////////////////////////////////////////////////////////////////

template <class T> inline
EASY_CONSTEXPR_FCN T numeric_max() {
    return ::std::numeric_limits<T>::max();
}

template <class T> inline
EASY_CONSTEXPR_FCN T numeric_max(T) {
    return ::std::numeric_limits<T>::max();
}

template <class T> inline
EASY_CONSTEXPR_FCN bool is_max(const T& _value) {
    return _value == ::std::numeric_limits<T>::max();
}

template <class T> inline
void set_max(T& _value) {
    _value = ::std::numeric_limits<T>::max();
}

//////////////////////////////////////////////////////////////////////////

inline EASY_CONSTEXPR_FCN QRgb alpha(::profiler::color_t _color) {
    return (_color & 0xff000000) >> 24;
}

inline EASY_CONSTEXPR_FCN QRgb toRgb(uint32_t _red, uint32_t _green, uint32_t _blue) {
    return (_red << 16) + (_green << 8) + _blue;
}

inline EASY_CONSTEXPR_FCN QRgb fromProfilerRgb(uint32_t _red, uint32_t _green, uint32_t _blue) {
    return _red == 0 && _green == 0 && _blue == 0 ? ::profiler::colors::Default : toRgb(_red, _green, _blue) | 0x00141414;
}

inline QRgb darken(::profiler::color_t _color, float _part) {
    const uint32_t r = (_color & 0x00ff0000) >> 16;
    const uint32_t g = (_color & 0x0000ff00) >> 8;
    const uint32_t b = _color & 0x000000ff;
    return (_color & 0xff000000) | toRgb(r - static_cast<uint32_t>(r * _part), g - static_cast<uint32_t>(g * _part), b - static_cast<uint32_t>(b * _part));
}

EASY_FORCE_INLINE EASY_CONSTEXPR_FCN qreal colorSum(::profiler::color_t _color) {
    return 255. - (((_color & 0x00ff0000) >> 16) * 0.299 + ((_color & 0x0000ff00) >> 8) * 0.587 + (_color & 0x000000ff) * 0.114);
}

inline EASY_CONSTEXPR_FCN bool isLightColor(::profiler::color_t _color) {
    return colorSum(_color) < 76.5 || ((_color & 0xff000000) >> 24) < 0x80;
}

inline EASY_CONSTEXPR_FCN bool isLightColor(::profiler::color_t _color, qreal _maxSum) {
    return colorSum(_color) < _maxSum || ((_color & 0xff000000) >> 24) < 0x80;
}

inline EASY_CONSTEXPR_FCN ::profiler::color_t textColorForFlag(bool _is_light) {
    return _is_light ? ::profiler::colors::Dark : ::profiler::colors::CreamWhite;
}

inline EASY_CONSTEXPR_FCN ::profiler::color_t textColorForRgb(::profiler::color_t _color) {
    return isLightColor(_color) ? ::profiler::colors::Dark : ::profiler::colors::CreamWhite;
}

inline uint32_t rand255() {
    return static_cast<uint32_t>(rand() % 255);
}

inline ::profiler::color_t randomColor() {
    return toRgb(rand255(), rand255(), rand255());
}

//////////////////////////////////////////////////////////////////////////

qreal timeFactor(qreal _interval);

QString autoTimeStringReal(qreal _interval, int _precision = 1);
QString autoTimeStringInt(qreal _interval);
QString autoTimeStringRealNs(::profiler::timestamp_t _interval, int _precision = 1);
QString autoTimeStringIntNs(::profiler::timestamp_t _interval);

QString timeStringReal(TimeUnits _units, qreal _interval, int _precision = 1);
QString timeStringRealNs(TimeUnits _units, ::profiler::timestamp_t _interval, int _precision = 1);
QString timeStringInt(TimeUnits _units, qreal _interval);
QString timeStringIntNs(TimeUnits _units, ::profiler::timestamp_t _interval);

//////////////////////////////////////////////////////////////////////////

QString shortenCountString(size_t count, int precision = 1);
QString shortenCountString(uint32_t count, int precision = 1);
QString shortenCountString(int64_t count, int precision = 1);
QString shortenCountString(int count, int precision = 1);

//////////////////////////////////////////////////////////////////////////

inline double percentReal(::profiler::timestamp_t _partial, ::profiler::timestamp_t _total) {
    return _total != 0 ? 100. * static_cast<double>(_partial) / static_cast<double>(_total) : 0.;
}

inline int percent(::profiler::timestamp_t _partial, ::profiler::timestamp_t _total) {
    return static_cast<int>(0.5 + percentReal(_partial, _total));
}

//////////////////////////////////////////////////////////////////////////

QFont EFont(QFont::StyleHint _hint, const char* _family, int _size, int _weight = -1);

inline QFont EFont(const char* _family, int _size, int _weight = -1) {
    return EFont(QFont::Helvetica, _family, _size, _weight);
}

//////////////////////////////////////////////////////////////////////////

QString valueTypeString(::profiler::DataType _dataType);
QString valueTypeString(const ::profiler::ArbitraryValue& _serializedValue);
QString valueString(const ::profiler::ArbitraryValue& _serializedValue);
QString shortValueString(const ::profiler::ArbitraryValue& _serializedValue);
QString valueString(const ::profiler::ArbitraryValue& _serializedValue, int _index);
int valueArraySize(const ::profiler::ArbitraryValue& _serializedValue);
double value2real(const ::profiler::ArbitraryValue& _serializedValue, int _index = 0);

//////////////////////////////////////////////////////////////////////////

template <class T>
void updateProperty(QWidget* widget, const char* name, T&& property)
{
    widget->setProperty(name, std::forward<T>(property));
    widget->style()->unpolish(widget);
    widget->style()->polish(widget);

    if (widget->isVisible())
    {
        widget->update();
    }
}

///////////////////////////////////////////////////////////////////////

void deleteTreeItem(QTreeWidgetItem* item);

///////////////////////////////////////////////////////////////////////

profiler::timestamp_t calculateMedian(const DurationsCountMap& durations);

///////////////////////////////////////////////////////////////////////

void clear_stream(std::stringstream& _stream);

} // END of namespace profiler_gui.

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

#endif // EASY_PROFILER_GUI_COMMON_FUNCTIONS_H
