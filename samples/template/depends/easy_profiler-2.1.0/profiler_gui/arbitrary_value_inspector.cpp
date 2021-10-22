/************************************************************************
* file name         : arbitrary_value_inspector.cpp
* ----------------- :
* creation time     : 2017/11/30
* author            : Victor Zarubkin
* email             : v.s.zarubkin@gmail.com
* ----------------- :
* description       : The file contains implementation of .
* ----------------- :
* change log        : * 2017/11/30 Victor Zarubkin: initial commit.
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

#include <QAction>
#include <QActionGroup>
#include <QColor>
#include <QComboBox>
#include <QFileDialog>
#include <QFileInfo>
#include <QGraphicsScene>
#include <QHeaderView>
#include <QLabel>
#include <QMessageBox>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QResizeEvent>
#include <QSettings>
#include <QSpinBox>
#include <QSplitter>
#include <QToolBar>
#include <QVariant>
#include <QVBoxLayout>
#include <list>
#include <set>
#include <cmath>
#include "arbitrary_value_inspector.h"
#include "dialog.h"
#include "globals.h"
#include "complexity_calculator.h"

//////////////////////////////////////////////////////////////////////////

EASY_CONSTEXPR int ChartBound = 2; ///< Top and bottom bounds for chart
EASY_CONSTEXPR int ChartBounds = ChartBound << 1;

void gaussFilter(std::vector<QPointF>& _points, int _windowSize, const std::atomic_bool& _interrupt)
{
    if (_points.size() < 3 || _windowSize < 3 || _interrupt.load(std::memory_order_acquire))
        return;

    std::vector<QPointF> out;
    out.reserve(_points.size());

    for (size_t i = 0, size = _points.size(); i < size; ++i)
    {
        if (_interrupt.load(std::memory_order_acquire))
            return;

        const auto next = i + 1;
        qreal sum = 0;

        if (static_cast<int>(next) < _windowSize)
        {
            for (size_t j = 0; j <= i; ++j)
                sum += _points[j].y();
            sum /= i + 1;
        }
        else
        {
            for (size_t j = next - _windowSize; j <= i; ++j)
                sum += _points[j].y();
            sum /= _windowSize;
        }

        out.emplace_back(_points[i].x(), sum);
    }

    _points = std::move(out);
}

void medianFilter(std::vector<QPointF>& _points, int _windowSize, const std::atomic_bool& _interrupt)
{
    if (_points.size() < 3 || _windowSize < 3 || _interrupt.load(std::memory_order_acquire))
        return;

    const auto windowSizeHalf = _windowSize >> 1;

    std::vector<QPointF> out;
    out.reserve(_points.size());

    std::vector<qreal> window(static_cast<size_t>(_windowSize));

    for (size_t i = 0, size = _points.size(); i < size; ++i)
    {
        if (_interrupt.load(std::memory_order_acquire))
            return;

        const auto next = i + 1;
        if (next < windowSizeHalf)
        {
            int k = 0;
            for (int j = static_cast<int>(next) - windowSizeHalf; k < _windowSize; ++j, ++k)
                window[k] = _points[abs(j)].y();
        }
        else
        {
            int k = 0;
            for (size_t j = next - windowSizeHalf; k < _windowSize; ++j, ++k)
                window[k] = _points[j].y();
        }

        std::sort(window.begin(), window.end());
        out.emplace_back(_points[i].x(), window[windowSizeHalf]);
    }

    _points = std::move(out);
}

void getChartPoints(const ArbitraryValuesCollection& _collection, Points& _points, qreal& _minValue, qreal& _maxValue)
{
    _minValue =  1e300;
    _maxValue = -1e300;
    _points.clear();

    const auto& values = _collection.values();
    if (values.empty())
        return;

    _points.reserve(values.size());
    for (auto value : values)
    {
        const qreal x = sceneX(value->begin());
        const qreal y = profiler_gui::value2real(*value);
        _points.emplace_back(x, y);

        if (y > _maxValue)
            _maxValue = y;
        if (y < _minValue)
            _minValue = y;
    }
}

//////////////////////////////////////////////////////////////////////////

ArbitraryValuesCollection::ArbitraryValuesCollection()
    : m_beginTime(0)
    , m_minDuration(profiler_gui::numeric_max<decltype(m_minDuration)>())
    , m_maxDuration(0)
    , m_minValue(1e300)
    , m_maxValue(-1e300)
    , m_chartType(ChartType::Regular)
    , m_jobType(0)
{
    m_status = Idle;
    m_bInterrupt = false;
}

ArbitraryValuesCollection::~ArbitraryValuesCollection()
{
    interrupt();
}

ChartType ArbitraryValuesCollection::chartType() const
{
    return m_chartType;
}

const ArbitraryValues& ArbitraryValuesCollection::values() const
{
    return m_values;
}

const ComplexityValuesMap& ArbitraryValuesCollection::complexityMap() const
{
    return m_complexityMap;
}

const Points& ArbitraryValuesCollection::points() const
{
    return m_points;
}

ArbitraryValuesCollection::JobStatus ArbitraryValuesCollection::status() const
{
    return static_cast<JobStatus>(m_status.load(std::memory_order_acquire));
}

profiler::timestamp_t ArbitraryValuesCollection::minDuration() const
{
    return m_minDuration;
}

profiler::timestamp_t ArbitraryValuesCollection::maxDuration() const
{
    return m_maxDuration;
}

qreal ArbitraryValuesCollection::minValue() const
{
    return m_minValue;
}

qreal ArbitraryValuesCollection::maxValue() const
{
    return m_maxValue;
}

void ArbitraryValuesCollection::collectValues(ChartType _chartType, profiler::thread_id_t _threadId, profiler::vin_t _valueId
    , const char* _valueName, profiler::block_id_t _parentBlockId, int _index)
{
    interrupt();

    setStatus(InProgress);
    m_points.clear();
    m_chartType = _chartType;
    m_jobType = ValuesJob;

    if (m_chartType == ChartType::Complexity && profiler_gui::is_max(_parentBlockId))
    {
        setStatus(Ready);
        return;
    }

    if (_valueId == 0)
        m_worker.enqueue([=] { collectByName(_threadId, _valueName, _parentBlockId, _index); }, m_bInterrupt);
    else
        m_worker.enqueue([=] { collectById(_threadId, _valueId, _parentBlockId, _index); }, m_bInterrupt);
}

void ArbitraryValuesCollection::collectValuesAndPoints(ChartType _chartType, profiler::thread_id_t _threadId, profiler::vin_t _valueId
    , const char* _valueName, profiler::timestamp_t _beginTime, profiler::block_id_t _parentBlockId, int _index)
{
    interrupt();

    setStatus(InProgress);
    m_points.clear();
    m_beginTime = _beginTime;
    m_minValue =  1e300;
    m_maxValue = -1e300;
    m_chartType = _chartType;
    m_jobType = ValuesJob | PointsJob;

    if (m_chartType == ChartType::Complexity && profiler_gui::is_max(_parentBlockId))
    {
        setStatus(Ready);
        return;
    }

    if (_valueId == 0)
        m_worker.enqueue([=] { collectByName(_threadId, _valueName, _parentBlockId, _index); }, m_bInterrupt);
    else
        m_worker.enqueue([=] { collectById(_threadId, _valueId, _parentBlockId, _index); }, m_bInterrupt);
}

bool ArbitraryValuesCollection::calculatePoints(profiler::timestamp_t _beginTime)
{
    if (status() != Ready || m_values.empty())
        return false;

    m_worker.dequeue();

    setStatus(InProgress);
    m_points.clear();
    m_beginTime = _beginTime;
    m_minValue =  1e300;
    m_maxValue = -1e300;
    m_jobType = PointsJob;

    m_worker.enqueue([this]
    {
        getChartPoints(*this, m_points, m_minValue, m_maxValue);
        setStatus(Ready);
    }, m_bInterrupt);

    return true;
}

void ArbitraryValuesCollection::interrupt()
{
    m_worker.dequeue();
    setStatus(Idle);
    m_jobType = None;
    m_values.clear();
    m_complexityMap.clear();
    profiler_gui::set_max(m_minDuration);
    m_maxDuration = 0;
}

void ArbitraryValuesCollection::setStatus(JobStatus _status)
{
    m_status.store(static_cast<uint8_t>(_status), std::memory_order_release);
}

void ArbitraryValuesCollection::collectById(profiler::thread_id_t _threadId, profiler::vin_t _valueId
    , profiler::block_id_t _parentBlockId, int _index)
{
    const bool doCalculatePoints = (m_jobType & PointsJob) != 0;

    if (_threadId == 0)
    {
        const auto threadsCount = EASY_GLOBALS.profiler_blocks.size();
        if (threadsCount != 0)
        {
            const bool calculatePointsInner = doCalculatePoints && threadsCount == 1;

            for (const auto& it : EASY_GLOBALS.profiler_blocks)
            {
                if (!collectByIdForThread(it.second, _valueId, calculatePointsInner, _parentBlockId, _index))
                    return;
            }

            if (threadsCount > 1)
            {
                using Val = const profiler::ArbitraryValue*;
                std::sort(m_values.begin(), m_values.end(), [] (Val lhs, Val rhs) {
                    return lhs->begin() < rhs->begin();
                });
            }

            if (doCalculatePoints && !calculatePointsInner)
                getChartPoints(*this, m_points, m_minValue, m_maxValue);
        }
    }
    else
    {
        const auto t = EASY_GLOBALS.profiler_blocks.find(_threadId);
        if (t != EASY_GLOBALS.profiler_blocks.end() &&
            !collectByIdForThread(t->second, _valueId, doCalculatePoints, _parentBlockId, _index))
        {
            return;
        }
    }

    if (m_chartType == ChartType::Complexity)
    {
        for (auto& it : m_complexityMap)
        {
            if (m_bInterrupt.load(std::memory_order_acquire))
                return;
            auto& durations = it.second;
            std::sort(durations.begin(), durations.end());
        }
    }

    setStatus(Ready);
}

bool ArbitraryValuesCollection::collectByIdForThread(const profiler::BlocksTreeRoot& _threadRoot
    , profiler::vin_t _valueId, bool _calculatePoints, profiler::block_id_t _parentBlockId, int _index)
{
    if (profiler_gui::is_max(_parentBlockId))
    {
        // All values

        for (auto i : _threadRoot.events)
        {
            if (m_bInterrupt.load(std::memory_order_acquire))
                return false;

            const auto& block = easyBlock(i).tree;
            const auto& desc = easyDescriptor(block.node->id());
            if (desc.type() != profiler::BlockType::Value)
                continue;

            const auto value = block.value;
            if (value->value_id() != _valueId)
                continue;

            if (_index >= 0 && (!value->isArray() || profiler_gui::valueArraySize(*value) <= _index))
                continue;

            m_values.push_back(value);
            if (_calculatePoints)
                addPoint(*value, _index);
        }

        return true;
    }

    return depthFirstSearch(_threadRoot, _calculatePoints, _parentBlockId, _index
        , [=] (profiler::vin_t _id, const char*) -> bool { return _id == _valueId; });
}

void ArbitraryValuesCollection::collectByName(profiler::thread_id_t _threadId, const std::string _valueName
    , profiler::block_id_t _parentBlockId, int _index)
{
    const bool doCalculatePoints = (m_jobType & PointsJob) != 0;

    if (_threadId == 0)
    {
        const auto threadsCount = EASY_GLOBALS.profiler_blocks.size();
        if (threadsCount != 0)
        {
            const bool calculatePointsInner = doCalculatePoints && threadsCount == 1;

            for (const auto& it : EASY_GLOBALS.profiler_blocks)
            {
                if (!collectByNameForThread(it.second, _valueName, calculatePointsInner, _parentBlockId, _index))
                    return;
            }

            if (threadsCount > 1)
            {
                using Val = const profiler::ArbitraryValue*;
                std::sort(m_values.begin(), m_values.end(), [] (Val lhs, Val rhs) {
                    return lhs->begin() < rhs->begin();
                });
            }

            if (doCalculatePoints && !calculatePointsInner)
                getChartPoints(*this, m_points, m_minValue, m_maxValue);
        }
    }
    else
    {
        const auto t = EASY_GLOBALS.profiler_blocks.find(_threadId);
        if (t != EASY_GLOBALS.profiler_blocks.end() &&
            !collectByNameForThread(t->second, _valueName, doCalculatePoints, _parentBlockId, _index))
        {
            return;
        }
    }

    if (m_chartType == ChartType::Complexity)
    {
        for (auto& it : m_complexityMap)
        {
            if (m_bInterrupt.load(std::memory_order_acquire))
                return;
            auto& durations = it.second;
            std::sort(durations.begin(), durations.end());
        }
    }

    setStatus(Ready);
}

bool ArbitraryValuesCollection::collectByNameForThread(const profiler::BlocksTreeRoot& _threadRoot
    , const std::string& _valueName, bool _calculatePoints, profiler::block_id_t _parentBlockId, int _index)
{
    if (profiler_gui::is_max(_parentBlockId))
    {
        // All values

        for (auto i : _threadRoot.events)
        {
            if (m_bInterrupt.load(std::memory_order_acquire))
                return false;

            const auto& block = easyBlock(i).tree;
            const auto& desc = easyDescriptor(block.node->id());
            if (desc.type() != profiler::BlockType::Value || _valueName != desc.name())
                continue;

            const auto value = block.value;
            if (_index >= 0 && (!value->isArray() || profiler_gui::valueArraySize(*value) <= _index))
                continue;

            m_values.push_back(value);
            if (_calculatePoints)
                addPoint(*value, _index);
        }

        return true;
    }

    return depthFirstSearch(_threadRoot, _calculatePoints, _parentBlockId, _index
        , [&_valueName] (profiler::vin_t, const char* _name) -> bool { return _valueName == _name; });
}

bool ArbitraryValuesCollection::depthFirstSearch(const profiler::BlocksTreeRoot& _threadRoot, bool _calculatePoints
    , profiler::block_id_t _parentBlockId, int _index, std::function<bool(profiler::vin_t, const char*)> _isSuitableValue)
{
    if (_threadRoot.children.empty())
        return true;

    using StackEntry = std::pair<profiler::block_index_t, profiler::block_index_t>;
    using Stack = std::vector<StackEntry>;

    Stack stack;
    for (const auto index : _threadRoot.children)
    {
        if (m_bInterrupt.load(std::memory_order_acquire))
            return false;

        stack.clear();
        stack.emplace_back(index, static_cast<profiler::block_index_t>(0));

        profiler::timestamp_t lastMatchedParentDuration = 0;
        size_t matchedParentIdStackDepth = 0;
        bool matchedParentId = false;
        while (!stack.empty())
        {
            if (m_bInterrupt.load(std::memory_order_acquire))
                return false;

            auto& top = stack.back();
            auto& first = top.second;
            const auto i = top.first;

            const auto& block = easyBlock(i).tree;
            const auto& desc = easyDescriptor(block.node->id());

            if (desc.type() == profiler::BlockType::Value && matchedParentId)
            {
                const auto value = block.value;
                if (_isSuitableValue(value->value_id(), desc.name()))
                {
                    if (_index < 0 || (value->isArray() && _index < profiler_gui::valueArraySize(*value)))
                    {
                        m_values.push_back(value);
                        if (_calculatePoints)
                        {
                            const auto val = addPoint(*value, _index);
                            if (m_chartType == ChartType::Complexity)
                            {
                                m_complexityMap[val].push_back(lastMatchedParentDuration);
                                if (lastMatchedParentDuration < m_minDuration)
                                    m_minDuration = lastMatchedParentDuration;
                                if (lastMatchedParentDuration > m_maxDuration)
                                    m_maxDuration = lastMatchedParentDuration;
                            }
                        }
                    }
                }
            }

            if (first < block.children.size())
            {
                if (block.node->id() == _parentBlockId || desc.id() == _parentBlockId)
                {
                    if (!matchedParentId)
                        matchedParentIdStackDepth = stack.size();
                    matchedParentId = true;
                    lastMatchedParentDuration = block.node->duration();
                }

                const auto child = block.children[first++];
                stack.emplace_back(child, static_cast<profiler::block_index_t>(0));
            }
            else
            {
                if (stack.size() == matchedParentIdStackDepth)
                    matchedParentId = false;
                stack.pop_back();
            }
        }
    }

    return true;
}

double ArbitraryValuesCollection::addPoint(const profiler::ArbitraryValue& _value, int _index)
{
    const auto p = point(_value, _index);

    if (p.y() > m_maxValue)
        m_maxValue = p.y();

    if (p.y() < m_minValue)
        m_minValue = p.y();

    m_points.push_back(p);

    return p.y();
}

QPointF ArbitraryValuesCollection::point(const profiler::ArbitraryValue& _value, int _index) const
{
    const qreal x = PROF_MICROSECONDS(qreal(_value.begin() - m_beginTime));
    const qreal y = profiler_gui::value2real(_value, std::max(_index, 0));
    return {x, y};
}

//////////////////////////////////////////////////////////////////////////

ArbitraryValuesChartItem::ArbitraryValuesChartItem()
    : Parent()
    , m_workerMaxValue(0)
    , m_workerMinValue(0)
    , m_workerMaxDuration(0)
    , m_workerMinDuration(0)
    , m_maxDuration(0)
    , m_minDuration(0)
    , m_filterWindowSize(8)
    , m_chartType(ChartType::Regular)
    , m_filterType(FilterType::None)
{
}

ArbitraryValuesChartItem::~ArbitraryValuesChartItem()
{

}

void ArbitraryValuesChartItem::paint(QPainter* _painter, const QStyleOptionGraphicsItem*, QWidget*)
{
    const auto widget = static_cast<const GraphicsSliderArea*>(scene()->parent());
    const auto currentScale = widget->getWindowScale();
    const bool bindMode = widget->bindMode();
    const auto bottom = m_boundingRect.bottom();
    const auto width = m_boundingRect.width() * currentScale;

    _painter->save();
    _painter->setTransform(QTransform::fromScale(1.0 / currentScale, 1), true);

    const auto font_h = widget->fontHeight();

    const auto drawImage = [=] {
        if (!bindMode)
            paintImage(_painter);
        else
            paintImage(_painter, currentScale, widget->minimum(), widget->maximum(), widget->value(), widget->sliderWidth());
    };

    if (!EASY_GLOBALS.scene.empty)
    {
        if (!profiler_gui::is_max(EASY_GLOBALS.selected_block) && m_chartType == ChartType::Regular)
        {
            const auto& block = easyBlocksTree(EASY_GLOBALS.selected_block);
            qreal left = PROF_MICROSECONDS(qreal(block.node->begin() - EASY_GLOBALS.begin_time)) * currentScale;
            qreal right = PROF_MICROSECONDS(qreal(block.node->end() - EASY_GLOBALS.begin_time)) * currentScale;

            if (bindMode)
            {
                const auto scale = widget->range() / widget->sliderWidth();
                const auto offset = widget->value() * currentScale;

                left -= offset;
                right -= offset;
                left *= scale;
                right *= scale;
            }

            const auto leftBorder = m_boundingRect.left() * currentScale;
            const auto rightBorder = leftBorder + width - 2;
            const auto l = estd::clamp(leftBorder,  left, rightBorder);
            const auto r = estd::clamp(leftBorder, right, rightBorder);

            const auto sceneRect = scene()->sceneRect();
            const auto w = r - l;
            if (w > 1)
            {
                const auto color = (easyDescriptor(block.node->id()).color() & 0x00ffffff) | 0x30000000;
                _painter->fillRect(QRectF(l, sceneRect.top() - 1, w, sceneRect.height() + 2), QColor::fromRgba(color));
            }

            drawImage();

            _painter->setPen(QPen(QColor::fromRgba(0xb0000000), 2, Qt::DotLine));
            _painter->drawLine(QLineF(l, sceneRect.top(), l, sceneRect.bottom()));

            if (w > 1)
                _painter->drawLine(QLineF(r, sceneRect.top(), r, sceneRect.bottom()));
        }
        else
        {
            drawImage();
        }

        const auto range = bindMode ? widget->sliderWidth() : widget->range();
        paintMouseIndicator(_painter, m_boundingRect.top(), bottom, width,
                            m_boundingRect.height(), font_h, widget->value(), range);
    }
    else
    {
        drawImage();
    }

    // MODE
    {
        QRectF rect(3, m_boundingRect.top() - widget->margin(), width - 3, m_boundingRect.height() + widget->margins());
        QRectF textBounds;

        _painter->setPen(Qt::blue);
        _painter->drawText(
            rect,
            Qt::AlignLeft | Qt::AlignTop | Qt::TextDontClip | Qt::TextIncludeTrailingSpaces,
            QStringLiteral("MODE: "),
            &textBounds
        );
        rect.adjust(textBounds.width(), 0, 0, 0);

        _painter->setPen(profiler_gui::TEXT_COLOR);
        _painter->drawText(rect, Qt::AlignLeft | Qt::AlignTop | Qt::TextDontClip, bindMode ? "Zoom" : "Overview");
    }

    _painter->setPen(Qt::darkGray);
    _painter->drawLine(QLineF(0, bottom, width, bottom));
    _painter->drawLine(QLineF(0, m_boundingRect.top(), width, m_boundingRect.top()));

    _painter->restore();
}

void ArbitraryValuesChartItem::paintMouseIndicator(QPainter* _painter, qreal _top, qreal _bottom, qreal _width, qreal _height,
                                                   int _font_h, qreal _visibleRegionLeft, qreal _visibleRegionWidth)
{
    if (_font_h == 0)
        return;

    const auto x = m_mousePos.x();
    auto y = m_mousePos.y();
    QString valueString;

    // Horizontal
    const bool visibleY = (_top < y && y < _bottom);
    y = estd::clamp(_top, y, _bottom);
    {
        _height -= ChartBounds;

        const int half_font_h = _font_h >> 1;
        const auto yvalue = estd::clamp(_top + ChartBound, y, _bottom - ChartBound);

        if (m_chartType == ChartType::Regular)
        {
            const auto value = m_minValue + ((_bottom - ChartBound - yvalue) / _height) * (m_maxValue - m_minValue);
            valueString = QString::number(value, 'f', 3);
            if (valueString.endsWith(QStringLiteral(".000")))
                valueString.chop(4);
        }
        else
        {
            const auto value = m_minDuration +
                static_cast<profiler::timestamp_t>(((_bottom - ChartBound - yvalue) / _height) * (m_maxDuration - m_minDuration));
            valueString = profiler_gui::autoTimeStringRealNs(value, 3);
        }

        const int textWidth = _painter->fontMetrics().width(valueString) + 3;
        const QRectF rect(0, y - _font_h - 2, _width - 3, 4 + (_font_h << 1));

        _painter->setPen(Qt::blue);

        qreal left = 0, right = _width - 3;
        const Qt::AlignmentFlag alignment = x < textWidth ? Qt::AlignRight : Qt::AlignLeft;

        if (y > _bottom - half_font_h)
        {
            _painter->drawText(rect, alignment | Qt::AlignTop, valueString);
        }
        else if (y < _top + half_font_h)
        {
            _painter->drawText(rect, alignment | Qt::AlignBottom, valueString);
        }
        else
        {
            _painter->drawText(rect, alignment | Qt::AlignVCenter, valueString);
            if (x < textWidth)
                right = _width - textWidth - 3;
            else
                left = textWidth;
        }

        if (visibleY)
            _painter->drawLine(QLineF(left, y, right, y));
    }

    // Vertical
    if (0 <= x && x <= _width)
    {
        if (m_chartType == ChartType::Regular)
        {
            const auto value = _visibleRegionLeft + _visibleRegionWidth * x / _width;
            valueString = profiler_gui::timeStringReal(EASY_GLOBALS.time_units, value, 3);
        }
        else
        {
            const auto value = m_minValue + (m_maxValue - m_minValue) * x / _width;
            valueString = QString::number(value, 'f', 3);
        }

        const int textWidth = _painter->fontMetrics().width(valueString) + 6;
        const int textWidthHalf = textWidth >> 1;

        qreal left = x - textWidthHalf;
        if (x < textWidthHalf)
            left = 0;
        else if (x > (_width - textWidthHalf))
            left = _width - textWidth;

        const QRectF rect(left, _bottom + 2, textWidth, _font_h);
        _painter->drawText(rect, Qt::AlignCenter, valueString);
        _painter->drawLine(QLineF(x, _top, x, _bottom));
    }
}

bool ArbitraryValuesChartItem::updateImage()
{
    if (!Parent::updateImage())
        return false;

    const auto widget = static_cast<const GraphicsSliderArea*>(scene()->parent());

    m_imageScaleUpdate = widget->range() / widget->sliderWidth();
    m_imageOriginUpdate = widget->bindMode() ? (widget->value() - widget->sliderWidth() * 3) : widget->minimum();

    // Ugly, but doen't use exceeded count of threads
    const auto rect = m_boundingRect;
    const auto scale = widget->getWindowScale();
    const auto left = widget->minimum();
    const auto right = widget->maximum();
    const auto value = widget->value();
    const auto window = widget->sliderWidth();
    const auto bindMode = widget->bindMode();
    const auto beginTime = EASY_GLOBALS.begin_time;
    const auto autoHeight = EASY_GLOBALS.auto_adjust_chart_height;

    if (m_chartType == ChartType::Regular)
    {
        m_worker.enqueue([=]
        {
            updateRegularImageAsync(rect, scale, left, right, right - left, value, window, bindMode, beginTime,
                                    autoHeight);
        }, m_bReady);
    }
    else
    {
        m_worker.enqueue([=]
        {
            updateComplexityImageAsync(rect, scale, left, right, right - left, value, window, bindMode, beginTime,
                                       autoHeight);
        }, m_bReady);
    }

    return true;
}

void ArbitraryValuesChartItem::onImageUpdated()
{
    m_maxValue = m_workerMaxValue;
    m_minValue = m_workerMinValue;
    m_maxDuration = m_workerMaxDuration;
    m_minDuration = m_workerMinDuration;
}

void ArbitraryValuesChartItem::drawGrid(QPainter& _painter, int _width, int _height) const
{
    auto pen = _painter.pen();
    pen.setColor(Qt::darkGray);
    pen.setStyle(Qt::DotLine);
    _painter.setPen(pen);

    const int left = 0;
    const int top = 0;

    const int hlines = _height / 20;
    for (int i = 0; i < hlines; ++i)
    {
        const auto y = top + 20 + i * 20;
        _painter.drawLine(left, y, left + _width, y);
    }

    const int vlines = _width / 20;
    for (int i = 0; i < vlines; ++i)
    {
        const auto x = left + 20 + i * 20;
        _painter.drawLine(x, top, x, top + _height);
    }

    _painter.setPen(Qt::SolidLine);
}

void ArbitraryValuesChartItem::updateRegularImageAsync(QRectF _boundingRect, qreal _current_scale, qreal _minimum
    , qreal _maximum, qreal _range, qreal _value, qreal _width, bool _bindMode, profiler::timestamp_t _begin_time
    , bool _autoAdjust)
{
    const auto screenWidth = _boundingRect.width() * _current_scale;
    //const auto maxColumnHeight = _boundingRect.height();
    const auto viewScale = _range / _width;

    if (_bindMode)
    {
        m_workerImageScale = viewScale;
        m_workerImageOrigin = _value - _width * 3;
        m_workerImage = new QImage(screenWidth * 7 + 0.5, _boundingRect.height(), QImage::Format_ARGB32);
    }
    else
    {
        m_workerImageScale = 1;
        m_workerImageOrigin = _minimum;
        m_workerImage = new QImage(screenWidth + 0.5, _boundingRect.height(), QImage::Format_ARGB32);
    }

    m_workerImage->fill(0);
    QPainter p(m_workerImage);
    p.setBrush(Qt::NoBrush);
    p.setRenderHint(QPainter::Antialiasing, true);

    // Draw grid
    drawGrid(p, m_workerImage->width(), m_workerImage->height());

    if (m_collections.empty() || isReady())
    {
        setReady(true);
        return;
    }

    using LeftBounds = std::vector<Points::const_iterator>;
    qreal realScale = _current_scale, offset = 0;

    LeftBounds leftBounds;
    leftBounds.reserve(m_collections.size());

    if (_bindMode)
    {
        _minimum = m_workerImageOrigin;
        _maximum = m_workerImageOrigin + _width * 7;
        realScale *= viewScale;
        offset = _minimum * realScale;
    }

    const auto right = std::min(_value + _width, _maximum);
    qreal minValue = 1e300, maxValue = -1e300;
    for (const auto& c : m_collections)
    {
        if (isReady())
            return;

        const auto& collection = *c.ptr;
        const auto& points = collection.points();

        if (points.empty())
        {
            leftBounds.emplace_back(points.end());
            continue;
        }

        if (_bindMode)
        {
            auto first = std::lower_bound(points.begin(), points.end(), _minimum, [](const QPointF& point, qreal x)
            {
                return point.x() < x;
            });

            if (first != points.end())
            {
                if (first != points.begin())
                    --first;
            }
            else
            {
                first = points.begin() + points.size() - 1;
            }

            leftBounds.emplace_back(first);

            if (_autoAdjust)
            {
                for (auto it = first; it != points.end() && it->x() < right && !isReady(); ++it)
                {
                    if (it->x() < _value)
                        continue;

                    const auto value = it->y();
                    minValue = std::min(minValue, value);
                    maxValue = std::max(maxValue, value);
                }

                continue;
            }
        }
        else
        {
            leftBounds.emplace_back(points.begin());
        }

        minValue = std::min(minValue, collection.minValue());
        maxValue = std::max(maxValue, collection.maxValue());
    }

    if (minValue > maxValue)
    {
        // No points
        m_workerMinValue = 0;
        m_workerMaxValue = 0;
        setReady(true);
        return;
    }

    m_workerMinValue = minValue;
    m_workerMaxValue = maxValue;

    if (isReady())
        return;

    const bool singleValue = fabs(maxValue - minValue) < 2 * std::numeric_limits<qreal>::epsilon();
    const auto middle = _boundingRect.height() * 0.5;

    const qreal height = std::max(maxValue - minValue, 0.01);
    const auto gety = [&_boundingRect, maxValue, height, singleValue, middle] (qreal y)
    {
        if (singleValue)
        {
            y = middle;
        }
        else
        {
            y = maxValue - y;
            y /= height;
            y *= _boundingRect.height() - ChartBounds;
            y += ChartBound;
        }

        return y;
    };

    size_t i = 0;
    for (const auto& c : m_collections)
    {
        if (isReady())
            return;

        const auto& points = c.ptr->points();
        if (points.empty())
        {
            ++i;
            continue;
        }

        if (c.selected)
        {
            auto pen = p.pen();
            pen.setColor(QColor::fromRgba(c.color));
            pen.setWidth(2);
            p.setPen(pen);
        }
        else
        {
            p.setPen(QColor::fromRgba(c.color));
        }

        const auto first = leftBounds[i];

        if (c.chartPenStyle == ChartPenStyle::Points)
        {
            qreal prevX = 1e300, prevY = 1e300;
            for (auto it = first; it != points.end() && it->x() < _maximum; ++it)
            {
                if (it->x() < _minimum)
                    continue;

                if (isReady())
                    return;

                const qreal x = it->x() * realScale - offset;
                const qreal y = gety(it->y());
                const auto dx = fabs(x - prevX), dy = fabs(y - prevY);

                if (dx > 1 || dy > 1)
                {
                    p.drawPoint(QPointF{x, y});
                    prevX = x;
                    prevY = y;
                }
            }
        }
        else if (first != points.end() && first->x() < _maximum)
        {
            QPointF p1 = *first;
            qreal x = p1.x() * realScale - offset;
            qreal y = gety(p1.y());
            p1.setX(x);
            p1.setY(y);

            auto it = first;
            for (++it; it != points.end(); ++it)
            {
                if (isReady())
                    return;

                QPointF p2 = *it;
                x = p2.x() * realScale - offset;
                y = gety(p2.y());
                p2.setX(x);
                p2.setY(y);

                if (it->x() >= _minimum)
                {
                    const auto dx = fabs(x - p1.x()), dy = fabs(y - p1.y());
                    if (dx > 1 || dy > 1)
                        p.drawLine(p1, p2);
                    else
                        continue;
                }

                if (it->x() >= _maximum)
                    break;

                p1 = p2;
            }
        }

        if (c.selected)
        {
            auto color = profiler_gui::darken(c.color, 0.65f);
            if (profiler_gui::alpha(color) < 0xc0)
                p.setPen(QColor::fromRgba(profiler::colors::modify_alpha32(color, 0xc0000000)));
            else
                p.setPen(QColor::fromRgba(color));
            p.setBrush(QColor::fromRgba(0xc8ffffff));

            qreal prevX = -offset * 2, prevY = -500;
            for (auto it = first; it != points.end() && it->x() < _maximum; ++it)
            {
                if (it->x() < _minimum)
                    continue;

                if (isReady())
                    return;

                const qreal x = it->x() * realScale - offset;
                const qreal y = gety(it->y());

                const auto dx = x - prevX, dy = y - prevY;
                const auto delta = estd::sqr(dx) + estd::sqr(dy);

                if (delta > 25)
                {
                    p.drawEllipse(QPointF {x, y}, 3, 3);
                    prevX = x;
                    prevY = y;
                }
            }
        }

        ++i;
    }

    setReady(true);
}

void ArbitraryValuesChartItem::updateComplexityImageAsync(QRectF _boundingRect, qreal _current_scale, qreal _minimum
    , qreal _maximum, qreal _range, qreal _value, qreal _width, bool _bindMode, profiler::timestamp_t _begin_time
    , bool _autoAdjust)
{
    const auto rectHeight = _boundingRect.height();
    const auto screenWidth = _boundingRect.width() * _current_scale;
    const auto globalSceneLeft = _minimum;
    //const auto maxColumnHeight = _boundingRect.height();

    if (_bindMode)
    {
        m_workerImageScale = _range / _width;
        m_workerImageOrigin = _value - _width * 3;
        m_workerImage = new QImage(screenWidth * 7 + 0.5, rectHeight, QImage::Format_ARGB32);
    }
    else
    {
        m_workerImageScale = 1;
        m_workerImageOrigin = _minimum;
        m_workerImage = new QImage(screenWidth + 0.5, rectHeight, QImage::Format_ARGB32);
    }

    m_workerImage->fill(0);
    QPainter p(m_workerImage);
    p.setBrush(Qt::NoBrush);
    p.setRenderHint(QPainter::Antialiasing, true);

    // Draw grid
    drawGrid(p, m_workerImage->width(), m_workerImage->height());

    if (m_collections.empty() || isReady())
    {
        setReady(true);
        return;
    }

    qreal minValue = 1e300, maxValue = -1e300;
    profiler::timestamp_t minDuration = profiler_gui::numeric_max<profiler::timestamp_t>(), maxDuration = 0;
    for (const auto& c : m_collections)
    {
        if (isReady())
            return;

        const auto& collection = *c.ptr;
        const auto& complexityMap = collection.complexityMap();

        if (complexityMap.empty())
            continue;

        minValue = std::min(minValue, collection.minValue());
        maxValue = std::max(maxValue, collection.maxValue());

        minDuration = std::min(minDuration, collection.minDuration());
        maxDuration = std::max(maxDuration, collection.maxDuration());
    }

    if (minValue > maxValue || minDuration > maxDuration)
    {
        // No points
        m_workerMaxValue = 0;
        m_workerMinValue = 0;
        m_workerMaxDuration = 0;
        m_workerMinDuration = 0;
        setReady(true);
        return;
    }

    using LeftBounds = std::vector<ComplexityValuesMap::const_iterator>;
    qreal left = minValue, right = maxValue;

    LeftBounds leftBounds;
    leftBounds.reserve(m_collections.size());

    if (_bindMode)
    {
        const auto valueRange = maxValue - minValue;

        _minimum = m_workerImageOrigin;
        _maximum = m_workerImageOrigin + _width * 7;

        left = minValue + valueRange * (_minimum - globalSceneLeft) / _range;
        right = minValue + valueRange * (_maximum - globalSceneLeft) / _range;

        if (_autoAdjust)
        {
            minDuration = profiler_gui::numeric_max<profiler::timestamp_t>();
            maxDuration = 0;
        }

        maxValue = minValue + valueRange * (_value + _width - globalSceneLeft) / _range;
        minValue += valueRange * (_value - globalSceneLeft) / _range;
    }

    for (const auto& c : m_collections)
    {
        if (isReady())
            return;

        const auto& complexityMap = c.ptr->complexityMap();

        if (complexityMap.empty())
        {
            leftBounds.emplace_back(complexityMap.end());
            continue;
        }

        if (!_bindMode)
        {
            leftBounds.emplace_back(complexityMap.begin());
            continue;
        }

        auto first = complexityMap.lower_bound(left);

        if (first != complexityMap.end())
        {
            if (first != complexityMap.begin())
                --first;
        }
        else
        {
            auto last = complexityMap.rbegin();
            first = (++last).base();
        }

        leftBounds.emplace_back(first);

        if (_autoAdjust)
        {
            for (auto it = first; it != complexityMap.end() && it->first < maxValue && !isReady(); ++it)
            {
                const auto value = it->first;
                if (value < minValue || it->second.empty())
                    continue;

                minDuration = std::min(minDuration, it->second.front());
                maxDuration = std::max(maxDuration, it->second.back());
            }
        }
    }

    m_workerMaxValue = maxValue;
    m_workerMinValue = minValue;
    m_workerMaxDuration = maxDuration;
    m_workerMinDuration = minDuration;

    if (isReady())
        return;

    const auto ymiddle = rectHeight * 0.5;
    const auto dt = maxDuration - minDuration;
    bool singleValue = dt == 0;

    const qreal height = std::max(static_cast<qreal>(dt), 0.01);
    const auto gety = [=] (profiler::timestamp_t duration)
    {
        qreal y;
        if (singleValue)
        {
            y = ymiddle;
        }
        else
        {
            y = static_cast<qreal>(maxDuration - duration);
            y /= height;
            y *= rectHeight - ChartBounds;
            y += ChartBound;
        }

        return y;
    };

    const qreal imageWidth = m_workerImage->width();
    const auto xmiddle = imageWidth * 0.5;
    singleValue = fabs(right - left) < 2 * std::numeric_limits<qreal>::epsilon();
    const qreal width = std::max(right - left, 0.01);
    const auto getx = [=] (qreal x)
    {
        if (singleValue)
        {
            x = xmiddle;
        }
        else
        {
            x -= left;
            x *= imageWidth / width;
        }

        return x;
    };

    std::vector<QPointF> averages;
    size_t i = 0;
    for (const auto& c : m_collections)
    {
        if (isReady())
            return;

        const auto& complexityMap = c.ptr->complexityMap();
        if (complexityMap.empty())
        {
            ++i;
            continue;
        }

        const auto first = leftBounds[i++];
        if (first == complexityMap.end() || first->first > right)
            continue;

        if (c.selected)
        {
            auto pen = p.pen();
            pen.setColor(QColor::fromRgba(c.color));
            pen.setWidth(2);
            p.setPen(pen);
        }
        else
        {
            p.setPen(QColor::fromRgba(c.color));
        }

        const bool drawApproximateLine = c.selected || m_collections.size() == 1;

        averages.clear();
        if (drawApproximateLine)
            averages.reserve(complexityMap.size());

        auto it = first;
        while (it != complexityMap.end() && it->first < left)
            ++it;

        if (it == complexityMap.end() || it->first > right)
            continue;

        qreal x = getx(it->first);

        profiler::timestamp_t average = 0;
        for (auto duration : it->second)
        {
            average += duration;
            p.drawPoint(QPointF {x, gety(duration)});
        }

        if (drawApproximateLine)
        {
            average /= it->second.size();
            averages.emplace_back(x, gety(average));
        }

        for (++it; it != complexityMap.end(); ++it)
        {
            if (isReady())
                return;

            x = getx(it->first);
            average = 0;
            for (auto duration : it->second)
            {
                average += duration;
                p.drawPoint(QPointF {x, gety(duration)});
            }

            if (drawApproximateLine)
            {
                average /= it->second.size();
                averages.emplace_back(x, gety(average));
            }

            if (it->first > right)
                break;
        }

        if (drawApproximateLine)
        {
            // drawPolyLine() with 2 pixel width is VERY slow! Do not use it!
            //p.drawPolyline(averages.data(), static_cast<int>(averages.size()));

            // Draw polyline
            {
                QPointF p1 = averages.front();

                auto averages_it = averages.begin();
                for (++averages_it; averages_it != averages.end(); ++averages_it)
                {
                    if (isReady())
                        return;

                    const QPointF& p2 = *averages_it;
                    const auto dx = fabs(p2.x() - p1.x()), dy = fabs(p2.y() - p1.y());
                    if (dx > 1 || dy > 1)
                        p.drawLine(p1, p2);
                    else
                        continue;

                    p1 = p2;
                }
            }

            auto color = profiler_gui::darken(c.color, 0.65f);
            if (profiler_gui::alpha(color) < 0xc0)
                p.setPen(QColor::fromRgba(profiler::colors::modify_alpha32(color, 0xc0000000)));
            else
                p.setPen(QColor::fromRgba(color));
            p.setBrush(QColor::fromRgba(0xc8ffffff));

            QPointF prev {-500., -500.};
            for (const auto& point : averages)
            {
                if (isReady())
                    return;

                const auto line = point - prev;
                const auto delta = estd::sqr(line.x()) + estd::sqr(line.y());

                if (delta > 25)
                {
                    p.drawEllipse(point, 3, 3);
                    prev = point;
                }
            }

            p.setBrush(Qt::NoBrush);
            auto pen = p.pen();
            pen.setWidth(2);
            p.setPen(pen);

            switch (m_filterType)
            {
                case FilterType::Median:
                    medianFilter(averages, m_filterWindowSize, m_bReady);
                    break;

                case FilterType::Gauss:
                    gaussFilter(averages, m_filterWindowSize, m_bReady);
                    break;

                default:
                    break;
            }

            if (isReady())
                return;

            QPainterPath pp;
            pp.moveTo(averages.front());
            const auto last = averages.size() - 1;
            size_t last_k = 0;
            size_t step = std::max(averages.size() / (size_t)40, (size_t)1);
            const auto dubstep = step << 1; // lol :P
            const auto iteration_step = 3 * step;
            for (size_t k = iteration_step; k < averages.size(); k += iteration_step)
            {
                if (isReady())
                    return;

                last_k = k;
                pp.cubicTo(averages[k - dubstep], averages[k - step], averages[k]);
            }

            const auto rest = last - last_k;
            if (rest < iteration_step)
            {
                if ((rest % 4) == 0)
                {
                    step = rest / 4;
                    pp.cubicTo(averages[last - (step << 1)], averages[last - step], averages[last]);
                }
                else
                {
                    step = std::max(rest / (size_t)4, (size_t)1);
                    const auto k1 = std::max(last - step, last_k);
                    const auto k0 = std::max(k1   - step, last_k);
                    pp.cubicTo(averages[k0], averages[k1], averages[last]);
                }
            }

            p.drawPath(pp);
        }
    }

    setReady(true);
}

void ArbitraryValuesChartItem::clear()
{
    cancelAnyJob();
    m_boundaryTimer.stop();
    m_collections.clear();
    m_minValue = m_maxValue = 0;
    m_minDuration = m_maxDuration = 0;
    setEmpty(true);
}

void ArbitraryValuesChartItem::update(Collections _collections)
{
    cancelImageUpdate();
    m_collections = std::move(_collections);
    setEmpty(m_collections.empty());
    updateImage();
}

void ArbitraryValuesChartItem::update(const ArbitraryValuesCollection* _selected)
{
    cancelImageUpdate();

    for (auto& collection : m_collections)
        collection.selected = collection.ptr == _selected;

    updateImage();
}

void ArbitraryValuesChartItem::setChartType(ChartType _chartType)
{
    if (m_chartType == _chartType)
        return;

    cancelImageUpdate();
    m_chartType = _chartType;
    updateImage();
}

void ArbitraryValuesChartItem::setFilterType(FilterType _filterType)
{
    if (m_filterType == _filterType)
        return;

    if (m_chartType == ChartType::Regular)
    {
        m_filterType = _filterType;
        return;
    }

    cancelImageUpdate();
    m_filterType = _filterType;
    updateImage();
}

void ArbitraryValuesChartItem::setFilterWindowSize(int _size)
{
    if (m_filterWindowSize == _size)
        return;

    if (m_chartType == ChartType::Regular || m_filterType == FilterType::None)
    {
        m_filterWindowSize = _size;
        return;
    }

    cancelImageUpdate();
    m_filterWindowSize = _size;
    updateImage();
}

ChartType ArbitraryValuesChartItem::chartType() const
{
    return m_chartType;
}

FilterType ArbitraryValuesChartItem::filterType() const
{
    return m_filterType;
}

int ArbitraryValuesChartItem::filterWindowSize() const
{
    return m_filterWindowSize;
}

//////////////////////////////////////////////////////////////////////////

GraphicsChart::GraphicsChart(QWidget* _parent)
    : Parent(_parent)
    , m_chartItem(new ArbitraryValuesChartItem())
{
    m_imageItem = m_chartItem;
    scene()->addItem(m_chartItem);

    const auto rect = scene()->sceneRect();
    m_chartItem->setPos(0, 0);
    m_chartItem->setBoundingRect(0, rect.top() + margin(), scene()->width(), rect.height() - margins() - 1);

    connect(&EASY_GLOBALS.events, &profiler_gui::GlobalSignals::autoAdjustChartChanged, this, &This::onAutoAdjustChartChanged);

    m_chartItem->updateImage();
}

GraphicsChart::~GraphicsChart()
{

}

void GraphicsChart::onAutoAdjustChartChanged()
{
    if (m_chartItem->isVisible())
        m_chartItem->onModeChanged();
}

void GraphicsChart::clear()
{
    cancelImageUpdate();
    Parent::clear();
}

void GraphicsChart::cancelImageUpdate()
{
    m_chartItem->clear();
}

void GraphicsChart::update(Collections _collections)
{
    m_chartItem->update(std::move(_collections));
}

void GraphicsChart::update(const ArbitraryValuesCollection* _selected)
{
    m_chartItem->update(_selected);
}

void GraphicsChart::setChartType(ChartType _chartType)
{
    m_chartItem->setChartType(_chartType);
    m_slider->setVisible(_chartType != ChartType::Complexity && !m_bBindMode);
}

void GraphicsChart::setFilterType(FilterType _filterType)
{
    m_chartItem->setFilterType(_filterType);
}

void GraphicsChart::setFilterWindowSize(int _size)
{
    m_chartItem->setFilterWindowSize(_size);
}

ChartType GraphicsChart::chartType() const
{
    return m_chartItem->chartType();
}

FilterType GraphicsChart::filterType() const
{
    return m_chartItem->filterType();
}

int GraphicsChart::filterWindowSize() const
{
    return m_chartItem->filterWindowSize();
}

bool GraphicsChart::canShowSlider() const
{
    return chartType() != ChartType::Complexity && !bindMode();
}

//////////////////////////////////////////////////////////////////////////

enum class ArbitraryColumns : uint8_t
{
    Name = 0,
    Type,
    Value,
    Vin,

    Count
};

EASY_CONSTEXPR auto CheckColumn = int_cast(ArbitraryColumns::Name);
EASY_CONSTEXPR auto StdItemType = QTreeWidgetItem::UserType;
EASY_CONSTEXPR auto ValueItemType = QTreeWidgetItem::UserType + 1;

struct UsedValueTypes {
    ArbitraryTreeWidgetItem* items[int_cast(profiler::DataType::TypesCount)];
    UsedValueTypes(int = 0) { memset(items, 0, sizeof(items)); }
};

//////////////////////////////////////////////////////////////////////////

ArbitraryTreeWidgetItem::ArbitraryTreeWidgetItem(QTreeWidgetItem* _parent, bool _checkable, profiler::color_t _color, const profiler::ArbitraryValue& _value)
    : Parent(_parent, ValueItemType)
    , m_value(_value)
    , m_color(_color)
    , m_widthHint(0)
{
    if (_checkable)
    {
        setFlags(flags() | Qt::ItemIsUserCheckable | Qt::ItemIsSelectable);
        setCheckState(CheckColumn, Qt::Unchecked);
    }
    else
    {
        setFlags(flags() & ~Qt::ItemIsUserCheckable);
    }
}

ArbitraryTreeWidgetItem::~ArbitraryTreeWidgetItem()
{
    interrupt();
}

QVariant ArbitraryTreeWidgetItem::data(int _column, int _role) const
{
    if (_column == CheckColumn && _role == Qt::SizeHintRole)
        return QSize(static_cast<int>(m_widthHint * (m_font.bold() ? 1.2f : 1.f)), 26);
    if (_role == Qt::FontRole)
        return m_font;
    if (_column == int_cast(ArbitraryColumns::Vin) && _role == Qt::DisplayRole && getSelfIndexInArray() < 0)
        return QString("0x%1").arg(m_value.value_id(), 0, 16);
    return Parent::data(_column, _role);
}

const profiler::ArbitraryValue& ArbitraryTreeWidgetItem::value() const
{
    return m_value;
}

void ArbitraryTreeWidgetItem::setWidthHint(int _width)
{
    m_widthHint = _width;
}

void ArbitraryTreeWidgetItem::setBold(bool _isBold)
{
    m_font.setBold(_isBold);
}

const ArbitraryValuesCollection* ArbitraryTreeWidgetItem::collection() const
{
    return m_collection.get();
}

ArbitraryValuesCollection* ArbitraryTreeWidgetItem::collection()
{
    return m_collection.get();
}

bool ArbitraryTreeWidgetItem::isArrayItem() const
{
    return childCount() != 0;
}

profiler::block_id_t ArbitraryTreeWidgetItem::getParentBlockId(QTreeWidgetItem* _item) const
{
    auto parentItem = _item->parent();
    const auto parentRole = parentItem->data(int_cast(ArbitraryColumns::Type), Qt::UserRole).toInt();
    switch (parentRole)
    {
        case 1:
            return parentItem->data(int_cast(ArbitraryColumns::Vin), Qt::UserRole).toUInt();

        case 2:
            return getParentBlockId(parentItem);

        default:
            return EASY_GLOBALS.selected_block_id;
    }
}

int ArbitraryTreeWidgetItem::getSelfIndexInArray() const
{
    if (data(int_cast(ArbitraryColumns::Type), Qt::UserRole).toInt() != 3)
        return -1;
    return parent()->indexOfChild(const_cast<ArbitraryTreeWidgetItem*>(this));
}

void ArbitraryTreeWidgetItem::collectValues(profiler::thread_id_t _threadId, ChartType _chartType)
{
    if (!m_collection)
        m_collection = CollectionPtr(new ArbitraryValuesCollection);
    else
        m_collection->interrupt();

    const auto parentBlockId = getParentBlockId(this);
    const int index = getSelfIndexInArray();

    EASY_CONSTEXPR auto nameColumn = int_cast(ArbitraryColumns::Name);
    const auto name = index < 0 ? text(nameColumn).toStdString() : parent()->text(nameColumn).toStdString();

    m_collection->collectValuesAndPoints(_chartType, _threadId, m_value.value_id(), name.c_str(),
                                         EASY_GLOBALS.begin_time, parentBlockId, index);
}

void ArbitraryTreeWidgetItem::interrupt()
{
    if (!m_collection)
        return;

    m_collection->interrupt();
    m_collection.release();
}

profiler::color_t ArbitraryTreeWidgetItem::color() const
{
    return m_color;
}

//////////////////////////////////////////////////////////////////////////

ArbitraryValuesWidget::ArbitraryValuesWidget(bool _isMainWidget, profiler::thread_id_t _threadId
    , profiler::block_index_t _blockIndex, profiler::block_id_t _blockId, QWidget* _parent)
    : Parent(_parent)
    , m_splitter(new QSplitter(Qt::Horizontal, this))
    , m_treeWidget(new QTreeWidget(this))
    , m_chart(new GraphicsChart(this))
    , m_filterBoxLabel(new QLabel(tr(" Filter:"), this))
    , m_filterComboBox(new QComboBox(this))
    , m_filterWindowLabel(new QLabel(tr(" Window size:"), this))
    , m_filterWindowPicker(new QSpinBox(this))
    , m_exportToCsvAction(nullptr)
    , m_boldItem(nullptr)
    , m_threadId(_threadId)
    , m_blockIndex(_blockIndex)
    , m_blockId(_blockId)
    , m_bInitialized(false)
    , m_bMainWidget(_isMainWidget)
{
    m_collectionsTimer.setInterval(100);

    m_splitter->setHandleWidth(1);
    m_splitter->setContentsMargins(0, 0, 0, 0);
    m_splitter->addWidget(m_treeWidget);
    m_splitter->addWidget(m_chart);
    m_splitter->setStretchFactor(0, 1);
    m_splitter->setStretchFactor(1, 1);

    m_filterWindowPicker->setRange(3, 50);
    m_filterWindowPicker->setSingleStep(1);
    m_filterWindowPicker->setValue(8);

    m_filterComboBox->addItem(tr("None"));
    m_filterComboBox->addItem(tr("Gauss"));
    m_filterComboBox->addItem(tr("Median"));
    m_filterComboBox->setCurrentIndex(0);

    auto tb = new QToolBar(this);
    tb->setIconSize(applicationIconsSize());

    auto action = tb->addAction(QIcon(imagePath("reload")), tr("Refresh values list"));
    connect(action, &QAction::triggered, this, Overload<void>::of(&This::rebuild));

    action = tb->addAction(QIcon(imagePath("window")), tr("Open new window"));
    connect(action, &QAction::triggered, this, &This::onOpenInNewWindowClicked);

    m_exportToCsvAction = tb->addAction(QIcon(imagePath("csv")), tr("Export to csv"));
    connect(m_exportToCsvAction, &QAction::triggered, this, &This::onExportToCsvClicked);

    tb->addSeparator();

    auto actionGroup = new QActionGroup(this);
    actionGroup->setExclusive(true);

    auto actionRegulatChart = new QAction(QIcon(imagePath("yx-chart")), tr("Regular chart [ v(t) ]"), actionGroup);
    actionRegulatChart->setCheckable(true);
    actionRegulatChart->setChecked(true);
    tb->addAction(actionRegulatChart);

    auto actionComplexityChart = new QAction(QIcon(imagePath("big-o-chart")), tr("Complexity chart [ t(v) ]"), actionGroup);
    actionComplexityChart->setCheckable(true);
    tb->addAction(actionComplexityChart);

    auto filtersWidget = new QWidget(this);
    auto filtersLay = new QHBoxLayout(filtersWidget);
    filtersLay->setContentsMargins(0, 0, 0, 0);
    filtersLay->addWidget(m_filterBoxLabel);
    filtersLay->addWidget(m_filterComboBox);
    filtersLay->addWidget(m_filterWindowLabel);
    filtersLay->addWidget(m_filterWindowPicker);

    tb->addWidget(filtersWidget);

    auto layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(tb);
    layout->addWidget(m_splitter);

    m_treeWidget->setAutoFillBackground(false);
    m_treeWidget->setAlternatingRowColors(true);
    m_treeWidget->setItemsExpandable(true);
    m_treeWidget->setAnimated(true);
    //m_treeWidget->setSortingEnabled(false);
    m_treeWidget->setColumnCount(int_cast(ArbitraryColumns::Count));
    m_treeWidget->setSelectionBehavior(QAbstractItemView::SelectRows);

    auto headerItem = new QTreeWidgetItem();
    headerItem->setText(int_cast(ArbitraryColumns::Type), "Type");
    headerItem->setText(int_cast(ArbitraryColumns::Name), "Name");
    headerItem->setText(int_cast(ArbitraryColumns::Value), "Value");
    headerItem->setText(int_cast(ArbitraryColumns::Vin), "ID");
    m_treeWidget->setHeaderItem(headerItem);

    connect(&m_collectionsTimer, &QTimer::timeout, this, &This::onCollectionsTimeout);

    using profiler_gui::GlobalSignals;
    auto globalEvents = &EASY_GLOBALS.events;
    connect(globalEvents, &GlobalSignals::selectedBlockChanged, this, &This::onSelectedBlockChanged);
    connect(globalEvents, &GlobalSignals::selectedBlockIdChanged, this, &This::onSelectedBlockIdChanged);
    connect(globalEvents, &GlobalSignals::allDataGoingToBeDeleted, this, &This::clear);

    connect(m_treeWidget->header(), &QHeaderView::sectionResized, this, &This::onHeaderSectionResized);

    if (_isMainWidget)
    {
        connect(m_treeWidget, &QTreeWidget::itemDoubleClicked, this, &This::onItemDoubleClicked);
        connect(m_treeWidget, &QTreeWidget::itemChanged, this, &This::onItemChanged);
        connect(m_treeWidget, &QTreeWidget::currentItemChanged, this, &This::onCurrentItemChanged);

        connect(globalEvents, &GlobalSignals::fileOpened, this, Overload<void>::of(&This::rebuild));
        connect(globalEvents, &GlobalSignals::selectedThreadChanged, this, &This::onSelectedThreadChanged);
    }

    loadSettings();

    m_filterComboBox->setCurrentIndex(int_cast(m_chart->filterType()));
    m_filterWindowPicker->setValue(m_chart->filterWindowSize());

    m_filterBoxLabel->setVisible(m_chart->chartType() == ChartType::Complexity);
    m_filterComboBox->setVisible(m_chart->chartType() == ChartType::Complexity);
    m_filterWindowLabel->setVisible(m_chart->chartType() == ChartType::Complexity && m_chart->filterType() != FilterType::None);
    m_filterWindowPicker->setVisible(m_chart->chartType() == ChartType::Complexity && m_chart->filterType() != FilterType::None);

    if (m_chart->chartType() == ChartType::Complexity)
        actionComplexityChart->setChecked(true);
    else
        actionRegulatChart->setChecked(true);

    connect(actionRegulatChart, &QAction::triggered, this, &This::onRegularChartTypeChecked);
    connect(actionComplexityChart, &QAction::triggered, this, &This::onComplexityChartTypeChecked);
    connect(m_filterComboBox, Overload<int>::of(&QComboBox::currentIndexChanged), this, &This::onFilterComboBoxChanged);
    connect(m_filterWindowPicker, Overload<int>::of(&QSpinBox::valueChanged), this, &This::onFilterWindowSizeChanged);

    rebuild(_threadId, _blockIndex, _blockId);
}

ArbitraryValuesWidget::ArbitraryValuesWidget(QWidget* _parent)
    : ArbitraryValuesWidget(true, EASY_GLOBALS.selected_thread, EASY_GLOBALS.selected_block, EASY_GLOBALS.selected_block_id, _parent)
{
    m_exportToCsvAction->setEnabled(false);
}

void ArbitraryValuesWidget::showEvent(QShowEvent* event)
{
    Parent::showEvent(event);

    if (!m_bInitialized)
    {
        m_columnsMinimumWidth.resize(static_cast<size_t>(ArbitraryColumns::Count), 0);

#if !defined(_WIN32) && !defined(__APPLE__)
        const auto padding = px(9);
#else
        const auto padding = px(6);
#endif

        auto header = m_treeWidget->header();
        auto headerItem = m_treeWidget->headerItem();

        auto f = header->font();
#if !defined(_WIN32) && !defined(__APPLE__)
        f.setBold(true);
#endif
        QFontMetrics fm(f);

        const auto indicatorSize = header->isSortIndicatorShown() ? px(11) : 0;
        for (int i = 0; i < static_cast<int>(m_columnsMinimumWidth.size()); ++i)
        {
            auto minSize = static_cast<int>(fm.width(headerItem->text(i)) * profiler_gui::FONT_METRICS_FACTOR + padding);
            m_columnsMinimumWidth[i] = minSize;

            if (header->isSortIndicatorShown() && header->sortIndicatorSection() == i)
            {
                minSize += indicatorSize;
            }

            if (header->sectionSize(i) < minSize)
            {
                header->resizeSection(i, minSize);
            }
        }

        m_bInitialized = true;
    }
}

void ArbitraryValuesWidget::onHeaderSectionResized(int logicalIndex, int /*oldSize*/, int newSize)
{
    if (logicalIndex >= m_columnsMinimumWidth.size())
    {
        return;
    }

    auto header = m_treeWidget->header();
    const auto indicatorSize = header->isSortIndicatorShown() && header->sortIndicatorSection() == logicalIndex ? px(11) : 0;
    const auto minSize = m_columnsMinimumWidth[logicalIndex] + indicatorSize;

    if (!m_bInitialized || newSize >= minSize)
    {
        return;
    }

    header->resizeSection(logicalIndex, minSize);
}

ArbitraryTreeWidgetItem* findSimilarItem(QTreeWidgetItem* _parentItem, ArbitraryTreeWidgetItem* _item)
{
    const auto index = _item->getSelfIndexInArray();
    for (int c = 0, childrenCount = _parentItem->childCount(); c < childrenCount; ++c)
    {
        auto child = _parentItem->child(c);
        if (child->type() == ValueItemType)
        {
            auto item = reinterpret_cast<ArbitraryTreeWidgetItem*>(child);
            if (item->getSelfIndexInArray() == index)
            {
                if (&_item->value() == &item->value())
                {
                    return item;
                }

                else if (_item->value().value_id() == item->value().value_id() &&
                         _item->value().type() == item->value().type() &&
                         _item->value().isArray() == item->value().isArray() &&
                         _item->text(int_cast(ArbitraryColumns::Name)) == item->text(int_cast(ArbitraryColumns::Name)))
                {
                    return item;
                }
            }
        }

        auto item = findSimilarItem(child, _item);
        if (item != nullptr)
            return item;
    }

    return nullptr;
}

ArbitraryValuesWidget::ArbitraryValuesWidget(const QList<ArbitraryTreeWidgetItem*>& _checkedItems
    , QTreeWidgetItem* _currentItem, profiler::thread_id_t _threadId, profiler::block_index_t _blockIndex
    , profiler::block_id_t _blockId, QWidget* _parent)
    : ArbitraryValuesWidget(false, _threadId, _blockIndex, _blockId, _parent)
{
    for (auto item : _checkedItems)
    {
        for (int i = 0, topLevelItemsCount = m_treeWidget->topLevelItemCount(); i < topLevelItemsCount; ++i)
        {
            auto foundItem = findSimilarItem(m_treeWidget->topLevelItem(i), item);
            if (foundItem != nullptr)
            {
                const auto checkState = item->checkState(CheckColumn);
                foundItem->setCheckState(CheckColumn, checkState);
                if (checkState == Qt::Checked)
                {
                    m_checkedItems.push_back(foundItem);
                    foundItem->collectValues(m_threadId, m_chart->chartType());
                }
            }
        }
    }

    if (!m_checkedItems.empty())
    {
        m_exportToCsvAction->setEnabled(true);
        m_collectionsTimer.start();

        std::set<QTreeWidgetItem*> checked;
        for (auto item : m_checkedItems)
        {
            if (item->getSelfIndexInArray() >= 0)
            {
                auto parentItem = item->parent();
                if (checked.find(parentItem) != checked.end())
                    continue;

                checked.insert(parentItem);

                Qt::CheckState newState = Qt::Checked;
                for (int i = 0, childCount = parentItem->childCount(); i < childCount; ++i)
                {
                    auto child = parentItem->child(i);
                    if (child->checkState(CheckColumn) != Qt::Checked)
                    {
                        newState = Qt::PartiallyChecked;
                        break;
                    }
                }

                parentItem->setCheckState(CheckColumn, newState);
            }
        }
    }
    else
    {
        m_exportToCsvAction->setEnabled(false);
    }

    if (_currentItem != nullptr)
    {
        if (_currentItem->type() == ValueItemType)
        {
            auto item = reinterpret_cast<ArbitraryTreeWidgetItem*>(_currentItem);
            for (int i = 0, topLevelItemsCount = m_treeWidget->topLevelItemCount(); i < topLevelItemsCount; ++i)
            {
                auto foundItem = findSimilarItem(m_treeWidget->topLevelItem(i), item);
                if (foundItem != nullptr)
                {
                    m_treeWidget->setCurrentItem(foundItem);
                    break;
                }
            }
        }
        else
        {
            const int col = static_cast<int>(ArbitraryColumns::Name);
            auto items = m_treeWidget->findItems(_currentItem->text(col), Qt::MatchExactly | Qt::MatchCaseSensitive | Qt::MatchRecursive, col);
            if (!items.empty())
                m_treeWidget->setCurrentItem(items.front());
        }
    }

    connect(m_treeWidget, &QTreeWidget::itemDoubleClicked, this, &This::onItemDoubleClicked);
    connect(m_treeWidget, &QTreeWidget::itemChanged, this, &This::onItemChanged);
    connect(m_treeWidget, &QTreeWidget::currentItemChanged, this, &This::onCurrentItemChanged);
}

ArbitraryValuesWidget::~ArbitraryValuesWidget()
{
    saveSettings();
}

void ArbitraryValuesWidget::clear()
{
    if (m_collectionsTimer.isActive())
        m_collectionsTimer.stop();

    // Warning! Cancel image update first because it uses POINTERS which will be destroyed
    // in m_treeWidget->clear()
    m_chart->cancelImageUpdate();

    m_exportToCsvAction->setEnabled(false);
    m_checkedItems.clear();
    m_treeWidget->clear();
    m_boldItem = nullptr;
}

void ArbitraryValuesWidget::onSelectedThreadChanged(profiler::thread_id_t)
{
    //rebuild();
}

void ArbitraryValuesWidget::onSelectedBlockChanged(uint32_t)
{
    m_chart->scene()->update();

    if (profiler_gui::is_max(EASY_GLOBALS.selected_block))
    {
        onSelectedBlockIdChanged(EASY_GLOBALS.selected_block_id);
        return;
    }

    // TODO: find item corresponding to selected_block and make it bold
}

void ArbitraryValuesWidget::onSelectedBlockIdChanged(::profiler::block_id_t)
{
    if (!profiler_gui::is_max(EASY_GLOBALS.selected_block))
        return;

    if (profiler_gui::is_max(EASY_GLOBALS.selected_block_id))
    {
        if (m_boldItem != nullptr)
        {
            m_boldItem->setBold(false);
            m_boldItem = nullptr;
        }

        return;
    }

    // TODO: find item corresponding to selected_block_id and make it bold
}

void ArbitraryValuesWidget::onItemDoubleClicked(QTreeWidgetItem* _item, int)
{
    if (_item == nullptr || _item->type() != ValueItemType || (_item->flags() & Qt::ItemIsUserCheckable) == 0)
        return;

    _item->setCheckState(CheckColumn, _item->checkState(CheckColumn) != Qt::Unchecked ? Qt::Unchecked : Qt::Checked);
}

void ArbitraryValuesWidget::onItemChanged(QTreeWidgetItem* _item, int _column)
{
    if (_item == nullptr || _item->type() != ValueItemType || _column != CheckColumn)
        return;

    if (_item->checkState(CheckColumn) == Qt::PartiallyChecked)
        return;

    auto item = static_cast<ArbitraryTreeWidgetItem*>(_item);

    if (item->checkState(CheckColumn) == Qt::Checked)
    {
        m_exportToCsvAction->setEnabled(true);

        const auto prevSize = m_checkedItems.size();
        if (!item->isArrayItem())
        {
            m_checkedItems.push_back(item);
            item->collectValues(EASY_GLOBALS.selected_thread, m_chart->chartType());

            if (item->getSelfIndexInArray() >= 0)
            {
                Qt::CheckState newState = Qt::Checked;
                auto parentItem = item->parent();
                for (int i = 0; i < parentItem->childCount(); ++i)
                {
                    auto child = parentItem->child(i);
                    if (child->checkState(CheckColumn) != Qt::Checked)
                    {
                        newState = Qt::PartiallyChecked;
                        break;
                    }
                }

                disconnect(m_treeWidget, &QTreeWidget::itemChanged, this, &This::onItemChanged);
                parentItem->setCheckState(CheckColumn, newState);
                connect(m_treeWidget, &QTreeWidget::itemChanged, this, &This::onItemChanged);
            }
        }
        else
        {
            disconnect(m_treeWidget, &QTreeWidget::itemChanged, this, &This::onItemChanged);
            for (int i = 0; i < item->childCount(); ++i)
            {
                auto child = static_cast<ArbitraryTreeWidgetItem*>(item->child(i));
                if (child->checkState(CheckColumn) != Qt::Checked)
                {
                    child->setCheckState(CheckColumn, Qt::Checked);
                    m_checkedItems.push_back(child);
                    child->collectValues(EASY_GLOBALS.selected_thread, m_chart->chartType());
                }
            }
            connect(m_treeWidget, &QTreeWidget::itemChanged, this, &This::onItemChanged);
        }

        if (prevSize != m_checkedItems.size())
        {
            if (!m_collectionsTimer.isActive())
                m_collectionsTimer.start();
        }
    }
    else
    {
        // !!!
        // Warning! Spaghetti-code detected! :)
        //
        // m_chart passes POINTERS to updateImage() so at first we MUST cancel image update
        // and only then we can invoke item->interrupt() because passed pointer will be destroyed
        // in interrupt().
        // !!!

        decltype(m_checkedItems) uncheckedItems;

        if (!item->isArrayItem())
        {
            uncheckedItems.push_back(item);
            m_checkedItems.removeOne(item);

            if (item->getSelfIndexInArray() >= 0)
            {
                Qt::CheckState newState = Qt::Unchecked;
                auto parentItem = item->parent();
                for (int i = 0; i < parentItem->childCount(); ++i)
                {
                    auto child = parentItem->child(i);
                    if (child->checkState(CheckColumn) != Qt::Unchecked)
                    {
                        newState = Qt::PartiallyChecked;
                        break;
                    }
                }

                disconnect(m_treeWidget, &QTreeWidget::itemChanged, this, &This::onItemChanged);
                parentItem->setCheckState(CheckColumn, newState);
                connect(m_treeWidget, &QTreeWidget::itemChanged, this, &This::onItemChanged);
            }
        }
        else
        {
            disconnect(m_treeWidget, &QTreeWidget::itemChanged, this, &This::onItemChanged);
            for (int i = 0; i < item->childCount(); ++i)
            {
                auto child = static_cast<ArbitraryTreeWidgetItem*>(item->child(i));
                if (child->checkState(CheckColumn) == Qt::Checked)
                {
                    child->setCheckState(CheckColumn, Qt::Unchecked);
                    uncheckedItems.push_back(child);
                    m_checkedItems.removeOne(child);
                }
            }
            connect(m_treeWidget, &QTreeWidget::itemChanged, this, &This::onItemChanged);
        }

        if (!uncheckedItems.isEmpty())
        {
            const bool hasCheckedItems = !m_checkedItems.empty();
            m_exportToCsvAction->setEnabled(hasCheckedItems);

            onCollectionsTimeout();

            for (auto uncheckedItem : uncheckedItems)
                uncheckedItem->interrupt();
        }
    }
}

void ArbitraryValuesWidget::onCurrentItemChanged(QTreeWidgetItem* _current, QTreeWidgetItem*)
{
    if (_current == nullptr || _current->type() != ValueItemType)
    {
        m_chart->update(nullptr);
        return;
    }

    auto item = static_cast<const ArbitraryTreeWidgetItem*>(_current);
    m_chart->update(item->collection());
}

void ArbitraryValuesWidget::rebuild()
{
    rebuild(EASY_GLOBALS.selected_thread, EASY_GLOBALS.selected_block, EASY_GLOBALS.selected_block_id);
}

void ArbitraryValuesWidget::rebuild(profiler::thread_id_t _threadId, profiler::block_index_t _blockIndex
    , profiler::block_id_t _blockId)
{
    clear();

    m_threadId = _threadId;
    m_blockIndex = _blockIndex;
    m_blockId = _blockId;

    buildTree(_threadId, _blockIndex, _blockId);

    m_treeWidget->expandAll();
    for (int i = 0, columns = m_treeWidget->columnCount(); i < columns; ++i)
        m_treeWidget->resizeColumnToContents(i);

    if (m_bMainWidget && !profiler_gui::is_max(_blockIndex))
    {
        const auto& block = easyBlocksTree(_blockIndex);
        if (easyDescriptor(block.node->id()).type() == profiler::BlockType::Value)
            select(*block.value, false);
    }
}

void ArbitraryValuesWidget::select(const profiler::ArbitraryValue& _value, bool _resetOthers)
{
    if (!m_checkedItems.empty() && _resetOthers)
    {
        disconnect(m_treeWidget, &QTreeWidget::itemChanged, this, &This::onItemChanged);

        for (auto item : m_checkedItems)
            item->setCheckState(CheckColumn, Qt::Unchecked);
        decltype(m_checkedItems) uncheckedItems(std::move(m_checkedItems));

        m_exportToCsvAction->setEnabled(false);

        onCollectionsTimeout();

        for (auto item : uncheckedItems)
            item->interrupt();

        connect(m_treeWidget, &QTreeWidget::itemChanged, this, &This::onItemChanged);
    }

    auto items = m_treeWidget->findItems(easyDescriptor(_value.id()).name(), Qt::MatchExactly | Qt::MatchCaseSensitive | Qt::MatchRecursive, int_cast(ArbitraryColumns::Name));
    if (!items.empty())
    {
        for (auto i : items)
        {
            if (i->type() != ValueItemType)
                continue;

            auto item = reinterpret_cast<ArbitraryTreeWidgetItem*>(i);
            const auto& value = item->value();
            if (value.value_id() != _value.value_id() || value.type() != _value.type())
                continue;

            m_treeWidget->setCurrentItem(item);
            item->setCheckState(CheckColumn, Qt::Checked);

            break;
        }
    }
}

void ArbitraryValuesWidget::onCollectionsTimeout()
{
    if (m_checkedItems.empty())
    {
        if (m_collectionsTimer.isActive())
            m_collectionsTimer.stop();
        m_chart->update(Collections {});
        return;
    }

    Collections collections;
    collections.reserve(static_cast<size_t>(m_checkedItems.size()));
    for (auto item : m_checkedItems)
    {
        if (item->collection()->status() != ArbitraryValuesCollection::InProgress)
        {
            collections.push_back(CollectionPaintData {item->collection(), item->color(),
                ChartPenStyle::Line, item == m_treeWidget->currentItem()});
        }
    }

    if (collections.size() == m_checkedItems.size())
    {
        if (m_collectionsTimer.isActive())
            m_collectionsTimer.stop();
        m_chart->update(std::move(collections));
    }
}

void ArbitraryValuesWidget::repaint()
{
    if (!m_checkedItems.empty())
    {
        m_chart->cancelImageUpdate();

        for (auto item : m_checkedItems)
            item->collectValues(EASY_GLOBALS.selected_thread, m_chart->chartType());

        if (!m_collectionsTimer.isActive())
            m_collectionsTimer.start();
    }
}

void ArbitraryValuesWidget::onRegularChartTypeChecked(bool _checked)
{
    if (!_checked)
        return;

    m_chart->setChartType(ChartType::Regular);

    m_filterWindowPicker->hide();
    m_filterWindowLabel->hide();
    m_filterComboBox->hide();
    m_filterBoxLabel->hide();

    repaint();
}

void ArbitraryValuesWidget::onComplexityChartTypeChecked(bool _checked)
{
    if (!_checked)
        return;

    m_chart->setChartType(ChartType::Complexity);

    m_filterBoxLabel->show();
    m_filterComboBox->show();
    m_filterWindowLabel->setVisible(m_filterComboBox->currentIndex() != 0);
    m_filterWindowPicker->setVisible(m_filterComboBox->currentIndex() != 0);

    repaint();
}

void ArbitraryValuesWidget::onFilterComboBoxChanged(int _index)
{
    switch (_index)
    {
        case 1:
            m_filterWindowLabel->show();
            m_filterWindowPicker->show();
            m_chart->setFilterType(FilterType::Gauss);
            break;

        case 2:
            m_filterWindowLabel->show();
            m_filterWindowPicker->show();
            m_chart->setFilterType(FilterType::Median);
            break;

        default:
            m_filterWindowLabel->hide();
            m_filterWindowPicker->hide();
            m_chart->setFilterType(FilterType::None);
            break;
    }
}

void ArbitraryValuesWidget::onFilterWindowSizeChanged(int _size)
{
    m_chart->setFilterWindowSize(_size);
}

void ArbitraryValuesWidget::onExportToCsvClicked(bool)
{
    if (m_checkedItems.empty())
        return;

    auto filename = QFileDialog::getSaveFileName(this, "Export arbitrary values to csv", EASY_GLOBALS.lastFileDir,
                                                 "CSV File Format (*.csv)");

    if (filename.isEmpty())
        return;

    QFileInfo fileinfo(filename);
    EASY_GLOBALS.lastFileDir = fileinfo.absoluteDir().canonicalPath();
    if (fileinfo.suffix() != QStringLiteral("csv"))
        filename += QStringLiteral(".csv");

    QFile csv(filename);
    if (!csv.open(QIODevice::WriteOnly | QIODevice::Text) || !csv.isOpen())
    {
        QMessageBox::warning(this, "Warning", "Can not open file for writing", QMessageBox::Close);
        return;
    }

    for (auto item : m_checkedItems)
    {
        if (item->collection()->status() == ArbitraryValuesCollection::InProgress)
        {
            QMessageBox::warning(this, "Warning", "Not all values collected to the moment.\nTry again a bit later.",
                                 QMessageBox::Close);
            return;
        }
    }

    if (m_chart->chartType() == ChartType::Regular)
    {
        for (auto item : m_checkedItems)
        {
            const auto header = QString(" ; ;\nname:;%1;\ntimestamp;value;\n").arg(item->text(int_cast(ArbitraryColumns::Name)));
            csv.write(header.toStdString().c_str());

            const auto& values = item->collection()->values();
            for (auto value : values)
            {
                const auto str = QString("%1;%2;\n").arg(value->begin()).arg(profiler_gui::valueString(*value));
                csv.write(str.toStdString().c_str());
            }
        }
    }
    else
    {
        for (auto item : m_checkedItems)
        {
            const auto header = QString(" ; ; ; ; ;\nname:;%1; ; ; ;\nvalue;avg duration;min duration;max duration;count;\n")
                .arg(item->text(int_cast(ArbitraryColumns::Name)));

            csv.write(header.toStdString().c_str());

            const auto& complexityMap = item->collection()->complexityMap();
            for (const auto& kv : complexityMap)
            {
                const auto& durations = kv.second;

                auto val = QString::number(kv.first, 'f', 12);

                const int last = val.size() - 1;
                int c = last;
                for (; c >= 0 && val[c] == QChar('0'); --c);
                if (val[c] == QChar('.'))
                    --c;

                if (c != last)
                    val.chop(last - c);

                if (durations.empty())
                {
                    const auto str = QString("%1;0;0;0;0;\n").arg(val);
                    csv.write(str.toStdString().c_str());
                    continue;
                }

                const auto avg = std::accumulate(durations.begin(), durations.end(), profiler::timestamp_t(0)) / durations.size();
                const auto str = QString("%1;%2;%3;%4;%5;\n").arg(val).arg(avg).arg(durations.front())
                    .arg(durations.back()).arg(durations.size());

                csv.write(str.toStdString().c_str());
            }
        }
    }

    csv.write("\n");
}

void ArbitraryValuesWidget::onOpenInNewWindowClicked(bool)
{
    saveSettings();

    auto viewer = new ArbitraryValuesWidget(m_checkedItems, m_treeWidget->currentItem(), m_threadId, m_blockIndex, m_blockId);

    auto dialog = new Dialog(nullptr, "EasyProfiler", viewer, WindowHeader::AllButtons, QMessageBox::NoButton);
    dialog->setProperty("stayVisible", true);
    dialog->setAttribute(Qt::WA_DeleteOnClose, true);
    connect(&EASY_GLOBALS.events, &profiler_gui::GlobalSignals::allDataGoingToBeDeleted, dialog, &QDialog::reject);
    connect(&EASY_GLOBALS.events, &profiler_gui::GlobalSignals::closeEvent, dialog, &QDialog::reject);

    // Load last dialog geometry
    {
        QSettings settings(profiler_gui::ORGANAZATION_NAME, profiler_gui::APPLICATION_NAME);
        settings.beginGroup("ArbitraryValuesWidgetWindow");
        auto geometry = settings.value("dialog/geometry").toByteArray();
        settings.endGroup();

        if (!geometry.isEmpty())
            dialog->restoreGeometry(geometry);
    }

    dialog->show();
}

void ArbitraryValuesWidget::buildTree(profiler::thread_id_t _threadId, profiler::block_index_t _blockIndex, profiler::block_id_t _blockId)
{
    m_treeWidget->clear();
    m_treeWidget->setColumnHidden(int_cast(ArbitraryColumns::Value), profiler_gui::is_max(_blockIndex));

    if (_threadId != 0)
    {
        auto it = EASY_GLOBALS.profiler_blocks.find(_threadId);
        if (it != EASY_GLOBALS.profiler_blocks.end())
        {
            auto threadItem = buildTreeForThread(it->second, _blockIndex, _blockId);
            m_treeWidget->addTopLevelItem(threadItem);
        }
    }
    else
    {
        for (const auto& it : EASY_GLOBALS.profiler_blocks)
        {
            auto threadItem = buildTreeForThread(it.second, _blockIndex, _blockId);
            m_treeWidget->addTopLevelItem(threadItem);
        }
    }
}

QTreeWidgetItem* ArbitraryValuesWidget::buildTreeForThread(const profiler::BlocksTreeRoot& _threadRoot, profiler::block_index_t _blockIndex, profiler::block_id_t _blockId)
{
    auto fm = m_treeWidget->fontMetrics();

    auto rootItem = new QTreeWidgetItem(StdItemType);
    rootItem->setText(int_cast(ArbitraryColumns::Type), QStringLiteral("Thread"));
    rootItem->setText(int_cast(ArbitraryColumns::Name),
        profiler_gui::decoratedThreadName(EASY_GLOBALS.use_decorated_thread_name, _threadRoot, EASY_GLOBALS.hex_thread_id));
    rootItem->setData(int_cast(ArbitraryColumns::Type), Qt::UserRole, 0);

    bool hasParticularBlockIndex = !profiler_gui::is_max(_blockIndex);
    if (hasParticularBlockIndex)
    {
        const auto& block = easyBlocksTree(_blockIndex);
        const auto& desc = easyDescriptor(block.node->id());
        if (desc.type() != profiler::BlockType::Block)
        {
            hasParticularBlockIndex = false;
            profiler_gui::set_max(_blockIndex);
            profiler_gui::set_max(_blockId);

            /*
            auto value = block.value;
            const bool isString = value->type() == profiler::DataType::String;
            auto valueItem = new ArbitraryTreeWidgetItem(rootItem, !isString, desc.color(), *value);
            valueItem->setText(int_cast(ArbitraryColumns::Type), profiler_gui::valueTypeString(*value));
            valueItem->setText(int_cast(ArbitraryColumns::Name), desc.name());
            valueItem->setText(int_cast(ArbitraryColumns::Value), profiler_gui::valueString(*value));
            valueItem->setData(int_cast(ArbitraryColumns::Type), Qt::UserRole, 2);

            const auto sizeHintWidth = valueItem->sizeHint(CheckColumn).width();
            valueItem->setWidthHint(std::max(sizeHintWidth, fm.width(valueItem->text(CheckColumn))) + 32);

            return rootItem;
            */
        }
        else
        {
            _blockId = block.node->id();
        }
    }

    const bool anyBlockId = profiler_gui::is_max(_blockId);
    const bool hasParticularBlockId = !anyBlockId;

    using Vins = std::unordered_map<profiler::vin_t, UsedValueTypes, estd::hash<profiler::vin_t> >;
    using BlocksMap = std::unordered_map<profiler::block_id_t, QTreeWidgetItem*, estd::hash<profiler::block_id_t> >;
    using GlobalValues = std::unordered_map<std::string, UsedValueTypes>;

    using StackEntry = std::pair<profiler::block_index_t, profiler::block_index_t>;
    using Stack = std::vector<StackEntry>;

    Vins vins;
    BlocksMap blocks;
    GlobalValues globalValues;

    QTreeWidgetItem* blockItem = nullptr;
    if (hasParticularBlockId)
    {
        blockItem = new QTreeWidgetItem(rootItem, StdItemType);
        blockItem->setText(int_cast(ArbitraryColumns::Type), QStringLiteral("Block"));

        if (hasParticularBlockIndex)
            blockItem->setText(int_cast(ArbitraryColumns::Name), easyBlockName(_blockIndex));
        else
            blockItem->setText(int_cast(ArbitraryColumns::Name), easyDescriptor(_blockId).name());

        blockItem->setData(int_cast(ArbitraryColumns::Type), Qt::UserRole, 1);
        blockItem->setData(int_cast(ArbitraryColumns::Vin), Qt::UserRole, _blockId);

        blocks[_blockId] = blockItem;
    }

    // Depth-first search traverse

    Stack stack;
    for (const auto index : _threadRoot.children)
    {
        size_t matchedParentIdStackDepth = 0;
        bool matchedParentId = anyBlockId;

        stack.clear();
        stack.emplace_back(index, static_cast<profiler::block_index_t>(0));

        if (anyBlockId)
            blockItem = nullptr;

        while (!stack.empty())
        {
            auto& top = stack.back();
            auto& first = top.second;
            const auto i = top.first;

            const auto& block = easyBlock(i).tree;
            const auto& desc = easyDescriptor(block.node->id());

            if (desc.type() == profiler::BlockType::Value && matchedParentId)
            {
                const auto value = block.value;

                const auto typeIndex = int_cast(value->type());
                auto vin = value->value_id();

                ArbitraryTreeWidgetItem** usedItems = nullptr;
                ArbitraryTreeWidgetItem* valueItem = nullptr;
                if (vin == 0)
                {
                    auto result = globalValues.emplace(desc.name(), 0);
                    usedItems = result.first->second.items;
                    if (!result.second)
                        valueItem = *(usedItems + typeIndex);
                }
                else
                {
                    auto result = vins.emplace(vin, 0);
                    usedItems = result.first->second.items;
                    if (!result.second)
                        valueItem = *(usedItems + typeIndex);
                }

                if (valueItem != nullptr)
                {
                    if (i == _blockIndex)
                        valueItem->setText(int_cast(ArbitraryColumns::Value), profiler_gui::shortValueString(*value));
                    //continue; // already in set

                    if (value->isArray() && value->type() != profiler::DataType::String)
                    {
                        const int size = profiler_gui::valueArraySize(*value);
                        if (valueItem->childCount() < size)
                        {
                            for (int childIndex = valueItem->childCount(); childIndex < size; ++childIndex)
                            {
                                auto item = new ArbitraryTreeWidgetItem(valueItem, true, desc.color(), *value);
                                item->setText(int_cast(ArbitraryColumns::Name), QString("%1[%2]").arg(desc.name()).arg(childIndex));
                                item->setData(int_cast(ArbitraryColumns::Type), Qt::UserRole, 3);

                                if (i == _blockIndex)
                                    item->setText(int_cast(ArbitraryColumns::Value), profiler_gui::valueString(*value, childIndex));

                                const auto sizeHintWidth = valueItem->sizeHint(CheckColumn).width();
                                item->setWidthHint(std::max(sizeHintWidth, fm.width(valueItem->text(CheckColumn))) + 32);
                            }

                            auto typeString = profiler_gui::valueTypeString(*value);
                            valueItem->setText(int_cast(ArbitraryColumns::Type), typeString.replace("[]", "[%1]").arg(size));
                        }
                    }
                }
                else
                {
                    if (blockItem == nullptr)
                    {
                        // Enter here only if anyBlockId is true.
                        // matchedParentIdStackDepth is always == 0 in such case.
                        const auto parentBlockIndex = stack[matchedParentIdStackDepth].first;
                        const auto& parentBlock = easyBlock(parentBlockIndex).tree;
                        const auto id = parentBlock.node->id();
                        auto it = blocks.find(id);
                        if (it != blocks.end())
                        {
                            blockItem = it->second;
                        }
                        else
                        {
                            blockItem = new QTreeWidgetItem(rootItem, StdItemType);
                            blockItem->setText(int_cast(ArbitraryColumns::Type), QStringLiteral("Block"));
                            blockItem->setText(int_cast(ArbitraryColumns::Name), easyBlockName(parentBlock));
                            blockItem->setData(int_cast(ArbitraryColumns::Type), Qt::UserRole, 1);
                            blockItem->setData(int_cast(ArbitraryColumns::Vin), Qt::UserRole, id);
                            blocks.emplace(id, blockItem);
                        }
                    }

                    const bool isString = value->type() == profiler::DataType::String;
                    valueItem = new ArbitraryTreeWidgetItem(blockItem, !isString, desc.color(), *value);
                    valueItem->setText(int_cast(ArbitraryColumns::Type), profiler_gui::valueTypeString(*value));
                    valueItem->setText(int_cast(ArbitraryColumns::Name), desc.name());
                    valueItem->setData(int_cast(ArbitraryColumns::Type), Qt::UserRole, 2);

                    if (i == _blockIndex)
                        valueItem->setText(int_cast(ArbitraryColumns::Value), profiler_gui::shortValueString(*value));

                    auto sizeHintWidth = valueItem->sizeHint(CheckColumn).width();
                    valueItem->setWidthHint(std::max(sizeHintWidth, fm.width(valueItem->text(CheckColumn))) + 32);

                    *(usedItems + typeIndex) = valueItem;

                    if (value->isArray() && !isString)
                    {
                        const int size = profiler_gui::valueArraySize(*value);
                        if (valueItem->childCount() < size)
                        {
                            for (int childIndex = valueItem->childCount(); childIndex < size; ++childIndex)
                            {
                                auto item = new ArbitraryTreeWidgetItem(valueItem, true, desc.color(), *value);
                                item->setText(int_cast(ArbitraryColumns::Name), QString("%1[%2]").arg(desc.name()).arg(childIndex));
                                item->setData(int_cast(ArbitraryColumns::Type), Qt::UserRole, 3);

                                if (i == _blockIndex)
                                    item->setText(int_cast(ArbitraryColumns::Value), profiler_gui::valueString(*value, childIndex));

                                sizeHintWidth = valueItem->sizeHint(CheckColumn).width();
                                item->setWidthHint(std::max(sizeHintWidth, fm.width(valueItem->text(CheckColumn))) + 32);
                            }

                            auto typeString = profiler_gui::valueTypeString(*value);
                            valueItem->setText(int_cast(ArbitraryColumns::Type), typeString.replace("[]", "[%1]").arg(size));
                        }
                    }
                }
            }

            if (first < block.children.size())
            {
                if (hasParticularBlockId && (block.node->id() == _blockId || desc.id() == _blockId))
                {
                    if (!matchedParentId)
                        matchedParentIdStackDepth = stack.size();
                    matchedParentId = true;
                }

                const auto child = block.children[first++];
                stack.emplace_back(child, static_cast<profiler::block_index_t>(0));
            }
            else
            {
                if (hasParticularBlockId && stack.size() == matchedParentIdStackDepth)
                {
                    matchedParentId = false;
                    blockItem = nullptr;
                }

                stack.pop_back();
            }
        }
    }

    return rootItem;
}

void ArbitraryValuesWidget::loadSettings()
{
    QSettings settings(::profiler_gui::ORGANAZATION_NAME, ::profiler_gui::APPLICATION_NAME);
    settings.beginGroup("ArbitraryValuesWidget");

    auto value = settings.value("chart/filterWindow");
    if (!value.isNull())
        m_chart->setFilterWindowSize(value.toInt());

    value = settings.value("chart/filter");
    if (!value.isNull())
        m_chart->setFilterType(static_cast<FilterType>(value.toInt()));

    value = settings.value("chart/type");
    if (!value.isNull())
        m_chart->setChartType(static_cast<ChartType>(value.toInt()));

    if (!m_bMainWidget)
    {
        settings.endGroup();
        settings.beginGroup("ArbitraryValuesWidgetWindow");
    }

    auto geometry = settings.value("hsplitter/geometry").toByteArray();
    if (!geometry.isEmpty())
        m_splitter->restoreGeometry(geometry);

    auto state = settings.value("hsplitter/state").toByteArray();
    if (!state.isEmpty())
        m_splitter->restoreState(state);

    settings.endGroup();
}

void ArbitraryValuesWidget::saveSettings()
{
    QSettings settings(::profiler_gui::ORGANAZATION_NAME, ::profiler_gui::APPLICATION_NAME);

    if (!m_bMainWidget)
    {
        settings.beginGroup("ArbitraryValuesWidgetWindow");
        settings.setValue("dialog/geometry", parentWidget()->saveGeometry());
        settings.setValue("hsplitter/geometry", m_splitter->saveGeometry());
        settings.setValue("hsplitter/state", m_splitter->saveState());
    }
    else
    {
        settings.beginGroup("ArbitraryValuesWidget");
        settings.setValue("hsplitter/geometry", m_splitter->saveGeometry());
        settings.setValue("hsplitter/state", m_splitter->saveState());
        settings.setValue("chart/type", static_cast<int>(m_chart->chartType()));
        settings.setValue("chart/filter", static_cast<int>(m_chart->filterType()));
        settings.setValue("chart/filterWindow", m_chart->filterWindowSize());
    }

    settings.endGroup();
}
