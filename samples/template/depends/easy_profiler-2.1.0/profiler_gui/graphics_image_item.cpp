/************************************************************************
* file name         : graphics_image_item.cpp
* ----------------- :
* creation time     : 2018/01/20
* author            : Victor Zarubkin
* email             : v.s.zarubkin@gmail.com
* ----------------- :
* description       : The file contains implementation of GraphicsImageItem.
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
#include <QPainter>
#include "graphics_image_item.h"
#include "graphics_slider_area.h"

EASY_CONSTEXPR int TimerInterval = 40;
EASY_CONSTEXPR int BoundaryTimerInterval = 100;

GraphicsImageItem::GraphicsImageItem() : Parent(nullptr)
    , m_boundaryTimer([this] { updateImage(); }, true)
    , m_workerImage(nullptr)
    , m_imageOrigin(0)
    , m_imageScale(1)
    , m_imageOriginUpdate(0)
    , m_imageScaleUpdate(1)
    , m_workerImageOrigin(0)
    , m_workerImageScale(1)
    , m_topValue(0)
    , m_bottomValue(0)
    , m_maxValue(0)
    , m_minValue(0)
    , m_timer(::std::bind(&This::onTimeout, this))
    , m_bEmpty(true)
    , m_bPermitImageUpdate(true)
{
    m_bReady = false;
    m_boundaryTimer.setInterval(BoundaryTimerInterval);
    m_timer.setInterval(TimerInterval);
}

GraphicsImageItem::~GraphicsImageItem()
{
    cancelAnyJob();
}

QRectF GraphicsImageItem::boundingRect() const
{
    return m_boundingRect;
}

void GraphicsImageItem::setBoundingRect(const QRectF& _rect)
{
    m_boundingRect = _rect;
}

void GraphicsImageItem::setBoundingRect(qreal x, qreal y, qreal w, qreal h)
{
    m_boundingRect.setRect(x, y, w, h);
}

bool GraphicsImageItem::isEmpty() const
{
    return m_bEmpty;
}

void GraphicsImageItem::setEmpty(bool empty)
{
    m_bEmpty = empty;
}

void GraphicsImageItem::setMousePos(const QPointF& pos)
{
    m_mousePos = pos;
}

void GraphicsImageItem::setMousePos(qreal x, qreal y)
{
    m_mousePos.setX(x);
    m_mousePos.setY(y);
}

bool GraphicsImageItem::updateImage()
{
    if (!cancelImageUpdate())
        return false;

    setReady(false);
    startTimer();

    return true;
}

void GraphicsImageItem::onValueChanged()
{
    const auto widget = qobject_cast<const GraphicsSliderArea*>(scene()->parent());
    if (widget == nullptr || !widget->bindMode())
        return;

    m_boundaryTimer.stop();

    const auto sliderWidth_inv = 1.0 / widget->sliderWidth();
    const auto k = widget->range() * sliderWidth_inv;

    const auto deltaScale = m_imageScaleUpdate < k ? (k / m_imageScaleUpdate) : (m_imageScaleUpdate / k);
    if (deltaScale > 4)
    {
        updateImage();
        return;
    }

    const auto deltaOffset = (widget->value() - m_imageOriginUpdate) * sliderWidth_inv;
    if (deltaOffset < 1.5 || deltaOffset > 4.5)
    {
        updateImage();
        return;
    }

    m_boundaryTimer.start();
}

void GraphicsImageItem::onModeChanged()
{
    if (!isImageUpdatePermitted())
        return;

    m_boundaryTimer.stop();
    updateImage();
}

void GraphicsImageItem::onImageUpdated()
{

}

bool GraphicsImageItem::cancelImageUpdate()
{
    if (!isImageUpdatePermitted())
        return false;

    cancelAnyJob();

    return true;
}

bool GraphicsImageItem::pickTopValue()
{
    if (isEmpty())
    {
        return false;
    }

    const auto y = m_mousePos.y();
    if (isImageUpdatePermitted() && m_boundingRect.top() < y && y < m_boundingRect.bottom())
    {
        m_topValue = m_bottomValue + (m_topValue - m_bottomValue) * (m_boundingRect.bottom() - y) / m_boundingRect.height();
        m_boundaryTimer.stop();
        updateImage();
        return true;
    }

    return false;
}

bool GraphicsImageItem::increaseTopValue()
{
    if (isEmpty())
    {
        return false;
    }

    if (isImageUpdatePermitted() && m_topValue < m_maxValue)
    {
        auto step = 0.05 * (m_maxValue - m_bottomValue);
        if (m_topValue < (m_bottomValue + 1.25 * step))
            step = 0.1 * (m_topValue - m_bottomValue);
        m_topValue = std::min(m_maxValue, m_topValue + step);
        m_boundaryTimer.start();
        return true;
    }

    return false;
}

bool GraphicsImageItem::decreaseTopValue()
{
    if (isEmpty())
    {
        return false;
    }

    if (isImageUpdatePermitted() && m_topValue > m_bottomValue)
    {
        auto step = 0.05 * (m_maxValue - m_bottomValue);
        if (m_topValue < (m_bottomValue + 1.25 * step))
            step = std::max(0.1 * (m_topValue - m_bottomValue), 0.3);

        if (m_topValue > (m_bottomValue + 1.25 * step))
        {
            m_topValue = std::max(m_bottomValue + step, m_topValue - step);
            m_boundaryTimer.start();
            return true;
        }
    }

    return false;
}

bool GraphicsImageItem::pickBottomValue()
{
    if (isEmpty())
    {
        return false;
    }

    const auto y = m_mousePos.y();
    if (isImageUpdatePermitted() && m_boundingRect.top() < y && y < m_boundingRect.bottom())
    {
        m_bottomValue = m_bottomValue + (m_topValue - m_bottomValue) * (m_boundingRect.bottom() - y) / m_boundingRect.height();
        m_boundaryTimer.stop();
        updateImage();
        return true;
    }

    return false;
}

bool GraphicsImageItem::increaseBottomValue()
{
    if (isEmpty())
    {
        return false;
    }

    if (isImageUpdatePermitted() && m_bottomValue < m_topValue)
    {
        auto step = 0.05 * (m_topValue - m_minValue);
        if (m_bottomValue > (m_topValue - 1.25 * step))
            step = 0.1 * (m_topValue - m_bottomValue);

        if (m_bottomValue < (m_topValue - 1.25 * step))
        {
            m_bottomValue = std::min(m_topValue - step, m_bottomValue + step);
            m_boundaryTimer.start();
            return true;
        }
    }

    return false;
}

bool GraphicsImageItem::decreaseBottomValue()
{
    if (isEmpty())
    {
        return false;
    }

    if (isImageUpdatePermitted() && m_bottomValue > m_minValue)
    {
        auto step = 0.05 * (m_topValue - m_minValue);
        if (m_bottomValue > (m_topValue - 1.25 * step))
            step = std::max(0.1 * (m_topValue - m_bottomValue), 0.3);
        m_bottomValue = std::max(m_minValue, m_bottomValue - step);
        m_boundaryTimer.start();
        return true;
    }

    return false;
}

void GraphicsImageItem::paintImage(QPainter* _painter)
{
    _painter->setPen(Qt::NoPen);
    _painter->drawImage(0, static_cast<int>(m_boundingRect.top()), m_image);
}

void GraphicsImageItem::paintImage(QPainter* _painter, qreal _scale, qreal _sceneLeft, qreal _sceneRight,
                                   qreal _visibleRegionLeft, qreal _visibleRegionWidth)
{
    const auto dscale = (_sceneRight - _sceneLeft) / (_visibleRegionWidth * m_imageScale);

    _painter->setPen(Qt::NoPen);
    _painter->setTransform(QTransform::fromScale(dscale, 1), true);
    _painter->drawImage(QPointF {(_sceneLeft + m_imageOrigin - _visibleRegionLeft) * _scale * m_imageScale, m_boundingRect.top()}, m_image);
    _painter->setTransform(QTransform::fromScale(1. / dscale, 1), true);
}

void GraphicsImageItem::onTimeout()
{
    if (!isVisible())
    {
        stopTimer();
        return;
    }

    if (isReady())
    {
        stopTimer();

        if (!isImageUpdatePermitted())
        {
            // Worker thread have finished parsing input data (when setSource(_block_id) was called)
            setImageUpdatePermitted(true); // From now we can update an image
            updateImage();
        }
        else
        {
            // Image updated

            m_workerImage->swap(m_image);
            delete m_workerImage;
            m_workerImage = nullptr;

            m_imageOriginUpdate = m_imageOrigin = m_workerImageOrigin;
            m_imageScaleUpdate = m_imageScale = m_workerImageScale;

            onImageUpdated();
            scene()->update();
        }
    }
}

void GraphicsImageItem::setImageUpdatePermitted(bool _permit)
{
    m_bPermitImageUpdate = _permit;
}

bool GraphicsImageItem::isImageUpdatePermitted() const
{
    return m_bPermitImageUpdate;
}

void GraphicsImageItem::cancelAnyJob()
{
    stopTimer();

    m_worker.dequeue();

    delete m_workerImage;
    m_workerImage = nullptr;

    m_imageOriginUpdate = m_imageOrigin;
    m_imageScaleUpdate = m_imageScale;
}

void GraphicsImageItem::resetTopBottomValues()
{
    m_topValue = m_maxValue;
    m_bottomValue = m_minValue;
}

bool GraphicsImageItem::isReady() const
{
    return m_bReady.load(std::memory_order_acquire);
}

void GraphicsImageItem::setReady(bool _ready)
{
    m_bReady.store(_ready, std::memory_order_release);
}

void GraphicsImageItem::startTimer()
{
    m_timer.start();
}

void GraphicsImageItem::stopTimer()
{
    m_timer.stop();
}
