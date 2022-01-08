/************************************************************************
* file name         : graphics_scrollbar.h
* ----------------- : 
* creation time     : 2016/07/04
* author            : Victor Zarubkin
* email             : v.s.zarubkin@gmail.com
* ----------------- : 
* description       : This file contains declaration of 
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

#ifndef GRAPHICS_SCROLLBAR_H
#define GRAPHICS_SCROLLBAR_H

#include <stdlib.h>
#include <thread>
#include <atomic>
#include <QImage>
#include "timer.h"
#include "graphics_slider_area.h"

//////////////////////////////////////////////////////////////////////////

class GraphicsHistogramItem : public GraphicsImageItem
{
    using Parent = GraphicsImageItem;
    using This = GraphicsHistogramItem;

public:

    enum HistRegime : uint8_t { Hist_Pointer, Hist_Id };

private:

    qreal                         m_workerTopDuration;
    qreal                      m_workerBottomDuration;
    profiler::timestamp_t         m_blockTotalDuraion;
    QString                          m_topDurationStr;
    QString                       m_bottomDurationStr;
    QString                       m_medianDurationStr;
    QString                          m_avgDurationStr;
    QString                              m_threadName;
    QString                               m_blockName;
    QString                               m_blockType;
    profiler::BlocksTree::children_t m_selectedBlocks;
    profiler::timestamp_t            m_threadDuration;
    profiler::timestamp_t        m_threadProfiledTime;
    profiler::timestamp_t            m_threadWaitTime;
    profiler::timestamp_t            m_medianDuration;
    profiler::timestamp_t               m_avgDuration;
    profiler::timestamp_t        m_medianDurationFull;
    profiler::timestamp_t           m_avgDurationFull;
    profiler::timestamp_t      m_workerMedianDuration;
    profiler::timestamp_t         m_workerAvgDuration;
    const profiler_gui::EasyItems*          m_pSource;
    const profiler::BlocksTreeRoot* m_pProfilerThread;
    profiler::thread_id_t                  m_threadId;
    profiler::block_index_t                 m_blockId;
    profiler_gui::TimeUnits               m_timeUnits;
    HistRegime                               m_regime;

public:

    explicit GraphicsHistogramItem();
    ~GraphicsHistogramItem() override;

    void paint(QPainter* _painter, const QStyleOptionGraphicsItem* _option, QWidget* _widget) override;

    bool updateImage() override;
    void onImageUpdated() override;

    bool pickTopValue() override;
    bool increaseTopValue() override;
    bool decreaseTopValue() override;

    bool pickBottomValue() override;
    bool increaseBottomValue() override;
    bool decreaseBottomValue() override;

    void onModeChanged() override;

public:

    // Public non-virtual methods

    profiler::thread_id_t threadId() const;

    void setSource(profiler::thread_id_t _thread_id, const profiler_gui::EasyItems* _items);
    void setSource(profiler::thread_id_t _thread_id, profiler::block_id_t _block_id);
    void rebuildSource(HistRegime _regime);
    void rebuildSource();
    void validateName();

    void pickFrameTime(qreal _y) const;

private:

    void paintMouseIndicator(QPainter* _painter, qreal _top, qreal _bottom, qreal _width, qreal _height, qreal _top_width, qreal _mouse_y, qreal _delta_time, int _font_h);
    void paintByPtr(QPainter* _painter);
    void paintById(QPainter* _painter);

    void updateImageAsync(QRectF _boundingRect, HistRegime _regime, qreal _current_scale,
        qreal _minimum, qreal _maximum, qreal _range, qreal _value, qreal _width,
        qreal _top_duration, qreal _bottom_duration, int _min_column_width, bool _bindMode,
        float _frame_time, profiler::timestamp_t _begin_time, bool _autoAdjustHist, bool _drawBorders);

}; // END of class GraphicsHistogramItem.

//////////////////////////////////////////////////////////////////////////

class GraphicsScrollbar : public GraphicsSliderArea
{
    Q_OBJECT

private:

    using Parent = GraphicsSliderArea;
    using This = GraphicsScrollbar;

    GraphicsHistogramItem* m_histogramItem = nullptr;

public:

    explicit GraphicsScrollbar(int _initialHeight, QWidget* _parent = nullptr);
    ~GraphicsScrollbar() override;

    void clear() override;
    void mousePressEvent(QMouseEvent* _event) override;

public:

    // Public non-virtual methods

    profiler::thread_id_t histThread() const;

    void setHistogramSource(profiler::thread_id_t _thread_id, const profiler_gui::EasyItems* _items);
    void setHistogramSource(profiler::thread_id_t _thread_id, profiler::block_id_t _block_id);
    void setHistogramSource(profiler::thread_id_t _thread_id, const profiler_gui::EasyItems& _items) {
        setHistogramSource(_thread_id, &_items);
    }

public slots:

    void repaintHistogramImage();

private slots:

    void onThreadViewChanged();
    void onAutoAdjustHistogramChanged();
    void onDisplayOnlyFramesOnHistogramChanged();

}; // END of class GraphicsScrollbar.

//////////////////////////////////////////////////////////////////////////

#endif // GRAPHICS_SCROLLBAR_H
