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

#ifndef EASY_PROFILER_GRAPHICS_SLIDER_AREA_H
#define EASY_PROFILER_GRAPHICS_SLIDER_AREA_H

#include <stdlib.h>
#include <QGraphicsView>
#include <QGraphicsRectItem>
#include <QAction>
#include <QPolygonF>
#include "common_types.h"
#include "graphics_image_item.h"

//////////////////////////////////////////////////////////////////////////

class GraphicsSliderItem : public QGraphicsRectItem
{
    using Parent = QGraphicsRectItem;
    using This = GraphicsSliderItem;

private:

    QPolygonF m_indicator;
    qreal m_halfwidth;
    bool m_bMain;

public:

    explicit GraphicsSliderItem(int _size, bool _main);
    ~GraphicsSliderItem() override;

    void paint(QPainter* _painter, const QStyleOptionGraphicsItem* _option, QWidget* _widget) override;

    qreal width() const;
    qreal halfwidth() const;

    void setWidth(qreal _width);
    void setHalfwidth(qreal _halfwidth);

    void setColor(QRgb _color);
    void setColor(const QColor& _color);

}; // END of class GraphicsSliderItem.

//////////////////////////////////////////////////////////////////////////

class GraphicsSliderArea : public QGraphicsView
{
    Q_OBJECT

    using Parent = QGraphicsView;
    using This = GraphicsSliderArea;

protected:

    qreal                        m_minimumValue;
    qreal                        m_maximumValue;
    qreal                               m_value;
    qreal                         m_windowScale;
    QPoint                      m_mousePressPos;
    Qt::MouseButtons             m_mouseButtons;
    GraphicsSliderItem*                m_slider;
    GraphicsSliderItem*    m_selectionIndicator;
    GraphicsImageItem*              m_imageItem;
    int                            m_fontHeight;
    bool                           m_bScrolling;
    bool                            m_bBindMode;
    bool                              m_bLocked;
    bool                         m_bUpdatingPos;
    bool                          m_bEmitChange;
    bool                           m_bValidated;

public:

    explicit GraphicsSliderArea(QWidget* _parent = nullptr);
    ~GraphicsSliderArea() override;

    // Public virtual methods

    void showEvent(QShowEvent* _event) override;
    void mousePressEvent(QMouseEvent* _event) override;
    void mouseReleaseEvent(QMouseEvent* _event) override;
    void mouseMoveEvent(QMouseEvent* _event) override;
    void wheelEvent(QWheelEvent* _event) override;
    void resizeEvent(QResizeEvent* _event) override;
    void dragEnterEvent(QDragEnterEvent*) override {}
    void contextMenuEvent(QContextMenuEvent*) override {}

    virtual void clear();

public:

    // Public non-virtual methods

    bool bindMode() const;
    qreal getWindowScale() const;

    qreal minimum() const;
    qreal maximum() const;
    qreal range() const;
    qreal value() const;
    qreal sliderWidth() const;
    qreal sliderHalfWidth() const;
    int fontHeight() const;
    int margin() const;
    int margins() const;

    bool setValue(qreal _value);
    void setRange(qreal _minValue, qreal _maxValue);
    void setSliderWidth(qreal _width);
    void setSelectionPos(qreal _left, qreal _right);
    void showSelectionIndicator();
    void hideSelectionIndicator();

protected:

    virtual bool canShowSlider() const;
    void validateScene();

public slots:

    void lock() { m_bLocked = true; }
    void unlock() { m_bLocked = false; }

protected slots:

    void onExternalSliderWidthChanged(qreal _width);
    void onExternalChartSliderChanged(qreal _pos);
    void onSceneSizeChanged(qreal _left, qreal _right);
    void onWindowWidthChange(qreal _width);

}; // END of class GraphicsSliderArea.

//////////////////////////////////////////////////////////////////////////

#endif //EASY_PROFILER_GRAPHICS_SLIDER_AREA_H
