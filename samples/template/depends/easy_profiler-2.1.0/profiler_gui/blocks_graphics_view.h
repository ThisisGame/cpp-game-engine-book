/************************************************************************
* file name         : blocks_graphics_view.h
* ----------------- :
* creation time     : 2016/06/26
* author            : Victor Zarubkin
* email             : v.s.zarubkin@gmail.com
* ----------------- :
* description       : The file contains declaration of GraphicsScene and GraphicsView and
*                   : it's auxiliary classes for displyaing easy_profiler blocks tree.
* ----------------- :
* change log        : * 2016/06/26 Victor Zarubkin: moved sources from graphics_view.h
*                   :       and renamed classes from My* to Prof*.
*                   :
*                   : * 2016/06/29 Victor Zarubkin: Highly optimized painting performance and memory consumption.
*                   :
*                   : * 2016/06/30 Victor Zarubkin: Replaced doubles with floats (in ProfBlockItem) for less memory consumption.
*                   :
*                   : * 2016/09/15 Victor Zarubkin: Moved sources of GraphicsBlockItem and GraphicsRulerItem to separate files.
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

#ifndef EASY_GRAPHICS_VIEW_H
#define EASY_GRAPHICS_VIEW_H

#include <stdlib.h>
#include <unordered_set>

#include <QGraphicsView>
#include <QGraphicsItem>
#include <QPainterPath>
#include <QPoint>
#include <QRectF>
#include <QTimer>
#include <QLabel>

#include <easy/reader.h>

#include "common_functions.h"

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

class BlocksGraphicsView;
class GraphicsBlockItem;
class GraphicsScrollbar;
class GraphicsRulerItem;

//////////////////////////////////////////////////////////////////////////

class AuxItem : public QObject, public QGraphicsItem
{
    Q_OBJECT;
    Q_INTERFACES(QGraphicsItem);

protected:

    QRectF m_boundingRect;

public:

    explicit AuxItem() : QObject(nullptr), QGraphicsItem() {}
    ~AuxItem() override {}

    QRectF boundingRect() const override { return m_boundingRect; }
    void setBoundingRect(qreal x, qreal y, qreal w, qreal h) { m_boundingRect.setRect(x, y, w, h); }
    void setBoundingRect(const QRectF& _rect) { m_boundingRect = _rect; }
};

class ThreadNameItem : public AuxItem
{
public:
    explicit ThreadNameItem() : AuxItem() {}
    ~ThreadNameItem() override {}
    void paint(QPainter* _painter, const QStyleOptionGraphicsItem* _option, QWidget* _widget = nullptr) override;
};

class BackgroundItem : public AuxItem
{
    Q_OBJECT;

    QTimer          m_idleTimer;
    QPainterPath m_bookmarkSign;
    QLabel*           m_tooltip;
    size_t           m_bookmark;
    bool       m_bButtonPressed;

public:

    explicit BackgroundItem();
    ~BackgroundItem() override;

    void paint(QPainter* _painter, const QStyleOptionGraphicsItem* _option, QWidget* _widget = nullptr) override;

    bool mouseMove(const QPointF& scenePos);
    bool mousePress(const QPointF& scenePos);
    bool mouseRelease(const QPointF& scenePos);
    bool mouseDoubleClick(const QPointF& scenePos);
    void mouseLeave();

    bool contains(const QPointF& scenePos) const override;

signals:

    void bookmarkChanged(size_t index);
    void moved();

public slots:
    void onWindowActivationChanged(bool isActiveWindow);

private slots:

    void onIdleTimeout();
};

class ForegroundItem : public AuxItem
{
    Q_OBJECT;

    size_t m_bookmark;

public:

    explicit ForegroundItem();
    ~ForegroundItem() override {}

    void paint(QPainter* _painter, const QStyleOptionGraphicsItem* _option, QWidget* _widget = nullptr) override;

public slots:

    void onBookmarkChanged(size_t index);
    void onMoved();
};

//////////////////////////////////////////////////////////////////////////

struct BoldLabel : public QLabel {
    BoldLabel(const QString& _text, QWidget* _parent = nullptr);
    ~BoldLabel() override;
};

//////////////////////////////////////////////////////////////////////////

class BlocksGraphicsView : public QGraphicsView
{
    Q_OBJECT

private:

    using Parent = QGraphicsView;
    using This = BlocksGraphicsView;
    using Items = ::std::vector<GraphicsBlockItem*>;
    //using Keys = ::std::unordered_set<int, ::estd::hash<int> >;

    Items                               m_items; ///< Array of all GraphicsBlockItem items
    //Keys                                 m_keys; ///< Pressed keyboard keys
    ::profiler_gui::TreeBlocks m_selectedBlocks; ///< Array of items which were selected by selection zone (GraphicsRulerItem)
    QTimer                       m_flickerTimer; ///< Timer for flicking behavior
    QTimer                          m_idleTimer; ///< 
    QRectF                   m_visibleSceneRect; ///< Visible scene rectangle
    ::profiler::timestamp_t         m_beginTime; ///< Begin time of profiler session. Used to reduce values of all begin and end times of profiler blocks.
    qreal                          m_sceneWidth; ///< 
    qreal                               m_scale; ///< Current scale
    qreal                              m_offset; ///< Have to use manual offset for all scene content instead of using scrollbars because QScrollBar::value is 32-bit integer :(
    qreal                  m_visibleRegionWidth; ///< Visible scene rectangle in scene coordinates + width of vertical scrollbar (if visible)
    qreal                        m_timelineStep; ///< 
    uint64_t                         m_idleTime; ///< 
    QPoint                      m_mousePressPos; ///< Last mouse global position (used by mousePressEvent and mouseMoveEvent)
    QPoint                      m_mouseMovePath; ///< Mouse move path between press and release of any button
    Qt::MouseButtons             m_mouseButtons; ///< Pressed mouse buttons
    GraphicsScrollbar*             m_pScrollbar; ///< Pointer to the graphics scrollbar widget
    GraphicsRulerItem*          m_selectionItem; ///< Pointer to the GraphicsRulerItem which is displayed when you press right mouse button and move mouse left or right. This item is used to select blocks to display in tree widget.
    GraphicsRulerItem*              m_rulerItem; ///< Pointer to the GraphicsRulerItem which is displayed when you double click left mouse button and move mouse left or right. This item is used only to measure time.
    BackgroundItem*            m_backgroundItem; ///<
    QWidget*                      m_popupWidget; ///<
    int                         m_flickerSpeedX; ///< Current flicking speed x
    int                         m_flickerSpeedY; ///< Current flicking speed y
    int                       m_flickerCounterX;
    int                       m_flickerCounterY;
    bool                         m_bDoubleClick; ///< Is mouse buttons double clicked
    bool                        m_bUpdatingRect; ///< Stub flag which is used to avoid excess calculations on some scene update (flicking, scaling and so on)
    bool                               m_bEmpty; ///< Indicates whether scene is empty and has no items
    bool              m_isArbitraryValueTooltip;
    bool                             m_bHovered;

public:

    explicit BlocksGraphicsView(QWidget* _parent = nullptr);
    ~BlocksGraphicsView() override;

    // Public virtual methods

    void enterEvent(QEvent* _event) override;
    void leaveEvent(QEvent* _event) override;
    void wheelEvent(QWheelEvent* _event) override;
    void mousePressEvent(QMouseEvent* _event) override;
    void mouseDoubleClickEvent(QMouseEvent* _event) override;
    void mouseReleaseEvent(QMouseEvent* _event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

    void dragEnterEvent(QDragEnterEvent*) override {}

public:

    // Public non-virtual methods

    qreal sceneWidth() const;
    qreal chronoTime() const;
    qreal chronoTimeAux() const;

    void setScrollbar(GraphicsScrollbar* _scrollbar);
    void clear();

    void setTree(const ::profiler::thread_blocks_tree_t& _blocksTree);

    const Items& getItems() const;

    bool getSelectionRegionForSaving(profiler::timestamp_t& _beginTime, profiler::timestamp_t& _endTime) const;

    void inspectCurrentView(bool _strict) {
        onInspectCurrentView(_strict);
    }

public slots:
    void onWindowActivationChanged();
    void repaintHistogramImage();

signals:

    // Signals

    void sceneUpdated();
    void treeChanged();
    void intervalChanged(const ::profiler_gui::TreeBlocks& _blocks, ::profiler::timestamp_t _session_begin_time, ::profiler::timestamp_t _left, ::profiler::timestamp_t _right, bool _strict);

private:

    // Private non-virtual methods

    void notifySceneSizeChange();
    void notifyVisibleRegionSizeChange();
    void notifyVisibleRegionSizeChange(qreal _size);
    void notifyVisibleRegionPosChange();
    void notifyVisibleRegionPosChange(qreal _pos);

    void removePopup();
    bool needToIgnoreMouseEvent() const;

    GraphicsRulerItem* createRuler(bool _main = true);
    bool moveChrono(GraphicsRulerItem* ruler_item, qreal mouse_x);
    void initMode();
    int updateVisibleSceneRect();
    void updateTimelineStep(qreal _windowWidth);
    void scaleTo(qreal _scale);
    void scrollTo(const GraphicsBlockItem* _item);
    qreal mapToDiagram(qreal x) const;
    void onWheel(qreal _scenePos, int _wheelDelta);
    qreal setTree(GraphicsBlockItem* _item, const ::profiler::BlocksTree::children_t& _children, qreal& _height, uint32_t& _maxDepthChild, qreal _y, short _level);

    void revalidateOffset();

    void addSelectionToStatsTree();

private slots:

    // Private Slots

    void repaintScene();
    void onGraphicsScrollbarWheel(qreal _scenePos, int _wheelDelta);
    void onScrollbarValueChange(int);
    void onGraphicsScrollbarValueChange(qreal);
    void onFlickerTimeout();
    void onIdleTimeout();
    void onHierarchyFlagChange(bool _value);
    void onSelectedThreadChange(::profiler::thread_id_t id);
    void onSelectedBlockChange(unsigned int _block_index);
    void onRefreshRequired();
    void onThreadViewChanged();
    void onZoomSelection();
    void onInspectCurrentView(bool _strict);

public:

    // Public inline methods

    qreal scale() const
    {
        return m_scale;
    }

    qreal offset() const
    {
        return m_offset;
    }

    const QRectF& visibleSceneRect() const
    {
        return m_visibleSceneRect;
    }

    qreal timelineStep() const
    {
        return m_timelineStep;
    }

    qreal time2position(const profiler::timestamp_t& _time) const
    {
        return PROF_MICROSECONDS(qreal(_time - m_beginTime));
        //return PROF_MILLISECONDS(qreal(_time - m_beginTime));
    }

    ::profiler::timestamp_t position2time(qreal _pos) const
    {
        return PROF_FROM_MICROSECONDS(_pos);
        //return PROF_FROM_MILLISECONDS(_pos);
    }

}; // END of class BlocksGraphicsView.

//////////////////////////////////////////////////////////////////////////

class ThreadNamesWidget : public QGraphicsView
{
    Q_OBJECT

private:

    using Parent = QGraphicsView;
    using This = ThreadNamesWidget;

    QTimer                  m_idleTimer; ///< 
    uint64_t                 m_idleTime; ///< 
    BlocksGraphicsView*          m_view; ///<
    QWidget*              m_popupWidget; ///<
    int                     m_maxLength; ///<
    bool                     m_bHovered; ///<
    char                   m_padding[3]; ///<
    const int        m_additionalHeight; ///<

public:

    explicit ThreadNamesWidget(BlocksGraphicsView* _view, int _additionalHeight, QWidget* _parent = nullptr);
    ~ThreadNamesWidget() override;

    void enterEvent(QEvent* _event) override;
    void leaveEvent(QEvent* _event) override;
    void mousePressEvent(QMouseEvent* _event) override;
    void mouseDoubleClickEvent(QMouseEvent* _event) override;
    void mouseReleaseEvent(QMouseEvent* _event) override;
    void mouseMoveEvent(QMouseEvent* _event) override;
    void keyPressEvent(QKeyEvent* _event) override;
    void keyReleaseEvent(QKeyEvent* _event) override;
    void wheelEvent(QWheelEvent* _event) override;

    void dragEnterEvent(QDragEnterEvent*) override {}

    void clear();

    const BlocksGraphicsView* view() const
    {
        return m_view;
    }

public slots:
    void onWindowActivationChanged();

private:

    void removePopup();

private slots:

    void setVerticalScrollbarRange(int _minValue, int _maxValue);
    void onTreeChange();
    void onIdleTimeout();
    void repaintScene();

}; // END of class ThreadNamesWidget.

//////////////////////////////////////////////////////////////////////////

class DiagramWidget : public QWidget
{
    Q_OBJECT

private:

    class QSplitter*            m_splitter;
    GraphicsScrollbar*   m_scrollbar;
    BlocksGraphicsView*             m_view;
    ThreadNamesWidget* m_threadNamesWidget;

public:

    explicit DiagramWidget(QWidget* _parent = nullptr);
    ~DiagramWidget() override;

    BlocksGraphicsView* view();
    ThreadNamesWidget* threadsView();
    void clear();

    void save(class QSettings& settings);
    void restore(class QSettings& settings);

private:

    void initWidget();

}; // END of class DiagramWidget.

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

#endif // EASY_GRAPHICS_VIEW_H
