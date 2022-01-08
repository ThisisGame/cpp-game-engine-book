/**
Lightweight profiler library for c++
Copyright(C) 2016-2019  Sergey Yagovtsev, Victor Zarubkin

Licensed under either of
	* MIT license (LICENSE.MIT or http://opensource.org/licenses/MIT)
    * Apache License, Version 2.0, (LICENSE.APACHE or http://www.apache.org/licenses/LICENSE-2.0)
at your option.

The MIT License
	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
	of the Software, and to permit persons to whom the Software is furnished
	to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
	INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
	PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
	LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
	TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE
	USE OR OTHER DEALINGS IN THE SOFTWARE.


The Apache License, Version 2.0 (the "License");
	You may not use this file except in compliance with the License.
	You may obtain a copy of the License at

	http://www.apache.org/licenses/LICENSE-2.0

	Unless required by applicable law or agreed to in writing, software
	distributed under the License is distributed on an "AS IS" BASIS,
	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
	See the License for the specific language governing permissions and
	limitations under the License.

**/

#include <algorithm>
#include <QGraphicsScene>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QResizeEvent>
#include <easy/utility.h>
#include "graphics_slider_area.h"
#include "globals.h"

//////////////////////////////////////////////////////////////////////////

GraphicsSliderItem::GraphicsSliderItem(int _size, bool _main)
    : Parent()
    , m_halfwidth(0.5)
    , m_bMain(_main)
{
    const int sizeHalf = 1 + (_size >> 1);

    if (_main)
        _size = -_size;

    m_indicator.reserve(3);
    m_indicator.push_back(QPointF(0, 0));
    m_indicator.push_back(QPointF(-sizeHalf, _size));
    m_indicator.push_back(QPointF(sizeHalf, _size));

    setBrush(Qt::SolidPattern);
}

GraphicsSliderItem::~GraphicsSliderItem()
{

}

void GraphicsSliderItem::paint(QPainter* _painter, const QStyleOptionGraphicsItem* /* _option */, QWidget* /* _widget */)
{
    const auto view = static_cast<const GraphicsSliderArea*>(scene()->parent());
    if (view->bindMode())
        return;

    const auto currentScale = view->getWindowScale();
    const qreal w = width() * currentScale;

    const auto br = rect();
    const QRectF r(br.left() * currentScale, br.top() + view->margin(), w, br.height() - view->margins() - 1);
    const auto r_right = r.right();
    const auto r_bottom = r.bottom();
    const auto b = brush();

    _painter->save();
    _painter->setTransform(QTransform::fromScale(1.0 / currentScale, 1), true);
    _painter->setBrush(b);

    if (w > 1)
    {
        _painter->setPen(Qt::NoPen);
        _painter->drawRect(r);

        // Draw left and right borders
        if (m_bMain) _painter->setCompositionMode(QPainter::CompositionMode_Exclusion);
        _painter->setPen(QColor::fromRgba(0xe0000000 | b.color().rgb()));
        _painter->drawLine(QPointF(r.left(), r.top()), QPointF(r.left(), r_bottom));
        _painter->drawLine(QPointF(r_right, r.top()), QPointF(r_right, r_bottom));
    }
    else
    {
        if (m_bMain) _painter->setCompositionMode(QPainter::CompositionMode_Exclusion);
        _painter->setPen(QColor::fromRgba(0xe0000000 | b.color().rgb()));
        _painter->drawLine(QPointF(r.left(), r.top()), QPointF(r.left(), r_bottom));
    }

    // Draw triangle indicators for small slider
    if (m_bMain)
        _painter->setTransform(QTransform::fromTranslate(r.left() + w * 0.5, br.top() + view->margin()), true);
    else
        _painter->setTransform(QTransform::fromTranslate(r.left() + w * 0.5, br.bottom() - view->margin() - 1), true);

    _painter->setPen(b.color().rgb());
    _painter->drawPolygon(m_indicator);

    _painter->restore();
}

qreal GraphicsSliderItem::width() const
{
    return m_halfwidth * 2.0;
}

qreal GraphicsSliderItem::halfwidth() const
{
    return m_halfwidth;
}

void GraphicsSliderItem::setWidth(qreal _width)
{
    const auto sceneRect = scene()->sceneRect();

    m_halfwidth = _width * 0.5;
    setRect(-m_halfwidth, sceneRect.top(), _width, sceneRect.height());
}

void GraphicsSliderItem::setHalfwidth(qreal _halfwidth)
{
    const auto sceneRect = scene()->sceneRect();

    m_halfwidth = _halfwidth;
    setRect(-m_halfwidth, sceneRect.top(), width(), sceneRect.height());
}

void GraphicsSliderItem::setColor(QRgb _color)
{
    setColor(QColor::fromRgba(_color));
}

void GraphicsSliderItem::setColor(const QColor& _color)
{
    auto b = brush();
    b.setColor(_color);
    setBrush(b);
}

//////////////////////////////////////////////////////////////////////////

GraphicsSliderArea::GraphicsSliderArea(QWidget* _parent)
    : Parent(_parent)
    , m_minimumValue(0)
    , m_maximumValue(500)
    , m_value(10)
    , m_windowScale(1)
    , m_mouseButtons(Qt::NoButton)
    , m_slider(nullptr)
    , m_selectionIndicator(nullptr)
    , m_imageItem(nullptr)
    , m_fontHeight(0)
    , m_bScrolling(false)
    , m_bBindMode(false)
    , m_bLocked(false)
    , m_bUpdatingPos(false)
    , m_bEmitChange(true)
    , m_bValidated(false)
{
    setCacheMode(QGraphicsView::CacheNone);
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    //setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
    setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    setOptimizationFlag(QGraphicsView::DontSavePainterState, true);

    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    setContentsMargins(0, 0, 0, 0);

    setScene(new QGraphicsScene(this));

    m_fontHeight = EASY_GLOBALS.size.font_height;// + 1;

    EASY_CONSTEXPR int SceneHeight = 500;
    scene()->setSceneRect(0, -(SceneHeight >> 1), 500, SceneHeight);

    m_selectionIndicator = new GraphicsSliderItem(6, false);
    m_selectionIndicator->setZValue(1);
    scene()->addItem(m_selectionIndicator);

    m_selectionIndicator->setPos(0, 0);
    m_selectionIndicator->setColor(0x40000000 | profiler_gui::RULER_COLOR.rgba());
    m_selectionIndicator->hide();

    m_slider = new GraphicsSliderItem(6, true);
    m_slider->setZValue(2);
    scene()->addItem(m_slider);

    m_slider->setPos(0, 0);
    m_slider->setColor(0x40c0c0c0);
    m_slider->hide();

    centerOn(0, 0);

    auto globalEvents = &EASY_GLOBALS.events;
    connect(globalEvents, &profiler_gui::GlobalSignals::sceneCleared, this, &This::clear);
    connect(globalEvents, &profiler_gui::GlobalSignals::sceneVisibleRegionSizeChanged, this, &This::onExternalSliderWidthChanged);
    connect(globalEvents, &profiler_gui::GlobalSignals::sceneVisibleRegionPosChanged, this, &This::onExternalChartSliderChanged);
    connect(globalEvents, &profiler_gui::GlobalSignals::chartSliderChanged, this, &This::onExternalChartSliderChanged);
    connect(globalEvents, &profiler_gui::GlobalSignals::sceneSizeChanged, this, &This::onSceneSizeChanged);
    connect(globalEvents, &profiler_gui::GlobalSignals::lockCharts, this, &This::lock);
    connect(globalEvents, &profiler_gui::GlobalSignals::unlockCharts, this, &This::unlock);
}

GraphicsSliderArea::~GraphicsSliderArea()
{

}

void GraphicsSliderArea::showEvent(QShowEvent* _event)
{
    Parent::showEvent(_event);
    if (!m_bValidated)
    {
        m_bValidated = true;
        validateScene();
    }
}

bool GraphicsSliderArea::canShowSlider() const
{
    return !m_bBindMode;
}

void GraphicsSliderArea::validateScene()
{
    if (!EASY_GLOBALS.scene.empty)
    {
        const profiler_gui::BoolFlagGuard guard(m_bEmitChange, false);
        setRange(EASY_GLOBALS.scene.left, EASY_GLOBALS.scene.right);
        setSliderWidth(EASY_GLOBALS.scene.window);
        setValue(EASY_GLOBALS.scene.offset);
        if (canShowSlider())
        {
            m_slider->show();
            scene()->update();
        }
    }
}

void GraphicsSliderArea::onExternalSliderWidthChanged(qreal _width)
{
    if (!m_bUpdatingPos)
    {
        const profiler_gui::BoolFlagGuard guard(m_bEmitChange, false);
        setSliderWidth(_width);
    }
}

void GraphicsSliderArea::onExternalChartSliderChanged(qreal _pos)
{
    if (!m_bUpdatingPos)
    {
        const profiler_gui::BoolFlagGuard guard(m_bEmitChange, false);
        setValue(_pos);
    }
}

void GraphicsSliderArea::onSceneSizeChanged(qreal _left, qreal _right)
{
    const profiler_gui::BoolFlagGuard guard(m_bEmitChange, false);
    setRange(_left, _right);
    if (canShowSlider())
        m_slider->show();
}

//////////////////////////////////////////////////////////////////////////

void GraphicsSliderArea::clear()
{
    const profiler_gui::BoolFlagGuard guard(m_bEmitChange, false);
    setRange(0, 100);
    setSliderWidth(2);
    setValue(0);
    m_selectionIndicator->hide();
    m_slider->hide();
}

//////////////////////////////////////////////////////////////////////////

bool GraphicsSliderArea::bindMode() const
{
    return m_bBindMode;
}

qreal GraphicsSliderArea::getWindowScale() const
{
    return m_windowScale;
}

qreal GraphicsSliderArea::minimum() const
{
    return m_minimumValue;
}

qreal GraphicsSliderArea::maximum() const
{
    return m_maximumValue;
}

qreal GraphicsSliderArea::range() const
{
    return m_maximumValue - m_minimumValue;
}

qreal GraphicsSliderArea::value() const
{
    return m_value;
}

qreal GraphicsSliderArea::sliderWidth() const
{
    return m_slider->width();
}

qreal GraphicsSliderArea::sliderHalfWidth() const
{
    return m_slider->halfwidth();
}

int GraphicsSliderArea::fontHeight() const
{
    return m_fontHeight;
}

int GraphicsSliderArea::margin() const
{
    return m_fontHeight + 1;
}

int GraphicsSliderArea::margins() const
{
    return margin() << 1;
}

//////////////////////////////////////////////////////////////////////////

bool GraphicsSliderArea::setValue(qreal _value)
{
    if (m_bUpdatingPos)
        return false;

    const profiler_gui::BoolFlagGuard guard(m_bUpdatingPos, true);

    const auto newValue = estd::clamp(m_minimumValue, _value, std::max(m_minimumValue, m_maximumValue - m_slider->width()));
    if (fabs(m_value - newValue) < 2 * std::numeric_limits<decltype(m_value)>::epsilon())
    {
        m_slider->setX(m_value + m_slider->halfwidth());
        return false;
    }

    m_value = newValue;
    m_slider->setX(m_value + m_slider->halfwidth());

    if (m_bEmitChange)
    {
        emit EASY_GLOBALS.events.chartSliderChanged(m_value);
    }

    if (m_imageItem->isVisible())
    {
        m_imageItem->onValueChanged();
        if (!m_slider->isVisible())
            scene()->update();
    }

    return true;
}

void GraphicsSliderArea::setRange(qreal _minValue, qreal _maxValue)
{
    const auto oldRange = range();
    const auto oldValue = oldRange < 1e-3 ? 0.0 : m_value / oldRange;

    m_minimumValue = _minValue;
    m_maximumValue = _maxValue;
    const auto range = this->range();

    const auto sceneRect = scene()->sceneRect();
    scene()->setSceneRect(_minValue, sceneRect.top(), range, sceneRect.height());

    const auto histogramRect = m_imageItem->boundingRect();
    m_imageItem->cancelImageUpdate();
    m_imageItem->setBoundingRect(_minValue, histogramRect.top(), range, histogramRect.height());

    setValue(_minValue + oldValue * range);

    onWindowWidthChange(width());

    if (m_imageItem->isVisible())
        m_imageItem->updateImage();
}

void GraphicsSliderArea::setSliderWidth(qreal _width)
{
    m_slider->setWidth(_width);
    if (setValue(m_value))
        return;

    if (m_imageItem->isVisible())
    {
        m_imageItem->onValueChanged();
        if (!m_slider->isVisible())
            scene()->update();
    }
}

//////////////////////////////////////////////////////////////////////////

void GraphicsSliderArea::setSelectionPos(qreal _left, qreal _right)
{
    m_selectionIndicator->setWidth(_right - _left);
    m_selectionIndicator->setX(_left + m_selectionIndicator->halfwidth());
}

void GraphicsSliderArea::showSelectionIndicator()
{
    if (!m_selectionIndicator->isVisible())
    {
        m_selectionIndicator->show();
        scene()->update();
    }
}

void GraphicsSliderArea::hideSelectionIndicator()
{
    if (m_selectionIndicator->isVisible())
    {
        m_selectionIndicator->hide();
        scene()->update();
    }
}

//////////////////////////////////////////////////////////////////////////

void GraphicsSliderArea::mousePressEvent(QMouseEvent* _event)
{
    _event->accept();

    m_mouseButtons = _event->buttons();

    if (m_mouseButtons & Qt::LeftButton)
    {
        if (_event->modifiers() & Qt::ControlModifier)
        {
            m_imageItem->pickBottomValue();
        }
        else if (_event->modifiers() & Qt::ShiftModifier)
        {
            m_imageItem->pickTopValue();
        }
        else
        {
            m_bScrolling = true;
            m_mousePressPos = _event->pos();
            if (!m_bBindMode)
                setValue(mapToScene(m_mousePressPos).x() - m_minimumValue - m_slider->halfwidth());
        }
    }

    if (m_mouseButtons & Qt::RightButton)
    {
        if (!_event->modifiers())
        {
            m_bBindMode = !m_bBindMode;
            m_slider->setVisible(canShowSlider());
            if (m_imageItem->isVisible())
                m_imageItem->onModeChanged();
            scene()->update();
        }
    }

    //Parent::mousePressEvent(_event);
}

void GraphicsSliderArea::mouseReleaseEvent(QMouseEvent* _event)
{
    m_mouseButtons = _event->buttons();
    m_bScrolling = false;
    _event->accept();
    //Parent::mouseReleaseEvent(_event);
}

void GraphicsSliderArea::mouseMoveEvent(QMouseEvent* _event)
{
    const auto pos = _event->pos();

    if (m_mouseButtons & Qt::LeftButton)
    {
        const auto delta = pos - m_mousePressPos;
        m_mousePressPos = pos;

        if (m_bScrolling)
        {
            auto realScale = m_windowScale;
            if (bindMode())
                realScale *= -range() / sliderWidth();
            setValue(m_value + delta.x() / realScale);
        }
    }

    m_imageItem->setMousePos(pos.x(), mapToScene(pos).y());
    if (m_imageItem->isVisible())
        scene()->update();
}

void GraphicsSliderArea::wheelEvent(QWheelEvent* _event)
{
    _event->accept();

    if (_event->modifiers() & Qt::ShiftModifier)
    {
        // Shift + mouse wheel will change histogram top boundary

        if (m_imageItem->isVisible())
        {
            if (_event->delta() > 0)
                m_imageItem->increaseTopValue();
            else
                m_imageItem->decreaseTopValue();
        }

        return;
    }

    if (_event->modifiers() & Qt::ControlModifier)
    {
        // Ctrl + mouse wheel will change histogram bottom boundary

        if (m_imageItem->isVisible())
        {
            if (_event->delta() > 0)
                m_imageItem->increaseBottomValue();
            else
                m_imageItem->decreaseBottomValue();
        }

        return;
    }

    if (!bindMode())
    {
        const auto w = m_slider->halfwidth() * (_event->delta() < 0 ? profiler_gui::SCALING_COEFFICIENT : profiler_gui::SCALING_COEFFICIENT_INV);
        setValue(mapToScene(_event->pos()).x() - m_minimumValue - w);
        emit EASY_GLOBALS.events.chartWheeled(m_value + w * m_windowScale, _event->delta());
    }
    else
    {
        auto x = static_cast<qreal>(_event->pos().x()) / m_windowScale;
        if (m_bBindMode) // check m_bBindMode because it may differ from bindMode() for arbitrary value complexity chart
            x *= sliderWidth() / range();
        emit EASY_GLOBALS.events.chartWheeled(m_value + x, _event->delta());
    }
}

void GraphicsSliderArea::resizeEvent(QResizeEvent* _event)
{
    const int h = _event->size().height();

    if (h == 0)
    {
        if (m_imageItem->isVisible())
            m_imageItem->cancelImageUpdate();

        onWindowWidthChange(_event->size().width());

        return;
    }

    if (_event->oldSize().height() != h)
    {
        auto rect = scene()->sceneRect();

        const int sceneHeight = h - 2;
        const int top = -(sceneHeight >> 1);
        scene()->setSceneRect(rect.left(), top, rect.width(), sceneHeight);

        const auto br = m_imageItem->boundingRect();
        m_imageItem->setBoundingRect(br.left(), top + margin(), br.width(), sceneHeight - margins() - 1);

        rect = m_slider->rect();
        m_slider->setRect(rect.left(), top, rect.width(), sceneHeight);

        if (m_selectionIndicator->isVisible())
        {
            rect = m_selectionIndicator->rect();
            m_selectionIndicator->setRect(rect.left(), top, rect.width(), sceneHeight);
        }
    }

    onWindowWidthChange(_event->size().width());

    if (m_imageItem->isVisible())
        m_imageItem->updateImage();
}

//////////////////////////////////////////////////////////////////////////

void GraphicsSliderArea::onWindowWidthChange(qreal _width)
{
    const auto oldScale = m_windowScale;
    const auto scrollingRange = range();

    if (scrollingRange < 1e-3)
    {
        m_windowScale = 1;
    }
    else
    {
        m_windowScale = _width / scrollingRange;
    }

    scale(m_windowScale / oldScale, 1);
}
