/************************************************************************
* file name         : graphics_ruler_item.cpp
* ----------------- :
* creation time     : 2016/09/15
* author            : Victor Zarubkin
* email             : v.s.zarubkin@gmail.com
* ----------------- :
* description       : The file contains implementation of GraphicsRulerItem.
* ----------------- :
* change log        : * 2016/09/15 Victor Zarubkin: moved sources from blocks_graphics_view.cpp
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

#include <math.h>
#include <QGraphicsScene>
#include <QFontMetricsF>
#include "blocks_graphics_view.h"
#include "graphics_ruler_item.h"
#include "globals.h"

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

#ifdef max
#undef max
#endif

#ifdef min
#undef min
#endif

//////////////////////////////////////////////////////////////////////////

GraphicsRulerItem::GraphicsRulerItem(bool main)
    : Parent()
    , m_color(profiler_gui::RULER_COLOR)
    , m_left(0)
    , m_right(0)
    , m_main(main)
    , m_reverse(false)
    , m_strict(false)
    , m_hover_on_indicator(false)
    , m_hover_on_left_border(false)
    , m_hover_on_right_border(false)
{
    m_indicator.reserve(3);
}

GraphicsRulerItem::~GraphicsRulerItem()
{
}

QRectF GraphicsRulerItem::boundingRect() const
{
    return m_bounding_rect;
}

void GraphicsRulerItem::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*)
{
    auto const sceneView = view();
    const auto currentScale = sceneView->scale();
    const auto offset = sceneView->offset();
    const auto visibleSceneRect = sceneView->visibleSceneRect();
    auto sceneLeft = offset, sceneRight = offset + visibleSceneRect.width() / currentScale;

    if (m_main)
        m_indicator.clear();

    if (m_left > sceneRight || m_right < sceneLeft)
    {
        // This item is out of screen

        if (m_main)
        {
            const int size = m_hover_on_indicator ? 12 : 10;
            auto vcenter = visibleSceneRect.top() + visibleSceneRect.height() * 0.5;
            auto color = QColor::fromRgb(m_color.rgb());
            auto pen = painter->pen();
            pen.setColor(color);

            m_indicator.clear();
            if (m_left > sceneRight)
            {
                sceneRight = (sceneRight - offset) * currentScale;
                m_indicator.push_back(QPointF(sceneRight - size, vcenter - size));
                m_indicator.push_back(QPointF(sceneRight, vcenter));
                m_indicator.push_back(QPointF(sceneRight - size, vcenter + size));
            }
            else
            {
                sceneLeft = (sceneLeft - offset) * currentScale;
                m_indicator.push_back(QPointF(sceneLeft + size, vcenter - size));
                m_indicator.push_back(QPointF(sceneLeft, vcenter));
                m_indicator.push_back(QPointF(sceneLeft + size, vcenter + size));
            }

            painter->save();
            painter->setTransform(QTransform::fromTranslate(-x(), -y()), true);
            painter->setBrush(m_hover_on_indicator ? QColor::fromRgb(0xffff0000) : color);
            painter->setPen(pen);
            painter->drawPolygon(m_indicator);
            painter->restore();
        }

        return;
    }

    auto selectedInterval = width();
    QRectF rect(
        (m_left - offset) * currentScale,
        visibleSceneRect.top(),
        ::std::max(selectedInterval * currentScale, 1.0),
        visibleSceneRect.height()
    );
    selectedInterval = units2microseconds(selectedInterval);

    const QString text = profiler_gui::timeStringReal(EASY_GLOBALS.time_units, selectedInterval); // Displayed text
    const auto textRect = QFontMetricsF(EASY_GLOBALS.font.ruler, sceneView).boundingRect(text); // Calculate displayed text boundingRect
    const auto rgb = m_color.rgb() & 0x00ffffff;



    // Paint!--------------------------
    painter->save();

    // instead of scrollbar we're using manual offset
    painter->setTransform(QTransform::fromTranslate(-x(), -y()), true);

    if (m_left < sceneLeft)
        rect.setLeft(0);

    if (m_right > sceneRight)
        rect.setWidth((sceneRight - offset) * currentScale - rect.left());

    // draw transparent rectangle
    auto vcenter = rect.top() + rect.height() * 0.5;
    QLinearGradient g(rect.left(), vcenter, rect.right(), vcenter);
    g.setColorAt(0, m_color);
    g.setColorAt(0.2, QColor::fromRgba(0x14000000 | rgb));
    g.setColorAt(0.8, QColor::fromRgba(0x14000000 | rgb));
    g.setColorAt(1, m_color);
    painter->setBrush(g);
    painter->setPen(Qt::NoPen);
    painter->drawRect(rect);

    // draw left and right borders
    painter->setBrush(Qt::NoBrush);
    if (m_main && !m_strict)
    {
        QPen p(QColor::fromRgba(0xd0000000 | rgb));
        p.setStyle(Qt::DotLine);
        painter->setPen(p);
    }
    else
    {
        painter->setPen(QColor::fromRgba(0xd0000000 | rgb));
    }

    if (m_left > sceneLeft)
    {
        if (m_hover_on_left_border)
        {
            // Set bold if border is hovered
            QPen p = painter->pen();
            p.setWidth(3);
            painter->setPen(p);
        }

        painter->drawLine(QPointF(rect.left(), rect.top()), QPointF(rect.left(), rect.bottom()));
    }

    if (m_right < sceneRight)
    {
        if (m_hover_on_left_border)
        {
            // Restore width
            QPen p = painter->pen();
            p.setWidth(1);
            painter->setPen(p);
        }
        else if (m_hover_on_right_border)
        {
            // Set bold if border is hovered
            QPen p = painter->pen();
            p.setWidth(3);
            painter->setPen(p);
        }

        painter->drawLine(QPointF(rect.right(), rect.top()), QPointF(rect.right(), rect.bottom()));

        // This is not necessary because another setPen() invoked for draw text
        //if (m_hover_on_right_border)
        //{
        //    // Restore width
        //    QPen p = painter->pen();
        //    p.setWidth(1);
        //    painter->setPen(p);
        //}
    }

    // draw text
    painter->setCompositionMode(QPainter::CompositionMode_Difference); // This lets the text to be visible on every background
    painter->setRenderHint(QPainter::TextAntialiasing);
    painter->setPen(0x00ffffff - rgb);
    painter->setFont(EASY_GLOBALS.font.ruler);

    int textFlags = 0;
    switch (EASY_GLOBALS.chrono_text_position)
    {
        case profiler_gui::RulerTextPosition_Top:
            textFlags = Qt::AlignTop | Qt::AlignHCenter;
            if (!m_main) rect.setTop(rect.top() + textRect.height() * 0.75);
            break;

        case profiler_gui::RulerTextPosition_Center:
            textFlags = Qt::AlignCenter;
            if (!m_main) rect.setTop(rect.top() + textRect.height() * 1.5);
            break;

        case profiler_gui::RulerTextPosition_Bottom:
            textFlags = Qt::AlignBottom | Qt::AlignHCenter;
            if (!m_main) rect.setHeight(rect.height() - textRect.height() * 0.75);
            break;
    }

    const auto textRect_width = textRect.width() * profiler_gui::FONT_METRICS_FACTOR;
    if (textRect_width < rect.width())
    {
        // Text will be drawed inside rectangle
        painter->drawText(rect, textFlags, text);
        painter->restore();
        return;
    }

    const auto w = textRect_width / currentScale;
    if (m_right + w < sceneRight)
    {
        // Text will be drawed to the right of rectangle
        rect.translate(rect.width(), 0);
        textFlags &= ~Qt::AlignHCenter;
        textFlags |= Qt::AlignLeft;
    }
    else if (m_left - w > sceneLeft)
    {
        // Text will be drawed to the left of rectangle
        rect.translate(-rect.width(), 0);
        textFlags &= ~Qt::AlignHCenter;
        textFlags |= Qt::AlignRight;
    }
    //else // Text will be drawed inside rectangle

    painter->drawText(rect, textFlags | Qt::TextDontClip, text);

    painter->restore();
    // END Paint!~~~~~~~~~~~~~~~~~~~~~~
}

void GraphicsRulerItem::hide()
{
    m_hover_on_indicator = false;
    m_hover_on_left_border = false;
    m_hover_on_right_border = false;
    m_reverse = false;
    m_strict = false;
    Parent::hide();
}

bool GraphicsRulerItem::indicatorContains(const QPointF& pos) const
{
    if (m_indicator.empty())
        return false;

    const auto itemX = toItem(pos.x());
    return m_indicator.containsPoint(QPointF(itemX, pos.y()), Qt::OddEvenFill);
}

void GraphicsRulerItem::setHoverLeft(bool hover)
{
    m_hover_on_left_border = hover;
}

void GraphicsRulerItem::setHoverRight(bool hover)
{
    m_hover_on_right_border = hover;
}

bool GraphicsRulerItem::hoverLeft(qreal x) const
{
    const auto dx = fabs(x - m_left) * view()->scale();
    return dx < 4;
}

bool GraphicsRulerItem::hoverRight(qreal x) const
{
    const auto dx = fabs(x - m_right) * view()->scale();
    return dx < 4;
}

QPointF GraphicsRulerItem::toItem(const QPointF& pos) const
{
    const auto sceneView = view();
    return QPointF((pos.x() - sceneView->offset()) * sceneView->scale() - x(), pos.y());
}

qreal GraphicsRulerItem::toItem(qreal x) const
{
    const auto sceneView = view();
    return (x - sceneView->offset()) * sceneView->scale() - this->x();
}

void GraphicsRulerItem::setColor(const QColor& color)
{
    m_color = color;
}

void GraphicsRulerItem::setBoundingRect(qreal x, qreal y, qreal w, qreal h)
{
    m_bounding_rect.setRect(x, y, w, h);
}

void GraphicsRulerItem::setBoundingRect(const QRectF& rect)
{
    m_bounding_rect = rect;
}

void GraphicsRulerItem::setLeftRight(qreal left, qreal right)
{
    if (left < right)
    {
        m_left = left;
        m_right = right;
    }
    else
    {
        m_left = right;
        m_right = left;
    }
}

void GraphicsRulerItem::setReverse(bool reverse)
{
    m_reverse = reverse;
}

void GraphicsRulerItem::setStrict(bool strict)
{
    m_strict = strict;
}

void GraphicsRulerItem::setHoverIndicator(bool hover)
{
    m_hover_on_indicator = hover;
}

const BlocksGraphicsView* GraphicsRulerItem::view() const
{
    return static_cast<const BlocksGraphicsView*>(scene()->parent());
}

BlocksGraphicsView* GraphicsRulerItem::view()
{
    return static_cast<BlocksGraphicsView*>(scene()->parent());
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

