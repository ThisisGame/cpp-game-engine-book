/************************************************************************
* file name         : common_functions.cpp
* ----------------- :
* creation time     : 2017/12/06
* author            : Victor Zarubkin
* email             : v.s.zarubkin@gmail.com
* ----------------- :
* description       : The file contains implementaion of common functions used by different UI widgets.
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

#include <QList>
#include <QTreeWidgetItem>
#include "common_functions.h"

template <class T>
static QString toString(const profiler::ArbitraryValue& _serializedValue, int _index) {
    return QString::number(_serializedValue.toArray<T>()->at(_index));
}

template <class T>
static QString toString(const profiler::ArbitraryValue& _serializedValue) {
    return QString::number(_serializedValue.toValue<T>()->value());
}

template <class T>
static double toReal(const profiler::ArbitraryValue& _serializedValue, int _index) {
    return static_cast<double>(_serializedValue.toArray<T>()->at(_index));
}

template <class T>
static double toReal(const profiler::ArbitraryValue& _serializedValue) {
    return static_cast<double>(_serializedValue.toValue<T>()->value());
}

template <profiler::DataType type>
inline EASY_CONSTEXPR_FCN uint16_t sizeOf() {
    return static_cast<uint16_t>(sizeof(typename profiler::StdType<type>::value_type));
}

static QString arrayToString(const profiler::ArbitraryValue& _serializedValue, int _index)
{
    switch (_serializedValue.type())
    {
        case profiler::DataType::Bool:
        {
            const auto value = _serializedValue.toArray<bool>()->at(_index);
            return value ? QStringLiteral("true") : QStringLiteral("false");
        }

        case profiler::DataType::Char:   return QChar(_serializedValue.toArray<char>  ()->at(_index));
        case profiler::DataType::Int8:   return QChar(_serializedValue.toArray<int8_t>()->at(_index));
        case profiler::DataType::Uint8:  return toString<uint8_t> (_serializedValue, _index);
        case profiler::DataType::Int16:  return toString<int16_t> (_serializedValue, _index);
        case profiler::DataType::Uint16: return toString<uint16_t>(_serializedValue, _index);
        case profiler::DataType::Int32:  return toString<int32_t> (_serializedValue, _index);
        case profiler::DataType::Uint32: return toString<uint32_t>(_serializedValue, _index);
        case profiler::DataType::Int64:  return toString<int64_t> (_serializedValue, _index);
        case profiler::DataType::Uint64: return toString<uint64_t>(_serializedValue, _index);
        case profiler::DataType::Float:  return toString<float>   (_serializedValue, _index);
        case profiler::DataType::Double: return toString<double>  (_serializedValue, _index);
        case profiler::DataType::String: return QChar(_serializedValue.data()[_index]);
        default: return QStringLiteral("??");
    }
}

static QString singleValueToString(const profiler::ArbitraryValue& _serializedValue)
{
    switch (_serializedValue.type())
    {
        case profiler::DataType::Bool:   return _serializedValue.toValue<bool>()->value() ? QStringLiteral("true") : QStringLiteral("false");
        case profiler::DataType::Char:   return QChar(_serializedValue.toValue<char>  ()->value());
        case profiler::DataType::Int8:   return QChar(_serializedValue.toValue<int8_t>()->value());
        case profiler::DataType::Uint8:  return toString<uint8_t> (_serializedValue);
        case profiler::DataType::Int16:  return toString<int16_t> (_serializedValue);
        case profiler::DataType::Uint16: return toString<uint16_t>(_serializedValue);
        case profiler::DataType::Int32:  return toString<int32_t> (_serializedValue);
        case profiler::DataType::Uint32: return toString<uint32_t>(_serializedValue);
        case profiler::DataType::Int64:  return toString<int64_t> (_serializedValue);
        case profiler::DataType::Uint64: return toString<uint64_t>(_serializedValue);
        case profiler::DataType::Float:  return toString<float>   (_serializedValue);
        case profiler::DataType::Double: return toString<double>  (_serializedValue);
        case profiler::DataType::String: return _serializedValue.data();
        default: return QStringLiteral("??");
    }
}

template <class T>
static QString shortenCountStringUnsigned(T count, int precision)
{
    if (count >= 1000000)
        return QStringLiteral("%1m").arg(QString::number(count * 1e-6, 'f', precision));

    if (count >= 1000)
        return QStringLiteral("%1k").arg(QString::number(count * 1e-3, 'f', precision));

    return QString::number(count);
}

template <class T>
static QString shortenCountStringSigned(T count, int precision)
{
    const auto absCount = std::abs(count);

    if (absCount >= 1000000)
        return QStringLiteral("%1m").arg(QString::number(count * 1e-6, 'f', precision));

    if (absCount >= 1000)
        return QStringLiteral("%1k").arg(QString::number(count * 1e-3, 'f', precision));

    return QString::number(count);
}

namespace profiler_gui {

    //////////////////////////////////////////////////////////////////////////

    qreal timeFactor(qreal _interval)
    {
        if (_interval < 1) // interval in nanoseconds
            return 1e3;

        if (_interval < 1e3) // interval in microseconds
            return 1;

        if (_interval < 1e6) // interval in milliseconds
            return 1e-3;

        // interval in seconds
        return 1e-6;
    }

    //////////////////////////////////////////////////////////////////////////

    QString autoTimeStringReal(qreal _interval, int _precision)
    {
        if (_interval < 1) // interval in nanoseconds
            return QString("%1 ns").arg(static_cast<quint64>(_interval * 1e3));

        if (_interval < 1e3) // interval in microseconds
            return QString("%1 us").arg(_interval, 0, 'f', _precision);

        if (_interval < 1e6) // interval in milliseconds
            return QString("%1 ms").arg(_interval * 1e-3, 0, 'f', _precision);

        // interval in seconds
        return QString("%1 s").arg(_interval * 1e-6, 0, 'f', _precision);
    }

    QString autoTimeStringInt(qreal _interval)
    {
        if (_interval < 1) // interval in nanoseconds
            return QString("%1 ns").arg(static_cast<quint64>(_interval * 1e3 + 0.5));

        if (_interval < 1e3) // interval in microseconds
            return QString("%1 us").arg(static_cast<quint32>(_interval + 0.5));

        if (_interval < 1e6) // interval in milliseconds
            return QString("%1 ms").arg(static_cast<quint32>(_interval * 1e-3 + 0.5));

        // interval in seconds
        return QString("%1 s").arg(static_cast<quint32>(_interval * 1e-6 + 0.5));
    }

    QString autoTimeStringRealNs(::profiler::timestamp_t _interval, int _precision)
    {
        if (_interval < 1000) // interval in nanoseconds
            return QString("%1 ns").arg(_interval);

        if (_interval < 1000000) // interval in microseconds
            return QString("%1 us").arg(_interval * 1e-3, 0, 'f', _precision);

        if (_interval < 1000000000U) // interval in milliseconds
            return QString("%1 ms").arg(_interval * 1e-6, 0, 'f', _precision);

        // interval in seconds
        return QString("%1 s").arg(_interval * 1e-9, 0, 'f', _precision);
    }

    QString autoTimeStringIntNs(::profiler::timestamp_t _interval)
    {
        if (_interval < 1000) // interval in nanoseconds
            return QString("%1 ns").arg(_interval);

        if (_interval < 1000000) // interval in microseconds
            return QString("%1 us").arg(static_cast<quint32>(_interval * 1e-3 + 0.5));

        if (_interval < 1000000000U) // interval in milliseconds
            return QString("%1 ms").arg(static_cast<quint32>(_interval * 1e-6 + 0.5));

        // interval in seconds
        return QString("%1 s").arg(static_cast<quint32>(_interval * 1e-9 + 0.5));
    }

    //////////////////////////////////////////////////////////////////////////

    QString timeStringReal(TimeUnits _units, qreal _interval, int _precision)
    {
        switch (_units)
        {
            case TimeUnits_ms:{
                const char fmt = _interval <= 1 ? 'g' : 'f';
                return QString("%1 ms").arg(_interval * 1e-3, 0, fmt, _precision);
            }

            case TimeUnits_us:
                return QString("%1 us").arg(_interval, 0, 'f', _precision);

            case TimeUnits_ns:
                return QString("%1 ns").arg(static_cast<quint64>(_interval * 1e3 + 0.5));

            case TimeUnits_auto:
            default:
                return autoTimeStringReal(_interval, _precision);
        }
    }

    QString timeStringRealNs(TimeUnits _units, ::profiler::timestamp_t _interval, int _precision)
    {
        switch (_units)
        {
            case TimeUnits_ms:{
                const char fmt = _interval <= 1000 ? 'g' : 'f';
                return QString("%1 ms").arg(_interval * 1e-6, 0, fmt, _precision);
            }

            case TimeUnits_us:
                return QString("%1 us").arg(_interval * 1e-3, 0, 'f', _precision);

            case TimeUnits_ns:
                return QString("%1 ns").arg(_interval);

            case TimeUnits_auto:
            default:
                return autoTimeStringRealNs(_interval, _precision);
        }
    }

    QString timeStringInt(TimeUnits _units, qreal _interval)
    {
        switch (_units)
        {
            case TimeUnits_ms:
                return QString("%1 ms").arg(static_cast<quint32>(_interval * 1e-3 + 0.5));

            case TimeUnits_us:
                return QString("%1 us").arg(static_cast<quint32>(_interval + 0.5));

            case TimeUnits_ns:
                return QString("%1 ns").arg(static_cast<quint64>(_interval * 1e3 + 0.5));

            case TimeUnits_auto:
            default:
                return autoTimeStringInt(_interval);
        }
    }

    QString timeStringIntNs(TimeUnits _units, ::profiler::timestamp_t _interval)
    {
        switch (_units)
        {
            case TimeUnits_ms:
                return QString("%1 ms").arg(static_cast<quint32>(_interval * 1e-6 + 0.5));

            case TimeUnits_us:
                return QString("%1 us").arg(static_cast<quint32>(_interval * 1e-3 + 0.5));

            case TimeUnits_ns:
                return QString("%1 ns").arg(_interval);

            case TimeUnits_auto:
            default:
                return autoTimeStringIntNs(_interval);
        }
    }

    //////////////////////////////////////////////////////////////////////////

    QString shortenCountString(size_t count, int precision)
    {
        return shortenCountStringUnsigned(count, precision);
    }

    QString shortenCountString(uint32_t count, int precision)
    {
        return shortenCountStringUnsigned(count, precision);
    }

    QString shortenCountString(int64_t count, int precision)
    {
        return shortenCountStringSigned(count, precision);
    }

    QString shortenCountString(int count, int precision)
    {
        return shortenCountStringSigned(count, precision);
    }

    //////////////////////////////////////////////////////////////////////////

    QFont EFont(QFont::StyleHint _hint, const char* _family, int _size, int _weight)
    {
        QFont f(_family, _size, _weight);
        f.setStyleHint(_hint, QFont::PreferMatch);
        return f;
    }

    //////////////////////////////////////////////////////////////////////////

    QString valueTypeString(::profiler::DataType _dataType)
    {
        switch (_dataType)
        {
            case ::profiler::DataType::Bool:   return QStringLiteral("bool");
            case ::profiler::DataType::Char:   return QStringLiteral("char");
            case ::profiler::DataType::Int8:   return QStringLiteral("int8");
            case ::profiler::DataType::Uint8:  return QStringLiteral("unsigned int8");
            case ::profiler::DataType::Int16:  return QStringLiteral("int16");
            case ::profiler::DataType::Uint16: return QStringLiteral("unsigned int16");
            case ::profiler::DataType::Int32:  return QStringLiteral("int32");
            case ::profiler::DataType::Uint32: return QStringLiteral("unsigned int32");
            case ::profiler::DataType::Int64:  return QStringLiteral("int64");
            case ::profiler::DataType::Uint64: return QStringLiteral("unsigned int64");
            case ::profiler::DataType::Float:  return QStringLiteral("float");
            case ::profiler::DataType::Double: return QStringLiteral("double");
            case ::profiler::DataType::String: return QStringLiteral("string");
            default: return QStringLiteral("unknown");
        }
    }

    QString valueTypeString(const ::profiler::ArbitraryValue& _serializedValue)
    {
        const auto type = _serializedValue.type();
        if (_serializedValue.isArray() && type != ::profiler::DataType::String)
            return valueTypeString(type) + QStringLiteral("[]");
        return valueTypeString(type);
    }

    QString valueString(const ::profiler::ArbitraryValue& _serializedValue)
    {
        if (_serializedValue.isArray())
        {
            if (_serializedValue.type() == ::profiler::DataType::String)
                return _serializedValue.data();

            auto str = QString("[%1").arg(valueString(_serializedValue, 0));
            const int size = valueArraySize(_serializedValue);
            for (int i = 1; i < size; ++i)
                str.append(QString(", %1").arg(valueString(_serializedValue, i)));
            str.append(QChar(']'));

            return str;
        }

        return singleValueToString(_serializedValue);
    }

    QString shortValueString(const ::profiler::ArbitraryValue& _serializedValue)
    {
        if (_serializedValue.isArray())
        {
            if (_serializedValue.type() == ::profiler::DataType::String)
                return _serializedValue.data();

            auto str = QString("[%1").arg(valueString(_serializedValue, 0));
            const int size = valueArraySize(_serializedValue);
            if (size < 7)
            {
                for (int i = 1; i < size; ++i)
                    str.append(QString(", %1").arg(valueString(_serializedValue, i)));
            }
            else
            {
                for (int i = 1; i < 6; ++i)
                    str.append(QString(", %1").arg(valueString(_serializedValue, i)));
                str.append(QString(", ..., %1").arg(valueString(_serializedValue, size - 1)));
            }

            str.append(QChar(']'));
            return str;
        }

        return singleValueToString(_serializedValue);
    }

    QString valueString(const ::profiler::ArbitraryValue& _serializedValue, int _index)
    {
        if (_serializedValue.isArray())
            return arrayToString(_serializedValue, _index);
        return singleValueToString(_serializedValue);
    }

    int valueArraySize(const ::profiler::ArbitraryValue& _serializedValue)
    {
        EASY_STATIC_CONSTEXPR uint16_t DataSizes[] = {
            sizeOf<::profiler::DataType::Bool>(),
            sizeOf<::profiler::DataType::Char>(),
            sizeOf<::profiler::DataType::Int8>(),
            sizeOf<::profiler::DataType::Uint8>(),
            sizeOf<::profiler::DataType::Int16>(),
            sizeOf<::profiler::DataType::Uint16>(),
            sizeOf<::profiler::DataType::Int32>(),
            sizeOf<::profiler::DataType::Uint32>(),
            sizeOf<::profiler::DataType::Int64>(),
            sizeOf<::profiler::DataType::Uint64>(),
            sizeOf<::profiler::DataType::Float>(),
            sizeOf<::profiler::DataType::Double>(),
            sizeOf<::profiler::DataType::String>(),
            1
        };

        return _serializedValue.data_size() / DataSizes[int_cast(_serializedValue.type())];
    }

    double value2real(const ::profiler::ArbitraryValue& _serializedValue, int _index)
    {
        if (_serializedValue.isArray())
        {
            switch (_serializedValue.type())
            {
                case ::profiler::DataType::Bool:
                {
                    const auto value = _serializedValue.toArray<bool>()->at(_index);
                    return value ? 1 : 0;
                }

                case ::profiler::DataType::Char:   return toReal<char>    (_serializedValue, _index);
                case ::profiler::DataType::Int8:   return toReal<int8_t>  (_serializedValue, _index);
                case ::profiler::DataType::Uint8:  return toReal<uint8_t> (_serializedValue, _index);
                case ::profiler::DataType::Int16:  return toReal<int16_t> (_serializedValue, _index);
                case ::profiler::DataType::Uint16: return toReal<uint16_t>(_serializedValue, _index);
                case ::profiler::DataType::Int32:  return toReal<int32_t> (_serializedValue, _index);
                case ::profiler::DataType::Uint32: return toReal<uint32_t>(_serializedValue, _index);
                case ::profiler::DataType::Int64:  return toReal<int64_t> (_serializedValue, _index);
                case ::profiler::DataType::Uint64: return toReal<uint64_t>(_serializedValue, _index);
                case ::profiler::DataType::Float:  return toReal<float>   (_serializedValue, _index);
                case ::profiler::DataType::Double: return _serializedValue.toArray<double>()->at(_index);
                case ::profiler::DataType::String: return static_cast<double>(_serializedValue.data()[_index]);
                default: return 0;
            }
        }

        switch (_serializedValue.type())
        {
            case ::profiler::DataType::Bool:
            {
                const auto value = _serializedValue.toValue<bool>()->value();
                return value ? 1 : 0;
            }

            case ::profiler::DataType::Char:   return toReal<char>    (_serializedValue);
            case ::profiler::DataType::Int8:   return toReal<int8_t>  (_serializedValue);
            case ::profiler::DataType::Uint8:  return toReal<uint8_t> (_serializedValue);
            case ::profiler::DataType::Int16:  return toReal<int16_t> (_serializedValue);
            case ::profiler::DataType::Uint16: return toReal<uint16_t>(_serializedValue);
            case ::profiler::DataType::Int32:  return toReal<int32_t> (_serializedValue);
            case ::profiler::DataType::Uint32: return toReal<uint32_t>(_serializedValue);
            case ::profiler::DataType::Int64:  return toReal<int64_t> (_serializedValue);
            case ::profiler::DataType::Uint64: return toReal<uint64_t>(_serializedValue);
            case ::profiler::DataType::Float:  return toReal<float>   (_serializedValue);
            case ::profiler::DataType::Double: return _serializedValue.toValue<double>()->value();
            case ::profiler::DataType::String: return static_cast<double>(_serializedValue.data()[_index]);
            default: return 0;
        }
    }

    //////////////////////////////////////////////////////////////////////////

    void deleteTreeItem(QTreeWidgetItem* item)
    {
        if (item == nullptr)
        {
            return;
        }

        QList<QTreeWidgetItem*> stack;
        stack.append(item);

        while (!stack.isEmpty())
        {
            auto i = stack.takeFirst();
            stack.append(i->takeChildren());
            delete i;
        }
    }

    //////////////////////////////////////////////////////////////////////////

    profiler::timestamp_t calculateMedian(const DurationsCountMap& durations)
    {
        if (durations.empty())
        {
            return 0;
        }

        profiler::timestamp_t median = 0;

        size_t total_count = 0;
        for (auto& kv : durations)
        {
            total_count += kv.second.count;
        }

        if (total_count & 1)
        {
            const auto index = total_count >> 1;
            size_t i = 0;
            for (auto& kv : durations)
            {
                const auto count = kv.second.count;

                i += count;
                if (i < index)
                {
                    continue;
                }

                median = kv.first;
                break;
            }
        }
        else
        {
            const auto index2 = total_count >> 1;
            const auto index1 = index2 - 1;

            size_t i = 0;
            bool i1 = false;
            for (auto& kv : durations)
            {
                const auto count = kv.second.count;

                i += count;
                if (i < index1)
                {
                    continue;
                }

                if (!i1)
                {
                    i1 = true;
                    median = kv.first;
                }

                if (i < index2)
                {
                    continue;
                }

                median += kv.first;
                median >>= 1;

                break;
            }
        }

        return median;
    }

    //////////////////////////////////////////////////////////////////////////

    void clear_stream(std::stringstream& _stream)
    {
#if defined(__GNUC__) && __GNUC__ < 5
        // gcc 4 has a known bug which has been solved in gcc 5:
        // std::stringstream has no swap() method :(
        _stream.str(std::string());
#else
        std::stringstream().swap(_stream);
#endif
    }

} // end of namespace profiler_gui.
