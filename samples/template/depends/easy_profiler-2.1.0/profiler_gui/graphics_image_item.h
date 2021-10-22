/************************************************************************
* file name         : graphics_image_item.h
* ----------------- :
* creation time     : 2018/01/20
* author            : Victor Zarubkin
* email             : v.s.zarubkin@gmail.com
* ----------------- :
* description       : The file contains declaration of GraphicsImageItem - an item
*                   : used to display, scroll and zoom QImage on graphics scene.
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

#ifndef EASY_PROFILER_GRAPHICS_IMAGE_ITEM_H
#define EASY_PROFILER_GRAPHICS_IMAGE_ITEM_H

#include <QGraphicsItem>
#include <atomic>
#include "timer.h"
#include "thread_pool_task.h"

class GraphicsImageItem : public QGraphicsItem
{
protected:

    using Parent = QGraphicsItem;
    using This = GraphicsImageItem;

    QRectF      m_boundingRect;
    QImage             m_image;
    Timer      m_boundaryTimer;
    ThreadPoolTask    m_worker;
    QPointF         m_mousePos;
    QImage*      m_workerImage;
    qreal        m_imageOrigin;
    qreal         m_imageScale;
    qreal  m_imageOriginUpdate;
    qreal   m_imageScaleUpdate;
    qreal  m_workerImageOrigin;
    qreal   m_workerImageScale;
    qreal           m_topValue;
    qreal        m_bottomValue;
    qreal           m_maxValue;
    qreal           m_minValue;
    bool              m_bEmpty;
    std::atomic_bool  m_bReady;

private:

    Timer              m_timer;
    bool  m_bPermitImageUpdate; ///< Is false when m_workerThread is parsing input dataset (when setSource(_block_id) is called)

public:

    explicit GraphicsImageItem();
    ~GraphicsImageItem() override;

    QRectF boundingRect() const override;

    virtual bool pickTopValue();
    virtual bool increaseTopValue();
    virtual bool decreaseTopValue();

    virtual bool pickBottomValue();
    virtual bool increaseBottomValue();
    virtual bool decreaseBottomValue();

    virtual bool updateImage();
    virtual void onModeChanged();

protected:

    virtual void onImageUpdated();

public:

    bool isEmpty() const;
    void onValueChanged();
    void setMousePos(const QPointF& pos);
    void setMousePos(qreal x, qreal y);
    void setBoundingRect(const QRectF& _rect);
    void setBoundingRect(qreal x, qreal y, qreal w, qreal h);
    bool cancelImageUpdate();

protected:

    void setEmpty(bool empty);
    void paintImage(QPainter* _painter);
    void paintImage(QPainter* _painter, qreal _scale, qreal _sceneLeft, qreal _sceneRight,
                    qreal _visibleRegionLeft, qreal _visibleRegionWidth);

    void setImageUpdatePermitted(bool _permit);
    bool isImageUpdatePermitted() const;

    void cancelAnyJob();
    void resetTopBottomValues();

    bool isReady() const;
    void setReady(bool _ready);

    void startTimer();
    void stopTimer();

private:

    void onTimeout();

}; // end of class GraphicsImageItem.

#endif //EASY_PROFILER_GRAPHICS_IMAGE_ITEM_H
