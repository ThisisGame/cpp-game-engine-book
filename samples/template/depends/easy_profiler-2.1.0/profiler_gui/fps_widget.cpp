/************************************************************************
* file name         : fps_widget.cpp
* ----------------- :
* creation time     : 2017/04/02
* author            : Victor Zarubkin
* email             : v.s.zarubkin@gmail.com
* ----------------- :
* description       : This file contains implementation of FpsWidget widget.
* ----------------- :
* change log        : * 2017/04/02 Victor Zarubkin: Initial commit.
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

#include <QGraphicsScene>
#include <QResizeEvent>
#include <QContextMenuEvent>
#include <QMenu>
#include <QAction>
#include "fps_widget.h"
#include "globals.h"

const int INTERVAL_WIDTH = 20;

//////////////////////////////////////////////////////////////////////////

FpsGraphicsItem::FpsGraphicsItem() : Parent(nullptr)
{

}

FpsGraphicsItem::~FpsGraphicsItem()
{

}

//////////////////////////////////////////////////////////////////////////

QRectF FpsGraphicsItem::boundingRect() const
{
    return m_boundingRect;
}

void FpsGraphicsItem::setBoundingRect(const QRectF& _boundingRect)
{
    m_boundingRect = _boundingRect;
}

void FpsGraphicsItem::setBoundingRect(qreal x, qreal y, qreal w, qreal h)
{
    m_boundingRect.setRect(x, y, w, h);
}

//////////////////////////////////////////////////////////////////////////

void FpsGraphicsItem::paint(QPainter* _painter, const QStyleOptionGraphicsItem*, QWidget*)
{
    if (m_frames.empty())
        return;

    const auto fontMetrics = QFontMetrics(scene()->font());
    const int fontHeight = fontMetrics.height() + 1;
    const qreal h = m_boundingRect.height() - (fontHeight << 1) - 2;
    if (h < 0)
        return;

    const qreal halfWidth = m_boundingRect.width() * 0.5 - INTERVAL_WIDTH;
    const int halfMax = static_cast<int>(0.5 + halfWidth / INTERVAL_WIDTH);
    const int half = static_cast<int>(m_frames.size() / 2);
    const qreal top = fontHeight, bottom = h + 2 + fontHeight;
    qreal y;

    _painter->save();

    _painter->drawLine(QPointF(0, top), QPointF(m_boundingRect.width(), top));
    _painter->drawLine(QPointF(0, bottom), QPointF(m_boundingRect.width(), bottom));

    const auto maxLines = static_cast<int>(h / fontHeight);

    _painter->setPen(Qt::lightGray);

    y = m_boundingRect.height() * 0.5;

    if (maxLines != 2)
        _painter->drawLine(QPointF(0, y), QPointF(m_boundingRect.width(), y));

    y -= h * 0.25;
    if (maxLines >= 2)
        _painter->drawLine(QPointF(0, y), QPointF(m_boundingRect.width(), y));

    y += h * 0.5;
    if (maxLines >= 2)
        _painter->drawLine(QPointF(0, y), QPointF(m_boundingRect.width(), y));

    m_points1.reserve(m_frames.size());
    m_points2.reserve(m_frames.size());
    int n = 0;
    qreal x = m_boundingRect.width() * 0.5 + std::min(halfMax, half) * INTERVAL_WIDTH, localMax = 0, localMin = 1e30;
    const qreal xCurrent = x;
    for (int i = static_cast<int>(m_frames.size()) - 1; i > -1 && x >= 0; --i, x -= INTERVAL_WIDTH, ++n)
    {
        const auto& val = m_frames[i];

        if (val.first > localMax)
            localMax = val.first;
        if (val.first < localMin)
            localMin = val.first;
        m_points1.emplace_back(x, static_cast<qreal>(val.first) + 1e-3);

        if (val.second > localMax)
            localMax = val.second;
        if (val.second < localMin)
            localMin = val.second;
        m_points2.emplace_back(x, static_cast<qreal>(val.second) + 1e-3);

        _painter->drawLine(QPointF(x, top + 1), QPointF(x, bottom - 1));
    }

    const auto delta = std::max(localMax - localMin, 1e-3);
    _painter->setPen(Qt::black);

    qreal frameTime = std::max(localMax, 1.);
    auto fps = static_cast<quint64>(1e6 / frameTime);
    _painter->drawText(5, 0, m_boundingRect.width() - 10, fontHeight, Qt::AlignVCenter | Qt::AlignLeft, QString("Slowest   %1   (%2 FPS)")
                       .arg(::profiler_gui::timeStringReal(EASY_GLOBALS.time_units, localMax, 1)).arg(fps));

    frameTime = std::max(m_frames.back().first, 1U);
    fps = static_cast<quint64>(1e6 / frameTime);
    _painter->drawText(5, 0, xCurrent - 5, fontHeight, Qt::AlignVCenter | Qt::AlignRight, QString("Max current   %1   (%2 FPS)")
                       .arg(::profiler_gui::timeStringReal(EASY_GLOBALS.time_units, m_frames.back().first, 1)).arg(fps));

    frameTime = std::max(m_frames.back().second, 1U);
    fps = static_cast<quint64>(1e6 / frameTime);
    _painter->drawText(5, bottom, xCurrent - 5, fontHeight, Qt::AlignVCenter | Qt::AlignRight, QString("Avg current   %1   (%2 FPS)")
                       .arg(::profiler_gui::timeStringReal(EASY_GLOBALS.time_units, m_frames.back().second, 1)).arg(fps));

    frameTime = std::max(localMin, 1.);
    fps = static_cast<quint64>(1e6 / frameTime);
    _painter->drawText(5, bottom, m_boundingRect.width() - 10, fontHeight, Qt::AlignVCenter | Qt::AlignLeft, QString("Fastest   %1   (%2 FPS)")
                       .arg(::profiler_gui::timeStringReal(EASY_GLOBALS.time_units, localMin, 1)).arg(fps));

    if (localMin < EASY_GLOBALS.frame_time && EASY_GLOBALS.frame_time < localMax)
    {
        y = fontHeight + 2 + h * (1. - (EASY_GLOBALS.frame_time - localMin) / delta);
        _painter->setPen(Qt::DashLine);
        _painter->drawLine(QPointF(0, y), QPointF(m_boundingRect.width(), y));
    }

    for (int i = 0; i < n; ++i)
    {
        auto& point1 = m_points1[i];
        point1.setY(fontHeight + 2 + h * (1. - (point1.y() - localMin) / delta));

        auto& point2 = m_points2[i];
        point2.setY(fontHeight + 2 + h * (1. - (point2.y() - localMin) / delta));
    }

    _painter->setRenderHint(QPainter::Antialiasing, true);

    QPen pen(QColor::fromRgba(0x80ff0000));
    pen.setWidth(EASY_GLOBALS.fps_widget_line_width);
    _painter->setPen(pen);
    if (n > 1)
    {
        _painter->drawPolyline(m_points1.data(), n);

        pen.setColor(QColor::fromRgba(0x800000ff));
        _painter->setPen(pen);
        _painter->drawPolyline(m_points2.data(), n);
    }
    else
    {
        _painter->drawPoint(m_points1.back());

        pen.setColor(QColor::fromRgba(0x800000ff));
        _painter->setPen(pen);
        _painter->drawPoint(m_points2.back());
    }

    const auto txtTop = ::profiler_gui::timeStringReal(EASY_GLOBALS.time_units, localMin + delta * 0.75, 1);
    const auto txtMid = ::profiler_gui::timeStringReal(EASY_GLOBALS.time_units, localMin + delta * 0.5, 1);
    const auto txtBtm = ::profiler_gui::timeStringReal(EASY_GLOBALS.time_units, localMin + delta * 0.25, 1);

    _painter->setPen(Qt::NoPen);
    _painter->setBrush(Qt::white);
    _painter->drawRect(0, top + 1, std::max({fontMetrics.width(txtTop), fontMetrics.width(txtMid), fontMetrics.width(txtBtm)}) + 8, bottom - top - 1);

    _painter->setPen(Qt::black);
    _painter->setBrush(Qt::NoBrush);

    y = m_boundingRect.height() * 0.5;
    if (maxLines != 2)
        _painter->drawText(5, y - (fontHeight >> 1), m_boundingRect.width(), fontHeight, Qt::AlignVCenter | Qt::AlignLeft, txtMid);

    y -= h * 0.25;
    if (maxLines >= 2)
        _painter->drawText(5, y - (fontHeight >> 1), m_boundingRect.width(), fontHeight, Qt::AlignVCenter | Qt::AlignLeft, txtTop);

    y += h * 0.5;
    if (maxLines >= 2)
        _painter->drawText(5, y - (fontHeight >> 1), m_boundingRect.width(), fontHeight, Qt::AlignVCenter | Qt::AlignLeft, txtBtm);

    _painter->restore();

    m_points1.clear();
    m_points2.clear();
}

//////////////////////////////////////////////////////////////////////////

void FpsGraphicsItem::clear()
{
    m_frames.clear();
}

void FpsGraphicsItem::addPoint(uint32_t _maxFrameTime, uint32_t _avgFrameTime)
{
    m_frames.emplace_back(_maxFrameTime, _avgFrameTime);
    if (static_cast<int>(m_frames.size()) > EASY_GLOBALS.max_fps_history)
        m_frames.pop_front();
}

//////////////////////////////////////////////////////////////////////////

FpsWidget::FpsWidget(QWidget* _parent) : Parent(_parent), m_fpsItem(nullptr)
{
    setCacheMode(QGraphicsView::CacheNone);
    //setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
    setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    setOptimizationFlag(QGraphicsView::DontSavePainterState, true);

    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    setContentsMargins(0, 0, 0, 0);
    setScene(new QGraphicsScene(this));
    scene()->setSceneRect(0, 0, 50, 50);

    m_fpsItem = new FpsGraphicsItem();
    m_fpsItem->setPos(0, 0);
    m_fpsItem->setBoundingRect(0, 0, 50, 50);
    scene()->addItem(m_fpsItem);

    centerOn(0, 0);

    // Dirty hack for QDockWidget stupid initial size policy :(
    setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Preferred);
    setFixedHeight(10); // Set very small height to enable appropriate minimum height on the application startup
    QTimer::singleShot(100, [this]
    {
        // Now set appropriate minimum height
        setMinimumHeight((QFontMetrics(scene()->font()).height() + px(3)) * 3);
        setMaximumHeight(minimumHeight() * 20);
    });
}

FpsWidget::~FpsWidget()
{

}

void FpsWidget::clear()
{
    m_fpsItem->clear();
    scene()->update();
}

void FpsWidget::addPoint(uint32_t _maxFrameTime, uint32_t _avgFrameTime)
{
    m_fpsItem->addPoint(_maxFrameTime, _avgFrameTime);
    scene()->update();
}

QSize FpsWidget::sizeHint() const
{
    return QSize(Parent::sizeHint().width(), minimumHeight());
}

QSize FpsWidget::minimumSizeHint() const
{
    return QSize(Parent::minimumSizeHint().width(), minimumHeight());
}

void FpsWidget::resizeEvent(QResizeEvent* _event)
{
    Parent::resizeEvent(_event);

    auto size = _event->size();
    m_fpsItem->setBoundingRect(0, 0, size.width(), size.height());

    scene()->setSceneRect(m_fpsItem->boundingRect());
    scene()->update();
}

void FpsWidget::hideEvent(QHideEvent* _event)
{
    Parent::hideEvent(_event);
    EASY_GLOBALS.fps_enabled = isVisible();
    clear();
}

void FpsWidget::showEvent(QShowEvent* _event)
{
    Parent::showEvent(_event);
    EASY_GLOBALS.fps_enabled = isVisible();
    clear();
}

void FpsWidget::contextMenuEvent(QContextMenuEvent* _event)
{
    QMenu menu;
    QAction* action = nullptr;

    action = menu.addAction(QIcon(imagePath("delete")), "Clear");
    connect(action, &QAction::triggered, [this](bool){ clear(); });

    action = menu.addAction("Close");
    connect(action, &QAction::triggered, [this](bool){ parentWidget()->hide(); });

    menu.exec(QCursor::pos());

    _event->accept();
}

//////////////////////////////////////////////////////////////////////////

