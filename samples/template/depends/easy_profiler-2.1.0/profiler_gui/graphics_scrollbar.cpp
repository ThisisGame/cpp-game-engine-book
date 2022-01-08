/************************************************************************
* file name         : graphics_scrollbar.cpp
* ----------------- :
* creation time     : 2016/07/04
* author            : Victor Zarubkin
* email             : v.s.zarubkin@gmail.com
* ----------------- :
* description       : .
* ----------------- :
* change log        : * 2016/07/04 Victor Zarubkin: Initial commit.
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

#include <algorithm>
#include <QGraphicsScene>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QResizeEvent>
#include <easy/utility.h>
#include "graphics_scrollbar.h"
#include "globals.h"

namespace {

//////////////////////////////////////////////////////////////////////////

EASY_CONSTEXPR int HIST_COLUMN_MIN_HEIGHT = 2;

//////////////////////////////////////////////////////////////////////////

inline qreal calculate_color1(qreal h, qreal, qreal k)
{
    return std::min(h * k, 0.9999999);
}

inline qreal calculate_color2(qreal, qreal duration, qreal k)
{
    using estd::sqr;
    return std::min(sqr(sqr(duration)) * k, 0.9999999);
}

} // end of namespace <noname>.

//////////////////////////////////////////////////////////////////////////

GraphicsHistogramItem::GraphicsHistogramItem() : Parent()
    , m_workerTopDuration(0)
    , m_workerBottomDuration(0)
    , m_blockTotalDuraion(0)
    , m_threadDuration(0)
    , m_threadProfiledTime(0)
    , m_threadWaitTime(0)
    , m_medianDuration(0)
    , m_avgDuration(0)
    , m_medianDurationFull(0)
    , m_avgDurationFull(0)
    , m_workerMedianDuration(0)
    , m_workerAvgDuration(0)
    , m_pSource(nullptr)
    , m_pProfilerThread(nullptr)
    , m_threadId(0)
    , m_blockId(profiler_gui::numeric_max<decltype(m_blockId)>())
    , m_timeUnits(profiler_gui::TimeUnits_auto)
    , m_regime(Hist_Pointer)
{

}

GraphicsHistogramItem::~GraphicsHistogramItem()
{

}

void GraphicsHistogramItem::paint(QPainter* _painter, const QStyleOptionGraphicsItem* /* _option */, QWidget* /* _widget */)
{
    if (!isImageUpdatePermitted() || (m_regime == Hist_Pointer && m_pSource == nullptr) || (m_regime == Hist_Id && (m_threadId == 0 || profiler_gui::is_max(m_blockId))))
        return;

    if (m_regime == Hist_Pointer)
        paintByPtr(_painter);
    else
        paintById(_painter);
}

void GraphicsHistogramItem::paintMouseIndicator(QPainter* _painter, qreal _top, qreal _bottom, qreal _width, qreal _height, qreal _top_width, qreal _mouse_y, qreal _delta_time, int _font_h)
{
    if (isEmpty())
    {
        return;
    }

    if (_font_h != 0 && _top < _mouse_y && _mouse_y < _bottom)
    {
        const int half_font_h = _font_h >> 1;

        _painter->setPen(Qt::blue);

        const auto mouseStr = profiler_gui::timeStringReal(m_timeUnits, m_bottomValue + _delta_time * (_bottom - _mouse_y) / _height, 3);
        qreal mouseIndicatorRight = _width;
        if (_mouse_y < _top + half_font_h)
            mouseIndicatorRight = _top_width;

        qreal mouseIndicatorLeft = 0;
        const QRectF rect(0, _mouse_y - _font_h - 2, _width, 4 + (_font_h << 1));
        if (_mouse_y > _bottom - half_font_h)
        {
            _painter->drawText(rect, Qt::AlignLeft | Qt::AlignTop, mouseStr);
        }
        else if (_mouse_y < _top + half_font_h)
        {
            _painter->drawText(rect, Qt::AlignLeft | Qt::AlignBottom, mouseStr);
        }
        else
        {
            _painter->drawText(rect, Qt::AlignLeft | Qt::AlignVCenter, mouseStr);
            mouseIndicatorLeft = _painter->fontMetrics().width(mouseStr) + 3;
        }

        _painter->drawLine(QLineF(mouseIndicatorLeft, _mouse_y, mouseIndicatorRight, _mouse_y));
    }
}

void GraphicsHistogramItem::paintByPtr(QPainter* _painter)
{
    const auto widget = static_cast<const GraphicsScrollbar*>(scene()->parent());
    const bool bindMode = widget->bindMode();
    const auto currentScale = widget->getWindowScale();
    const auto bottom = m_boundingRect.bottom();
    const auto width = m_boundingRect.width() * currentScale;
    const auto dtime = m_topValue - m_bottomValue;
    const auto coeff = m_boundingRect.height() / (dtime > 1e-3 ? dtime : 1.);

    QBrush brush(Qt::SolidPattern);
    //QRgb previousColor = 0;

    _painter->save();
    _painter->setTransform(QTransform::fromScale(1.0 / currentScale, 1), true);

    if (!m_pSource->empty())
    {
        if (!bindMode)
            paintImage(_painter);
        else
            paintImage(_painter, currentScale, widget->minimum(), widget->maximum(), widget->value(), widget->sliderWidth());
    }

    qreal top_width = width, bottom_width = width;
    const auto font_h = widget->fontHeight();

    QRectF bottomRect(3, bottom + 2, width - 3, font_h);
    QRectF topRect(3, m_boundingRect.top() - widget->margin(), width - 3, font_h);
    QRectF textBounds;

    // MODE
    {
        _painter->setPen(Qt::blue);
        _painter->drawText(
            topRect,
            Qt::AlignLeft | Qt::AlignVCenter | Qt::TextDontClip | Qt::TextIncludeTrailingSpaces,
            QStringLiteral("MODE: "),
            &textBounds
        );
        topRect.adjust(textBounds.width(), 0, 0, 0);

        _painter->setPen(profiler_gui::TEXT_COLOR);
        _painter->drawText(
            topRect,
            Qt::AlignLeft | Qt::AlignVCenter | Qt::TextDontClip,
            bindMode ? "Zoom" : "Overview",
            &textBounds
        );
        topRect.adjust(textBounds.width() + 3, 0, 0, 0);
    }

    // TOP & BOTTOM duration
    if (!isEmpty() && !m_topDurationStr.isEmpty())
    {
        if (m_timeUnits != EASY_GLOBALS.time_units)
        {
            m_timeUnits = EASY_GLOBALS.time_units;
            m_topDurationStr = profiler_gui::timeStringReal(m_timeUnits, m_topValue, 3);
            m_bottomDurationStr = profiler_gui::timeStringReal(m_timeUnits, m_bottomValue, 3);

            if (m_avgDuration != 0 || m_medianDuration != 0)
            {
                m_medianDurationStr = profiler_gui::timeStringRealNs(m_timeUnits, m_medianDuration, 3);
                m_avgDurationStr = profiler_gui::timeStringRealNs(m_timeUnits, m_avgDuration, 3);
            }
        }

        //auto fm = _painter->fontMetrics();
        //top_width -= fm.width(m_topDurationStr) + 7;

        _painter->setPen(m_topValue < m_maxValue ? QColor(Qt::darkRed) : profiler_gui::TEXT_COLOR);
        _painter->drawText(topRect, Qt::AlignRight | Qt::AlignVCenter | Qt::TextDontClip, m_topDurationStr, &textBounds);
        topRect.adjust(0, 0, -textBounds.width() - 3, 0);

        _painter->setPen(m_bottomValue > m_minValue ? QColor(Qt::darkRed) : profiler_gui::TEXT_COLOR);
        _painter->drawText(bottomRect, Qt::AlignRight | Qt::AlignVCenter | Qt::TextDontClip, m_bottomDurationStr, &textBounds);
        bottomRect.adjust(0, 0, -textBounds.width() - 3, 0);
    }

    _painter->setPen(Qt::darkGray);
    _painter->drawLine(QLineF(0, bottom, bottom_width, bottom));
    _painter->drawLine(QLineF(0, m_boundingRect.top(), top_width, m_boundingRect.top()));

    paintMouseIndicator(_painter, m_boundingRect.top(), bottom, width, m_boundingRect.height(), top_width, m_mousePos.y(), dtime, font_h);

    // MEDIAN & EXPECTED FRAME TIME
    if (!isEmpty())
    {
        if (m_bottomValue < EASY_GLOBALS.frame_time && EASY_GLOBALS.frame_time < m_topValue)
        {
            // Draw marker displaying expected frame_time step
            const auto h = bottom - (EASY_GLOBALS.frame_time - m_bottomValue) * coeff;
            _painter->setPen(Qt::DashLine);

            auto w = width;
            const auto boundary = widget->margin() - font_h;
            if (h < (m_boundingRect.top() - boundary))
                w = top_width;
            else if (h > (bottom + boundary))
                w = bottom_width;

            _painter->drawLine(QLineF(0, h, w, h));
        }
    }

    _painter->setPen(profiler_gui::TEXT_COLOR);
    const auto eventsCount = m_pProfilerThread->events.size();
    const auto blocksCount = m_pProfilerThread->blocks_number - eventsCount;

    QString durationsStr;
    if (!m_medianDurationStr.isEmpty() || !m_avgDurationStr.isEmpty())
    {
        durationsStr = QString("avg: %1 | mdn: %2 | ").arg(m_avgDurationStr).arg(m_medianDurationStr);
    }

    _painter->drawText(topRect, Qt::AlignCenter, m_threadName);

    _painter->drawText(bottomRect, Qt::AlignCenter,
        QStringLiteral("time: %1 | profiled: %2 (%3%) | wait: %4 (%5%) | %6%7 frames | %8 blocks | %9 markers")
            .arg(profiler_gui::timeStringRealNs(EASY_GLOBALS.time_units, m_threadDuration))
            .arg(profiler_gui::timeStringRealNs(EASY_GLOBALS.time_units, m_threadProfiledTime))
            .arg(m_threadDuration ? QString::number(100. * (double) m_threadProfiledTime / (double) m_threadDuration, 'f', 2)
                                  : QString("0"))
            .arg(profiler_gui::timeStringRealNs(EASY_GLOBALS.time_units, m_threadWaitTime))
            .arg(m_threadDuration ? QString::number(100. * (double) m_threadWaitTime / (double) m_threadDuration, 'f', 2)
                                  : QString("0"))
            .arg(durationsStr)
            .arg(profiler_gui::shortenCountString(m_pProfilerThread->frames_number))
            .arg(profiler_gui::shortenCountString(blocksCount))
            .arg(profiler_gui::shortenCountString(eventsCount))
    );

    _painter->restore();
}

void GraphicsHistogramItem::paintById(QPainter* _painter)
{
    const auto widget = static_cast<const GraphicsScrollbar*>(scene()->parent());
    const bool bindMode = widget->bindMode();
    const auto currentScale = widget->getWindowScale();
    const auto bottom = m_boundingRect.bottom();
    const auto width = m_boundingRect.width() * currentScale;
    const auto dtime = m_topValue - m_bottomValue;
    const auto coeff = m_boundingRect.height() / (dtime > 1e-3 ? dtime : 1.);

    QBrush brush(Qt::SolidPattern);
    //QRgb previousColor = 0;

    _painter->save();
    _painter->setTransform(QTransform::fromScale(1.0 / currentScale, 1), true);

    const auto& items = m_selectedBlocks;
    if (!items.empty())
    {
        if (!bindMode)
            paintImage(_painter);
        else
            paintImage(_painter, currentScale, widget->minimum(), widget->maximum(), widget->value(), widget->sliderWidth());
    }

    qreal top_width = width, bottom_width = width;
    const auto font_h = widget->fontHeight();

    QRectF bottomRect(3, bottom + 2, width - 3, font_h);
    QRectF topRect(3, m_boundingRect.top() - widget->margin(), width - 3, font_h);
    QRectF textBounds;

    // MODE
    {
        _painter->setPen(Qt::blue);
        _painter->drawText(
            topRect,
            Qt::AlignLeft | Qt::AlignVCenter | Qt::TextDontClip | Qt::TextIncludeTrailingSpaces,
            QStringLiteral("MODE: "),
            &textBounds
        );
        topRect.adjust(textBounds.width(), 0, 0, 0);

        _painter->setPen(profiler_gui::TEXT_COLOR);
        _painter->drawText(
            topRect,
            Qt::AlignLeft | Qt::AlignVCenter | Qt::TextDontClip,
            bindMode ? "Zoom" : "Overview",
            &textBounds
        );
        topRect.adjust(textBounds.width() + 3, 0, 0, 0);
    }

    // TOP & BOTTOM duration
    if (!isEmpty() && !m_topDurationStr.isEmpty())
    {
        if (m_timeUnits != EASY_GLOBALS.time_units)
        {
            m_timeUnits = EASY_GLOBALS.time_units;
            m_topDurationStr = profiler_gui::timeStringReal(m_timeUnits, m_topValue, 3);
            m_bottomDurationStr = profiler_gui::timeStringReal(m_timeUnits, m_bottomValue, 3);

            if (m_avgDuration != 0 || m_medianDuration != 0)
            {
                m_medianDurationStr = profiler_gui::timeStringRealNs(m_timeUnits, m_medianDuration, 3);
                m_avgDurationStr = profiler_gui::timeStringRealNs(m_timeUnits, m_avgDuration, 3);
            }
        }

        //auto fm = _painter->fontMetrics();
        //top_width -= fm.width(m_topDurationStr) + 7;

        _painter->setPen(m_topValue < m_maxValue ? QColor(Qt::darkRed) : profiler_gui::TEXT_COLOR);
        _painter->drawText(topRect, Qt::AlignRight | Qt::AlignVCenter | Qt::TextDontClip, m_topDurationStr, &textBounds);
        topRect.adjust(0, 0, -textBounds.width() - 3, 0);

        _painter->setPen(m_bottomValue > m_minValue ? QColor(Qt::darkRed) : profiler_gui::TEXT_COLOR);
        _painter->drawText(bottomRect, Qt::AlignRight | Qt::AlignVCenter | Qt::TextDontClip, m_bottomDurationStr, &textBounds);
        bottomRect.adjust(0, 0, -textBounds.width() - 3, 0);
    }

    _painter->setPen(Qt::darkGray);
    _painter->drawLine(QLineF(0, bottom, bottom_width, bottom));
    _painter->drawLine(QLineF(0, m_boundingRect.top(), top_width, m_boundingRect.top()));

    paintMouseIndicator(_painter, m_boundingRect.top(), bottom, width, m_boundingRect.height(), top_width, m_mousePos.y(), dtime, font_h);

    // MEDIAN & EXPECTED FRAME TIME
    if (!isEmpty())
    {
        if (m_bottomValue < EASY_GLOBALS.frame_time && EASY_GLOBALS.frame_time < m_topValue)
        {
            // Draw marker displaying required frame_time step
            const auto h = bottom - (EASY_GLOBALS.frame_time - m_bottomValue) * coeff;
            _painter->setPen(Qt::DashLine);

            auto w = width;
            const auto boundary = widget->margin() - font_h;
            if (h < (m_boundingRect.top() - boundary))
                w = top_width;
            else if (h > (bottom + boundary))
                w = bottom_width;

            _painter->drawLine(QLineF(0, h, w, h));
        }
    }

    const auto colorIndicatorSize = font_h * 2 / 3;

    _painter->setBrush(QColor::fromRgba(easyDescriptor(m_blockId).color()));
    _painter->setPen(profiler_gui::BLOCK_BORDER_COLOR);
    _painter->drawRect(QRectF(bottomRect.left(), bottomRect.top() + 1 + font_h / 6., colorIndicatorSize, colorIndicatorSize));
    bottomRect.adjust(colorIndicatorSize + 3, 0, 0, 0);

    _painter->setPen(profiler_gui::TEXT_COLOR);
    _painter->drawText(topRect, Qt::AlignCenter, m_threadName);

    _painter->drawText(bottomRect, Qt::AlignLeft, m_blockName, &textBounds);
    bottomRect.adjust(textBounds.width() + 3, 0, 0, 0);

    _painter->drawText(bottomRect, Qt::AlignLeft, QStringLiteral("| %1").arg(m_blockType), &textBounds);
    bottomRect.adjust(textBounds.width() + 3, 0, 0, 0);

    if (!items.empty())
    {
        QString durationsStr;
        if (!m_medianDurationStr.isEmpty() || !m_avgDurationStr.isEmpty())
        {
            durationsStr = QString("avg: %1 | mdn: %2 | ").arg(m_avgDurationStr).arg(m_medianDurationStr);
        }

        if (m_threadProfiledTime != 0)
        {
            _painter->drawText(bottomRect, Qt::AlignCenter,
                QStringLiteral("%1 calls | %2%3% of thread profiled time")
                    .arg(items.size())
                    .arg(durationsStr)
                    .arg(QString::number(100. * (double) m_blockTotalDuraion / (double) m_threadProfiledTime, 'f', 2))
            );
        }
        else
        {
            _painter->drawText(bottomRect, Qt::AlignCenter,
                QStringLiteral("%1 calls | %2100% of thread profiled time")
                    .arg(items.size())
                    .arg(durationsStr)
            );
        }
    }
    else
    {
        _painter->drawText(bottomRect, Qt::AlignCenter, QStringLiteral("0 calls"));
    }

    _painter->restore();
}

profiler::thread_id_t GraphicsHistogramItem::threadId() const
{
    return m_threadId;
}

void GraphicsHistogramItem::rebuildSource(HistRegime _regime)
{
    if (m_regime == _regime)
        rebuildSource();
}

void GraphicsHistogramItem::rebuildSource()
{
    if (m_regime == Hist_Id)
    {
        m_regime = Hist_Pointer;
        setSource(m_threadId, m_blockId);
    }
    else
    {
        m_regime = Hist_Id;
        setSource(m_threadId, m_pSource);
    }
}

void GraphicsHistogramItem::setSource(profiler::thread_id_t _thread_id, const profiler_gui::EasyItems* _items)
{
    if (m_regime == Hist_Pointer && m_threadId == _thread_id && m_pSource == _items)
        return;

    cancelAnyJob();

    m_boundaryTimer.stop();

    m_blockName.clear();
    m_blockType = QStringLiteral("Thread");
    m_blockTotalDuraion = 0;
    m_medianDuration = 0;
    m_avgDuration = 0;
    m_medianDurationFull = 0;
    m_avgDurationFull = 0;

    m_imageOriginUpdate = m_imageOrigin = 0;
    m_imageScaleUpdate = m_imageScale = 1;

    m_selectedBlocks.clear();
    setEmpty(true);
    { profiler::BlocksTree::children_t().swap(m_selectedBlocks); }

    m_topDurationStr.clear();
    m_bottomDurationStr.clear();
    m_medianDurationStr.clear();
    m_avgDurationStr.clear();

    setImageUpdatePermitted(false);
    m_regime = Hist_Pointer;
    m_pSource = _items;
    m_threadId = _thread_id;
    profiler_gui::set_max(m_blockId);

    if (m_pSource != nullptr)
    {
        if (m_pSource->empty())
        {
            m_pSource = nullptr;
        }
        else
        {
            const auto& root = EASY_GLOBALS.profiler_blocks[_thread_id];
            m_threadName = profiler_gui::decoratedThreadName(EASY_GLOBALS.use_decorated_thread_name, root, EASY_GLOBALS.hex_thread_id);

            if (root.children.empty())
                m_threadDuration = 0;
            else
                m_threadDuration = easyBlock(root.children.back()).tree.node->end() - easyBlock(root.children.front()).tree.node->begin();

            m_threadProfiledTime = root.profiled_time;
            m_threadWaitTime = root.wait_time;
            m_pProfilerThread = &root;
            m_timeUnits = EASY_GLOBALS.time_units;

            setReady(false);

            auto source = m_pSource;
            m_worker.enqueue([this, source]
            {
                m_maxValue = 0;
                m_minValue = 1e30;

                size_t totalCount = 0;
                profiler_gui::DurationsCountMap durations;
                bool empty = true;
                for (const auto& item : *source)
                {
                    if (isReady())
                        return;

                    auto& block = easyBlock(item.block).tree;
                    auto& desc = easyDescriptor(block.node->id());
                    if (desc.type() != profiler::BlockType::Block)
                        continue;

                    const auto duration = block.node->duration();

                    ++totalCount;
                    ++durations[duration].count;
                    m_avgDuration += duration;

                    const auto w = item.width();

                    if (w > m_maxValue)
                        m_maxValue = w;

                    if (w < m_minValue)
                        m_minValue = w;

                    empty = false;
                }

                if (!empty)
                {
                    m_avgDuration /= totalCount;
                    m_medianDuration = profiler_gui::calculateMedian(durations);
                }

                if ((m_maxValue - m_minValue) < 1e-3)
                {
                    if (m_minValue > 0.1)
                    {
                        m_minValue -= 0.1;
                    }
                    else
                    {
                        m_maxValue = 0.1;
                        m_minValue = 0;
                    }
                }

                m_topValue = m_maxValue;
                m_bottomValue = m_minValue;

                if (!empty)
                {
                    m_topDurationStr = profiler_gui::timeStringReal(m_timeUnits, m_topValue, 3);
                    m_bottomDurationStr = profiler_gui::timeStringReal(m_timeUnits, m_bottomValue, 3);
                }

                if (m_avgDuration != 0 || m_medianDuration != 0)
                {
                    m_avgDurationStr = profiler_gui::timeStringRealNs(m_timeUnits, m_avgDuration, 3);
                    m_medianDurationStr = profiler_gui::timeStringRealNs(m_timeUnits, m_medianDuration, 3);
                    m_medianDurationFull = m_medianDuration;
                    m_avgDurationFull = m_avgDuration;
                }

                setEmpty(empty);
                setReady(true);

            }, m_bReady);

            startTimer();
            show();
        }
    }

    if (m_pSource == nullptr)
    {
        m_pProfilerThread = nullptr;
        m_threadName.clear();
        hide();
    }
}

void GraphicsHistogramItem::setSource(profiler::thread_id_t _thread_id, profiler::block_id_t _block_id)
{
    if (m_regime == Hist_Id && m_threadId == _thread_id && m_blockId == _block_id)
        return;

    cancelAnyJob();

    setImageUpdatePermitted(false); // Set to false because m_workerThread have to parse input data first. This will be set to true when m_workerThread finish - see onTimeout()
    m_regime = Hist_Id;

    m_boundaryTimer.stop();

    m_pSource = nullptr;
    m_topDurationStr.clear();
    m_bottomDurationStr.clear();
    m_medianDurationStr.clear();
    m_avgDurationStr.clear();
    m_blockName.clear();
    m_blockType.clear();
    m_blockTotalDuraion = 0;
    m_medianDuration = 0;
    m_avgDuration = 0;
    m_medianDurationFull = 0;
    m_avgDurationFull = 0;

    m_imageOriginUpdate = m_imageOrigin = 0;
    m_imageScaleUpdate = m_imageScale = 1;

    m_selectedBlocks.clear();
    setEmpty(true);
    { profiler::BlocksTree::children_t().swap(m_selectedBlocks); }

    m_threadId = _thread_id;
    m_blockId = _block_id;

    if (m_threadId != 0 && !profiler_gui::is_max(m_blockId))
    {
        auto& desc = easyDescriptor(m_blockId);
        m_blockName = profiler_gui::toUnicode(desc.name());

        switch (desc.type())
        {
            case profiler::BlockType::Block:
            {
                m_blockType = QStringLiteral("Block");
                break;
            }

            case profiler::BlockType::Event:
            {
                m_blockType = QStringLiteral("Event");
                break;
            }

            case profiler::BlockType::Value:
            {
                m_blockType = QStringLiteral("Value");
                break;
            }

            default: break;
        }

        const auto& root = EASY_GLOBALS.profiler_blocks[_thread_id];
        m_threadName = profiler_gui::decoratedThreadName(EASY_GLOBALS.use_decorated_thread_name, root, EASY_GLOBALS.hex_thread_id);
        m_pProfilerThread = &root;
        m_timeUnits = EASY_GLOBALS.time_units;

        if (root.children.empty())
        {
            m_threadDuration = 0;
            m_threadProfiledTime = 0;
            m_threadWaitTime = 0;

            m_topValue = m_maxValue = 0;
            m_bottomValue = m_minValue = 1e30;

            setImageUpdatePermitted(true);
            setReady(true);
        }
        else
        {
            m_threadDuration = easyBlock(root.children.back()).tree.node->end() - easyBlock(root.children.front()).tree.node->begin();
            m_threadProfiledTime = root.profiled_time;
            m_threadWaitTime = root.wait_time;

            setReady(false);

            const auto selected_thread = std::ref(root);
            const auto selected_block = EASY_GLOBALS.selected_block;
            const bool showOnlyTopLevelBlocks = EASY_GLOBALS.display_only_frames_on_histogram;
            m_worker.enqueue([this, selected_thread, selected_block, showOnlyTopLevelBlocks]
            {
                using Stack = std::vector<std::pair<profiler::block_index_t, profiler::block_index_t> >;

                const auto& profiler_thread = selected_thread.get();

                m_maxValue = 0;
                m_minValue = 1e30;
                //const auto& profiler_thread = EASY_GLOBALS.profiler_blocks[m_threadId];
                Stack stack;
                stack.reserve(profiler_thread.depth);

                const bool has_selected_block = !profiler_gui::is_max(selected_block);

                size_t totalCount = 0;
                profiler_gui::DurationsCountMap durations;
                for (auto frame : profiler_thread.children)
                {
                    const auto& frame_block = easyBlock(frame).tree;
                    if (frame_block.node->id() == m_blockId || (!has_selected_block && m_blockId == easyDescriptor(frame_block.node->id()).id()))
                    {
                        m_selectedBlocks.push_back(frame);

                        const auto w = frame_block.node->duration();
                        if (w > m_maxValue)
                            m_maxValue = w;

                        if (w < m_minValue)
                            m_minValue = w;

                        ++totalCount;
                        ++durations[w].count;
                        m_blockTotalDuraion += w;
                    }

                    if (showOnlyTopLevelBlocks)
                        continue;

                    stack.emplace_back(frame, 0U);
                    while (!stack.empty())
                    {
                        if (isReady())
                            return;

                        auto& top = stack.back();
                        const auto& top_children = easyBlock(top.first).tree.children;
                        const auto stack_size = stack.size();
                        for (auto end = top_children.size(); top.second < end; ++top.second)
                        {
                            if (isReady())
                                return;

                            const auto child_index = top_children[top.second];
                            const auto& child = easyBlock(child_index).tree;
                            if (child.node->id() == m_blockId || (!has_selected_block && m_blockId == easyDescriptor(child.node->id()).id()))
                            {
                                m_selectedBlocks.push_back(child_index);

                                const auto w = child.node->duration();
                                if (w > m_maxValue)
                                    m_maxValue = w;

                                if (w < m_minValue)
                                    m_minValue = w;

                                ++totalCount;
                                ++durations[w].count;
                                m_blockTotalDuraion += w;
                            }

                            if (!child.children.empty())
                            {
                                ++top.second;
                                stack.emplace_back(child_index, 0U);
                                break;
                            }
                        }

                        if (stack_size == stack.size())
                        {
                            stack.pop_back();
                        }
                    }
                }

                if (m_selectedBlocks.empty())
                {
                    m_topDurationStr.clear();
                    m_bottomDurationStr.clear();
                    m_medianDurationStr.clear();
                    m_avgDurationStr.clear();
                }
                else
                {
                    if (has_selected_block)
                    {
                        const auto& item = easyBlock(selected_block).tree;
                        if (*item.node->name() != 0)
                            m_blockName = profiler_gui::toUnicode(item.node->name());
                    }

                    m_maxValue *= 1e-3;
                    m_minValue *= 1e-3;

                    if ((m_maxValue - m_minValue) < 1e-3)
                    {
                        if (m_minValue > 0.1)
                        {
                            m_minValue -= 0.1;
                        }
                        else
                        {
                            m_maxValue = 0.1;
                            m_minValue = 0;
                        }
                    }

                    if (totalCount != 0)
                    {
                        m_avgDuration = m_blockTotalDuraion / totalCount;
                        m_medianDuration = profiler_gui::calculateMedian(durations);
                    }

                    m_medianDurationFull = m_medianDuration;
                    m_avgDurationFull = m_avgDuration;

                    m_topDurationStr = profiler_gui::timeStringReal(m_timeUnits, m_maxValue, 3);
                    m_bottomDurationStr = profiler_gui::timeStringReal(m_timeUnits, m_minValue, 3);

                    if (m_avgDuration != 0 || m_medianDuration != 0)
                    {
                        m_avgDurationStr = profiler_gui::timeStringRealNs(m_timeUnits, m_avgDuration, 3);
                        m_medianDurationStr = profiler_gui::timeStringRealNs(m_timeUnits, m_medianDuration, 3);
                    }
                }

                m_topValue = m_maxValue;
                m_bottomValue = m_minValue;

                setEmpty(m_selectedBlocks.empty());
                setReady(true);

            }, m_bReady);

            startTimer();
        }

        show();
    }
    else
    {
        m_pProfilerThread = nullptr;
        m_threadName.clear();
        hide();
    }
}

//////////////////////////////////////////////////////////////////////////

void GraphicsHistogramItem::validateName()
{
    if (m_threadName.isEmpty())
        return;
    m_threadName = profiler_gui::decoratedThreadName(EASY_GLOBALS.use_decorated_thread_name, EASY_GLOBALS.profiler_blocks[m_threadId], EASY_GLOBALS.hex_thread_id);
}

//////////////////////////////////////////////////////////////////////////

bool GraphicsHistogramItem::pickTopValue()
{
    const bool result = Parent::pickTopValue();

    if (result && !m_topDurationStr.isEmpty())
    {
        m_topDurationStr = profiler_gui::timeStringReal(m_timeUnits, m_topValue, 3);
        scene()->update(); // to update top-boundary text right now
    }

    return result;
}

bool GraphicsHistogramItem::increaseTopValue()
{
    const bool result = Parent::increaseTopValue();

    if (result && !m_topDurationStr.isEmpty())
    {
        m_topDurationStr = profiler_gui::timeStringReal(m_timeUnits, m_topValue, 3);
        scene()->update(); // to update top-boundary text right now
    }

    return result;
}

bool GraphicsHistogramItem::decreaseTopValue()
{
    const bool result = Parent::decreaseTopValue();

    if (result && !m_topDurationStr.isEmpty())
    {
        m_topDurationStr = profiler_gui::timeStringReal(m_timeUnits, m_topValue, 3);
        scene()->update(); // to update top-boundary text right now
    }

    return result;
}

//////////////////////////////////////////////////////////////////////////

bool GraphicsHistogramItem::pickBottomValue()
{
    const bool result = Parent::pickBottomValue();

    if (result && !m_bottomDurationStr.isEmpty())
    {
        m_bottomDurationStr = profiler_gui::timeStringReal(m_timeUnits, m_bottomValue, 3);
        scene()->update(); // to update top-boundary text right now
    }

    return result;
}

bool GraphicsHistogramItem::increaseBottomValue()
{
    const bool result = Parent::increaseBottomValue();

    if (result && !m_bottomDurationStr.isEmpty())
    {
        m_bottomDurationStr = profiler_gui::timeStringReal(m_timeUnits, m_bottomValue, 3);
        scene()->update(); // to update top-boundary text right now
    }

    return result;
}

bool GraphicsHistogramItem::decreaseBottomValue()
{
    const bool result = Parent::decreaseBottomValue();

    if (result && !m_bottomDurationStr.isEmpty())
    {
        m_bottomDurationStr = profiler_gui::timeStringReal(m_timeUnits, m_bottomValue, 3);
        scene()->update(); // to update top-boundary text right now
    }

    return result;
}

//////////////////////////////////////////////////////////////////////////

void GraphicsHistogramItem::pickFrameTime(qreal _y) const
{
    if (!isEmpty() && isImageUpdatePermitted() && m_boundingRect.top() < _y && _y < m_boundingRect.bottom() && !m_topDurationStr.isEmpty())
    {
        const auto frame_time = m_bottomValue + (m_topValue - m_bottomValue) * (m_boundingRect.bottom() - _y) / m_boundingRect.height();
        EASY_GLOBALS.frame_time = static_cast<decltype(EASY_GLOBALS.frame_time)>(frame_time);
        emit EASY_GLOBALS.events.expectedFrameTimeChanged();
    }
}

//////////////////////////////////////////////////////////////////////////

void GraphicsHistogramItem::onModeChanged()
{
    if (!isImageUpdatePermitted())
        return;

    const auto widget = static_cast<const GraphicsScrollbar*>(scene()->parent());
    if (!widget->bindMode() && EASY_GLOBALS.auto_adjust_histogram_height)
    {
        m_topValue = m_maxValue;
        m_bottomValue = m_minValue;
    }

    m_boundaryTimer.stop();
    updateImage();
}

//////////////////////////////////////////////////////////////////////////

bool GraphicsHistogramItem::updateImage()
{
    if (!Parent::updateImage())
        return false;

    const auto widget = static_cast<const GraphicsSliderArea*>(scene()->parent());

    m_imageScaleUpdate = widget->range() / widget->sliderWidth();
    m_imageOriginUpdate = widget->bindMode() ? (widget->value() - widget->sliderWidth() * 3) : widget->minimum();

    // Ugly, but doesn't use exceeded count of threads
    const auto rect = m_boundingRect;
    const auto regime = m_regime;
    const auto scale = widget->getWindowScale();
    const auto left = widget->minimum();
    const auto right = widget->maximum();
    const auto value = widget->value();
    const auto window = widget->sliderWidth();
    const auto top = m_topValue;
    const auto bottom = m_bottomValue;
    const auto bindMode = widget->bindMode();
    const auto frameTime = EASY_GLOBALS.frame_time;
    const auto beginTime = EASY_GLOBALS.begin_time;
    const auto autoHeight = EASY_GLOBALS.auto_adjust_histogram_height;
    const auto drawBorders = EASY_GLOBALS.draw_histogram_borders;
    const auto minColumnWidth = EASY_GLOBALS.histogram_column_width_min;
    m_worker.enqueue([=] {
        updateImageAsync(rect, regime, scale, left, right, right - left, value, window, top, bottom,
                         minColumnWidth, bindMode, frameTime, beginTime, autoHeight, drawBorders);
    }, m_bReady);

    return true;
}

void GraphicsHistogramItem::onImageUpdated()
{
    if (EASY_GLOBALS.auto_adjust_histogram_height && !m_topDurationStr.isEmpty())
    {
        m_topValue = m_workerTopDuration;
        m_bottomValue = m_workerBottomDuration;

        m_topDurationStr = profiler_gui::timeStringReal(m_timeUnits, m_topValue, 3);
        m_bottomDurationStr = profiler_gui::timeStringReal(m_timeUnits, m_bottomValue, 3);
    }

    if (static_cast<const GraphicsSliderArea*>(scene()->parent())->bindMode())
    {
        m_medianDuration = m_workerMedianDuration;
        m_avgDuration = m_workerAvgDuration;
    }
    else
    {
        m_medianDuration = m_medianDurationFull;
        m_avgDuration = m_avgDurationFull;
    }

    if (m_avgDuration != 0 || m_medianDuration != 0)
    {
        m_medianDurationStr = profiler_gui::timeStringRealNs(m_timeUnits, m_medianDuration, 3);
        m_avgDurationStr = profiler_gui::timeStringRealNs(m_timeUnits, m_avgDuration, 3);
    }
}

void GraphicsHistogramItem::updateImageAsync(QRectF _boundingRect, HistRegime _regime, qreal _current_scale,
    qreal _minimum, qreal _maximum, qreal _range, qreal _value, qreal _width, qreal _top_duration, qreal _bottom_duration,
    int _min_column_width, bool _bindMode, float _frame_time, profiler::timestamp_t _begin_time, bool _autoAdjustHist, bool _drawBorders)
{
    const auto bottom = _boundingRect.height();//_boundingRect.bottom();
    const auto screenWidth = _boundingRect.width() * _current_scale;
    const auto maxColumnHeight = _boundingRect.height();
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
    p.setPen(Qt::NoPen);

    QRectF rect;
    QBrush brush(Qt::SolidPattern);
    QRgb previousColor = 0;

    qreal previous_x = -1e30, previous_h = -1e30, offset = 0.;
    auto realScale = _current_scale;

    const bool gotFrame = _frame_time > 1e-6f;
    qreal frameCoeff = 1;
    if (gotFrame)
    {
        if (_frame_time <= _bottom_duration)
            frameCoeff = _boundingRect.height();
        else
            frameCoeff = 0.9 / _frame_time;
    }

    using estd::sqr;
    auto const calculate_color = gotFrame ? calculate_color2 : calculate_color1;
    auto const k = gotFrame ? sqr(sqr(frameCoeff)) : 1.0 / _boundingRect.height();

    size_t totalCount = 0;
    m_workerMedianDuration = 0;
    m_workerAvgDuration = 0;
    profiler_gui::DurationsCountMap durations;

    if (_regime == Hist_Pointer)
    {
        const auto& items = *m_pSource;
        if (items.empty())
            return;

        auto first = items.begin();

        if (_bindMode)
        {
            _minimum = m_workerImageOrigin;
            _maximum = m_workerImageOrigin + _width * 7;
            realScale *= viewScale;
            offset = _minimum * realScale;

            first = std::lower_bound(items.begin(), items.end(), _minimum, [] (const profiler_gui::EasyBlockItem& _item, qreal _value)
            {
                return _item.left() < _value;
            });

            if (first != items.end())
            {
                if (first != items.begin())
                    --first;
            }
            else
            {
                first = items.begin() + items.size() - 1;
            }

            if (_autoAdjustHist)
            {
                const auto maxVal = _value + _width;
                decltype(_top_duration) maxDuration = 0;
                decltype(_bottom_duration) minDuration = 1e30;
                size_t iterations = 0;
                for (auto it = first, end = items.end(); it != end; ++it)
                {
                    // Draw rectangle
                    if (it->left() > maxVal)
                        break;

                    if (it->right() < _value)
                        continue;

                    if (maxDuration < it->width())
                        maxDuration = it->width();

                    if (minDuration > it->width())
                        minDuration = it->width();

                    ++iterations;
                }

                if (iterations != 0)
                {
                    _top_duration = maxDuration;
                    _bottom_duration = minDuration;

                    if ((_top_duration - _bottom_duration) < 1e-3)
                    {
                        if (_bottom_duration > 0.1)
                        {
                            _bottom_duration -= 0.1;
                        }
                        else
                        {
                            _top_duration = 0.1;
                            _bottom_duration = 0;
                        }
                    }
                }
            }
        }

        const auto dtime = _top_duration - _bottom_duration;
        const auto coeff = _boundingRect.height() / (dtime > 1e-3 ? dtime : 1.);

        const qreal minWidth = _drawBorders ? _min_column_width : 1;
        if (_drawBorders)
            p.setPen(profiler_gui::BLOCK_BORDER_COLOR);

        for (auto it = first, end = items.end(); it != end; ++it)
        {
            // Draw rectangle
            if (it->left() > _maximum)
                break;

            if (it->right() < _minimum)
                continue;

            if (_bindMode)
            {
                // calculate avg and median
                const auto duration = easyBlock(it->block).tree.node->duration();
                m_workerAvgDuration += duration;
                ++totalCount;
                ++durations[duration].count;
            }

            auto maxItemWidth = it->width();

            // calculate column width and height
            qreal item_x = it->left() * realScale - offset;
            qreal item_w = it->width() * realScale;
            qreal item_r = item_x + item_w;
            const auto width = it->width();
            qreal h = width <= _bottom_duration ? HIST_COLUMN_MIN_HEIGHT :
                (width > _top_duration ? maxColumnHeight : (width - _bottom_duration) * coeff);

            if (_drawBorders)
            {
                if (item_r < previous_x)
                {
                    item_w -= previous_x - item_r;
                    item_x = previous_x;
                    item_r = item_x + item_w;
                }

                // if column width < minWidth then try to merge several columns together
                auto jt = it;
                while (item_w < minWidth && jt != end)
                {
                    if (jt->left() > _maximum)
                        break;

                    const qreal jx = jt->left() * realScale - offset;
                    auto dx = jx - item_r;
                    if (dx > std::max(item_w, minWidth))
                    {
                        item_w = minWidth;
                        break;
                    }

                    const qreal jw = jt->width() * realScale;
                    if (jw > (minWidth + item_w))
                    {
                        item_w = minWidth;
                        break;
                    }

                    const auto jwidth = jt->width();
                    const qreal jh = jwidth <= _bottom_duration ? HIST_COLUMN_MIN_HEIGHT :
                        (jwidth > _top_duration ? maxColumnHeight : (jwidth - _bottom_duration) * coeff);

                    item_w += jw;
                    h = std::max(h, jh);
                    maxItemWidth = std::max(maxItemWidth, jwidth);
                    ++jt;
                }

                item_r = item_x + item_w;

                if (_bindMode)
                {
                    // if merged several columns then avg and median should be calculated for these columns too
                    for (auto it2 = it; it2 != jt; ++it2)
                    {
                        const auto duration = easyBlock(it2->block).tree.node->duration();
                        m_workerAvgDuration += duration;
                        ++totalCount;
                        ++durations[duration].count;
                    }
                }

                if (jt != it)
                {
                    // bypass merged columns
                    it = jt;
                    --it;
                }
            }
            else if (item_w < 1)
            {
                item_w = 1;
                item_r = item_x + 1;
            }

            if (h < previous_h && item_r < previous_x)
                continue;

            const auto col = calculate_color(h, maxItemWidth, k);
            const auto color = 0x00ffffff & QColor::fromHsvF((1.0 - col) * 0.375, 0.85, 0.85).rgb();

            if (previousColor != color)
            {
                // Set background color brush for rectangle
                previousColor = color;
                brush.setColor(QColor::fromRgba(0xc0000000 | color));
                p.setBrush(brush);
            }

            rect.setRect(item_x, bottom - h, item_w, h);
            p.drawRect(rect);

            previous_x = item_r;
            previous_h = h;
        }
    }
    else
    {
        auto first = m_selectedBlocks.begin();

        if (_bindMode)
        {
            _minimum = m_workerImageOrigin;
            _maximum = m_workerImageOrigin + _width * 7;
            realScale *= viewScale;
            offset = _minimum * 1e3 * realScale;

            first = std::lower_bound(m_selectedBlocks.begin(), m_selectedBlocks.end(), _minimum * 1e3 + _begin_time, [](profiler::block_index_t _item, qreal _value)
            {
                return easyBlock(_item).tree.node->begin() < _value;
            });

            if (first != m_selectedBlocks.end())
            {
                if (first != m_selectedBlocks.begin())
                    --first;
            }
            else
            {
                first = m_selectedBlocks.begin() + m_selectedBlocks.size() - 1;
            }

            _minimum *= 1e3;
            _maximum *= 1e3;

            if (_autoAdjustHist)
            {
                const auto minVal = _value * 1e3, maxVal = (_value + _width) * 1e3;
                decltype(_top_duration) maxDuration = 0;
                decltype(_bottom_duration) minDuration = 1e30;
                size_t iterations = 0;
                for (auto it = first, end = m_selectedBlocks.end(); it != end; ++it)
                {
                    const auto item = easyBlock(*it).tree.node;

                    const auto beginTime = item->begin() - _begin_time;
                    if (beginTime > maxVal)
                        break;

                    const auto endTime = item->end() - _begin_time;
                    if (endTime < minVal)
                        continue;

                    const qreal duration = item->duration() * 1e-3;

                    if (maxDuration < duration)
                        maxDuration = duration;

                    if (minDuration > duration)
                        minDuration = duration;

                    ++iterations;
                }

                if (iterations != 0)
                {
                    _top_duration = maxDuration;
                    _bottom_duration = minDuration;

                    if ((_top_duration - _bottom_duration) < 1e-3)
                    {
                        if (_bottom_duration > 0.1)
                        {
                            _bottom_duration -= 0.1;
                        }
                        else
                        {
                            _top_duration = 0.1;
                            _bottom_duration = 0;
                        }
                    }
                }
            }
        }
        else
        {
            _minimum *= 1e3;
            _maximum *= 1e3;
        }

        const auto dtime = _top_duration - _bottom_duration;
        const auto coeff = _boundingRect.height() / (dtime > 1e-3 ? dtime : 1.);

        const qreal minWidth = _drawBorders ? _min_column_width : 1;
        if (_drawBorders)
            p.setPen(profiler_gui::BLOCK_BORDER_COLOR);

        for (auto it = first, end = m_selectedBlocks.end(); it != end; ++it)
        {
            // Draw rectangle
            const auto item = easyBlock(*it).tree.node;

            const auto beginTime = item->begin() - _begin_time;
            if (beginTime > _maximum)
                break;

            const auto endTime = item->end() - _begin_time;
            if (endTime < _minimum)
                continue;

            if (_bindMode)
            {
                // calculate avg and median
                const auto duration = item->duration();
                m_workerAvgDuration += duration;
                ++totalCount;
                ++durations[duration].count;
            }

            const qreal duration = item->duration() * 1e-3;
            auto maxItemDuration = duration;

            // calculate column width and height
            qreal item_x = (beginTime * realScale - offset) * 1e-3;
            qreal item_w = duration * realScale;
            qreal item_r = item_x + item_w;
            auto h = duration <= _bottom_duration ? HIST_COLUMN_MIN_HEIGHT :
                (duration > _top_duration ? maxColumnHeight : (duration - _bottom_duration) * coeff);

            if (_drawBorders)
            {
                if (item_r < previous_x)
                {
                    item_w -= previous_x - item_r;
                    item_x = previous_x;
                    item_r = item_x + item_w;
                }

                // if column width < minWidth then try to merge several columns together
                auto jt = it;
                while (item_w < minWidth && jt != end)
                {
                    const auto jtem = easyBlock(*jt).tree.node;
                    const auto jbeginTime = jtem->begin() - _begin_time;
                    if (jbeginTime > _maximum)
                        break;

                    const qreal jduration = jtem->duration() * 1e-3;

                    const qreal jx = ((jtem->begin() - _begin_time) * realScale - offset) * 1e-3;
                    auto dx = jx - item_r;
                    if (dx > std::max(item_w, minWidth))
                    {
                        item_w = minWidth;
                        break;
                    }

                    const qreal jw = jduration * realScale;
                    if (jw > (minWidth + item_w))
                    {
                        item_w = minWidth;
                        break;
                    }

                    const qreal jh = jduration <= _bottom_duration ? HIST_COLUMN_MIN_HEIGHT :
                        (jduration > _top_duration ? maxColumnHeight : (jduration - _bottom_duration) * coeff);

                    item_w += jw;
                    h = std::max(h, jh);
                    maxItemDuration = std::max(maxItemDuration, jduration);
                    ++jt;
                }

                item_r = item_x + item_w;

                if (_bindMode)
                {
                    // if merged several columns then avg and median should be calculated for these columns too
                    for (auto it2 = it; it2 != jt; ++it2)
                    {
                        const auto duration = easyBlock(*it2).tree.node->duration();
                        m_workerAvgDuration += duration;
                        ++totalCount;
                        ++durations[duration].count;
                    }
                }

                if (jt != it)
                {
                    // bypass merged columns
                    it = jt;
                    --it;
                }
            }
            else if (item_w < 1)
            {
                item_w = 1;
                item_r = item_x + 1;
            }

            if (h < previous_h && item_r < previous_x)
                continue;

            const auto col = calculate_color(h, maxItemDuration, k);
            const auto color = 0x00ffffff & QColor::fromHsvF((1.0 - col) * 0.375, 0.85, 0.85).rgb();

            if (previousColor != color)
            {
                // Set background color brush for rectangle
                previousColor = color;
                brush.setColor(QColor::fromRgba(0xc0000000 | color));
                p.setBrush(brush);
            }

            rect.setRect(item_x, bottom - h, item_w, h);
            p.drawRect(rect);

            previous_x = item_r;
            previous_h = h;
        }
    }

    m_workerTopDuration = _top_duration;
    m_workerBottomDuration = _bottom_duration;

    if (_bindMode && totalCount != 0)
    {
        m_workerAvgDuration /= totalCount;
        m_workerMedianDuration = profiler_gui::calculateMedian(durations);
    }

    setReady(true);
}

//////////////////////////////////////////////////////////////////////////

GraphicsScrollbar::GraphicsScrollbar(int _initialHeight, QWidget* _parent)
    : Parent(_parent)
    , m_histogramItem(nullptr)
{
    const int sceneHeight = _initialHeight - 2;
    scene()->setSceneRect(0, -(sceneHeight >> 1), 500, sceneHeight);

    m_histogramItem = new GraphicsHistogramItem();
    m_imageItem = m_histogramItem;
    scene()->addItem(m_histogramItem);

    m_histogramItem->setPos(0, 0);
    m_histogramItem->setBoundingRect(0, scene()->sceneRect().top() + margin(), scene()->width(), sceneHeight - margins() - 1);
    m_histogramItem->hide();

    connect(&EASY_GLOBALS.events, &profiler_gui::GlobalSignals::expectedFrameTimeChanged,
            this, &This::repaintHistogramImage);

    connect(&EASY_GLOBALS.events, &profiler_gui::GlobalSignals::autoAdjustHistogramChanged,
            this, &This::onAutoAdjustHistogramChanged);

    connect(&EASY_GLOBALS.events, &profiler_gui::GlobalSignals::displayOnlyFramesOnHistogramChanged,
            this, &This::onDisplayOnlyFramesOnHistogramChanged);

    connect(&EASY_GLOBALS.events, &profiler_gui::GlobalSignals::threadNameDecorationChanged, this, &This::onThreadViewChanged);
    connect(&EASY_GLOBALS.events, &profiler_gui::GlobalSignals::hexThreadIdChanged, this, &This::onThreadViewChanged);

    connect(&EASY_GLOBALS.events, &profiler_gui::GlobalSignals::allDataGoingToBeDeleted, this, &This::clear);
}

GraphicsScrollbar::~GraphicsScrollbar()
{

}

//////////////////////////////////////////////////////////////////////////

void GraphicsScrollbar::repaintHistogramImage()
{
    if (m_histogramItem->isVisible())
    {
        m_histogramItem->updateImage();
        scene()->update();
    }
}

void GraphicsScrollbar::onThreadViewChanged()
{
    if (m_histogramItem->isVisible())
    {
        m_histogramItem->validateName();
        scene()->update();
    }
}

void GraphicsScrollbar::onAutoAdjustHistogramChanged()
{
    if (m_histogramItem->isVisible())
        m_histogramItem->onModeChanged();
}

void GraphicsScrollbar::onDisplayOnlyFramesOnHistogramChanged()
{
    if (m_histogramItem->isVisible())
        m_histogramItem->rebuildSource(GraphicsHistogramItem::Hist_Id);
}

//////////////////////////////////////////////////////////////////////////

void GraphicsScrollbar::clear()
{
    setHistogramSource(0, nullptr);
    Parent::clear();
}

profiler::thread_id_t GraphicsScrollbar::histThread() const
{
    return m_histogramItem->threadId();
}

void GraphicsScrollbar::setHistogramSource(profiler::thread_id_t _thread_id, const profiler_gui::EasyItems* _items)
{
    if (m_bLocked)
        return;

    m_histogramItem->setSource(_thread_id, _items);
    scene()->update();
}

void GraphicsScrollbar::setHistogramSource(profiler::thread_id_t _thread_id, profiler::block_id_t _block_id)
{
    if (m_bLocked)
        return;

    m_histogramItem->setSource(_thread_id, _block_id);
    scene()->update();
}

void GraphicsScrollbar::mousePressEvent(QMouseEvent* _event)
{
    Parent::mousePressEvent(_event);
    if ((m_mouseButtons & Qt::RightButton) && _event->modifiers())
        m_histogramItem->pickFrameTime(mapToScene(_event->pos()).y());
}

//////////////////////////////////////////////////////////////////////////
