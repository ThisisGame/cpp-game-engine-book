/************************************************************************
* file name         : blocks_graphics_view.cpp
* ----------------- :
* creation time     : 2016/06/26
* author            : Victor Zarubkin
* email             : v.s.zarubkin@gmail.com
* ----------------- :
* description       : The file contains implementation of GraphicsScene and GraphicsView and
*                   : it's auxiliary classes for displyaing easy_profiler blocks tree.
* ----------------- :
* change log        : * 2016/06/26 Victor Zarubkin: Moved sources from graphics_view.h
*                   :       and renamed classes from My* to Prof*.
*                   :
*                   : * 2016/06/27 Victor Zarubkin: Added text shifting relatively to it's parent item.
*                   :       Disabled border lines painting because of vertical lines painting bug.
*                   :       Changed height of blocks. Variable thread-block height.
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

#include <math.h>

#include <QApplication>
#include <QDebug>
#include <QGraphicsDropShadowEffect>
#include <QGraphicsScene>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QScrollBar>
#include <QSettings>
#include <QSignalBlocker>
#include <QSplitter>
#include <QWheelEvent>

#include "arbitrary_value_tooltip.h"
#include "blocks_graphics_view.h"
#include "bookmarks_editor.h"
#include "graphics_block_item.h"
#include "graphics_ruler_item.h"
#include "graphics_scrollbar.h"
#include "globals.h"

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

const qreal MIN_SCALE = pow(profiler_gui::SCALING_COEFFICIENT_INV, 70); // Up to 1000 sec scale
const qreal MAX_SCALE = pow(profiler_gui::SCALING_COEFFICIENT, 45); // ~23000 --- Up to 10 ns scale
const qreal BASE_SCALE = pow(profiler_gui::SCALING_COEFFICIENT_INV, 25); // ~0.003

EASY_CONSTEXPR QRgb BACKGROUND_1 = 0xffe4e4ec;
EASY_CONSTEXPR QRgb BACKGROUND_2 = profiler::colors::White;
EASY_CONSTEXPR QRgb TIMELINE_BACKGROUND = 0x20000000 | (profiler::colors::Grey800 & 0x00ffffff);// 0x20303030;
EASY_CONSTEXPR QRgb TIMELINE_BORDER = 0xffa8a0a0;

EASY_CONSTEXPR int IDLE_TIMER_INTERVAL = 200; // 5Hz
EASY_CONSTEXPR uint64_t IDLE_TIME = 400;

EASY_CONSTEXPR int FLICKER_INTERVAL = 10; // 100Hz
EASY_CONSTEXPR qreal FLICKER_FACTOR = 16.0 / FLICKER_INTERVAL;

EASY_CONSTEXPR int BOOKMARK_WIDTH = 8;
EASY_CONSTEXPR int BOOKMARK_HEIGHT = 11;

#ifdef max
#undef max
#endif

#ifdef min
#undef min
#endif

using estd::clamp;

//////////////////////////////////////////////////////////////////////////

BoldLabel::BoldLabel(const QString& _text, QWidget* _parent) : QLabel(_text, _parent)
{
    auto f = font();
    f.setBold(true);
    setFont(f);
}

BoldLabel::~BoldLabel()
{

}

//////////////////////////////////////////////////////////////////////////

BackgroundItem::BackgroundItem() : AuxItem()
    , m_tooltip(nullptr)
    , m_bookmark(profiler_gui::numeric_max<decltype(m_bookmark)>())
    , m_bButtonPressed(false)
{
    m_bookmarkSign.lineTo(px(BOOKMARK_WIDTH), 0);
    m_bookmarkSign.lineTo(px(BOOKMARK_WIDTH), px(BOOKMARK_HEIGHT) + 1);
    m_bookmarkSign.lineTo(px(BOOKMARK_WIDTH >> 1), px((BOOKMARK_HEIGHT >> 1) + 1) + 1);
    m_bookmarkSign.lineTo(0, px(BOOKMARK_HEIGHT) + 1);
    m_bookmarkSign.lineTo(0, 0);

    m_idleTimer.setInterval(IDLE_TIMER_INTERVAL);
    m_idleTimer.setSingleShot(true);
    connect(&m_idleTimer, &QTimer::timeout, this, &BackgroundItem::onIdleTimeout);
}

BackgroundItem::~BackgroundItem()
{
    delete m_tooltip;
}

void BackgroundItem::paint(QPainter* _painter, const QStyleOptionGraphicsItem*, QWidget*)
{
    auto const sceneView = static_cast<BlocksGraphicsView*>(scene()->parent());
    const auto visibleSceneRect = sceneView->visibleSceneRect();
    const auto currentScale = sceneView->scale();
    const auto offset = sceneView->offset();
    const auto left = offset * currentScale;
    const auto h = visibleSceneRect.height();
    const auto visibleBottom = h - 1;
    const auto borderColor = QColor::fromRgb(TIMELINE_BORDER);
    const auto textShiftY = h + EASY_GLOBALS.size.timeline_height - 5;

    QRectF rect;

    _painter->save();
    _painter->setTransform(QTransform::fromTranslate(-x(), -y()));

    const auto& items = sceneView->getItems();
    if (!items.empty())
    {
        const auto overlap = EASY_GLOBALS.size.threads_row_spacing >> 1;
        static const QBrush brushes[2] = {QColor::fromRgb(BACKGROUND_1), QColor::fromRgb(BACKGROUND_2)};
        int i = -1;

        // Draw background
        _painter->setPen(profiler_gui::SYSTEM_BORDER_COLOR);
        for (auto item : items)
        {
            ++i;

            auto br = item->boundingRect();
            auto top = item->y() + br.top() - visibleSceneRect.top();
            auto bottom = top + br.height();

            if (top > h || bottom < 0)
                continue;

            if (item->threadId() == EASY_GLOBALS.selected_thread)
                _painter->setBrush(QBrush(QColor::fromRgb(profiler_gui::SELECTED_THREAD_BACKGROUND)));
            else
                _painter->setBrush(brushes[i & 1]);

            rect.setRect(0, top - overlap, visibleSceneRect.width(), br.height() + EASY_GLOBALS.size.threads_row_spacing);
            const auto dh = rect.bottom() - visibleBottom;
            if (dh > 0)
                rect.setHeight(rect.height() - dh);

            if (rect.top() < 0)
                rect.setTop(0);

            _painter->drawRect(rect);
        }
    }

    // Draw timeline scale marks ----------------
    _painter->setBrush(QColor::fromRgba(TIMELINE_BACKGROUND));

    const auto sceneStep = sceneView->timelineStep();
    const auto factor = profiler_gui::timeFactor(sceneStep);
    const auto step = sceneStep * currentScale;
    auto first = static_cast<quint64>(offset / sceneStep);
    const int odd = first & 1;
    const auto nsteps = (1 + odd) * 2 + static_cast<int>(visibleSceneRect.width() / step);
    first -= odd;

    QPen pen(borderColor);
    pen.setWidth(px(2));
    _painter->setPen(pen);
    _painter->drawLine(QPointF(0, h), QPointF(visibleSceneRect.width(), h));
    _painter->setPen(borderColor);

    QLineF marks[20];
    qreal first_x = first * sceneStep;

    const auto textWidth = QFontMetricsF(_painter->font(), sceneView).
        width(QString::number(static_cast<quint64>(0.5 + first_x * factor))) * profiler_gui::FONT_METRICS_FACTOR + px(10);

    const int n = 1 + static_cast<int>(textWidth / step);
    int next = first % n;
    if (next != 0)
        next = n - next;

    const int smallStroke = px(4);
    const int bigStroke = px(8);

    first_x *= currentScale;
    for (int i = 0; i < nsteps; ++i, --next)
    {
        auto current = first_x - left + step * i;

        if ((i & 1) == 0)
        {
            rect.setRect(current, 0, step, h);
            _painter->drawRect(rect);

            for (int j = 0; j < 20; ++j)
            {
                auto xmark = current + j * step * 0.1;
                marks[j].setLine(xmark, h, xmark, h + ((j % 5) ? smallStroke : bigStroke));
            }

            _painter->drawLines(marks, 20);
        }

        if (next <= 0)
        {
            next = n;
            _painter->setPen(profiler_gui::TEXT_COLOR);
            _painter->drawText(QPointF(current + 1, textShiftY),
                               QString::number(static_cast<quint64>(0.5 + (current + left) * factor / currentScale)));
            _painter->setPen(borderColor);
        }

        // TEST
        // this is for testing (order of lines will be painted):
        //_painter->setPen(Qt::black);
        //_painter->drawText(QPointF(current + step * 0.4, h - 20), QString::number(i));
        //_painter->setPen(Qt::gray);
        // TEST
    }
    // END Draw timeline scale marks ~~~~~~~~~~~~

    // Draw bookmarks
    const auto& bookmarks = EASY_GLOBALS.bookmarks;
    const auto beginTime = PROF_FROM_MICROSECONDS(offset) + EASY_GLOBALS.begin_time;
    const auto endTime = PROF_FROM_MICROSECONDS(0.5 + offset + visibleSceneRect.width() / currentScale) + EASY_GLOBALS.begin_time;
    auto first_it = std::lower_bound(bookmarks.cbegin(), bookmarks.cend(), beginTime,
                                     [](const profiler::Bookmark& bookmark, profiler::timestamp_t value)
    {
        return bookmark.pos < value;
    });

    for (; first_it != bookmarks.cend(); ++first_it)
    {
        const auto& bookmark = *first_it;
        if (bookmark.pos >= beginTime)
            break;
    }

    if (first_it != bookmarks.cend())
    {
        pen.setColor(Qt::black);
        pen.setWidth(px(1));

        _painter->setPen(borderColor);
        _painter->setBrush(Qt::transparent);
        _painter->setRenderHint(QPainter::Antialiasing);

        const int half = px(BOOKMARK_WIDTH) >> 1;
        auto color = QColor(Qt::transparent).rgb();
        qreal prevPos = -1e300;
        for (auto it = first_it; it != bookmarks.cend() && it->pos <= endTime; ++it)
        {
            const qreal pos =
                (PROF_MICROSECONDS(it->pos - EASY_GLOBALS.begin_time) - offset) * currentScale - half;

            const bool isSelectedBookmark = m_bookmark == static_cast<size_t>(std::distance(bookmarks.cbegin(), it));

            const auto delta = fabs(pos - prevPos);
            if (delta < half && !isSelectedBookmark)
                continue;

            if (color != it->color)
                _painter->setBrush(QColor::fromRgb(it->color));

            const auto path = m_bookmarkSign.translated(pos, h);
            _painter->drawPath(path);

            if (isSelectedBookmark)
                _painter->strokePath(path, pen);

            prevPos = pos;
        }
    }

    _painter->restore();
}

//////////////////////////////////////////////////////////////////////////

bool BackgroundItem::mouseMove(const QPointF& scenePos)
{
    const auto prev = m_bookmark;

    delete m_tooltip;
    m_tooltip = nullptr;

    auto& bookmarks = EASY_GLOBALS.bookmarks;
    if (bookmarks.empty())
    {
        if (m_idleTimer.isActive())
            m_idleTimer.stop();

        profiler_gui::set_max(m_bookmark);
        if (prev != m_bookmark)
        {
            qApp->restoreOverrideCursor();
            emit bookmarkChanged(m_bookmark);
            update();
        }

        return false;
    }

    const auto scene = static_cast<BlocksGraphicsView*>(this->scene()->parent());

    const auto y = scenePos.y() - this->y() - scene->visibleSceneRect().y();
    if (y < scene->visibleSceneRect().height())
    {
        if (!m_bButtonPressed || m_bookmark >= EASY_GLOBALS.bookmarks.size())
        {
            if (m_idleTimer.isActive())
                m_idleTimer.stop();

            profiler_gui::set_max(m_bookmark);
            if (prev != m_bookmark)
            {
                qApp->restoreOverrideCursor();
                emit bookmarkChanged(m_bookmark);
                update();
            }

            return false;
        }
    }

    const auto scale = scene->scale();
    const auto timestamp = PROF_FROM_MICROSECONDS(scenePos.x()) + EASY_GLOBALS.begin_time;

    if (!m_bButtonPressed)
    {
        profiler_gui::set_max(m_bookmark);

        auto first_it = std::lower_bound(bookmarks.cbegin(), bookmarks.cend(), timestamp,
                                         [](const profiler::Bookmark& bookmark, profiler::timestamp_t value)
        {
            return bookmark.pos < value;
        });

        if (first_it != bookmarks.cbegin())
            --first_it;

        const auto width = px(8);
        for (auto it = first_it; it != bookmarks.cend(); ++it)
        {
            const qreal pos = PROF_MICROSECONDS(it->pos - EASY_GLOBALS.begin_time);
            const auto dx = fabs(scenePos.x() - pos) * scale;
            if (dx < width)
            {
                m_bookmark = static_cast<size_t>(std::distance(bookmarks.cbegin(), it));
                break;
            }
        }

        if (prev != m_bookmark)
        {
            if (!profiler_gui::is_max(m_bookmark))
            {
                if (qApp->overrideCursor() == nullptr || qApp->overrideCursor()->shape() != Qt::PointingHandCursor)
                    qApp->setOverrideCursor(QCursor(Qt::PointingHandCursor));
            }
            else
            {
                qApp->restoreOverrideCursor();
            }
            emit bookmarkChanged(m_bookmark);
            update();
        }

        if (!profiler_gui::is_max(m_bookmark))
        {
            if (!m_idleTimer.isActive())
                m_idleTimer.start();
        }
        else
        {
            if (m_idleTimer.isActive())
                m_idleTimer.stop();
        }
    }
    else if (m_bookmark < bookmarks.size())
    {
        if (m_idleTimer.isActive())
            m_idleTimer.stop();

        auto& bookmark = bookmarks[m_bookmark];

        if (bookmark.pos != timestamp)
        {
            EASY_GLOBALS.has_local_changes = true;
            bookmark.pos = timestamp;
            update();
            emit moved();
        }

        return true;
    }

    return false;
}

bool BackgroundItem::mousePress(const QPointF& scenePos)
{
    m_bButtonPressed = !profiler_gui::is_max(m_bookmark) && contains(scenePos);

    delete m_tooltip;
    m_tooltip = nullptr;

    return false;
}

bool BackgroundItem::mouseRelease(const QPointF& scenePos)
{
    if (!m_bButtonPressed)
        return false;

    m_bButtonPressed = false;

    if (!profiler_gui::is_max(m_bookmark))
    {
        auto& bookmarks = EASY_GLOBALS.bookmarks;
        std::sort(bookmarks.begin(), bookmarks.end(),
                  [] (const profiler::Bookmark& lhs, const profiler::Bookmark& rhs)
        {
            return lhs.pos < rhs.pos;
        });

        mouseMove(scenePos);
        emit bookmarkChanged(m_bookmark);
    }

    return false;
}

bool BackgroundItem::mouseDoubleClick(const QPointF& scenePos)
{
    auto const sceneView = static_cast<BlocksGraphicsView*>(scene()->parent());
    if (!contains(scenePos))
        return false;

    if (!profiler_gui::is_max(m_bookmark))
    {
        qApp->restoreOverrideCursor();
        auto editor = new BookmarkEditor(m_bookmark, false, sceneView->parentWidget());
#ifndef _WIN32
        // Ugly workaround for Linux: without show-hide-show you can not drag the window
        editor->show();
        editor->hide();
#endif
        editor->exec();
    }
    else
    {
        auto& bookmarks = EASY_GLOBALS.bookmarks;

        profiler::Bookmark mark;
        mark.pos = PROF_FROM_MICROSECONDS(scenePos.x()) + EASY_GLOBALS.begin_time;
        mark.color = EASY_GLOBALS.bookmark_default_color == 0 ? profiler_gui::randomColor() : EASY_GLOBALS.bookmark_default_color;
        bookmarks.push_back(mark);

        std::sort(bookmarks.begin(), bookmarks.end(),
                  [] (const profiler::Bookmark& lhs, const profiler::Bookmark& rhs)
        {
            return lhs.pos < rhs.pos;
        });

        mouseMove(scenePos);

        if (!profiler_gui::is_max(m_bookmark))
        {
            qApp->restoreOverrideCursor();
            auto editor = new BookmarkEditor(m_bookmark, true, sceneView->parentWidget());
#ifndef _WIN32
            // Ugly workaround for Linux: without show-hide-show you can not drag the window
            editor->show();
            editor->hide();
#endif
            editor->exec();
        }
    }

    return true;
}

void BackgroundItem::mouseLeave()
{
    delete m_tooltip;
    m_tooltip = nullptr;

    if (m_idleTimer.isActive())
        m_idleTimer.stop();

    if (!profiler_gui::is_max(m_bookmark))
    {
        profiler_gui::set_max(m_bookmark);
        qApp->restoreOverrideCursor();
        emit bookmarkChanged(m_bookmark);
        update();
    }
}

bool BackgroundItem::contains(const QPointF& scenePos) const
{
    auto const sceneView = static_cast<BlocksGraphicsView*>(scene()->parent());
    const auto visibleSceneRect = sceneView->visibleSceneRect();
    const auto y = scenePos.y() - this->y() - visibleSceneRect.y();
    return y >= visibleSceneRect.height();
}

void BackgroundItem::onWindowActivationChanged(bool isActiveWindow)
{
    if (!isActiveWindow && qApp->activeWindow() != m_tooltip)
    {
        delete m_tooltip;
        m_tooltip = nullptr;
    }
}

void BackgroundItem::onIdleTimeout()
{
    auto parent = static_cast<QWidget*>(scene()->parent());

    delete m_tooltip;
    m_tooltip = nullptr;

    if (m_bookmark < EASY_GLOBALS.bookmarks.size() && parent->window()->isActiveWindow())
    {
        const auto& text = EASY_GLOBALS.bookmarks[m_bookmark].text;
        if (text.empty())
            return;

        m_tooltip = new QLabel(QString::fromStdString(text),
                               parent, Qt::ToolTip | Qt::WindowTransparentForInput);

        if (m_tooltip == nullptr)
            return;

        const auto delta = px(10);

        m_tooltip->setObjectName(QStringLiteral("BookmarkPopup"));
        m_tooltip->setAttribute(Qt::WA_ShowWithoutActivating, true);
        m_tooltip->setFocusPolicy(Qt::NoFocus);
        m_tooltip->setWordWrap(true);
        m_tooltip->move(QCursor::pos() + QPoint(delta >> 1, delta));
        m_tooltip->show();

        const int bottom = m_tooltip->mapToParent(m_tooltip->pos()).y() + m_tooltip->height();
        const int parentBottom = parent->y() + parent->height();
        if (bottom > parentBottom)
            m_tooltip->move(m_tooltip->pos() - QPoint(delta >> 1, m_tooltip->height() + delta));
    }
}

//////////////////////////////////////////////////////////////////////////

ForegroundItem::ForegroundItem() : AuxItem()
    , m_bookmark(profiler_gui::numeric_max<decltype(m_bookmark)>())
{

}

void ForegroundItem::paint(QPainter* _painter, const QStyleOptionGraphicsItem*, QWidget*)
{
    const auto sceneView = static_cast<const BlocksGraphicsView*>(scene()->parent());
    const auto visibleSceneRect = sceneView->visibleSceneRect();
    const auto step = sceneView->timelineStep() * sceneView->scale();
    const QString text = profiler_gui::autoTimeStringInt(units2microseconds(sceneView->timelineStep())); // Displayed text

    const auto px6  = px(6);
    const auto px10 = px(10);
    const auto px14 = px(14);

    // Draw scale indicator
    _painter->save();
    _painter->setTransform(QTransform::fromTranslate(-x(), -y()));
    //_painter->setCompositionMode(QPainter::CompositionMode_Difference);
    _painter->setBrush(Qt::NoBrush);

    QPen pen(Qt::black);
    pen.setWidth(px(3));
    _painter->setPen(pen);

    _painter->drawLine(QLineF(visibleSceneRect.width() - px(9) - step, visibleSceneRect.height() - px10, visibleSceneRect.width() - px(11), visibleSceneRect.height() - px10));

    _painter->setPen(Qt::black);
    _painter->drawLine(QLineF(visibleSceneRect.width() - px10 - step, visibleSceneRect.height() - px6, visibleSceneRect.width() - px10 - step, visibleSceneRect.height() - px14));
    _painter->drawLine(QLineF(visibleSceneRect.width() - px10, visibleSceneRect.height() - px6, visibleSceneRect.width() - px10, visibleSceneRect.height() - px14));

    _painter->setFont(EASY_GLOBALS.font.background);
    _painter->drawText(QRectF(visibleSceneRect.width() - px10 - step, visibleSceneRect.height() - px(63), step, px(50)),
                       Qt::AlignRight | Qt::AlignBottom | Qt::TextDontClip, text);

    if (m_bookmark < EASY_GLOBALS.bookmarks.size())
    {
        const auto& bookmark = EASY_GLOBALS.bookmarks[m_bookmark];
        const qreal pos =
            (PROF_MICROSECONDS(bookmark.pos - EASY_GLOBALS.begin_time) - sceneView->offset()) * sceneView->scale();

        pen.setWidth(px(1));
        _painter->setPen(pen);
        _painter->drawLine(QPointF(pos, 0), QPointF(pos, visibleSceneRect.height()));
    }

    _painter->restore();
}

void ForegroundItem::onBookmarkChanged(size_t index)
{
    m_bookmark = index;
    update();
}

void ForegroundItem::onMoved()
{
    update();
}

//////////////////////////////////////////////////////////////////////////

BlocksGraphicsView::BlocksGraphicsView(QWidget* _parent)
    : Parent(_parent)
    , m_beginTime(profiler_gui::numeric_max<decltype(m_beginTime)>())
    , m_sceneWidth(0)
    , m_scale(1)
    , m_offset(0)
    , m_visibleRegionWidth(0)
    , m_timelineStep(0)
    , m_idleTime(0)
    , m_mouseButtons(Qt::NoButton)
    , m_pScrollbar(nullptr)
    , m_selectionItem(nullptr)
    , m_rulerItem(nullptr)
    , m_backgroundItem(nullptr)
    , m_popupWidget(nullptr)
    , m_flickerSpeedX(0)
    , m_flickerSpeedY(0)
    , m_flickerCounterX(0)
    , m_flickerCounterY(0)
    , m_bDoubleClick(false)
    , m_bUpdatingRect(false)
    , m_bEmpty(true)
    , m_isArbitraryValueTooltip(false)
    , m_bHovered(false)
{
    initMode();
    setScene(new QGraphicsScene(this));
    updateVisibleSceneRect();
}

BlocksGraphicsView::~BlocksGraphicsView()
{
    removePopup();
}

//////////////////////////////////////////////////////////////////////////

void BlocksGraphicsView::onWindowActivationChanged()
{
    const bool isActive = window()->isActiveWindow();

    if (!isActive && qApp->activeWindow() != m_popupWidget)
    {
        removePopup();
    }

    if (m_backgroundItem != nullptr)
    {
        m_backgroundItem->onWindowActivationChanged(isActive);
    }
}

void BlocksGraphicsView::repaintHistogramImage()
{
    if (m_pScrollbar != nullptr)
    {
        m_pScrollbar->repaintHistogramImage();
    }
}

void BlocksGraphicsView::removePopup()
{
    delete m_popupWidget;
    m_popupWidget = nullptr;
    m_isArbitraryValueTooltip = false;
}

bool BlocksGraphicsView::needToIgnoreMouseEvent() const
{
    if (!m_isArbitraryValueTooltip)
        return false;

    const int size = std::min(m_popupWidget->width(), m_popupWidget->height()) >> 2;
    const auto rect = m_popupWidget->rect().adjusted(-size, -size, size, size);

    return rect.contains(m_popupWidget->mapFromGlobal(QCursor::pos()));
}

//////////////////////////////////////////////////////////////////////////

qreal BlocksGraphicsView::sceneWidth() const
{
    return m_sceneWidth;
}

qreal BlocksGraphicsView::chronoTime() const
{
    return m_selectionItem->width();
}

qreal BlocksGraphicsView::chronoTimeAux() const
{
    return m_rulerItem->width();
}

//////////////////////////////////////////////////////////////////////////

GraphicsRulerItem* BlocksGraphicsView::createRuler(bool _main)
{
    auto ruler = new GraphicsRulerItem(_main);

    ruler->setColor(_main ? profiler_gui::RULER_COLOR : profiler_gui::RULER_COLOR2);
    ruler->setBoundingRect(sceneRect());
    ruler->hide();
    scene()->addItem(ruler);

    return ruler;
}

//////////////////////////////////////////////////////////////////////////

void BlocksGraphicsView::clear()
{
    const QSignalBlocker blocker(this), sceneBlocker(scene()); // block all scene signals (otherwise clear() would be extremely slow!)

    // Stop flicking
    m_flickerTimer.stop();
    m_flickerSpeedX = 0;
    m_flickerSpeedY = 0;
    m_flickerCounterX = 0;
    m_flickerCounterY = 0;

    // Clear all items
    removePopup();
    scene()->clear();
    m_items.clear();
    m_selectedBlocks.clear();
    m_backgroundItem = nullptr;

    profiler_gui::set_max(m_beginTime); // reset begin time
    m_scale = 1; // scale back to initial 100% scale
    m_timelineStep = 1;
    m_offset = 0; // scroll back to the beginning of the scene

    if (m_idleTimer.isActive())
        m_idleTimer.stop();
    m_idleTime = 0;

    // Reset necessary flags
    m_bEmpty = true;

    m_sceneWidth = 10;
    m_visibleRegionWidth = 10;
    setSceneRect(0, 0, 10, 10);

    auto& sceneData = EASY_GLOBALS.scene;
    sceneData.left = 0;
    sceneData.right = m_sceneWidth;
    sceneData.window = m_visibleRegionWidth;
    sceneData.offset = m_offset;
    sceneData.empty = true;

    // notify ProfTreeWidget that selection was reset
    emit intervalChanged(m_selectedBlocks, m_beginTime, 0, 0, false);

    EASY_GLOBALS.selected_thread = 0;
    emit EASY_GLOBALS.events.selectedThreadChanged(0);

    emit EASY_GLOBALS.events.rulerVisible(false);
}

void BlocksGraphicsView::notifySceneSizeChange()
{
    EASY_GLOBALS.scene.left = 0;
    EASY_GLOBALS.scene.right = m_sceneWidth;
    emit EASY_GLOBALS.events.sceneSizeChanged(0, m_sceneWidth);
}

void BlocksGraphicsView::notifyVisibleRegionSizeChange()
{
    auto vbar = verticalScrollBar();
    const int vbar_width = (vbar != nullptr && vbar->isVisible() ? vbar->width() + 2 : 0);
    notifyVisibleRegionSizeChange((m_visibleSceneRect.width() + vbar_width) / m_scale);
}

void BlocksGraphicsView::notifyVisibleRegionSizeChange(qreal _size)
{
    m_visibleRegionWidth = _size;
    EASY_GLOBALS.scene.window = _size;
    emit EASY_GLOBALS.events.sceneVisibleRegionSizeChanged(_size);
}

void BlocksGraphicsView::notifyVisibleRegionPosChange()
{
    EASY_GLOBALS.scene.offset = m_offset;
    emit EASY_GLOBALS.events.sceneVisibleRegionPosChanged(m_offset);
}

void BlocksGraphicsView::notifyVisibleRegionPosChange(qreal _pos)
{
    if (m_sceneWidth <= m_visibleRegionWidth)
        m_offset = 0;
    else
        m_offset = estd::clamp(0., _pos, m_sceneWidth - m_visibleRegionWidth);
    notifyVisibleRegionPosChange();
}

void BlocksGraphicsView::setTree(const profiler::thread_blocks_tree_t& _blocksTree)
{
    // clear scene
    clear();
    emit EASY_GLOBALS.events.sceneCleared();

    if (_blocksTree.empty())
        return;

    m_backgroundItem = new BackgroundItem();
    scene()->addItem(m_backgroundItem);

    // set new blocks tree
    // calculate scene size and fill it with items

    // Calculating start and end time
    profiler::timestamp_t finish = 0, busyTime = 0;
    profiler::thread_id_t longestTree = 0, mainTree = 0;
    for (const auto& threadTree : _blocksTree)
    {
        const auto& t = threadTree.second;

        auto timestart = m_beginTime;
        auto timefinish = finish;
        
        if (!t.children.empty())
            timestart = easyBlocksTree(t.children.front()).node->begin();
        if (!t.sync.empty())
            timestart = std::min(timestart, easyBlocksTree(t.sync.front()).node->begin());

        if (!t.children.empty())
            timefinish = easyBlocksTree(t.children.back()).node->end();
        if (!t.sync.empty())
            timefinish = std::max(timefinish, easyBlocksTree(t.sync.back()).node->end());

        if (m_beginTime > timestart)
            m_beginTime = timestart;

        if (finish < timefinish)
            finish = timefinish;

        if (t.profiled_time > busyTime) {
            busyTime = t.profiled_time;
            longestTree = threadTree.first;
        }

        if (mainTree == 0 && strcmp(t.name(), "Main") == 0)
            mainTree = threadTree.first;
    }

    const decltype(m_beginTime) additional_offset = (finish - m_beginTime) / 20; // Additional 5% before first block and after last block
    finish += additional_offset;
    m_beginTime -= std::min(m_beginTime, additional_offset);
    EASY_GLOBALS.begin_time = m_beginTime;

    // Sort threads by name
    std::vector<std::reference_wrapper<const profiler::BlocksTreeRoot> > sorted_roots;
    sorted_roots.reserve(_blocksTree.size());
    for (const auto& threadTree : _blocksTree)
        sorted_roots.emplace_back(threadTree.second);
    std::sort(sorted_roots.begin(), sorted_roots.end(), [](const profiler::BlocksTreeRoot& _a, const profiler::BlocksTreeRoot& _b) {
        return _a.thread_name < _b.thread_name;
    });

    const auto row_height = EASY_GLOBALS.size.graphics_row_height;
    const auto threads_spacing = EASY_GLOBALS.size.threads_row_spacing;

    // Filling scene with items
    m_items.reserve(_blocksTree.size());
    qreal y = EASY_GLOBALS.size.timeline_height;
    const GraphicsBlockItem *longestItem = nullptr, *mainThreadItem = nullptr;
    for (const profiler::BlocksTreeRoot& t : sorted_roots)
    {
        if (m_items.size() == 0xff)
        {
            qWarning() << "Warning: Maximum threads number (255 threads) exceeded! See BlocksGraphicsView::setTree() : " << __LINE__ << " in file " << __FILE__;
            break;
        }

        // fill scene with new items
        qreal h = 0, x = 0;
        
        if (!t.children.empty())
            x = time2position(easyBlocksTree(t.children.front()).node->begin());
        else if (!t.sync.empty())
            x = time2position(easyBlocksTree(t.sync.front()).node->begin());

        auto item = new GraphicsBlockItem(static_cast<uint8_t>(m_items.size()), t);
        if (t.depth)
            item->setLevels(t.depth);
        item->setPos(0, y);

        qreal children_duration = 0;

        if (!t.children.empty())
        {
            uint32_t dummy = 0;
            children_duration = setTree(item, t.children, h, dummy, y, 0);
        }
        else
        {
            if (!t.sync.empty())
                children_duration = time2position(easyBlocksTree(t.sync.back()).node->end()) - x;
            h = EASY_GLOBALS.size.graphics_row_height;
        }

        item->setBoundingRect(0, 0, children_duration + x, h);
        m_items.push_back(item);
        scene()->addItem(item);

        y += h + EASY_GLOBALS.size.threads_row_spacing;

        if (longestTree == t.thread_id)
            longestItem = item;

        if (mainTree == t.thread_id)
            mainThreadItem = item;
    }

    // Calculating scene rect
    m_sceneWidth = time2position(finish);
    setSceneRect(0, 0, m_sceneWidth, y + EASY_GLOBALS.size.timeline_height);
    EASY_GLOBALS.scene.empty = false;

    // Center view on the beginning of the scene
    const int vbar_width = updateVisibleSceneRect();
    const auto windowWidth = (m_visibleSceneRect.width() + vbar_width) / m_scale;

    notifySceneSizeChange();
    notifyVisibleRegionSizeChange(windowWidth);

    // Create new chronometer item (previous item was destroyed by scene on scene()->clear()).
    // It will be shown on mouse right button click.
    m_rulerItem = createRuler(false);
    m_selectionItem = createRuler(true);

    m_backgroundItem->setBoundingRect(0, 0, m_sceneWidth, y + EASY_GLOBALS.size.timeline_height);
    auto indicator = new ForegroundItem();
    indicator->setBoundingRect(0, 0, m_sceneWidth, y);
    connect(m_backgroundItem, &BackgroundItem::bookmarkChanged, indicator, &ForegroundItem::onBookmarkChanged);
    connect(m_backgroundItem, &BackgroundItem::moved, indicator, &ForegroundItem::onMoved);
    scene()->addItem(indicator);

    // Setting flags
    m_bEmpty = false;

    scaleTo(BASE_SCALE);


    emit treeChanged();

    if (mainThreadItem != nullptr)
        longestItem = mainThreadItem;

    if (longestItem != nullptr)
    {
        EASY_GLOBALS.selected_thread = longestItem->threadId();
        emit EASY_GLOBALS.events.selectedThreadChanged(longestItem->threadId());

        scrollTo(longestItem);
        m_pScrollbar->setHistogramSource(longestItem->threadId(), longestItem->items(0));

        if (!longestItem->items(0).empty())
        {
            notifyVisibleRegionPosChange(longestItem->items(0).front().left() - m_visibleRegionWidth * 0.25);

            // Scale to fit all items
            const auto right = longestItem->items(0).back().right() - m_offset;
            const auto currentScale = m_scale + std::numeric_limits<decltype(m_scale)>::epsilon();

            auto scale = m_scale;
            while (scale < MAX_SCALE && right < (m_visibleSceneRect.width() + vbar_width) / scale)
            {
                m_scale = scale;
                scale *= profiler_gui::SCALING_COEFFICIENT;
            }

            if (currentScale < m_scale)
                scaleTo(m_scale);
        }
    }

    if (m_bHovered && !m_idleTimer.isActive())
        m_idleTimer.start();

    // Workaround for valid scene painting after setting a new tree
    QTimer::singleShot(0, this, &This::revalidateOffset);
}

void BlocksGraphicsView::revalidateOffset()
{
    notifyVisibleRegionPosChange(m_offset);
    repaintScene();
}

const BlocksGraphicsView::Items &BlocksGraphicsView::getItems() const
{
    return m_items;
}

bool BlocksGraphicsView::getSelectionRegionForSaving(profiler::timestamp_t& _beginTime, profiler::timestamp_t& _endTime) const
{
    if (m_bEmpty)
        return false;

    if (!m_selectionItem->isVisible() && !m_rulerItem->isVisible())
        return false;

    decltype(m_selectionItem) ruler = m_selectionItem->isVisible() ? m_selectionItem : m_rulerItem;
    _beginTime = m_beginTime + position2time(ruler->left());
    _endTime = m_beginTime + position2time(ruler->right());

    return true;
}

qreal BlocksGraphicsView::setTree(GraphicsBlockItem* _item, const profiler::BlocksTree::children_t& _children, qreal& _height, uint32_t& _maxDepthChild, qreal _y, short _level)
{
    if (_children.empty())
    {
        return 0;
    }

    const auto level = static_cast<uint8_t>(_level);
    const auto n = static_cast<unsigned int>(_children.size());
    _item->reserve(level, n);

    _maxDepthChild = 0;
    uint8_t maxDepth = 0;
    const short next_level = _level + 1;
    bool warned = false;
    qreal total_duration = 0, prev_end = 0, maxh = 0;
    qreal start_time = -1;
    uint32_t j = 0;
    for (auto child_index : _children)
    {
        auto& gui_block = easyBlock(child_index);
        const auto& child = gui_block.tree;
        if (child.depth > maxDepth)
        {
            maxDepth = child.depth;
            _maxDepthChild = j;
        }

        auto xbegin = time2position(child.node->begin());
        if (start_time < 0)
        {
            start_time = xbegin;
        }

        auto duration = time2position(child.node->end()) - xbegin;

        //const auto dt = xbegin - prev_end;
        //if (dt < 0)
        //{
        //    duration += dt;
        //    xbegin -= dt;
        //}

        //static const qreal MIN_DURATION = 0.25;
        //if (duration < MIN_DURATION)
        //    duration = MIN_DURATION;

        const auto i = _item->addItem(level);
        auto& b = _item->getItem(level, i);

        gui_block.graphics_item = _item->index();
        gui_block.graphics_item_level = level;
        gui_block.graphics_item_index = i;

        if (next_level < 256 && next_level < _item->levels() && !child.children.empty())
        {
            b.children_begin = static_cast<unsigned int>(_item->items(static_cast<uint8_t>(next_level)).size());
        }
        else
        {
            profiler_gui::set_max(b.children_begin);
        }

        qreal h = 0;
        qreal children_duration = 0;
        uint32_t maxDepthChild = 0;

        if (next_level < 256)
        {
            children_duration = setTree(_item, child.children, h, maxDepthChild,
                                        _y + EASY_GLOBALS.size.graphics_row_full, next_level);
        }
        else if (!child.children.empty() && !warned)
        {
            warned = true;
            qWarning() << "Warning: Maximum blocks depth (255) exceeded! See BlocksGraphicsView::setTree() : " << __LINE__ << " in file " << __FILE__;
        }

        if (duration < children_duration)
        {
            duration = children_duration;
        }

        if (h > maxh)
        {
            maxh = h;
        }

        b.block = child_index;// &child;

#ifndef EASY_GRAPHICS_ITEM_RECURSIVE_PAINT
        b.neighbours = n;
        b.state = j > 0 || level == 0 ? 0 : -1;
#else
        b.max_depth_child = maxDepthChild;
#endif

        b.setPos(xbegin, duration);
        //b.totalHeight = profiler_gui::GRAPHICS_ROW_SIZE + h;

        prev_end = xbegin + duration;
        total_duration = prev_end - start_time;

        ++j;
    }

    _height += EASY_GLOBALS.size.graphics_row_full + maxh;

    return total_duration;
}

//////////////////////////////////////////////////////////////////////////

void BlocksGraphicsView::setScrollbar(GraphicsScrollbar* _scrollbar)
{
    disconnect(&EASY_GLOBALS.events, &profiler_gui::GlobalSignals::chartSliderChanged, this, &This::onGraphicsScrollbarValueChange);
    disconnect(&EASY_GLOBALS.events, &profiler_gui::GlobalSignals::chartWheeled, this, &This::onGraphicsScrollbarWheel);

    m_pScrollbar = _scrollbar;
    m_pScrollbar->clear();
    m_pScrollbar->setRange(0, m_sceneWidth);

    auto vbar = verticalScrollBar();
    const int vbar_width = (vbar != nullptr && vbar->isVisible() ? vbar->width() + 2 : 0);
    m_pScrollbar->setSliderWidth(m_visibleSceneRect.width() + vbar_width);

    connect(&EASY_GLOBALS.events, &profiler_gui::GlobalSignals::chartSliderChanged, this, &This::onGraphicsScrollbarValueChange);
    connect(&EASY_GLOBALS.events, &profiler_gui::GlobalSignals::chartWheeled, this, &This::onGraphicsScrollbarWheel);

    EASY_GLOBALS.selected_thread = 0;
    emit EASY_GLOBALS.events.selectedThreadChanged(0);
}

//////////////////////////////////////////////////////////////////////////

int BlocksGraphicsView::updateVisibleSceneRect()
{
    m_visibleSceneRect = mapToScene(rect()).boundingRect();

    auto vbar = verticalScrollBar();
    int vbar_width = 0;
    if (vbar != nullptr && vbar->isVisible())
        vbar_width = vbar->width() + 2;

    m_visibleSceneRect.setWidth(m_visibleSceneRect.width() - vbar_width);
    m_visibleSceneRect.setHeight(m_visibleSceneRect.height() - EASY_GLOBALS.size.timeline_height);

    return vbar_width;
}

void BlocksGraphicsView::updateTimelineStep(qreal _windowWidth)
{
    const auto time = units2microseconds(_windowWidth);
    if (time < 100)
        m_timelineStep = 1e-2;
    else if (time < 10e3)
        m_timelineStep = 1;
    else if (time < 10e6)
        m_timelineStep = 1e3;
    else
        m_timelineStep = 1e6;

    const auto optimal_steps = static_cast<int>(40 * m_visibleSceneRect.width() / 1500);
    auto steps = time / m_timelineStep;
    while (steps > optimal_steps) {
        m_timelineStep *= 10;
        steps *= 0.1;
    }

    m_timelineStep = microseconds2units(m_timelineStep);
}

void BlocksGraphicsView::repaintScene()
{
    scene()->update(m_visibleSceneRect);
    emit sceneUpdated();
}

//////////////////////////////////////////////////////////////////////////

void BlocksGraphicsView::scaleTo(qreal _scale)
{
    if (m_bEmpty)
    {
        return;
    }

    // have to limit scale because of Qt's QPainter feature: it doesn't draw text
    // with very big coordinates (but it draw rectangles with the same coordinates good).
    m_scale = clamp(MIN_SCALE, _scale, MAX_SCALE); 
    const int vbar_width = updateVisibleSceneRect();

    // Update slider width for scrollbar
    const auto windowWidth = (m_visibleSceneRect.width() + vbar_width) / m_scale;
    notifyVisibleRegionSizeChange(windowWidth);

    updateTimelineStep(windowWidth);
}

//////////////////////////////////////////////////////////////////////////

void BlocksGraphicsView::enterEvent(QEvent* _event)
{
    Parent::enterEvent(_event);
    m_bHovered = true;
    if (!m_bEmpty && !m_idleTimer.isActive())
        m_idleTimer.start();
    m_idleTime = 0;
}

void BlocksGraphicsView::leaveEvent(QEvent* _event)
{
    Parent::leaveEvent(_event);

    m_bHovered = false;
    if (m_idleTimer.isActive())
        m_idleTimer.stop();
    m_idleTime = 0;

    if (!needToIgnoreMouseEvent())
        removePopup();

    if (m_backgroundItem != nullptr)
        m_backgroundItem->mouseLeave();
}

//////////////////////////////////////////////////////////////////////////

void BlocksGraphicsView::wheelEvent(QWheelEvent* _event)
{
    if (needToIgnoreMouseEvent())
        return;

    m_idleTime = 0;

    if (!m_bEmpty)
        onWheel(mapToDiagram(mapToScene(_event->pos()).x()), _event->delta());

    _event->accept();
}

void BlocksGraphicsView::onGraphicsScrollbarWheel(qreal _scenePos, int _wheelDelta)
{
    m_idleTime = 0;

    for (auto item : m_items)
    {
        if (item->threadId() == EASY_GLOBALS.selected_thread)
        {
            scrollTo(item);
            break;
        }
    }

    onWheel(_scenePos, _wheelDelta);
}

void BlocksGraphicsView::scrollTo(const GraphicsBlockItem* _item)
{
    m_bUpdatingRect = true;
    auto vbar = verticalScrollBar();
    vbar->setValue(static_cast<int>(_item->y() + (_item->boundingRect().height() - vbar->pageStep()) * 0.5));
    m_bUpdatingRect = false;
}

qreal BlocksGraphicsView::mapToDiagram(qreal x) const
{
    return m_offset + x / m_scale;
}

void BlocksGraphicsView::onWheel(qreal _scenePos, int _wheelDelta)
{
    const decltype(m_scale) scaleCoeff = _wheelDelta > 0 ? profiler_gui::SCALING_COEFFICIENT : profiler_gui::SCALING_COEFFICIENT_INV;

    // Remember current mouse position
    _scenePos = clamp(0., _scenePos, m_sceneWidth);
    const auto initialPosition = _scenePos;

    // have to limit scale because of Qt's QPainter feature: it doesn't draw text
    // with very big coordinates (but it draw rectangles with the same coordinates good).
    _scenePos -= m_offset;
    _scenePos *= m_scale;
    m_scale = clamp(MIN_SCALE, m_scale * scaleCoeff, MAX_SCALE);

    //updateVisibleSceneRect(); // Update scene rect

    // Update slider width for scrollbar
    notifyVisibleRegionSizeChange();

    // Calculate new offset to simulate QGraphicsView::AnchorUnderMouse scaling behavior
    if (m_sceneWidth <= m_visibleRegionWidth)
        m_offset = 0;
    else
        m_offset = clamp(0., initialPosition - _scenePos / m_scale, m_sceneWidth - m_visibleRegionWidth);

    // Update slider position
    profiler_gui::BoolFlagGuard guard(m_bUpdatingRect, true); // To be sure that updateVisibleSceneRect will not be called by scrollbar change
    notifyVisibleRegionPosChange();
    guard.restore();

    updateVisibleSceneRect(); // Update scene rect
    updateTimelineStep(m_visibleRegionWidth);
    repaintScene(); // repaint scene
}

//////////////////////////////////////////////////////////////////////////

void BlocksGraphicsView::mousePressEvent(QMouseEvent* _event)
{
    if (needToIgnoreMouseEvent())
        return;

    m_idleTime = 0;

    if (m_bEmpty)
    {
        _event->accept();
        return;
    }

    auto scenePos = mapToScene(_event->pos());
    scenePos.setX(m_offset + scenePos.x() / m_scale);
    if (m_backgroundItem->mousePress(scenePos))
    {
        _event->accept();
        return;
    }

    m_mouseButtons = _event->buttons();
    m_mousePressPos = _event->pos();

    if (m_mouseButtons & Qt::LeftButton)
    {
        if (m_rulerItem->isVisible() && (m_rulerItem->hoverLeft() || m_rulerItem->hoverRight()))
        {
            m_rulerItem->setReverse(m_rulerItem->hoverLeft());
            m_bDoubleClick = true;
        }
        else if (m_selectionItem->isVisible() && (m_selectionItem->hoverLeft() || m_selectionItem->hoverRight()))
        {
            m_selectionItem->setReverse(m_selectionItem->hoverLeft());
            m_mouseButtons = Qt::RightButton;
            return;
        }
    }

    if (m_mouseButtons & Qt::RightButton)
    {
        if (m_selectionItem->isVisible() && (m_selectionItem->hoverLeft() || m_selectionItem->hoverRight()))
        {
            m_selectionItem->setReverse(m_selectionItem->hoverLeft());
        }
        else
        {
            const auto mouseX = scenePos.x();
            m_selectionItem->setLeftRight(mouseX, mouseX);
            m_selectionItem->hide();
            m_pScrollbar->hideSelectionIndicator();
            emit EASY_GLOBALS.events.rulerVisible(m_rulerItem->isVisible());
        }
    }

    _event->accept();
}

void BlocksGraphicsView::mouseDoubleClickEvent(QMouseEvent* _event)
{
    if (needToIgnoreMouseEvent())
        return;

    m_idleTime = 0;

    if (m_bEmpty)
    {
        _event->accept();
        return;
    }

    auto scenePos = mapToScene(_event->pos());
    scenePos.setX(m_offset + scenePos.x() / m_scale);
    if (m_backgroundItem->mouseDoubleClick(scenePos))
    {
        _event->accept();
        return;
    }

    m_mouseButtons = _event->buttons();
    m_mousePressPos = _event->pos();
    m_bDoubleClick = true;

    if (m_mouseButtons & Qt::LeftButton)
    {
        const auto mouseX = scenePos.x();
        m_rulerItem->setLeftRight(mouseX, mouseX);
        m_rulerItem->hide();
        emit sceneUpdated();
        emit EASY_GLOBALS.events.rulerVisible(m_selectionItem->isVisible());
    }

    _event->accept();
}

//////////////////////////////////////////////////////////////////////////

void BlocksGraphicsView::mouseReleaseEvent(QMouseEvent* _event)
{
    if (needToIgnoreMouseEvent())
        return;

    if (m_bEmpty)
    {
        _event->accept();
        return;
    }

    auto scenePos = mapToScene(_event->pos());
    scenePos.setX(m_offset + scenePos.x() / m_scale);
    if (m_backgroundItem->mouseRelease(scenePos))
    {
        _event->accept();
        return;
    }

    bool chronoHidden = false;
    bool changedSelection = false, changedSelectedItem = false;
    if (m_mouseButtons & Qt::RightButton)
    {
        if (m_selectionItem->isVisible() && m_selectionItem->width() < 1e-6)
        {
            m_selectionItem->hide();
            m_pScrollbar->hideSelectionIndicator();
            emit EASY_GLOBALS.events.rulerVisible(m_rulerItem->isVisible());
        }

        if (!m_selectedBlocks.empty())
        {
            changedSelection = true;
            m_selectedBlocks.clear();
        }

        if (m_selectionItem->isVisible())
        {
            //printf("INTERVAL: {%lf, %lf} ms\n", m_selectionItem->left(), m_selectionItem->right());

            for (auto item : m_items)
            {
                if (!EASY_GLOBALS.only_current_thread_hierarchy || item->threadId() == EASY_GLOBALS.selected_thread)
                    item->getBlocks(m_selectionItem->left(), m_selectionItem->right(), m_selectedBlocks);
            }

            if (!m_selectedBlocks.empty())
            {
                changedSelection = true;
            }
        }
    }

    const profiler_gui::EasyBlock* selectedBlock = nullptr;
    profiler::thread_id_t selectedBlockThread = 0;
    bool jumpToZone = false;
    bool changedSelectionBySelectingItem = false;
    const bool leftClickSelect = (m_mouseButtons & Qt::LeftButton) != 0;
    if (leftClickSelect)
    {
        if (m_rulerItem->isVisible() && m_rulerItem->width() < 1e-6)
        {
            chronoHidden = true;
            m_rulerItem->hide();
            emit EASY_GLOBALS.events.rulerVisible(m_selectionItem->isVisible());
        }
        else if (m_selectionItem->isVisible() && m_selectionItem->hoverIndicator())
        {
            // Jump to selected zone
            jumpToZone = true;
            m_flickerSpeedX = m_flickerSpeedY = 0;
            notifyVisibleRegionPosChange(m_selectionItem->left() + (m_selectionItem->width() - m_visibleRegionWidth) * 0.5);
        }
    }

    const bool rightClickSelect = ((m_mouseButtons & Qt::RightButton) != 0 && !changedSelection);
    if ((leftClickSelect || rightClickSelect) && !jumpToZone && m_mouseMovePath.manhattanLength() < 5 && !m_backgroundItem->contains(scenePos))
    {
        // Handle Click

        //clicked = true;
        auto mouseClickPos = mapToScene(m_mousePressPos);
        if (mouseClickPos.x() >= 0)
        {
            mouseClickPos.setX(m_offset + mouseClickPos.x() / m_scale);

            // Try to select one of item blocks
            for (auto item : m_items)
            {
                profiler::block_index_t i = ~0U;
                auto block = item->intersect(mouseClickPos, i);
                if (block != nullptr)
                {
                    changedSelectedItem = true;
                    selectedBlock = block;
                    selectedBlockThread = item->threadId();
                    EASY_GLOBALS.selected_block = i;
                    EASY_GLOBALS.selected_block_id = block->tree.node->id();
                    break;
                }
            }

            if (!changedSelectedItem && !profiler_gui::is_max(EASY_GLOBALS.selected_block))
            {
                changedSelectedItem = true;
                profiler_gui::set_max(EASY_GLOBALS.selected_block);
                profiler_gui::set_max(EASY_GLOBALS.selected_block_id);
            }
        }

        if (selectedBlock != nullptr && rightClickSelect)
        {
            if (!m_selectedBlocks.empty())
            {
                changedSelection = true;
                m_selectedBlocks.clear();
            }

            const auto thread_item = m_items[selectedBlock->graphics_item];
            const auto& selectedItem = thread_item->items(selectedBlock->graphics_item_level)[selectedBlock->graphics_item_index];
            const auto left = selectedItem.left();
            const auto right = selectedItem.right();

            for (auto item : m_items)
            {
                if (!EASY_GLOBALS.only_current_thread_hierarchy || (EASY_GLOBALS.selecting_block_changes_thread && selectedBlockThread == item->threadId()))
                {
                    item->getBlocks(left, right, m_selectedBlocks);
                }
            }

            if (!m_selectedBlocks.empty())
            {
                changedSelection = true;
                m_selectionItem->setLeftRight(left, right);
                m_selectionItem->setReverse(true);
                m_selectionItem->setStrict(true);
                m_selectionItem->show();

                m_pScrollbar->setSelectionPos(left, right);
                m_pScrollbar->showSelectionIndicator();

                emit EASY_GLOBALS.events.rulerVisible(true);
            }

            changedSelectionBySelectingItem = changedSelection;
        }
    }

    const bool isDoubleClick = m_bDoubleClick;
    m_bDoubleClick = false;
    m_mouseButtons = _event->buttons();
    m_mouseMovePath = QPoint();
    _event->accept();

    if (changedSelection)
    {
        profiler::timestamp_t left=0, right=0;
        if (changedSelectionBySelectingItem)
        {
            left = selectedBlock->tree.node->begin() - m_beginTime;
            right = selectedBlock->tree.node->end() - m_beginTime;
        }
        else
        {
            left = position2time(m_selectionItem->left());
            right = position2time(m_selectionItem->right());
        }
        emit intervalChanged(m_selectedBlocks, m_beginTime, left, right, m_selectionItem->strict());
    }

    if (changedSelectedItem)
    {
        profiler_gui::BoolFlagGuard guard(m_bUpdatingRect, true);

        if (EASY_GLOBALS.selecting_block_changes_thread && selectedBlock != nullptr && EASY_GLOBALS.selected_thread != selectedBlockThread)
        {
            EASY_GLOBALS.selected_thread = selectedBlockThread;

            emit EASY_GLOBALS.events.lockCharts();
            emit EASY_GLOBALS.events.selectedThreadChanged(EASY_GLOBALS.selected_thread);
            emit EASY_GLOBALS.events.unlockCharts();
        }

        emit EASY_GLOBALS.events.selectedBlockChanged(EASY_GLOBALS.selected_block);

        if (selectedBlock != nullptr && isDoubleClick)
        {
            if (!selectedBlock->tree.children.empty())
            {
                auto& selected = EASY_GLOBALS.gui_blocks[EASY_GLOBALS.selected_block];
                selected.expanded = !selected.expanded;
                emit EASY_GLOBALS.events.itemsExpandStateChanged();
            }
            else if (easyDescriptor(selectedBlock->tree.node->id()).type() == profiler::BlockType::Value)
            {
                emit EASY_GLOBALS.events.selectValue(selectedBlockThread, EASY_GLOBALS.selected_block, *selectedBlock->tree.value);
            }
        }

        guard.restore();

        if (selectedBlock != nullptr && selectedBlockThread == EASY_GLOBALS.selected_thread)
            m_pScrollbar->setHistogramSource(EASY_GLOBALS.selected_thread, EASY_GLOBALS.selected_block_id);
        else
        {
            for (auto item : m_items)
            {
                if (item->threadId() == EASY_GLOBALS.selected_thread)
                {
                    m_pScrollbar->setHistogramSource(EASY_GLOBALS.selected_thread, item->items(0));
                    break;
                }
            }
        }

        repaintScene();
    }
    else if (jumpToZone)
    {
        repaintScene();
    }
    else if (chronoHidden)
    {
        emit sceneUpdated();
    }
}

//////////////////////////////////////////////////////////////////////////

void BlocksGraphicsView::addSelectionToStatsTree()
{
    if (!m_selectionItem->isVisible())
        return;

    //printf("INTERVAL: {%lf, %lf} ms\n", m_selectionItem->left(), m_selectionItem->right());

    bool changedSelection = false;

    if (!m_selectedBlocks.empty())
    {
        changedSelection = true;
        m_selectedBlocks.clear();
    }

    for (auto item : m_items)
    {
        if (!EASY_GLOBALS.only_current_thread_hierarchy || item->threadId() == EASY_GLOBALS.selected_thread)
            item->getBlocks(m_selectionItem->left(), m_selectionItem->right(), m_selectedBlocks);
    }

    if (!m_selectedBlocks.empty())
    {
        changedSelection = true;
    }

    if (changedSelection)
    {
        emit intervalChanged(
            m_selectedBlocks,
            m_beginTime,
            position2time(m_selectionItem->left()),
            position2time(m_selectionItem->right()),
            m_selectionItem->strict()
        );
    }
}

void BlocksGraphicsView::onZoomSelection()
{
    if (m_selectionItem->width() < 1e-6)
        return;

    auto deltaScale = m_visibleRegionWidth / m_selectionItem->width();

    if (fabs(deltaScale - 1) < 1e-6)
    {
        // Restore scale value multiple to SCALING_COEFFICIENT
        const int steps = static_cast<int>(log(m_scale / MIN_SCALE) / log(profiler_gui::SCALING_COEFFICIENT));
        const auto desiredScale = MIN_SCALE * pow(profiler_gui::SCALING_COEFFICIENT, steps);
        deltaScale = desiredScale / m_scale;
    }

    m_offset = m_selectionItem->left() + (m_selectionItem->width() - m_visibleRegionWidth / deltaScale) * 0.5;
    m_scale = clamp(MIN_SCALE, m_scale * deltaScale, MAX_SCALE);

    // Update slider width for scrollbar
    notifyVisibleRegionSizeChange();

    // Update slider position
    profiler_gui::BoolFlagGuard guard(m_bUpdatingRect, true); // To be sure that updateVisibleSceneRect will not be called by scrollbar change
    notifyVisibleRegionPosChange();
    guard.restore();

    updateVisibleSceneRect(); // Update scene rect
    updateTimelineStep(m_visibleRegionWidth);
    repaintScene(); // repaint scene
}

void BlocksGraphicsView::onInspectCurrentView(bool strict)
{
    if (m_bEmpty)
        return;

    if (!m_selectionItem->isVisible())
    {
        m_selectionItem->setReverse(strict);
        m_selectionItem->setStrict(strict);
        m_selectionItem->setLeftRight(m_offset, m_offset + m_visibleRegionWidth);
        m_selectionItem->show();
        m_pScrollbar->setSelectionPos(m_selectionItem->left(), m_selectionItem->right());
        m_pScrollbar->showSelectionIndicator();

        emit EASY_GLOBALS.events.rulerVisible(true);

        addSelectionToStatsTree();
    }
    else
    {
        onZoomSelection();
    }
}

//////////////////////////////////////////////////////////////////////////

bool BlocksGraphicsView::moveChrono(GraphicsRulerItem* ruler_item, qreal mouse_x)
{
    if (ruler_item->reverse())
    {
        if (mouse_x > ruler_item->right())
        {
            ruler_item->setReverse(false);
            ruler_item->setLeftRight(ruler_item->right(), mouse_x);

            if (ruler_item->hoverLeft())
            {
                ruler_item->setHoverLeft(false);
                ruler_item->setHoverRight(true);
            }
        }
        else
        {
            ruler_item->setLeftRight(mouse_x, ruler_item->right());
        }
    }
    else
    {
        if (mouse_x < ruler_item->left())
        {
            ruler_item->setReverse(true);
            ruler_item->setLeftRight(mouse_x, ruler_item->left());

            if (ruler_item->hoverRight())
            {
                ruler_item->setHoverLeft(true);
                ruler_item->setHoverRight(false);
            }
        }
        else
        {
            ruler_item->setLeftRight(ruler_item->left(), mouse_x);
        }
    }

    if (!ruler_item->hoverAnyBorder())
    {
        ruler_item->setStrict(ruler_item->reverse());
    }

    if (!ruler_item->isVisible() && ruler_item->width() > 1e-6)
    {
        ruler_item->show();
        emit EASY_GLOBALS.events.rulerVisible(true);
        return true;
    }

    return false;
}

void BlocksGraphicsView::mouseMoveEvent(QMouseEvent* event)
{
    if (needToIgnoreMouseEvent())
        return;

    m_idleTime = 0;

    if (m_bEmpty)
    {
        event->accept();
        return;
    }

    auto scenePos = mapToScene(event->pos());
    scenePos.setX(m_offset + scenePos.x() / m_scale);
    if (m_backgroundItem->mouseMove(scenePos))
    {
        event->accept();
        return;
    }

    if (m_mouseButtons == 0 && !m_selectionItem->isVisible() && !m_rulerItem->isVisible())
    {
        event->accept();
        return;
    }

    bool needUpdate = false;
    const auto pos = event->pos();
    const auto delta = pos - m_mousePressPos;
    m_mousePressPos = pos;

    if (m_mouseButtons != 0)
    {
        m_mouseMovePath.setX(m_mouseMovePath.x() + qAbs(delta.x()));
        m_mouseMovePath.setY(m_mouseMovePath.y() + qAbs(delta.y()));
    }

    auto mouseScenePos = mapToScene(m_mousePressPos);
    mouseScenePos.setX(m_offset + mouseScenePos.x() / m_scale);
    const auto x = clamp(0., mouseScenePos.x(), m_sceneWidth);

    if (m_mouseButtons & Qt::RightButton)
    {
        bool showItem = moveChrono(m_selectionItem, x);
        m_pScrollbar->setSelectionPos(m_selectionItem->left(), m_selectionItem->right());

        if (showItem)
        {
            m_pScrollbar->showSelectionIndicator();
        }

        needUpdate = true;
    }

    if (m_mouseButtons & Qt::LeftButton)
    {
        if (m_bDoubleClick)
        {
            moveChrono(m_rulerItem, x);
        }
        else
        {
            auto vbar = verticalScrollBar();

            // Block scrollbars from updating scene rect to make it possible to do it only once
            profiler_gui::BoolFlagGuard guard(m_bUpdatingRect, true);

            vbar->setValue(vbar->value() - delta.y());
            notifyVisibleRegionPosChange(m_offset - delta.x() / m_scale);
            guard.restore();
            // Seems like an ugly stub, but QSignalBlocker is also a bad decision
            // because if scrollbar does not emit valueChanged signal then viewport does not move

            updateVisibleSceneRect(); // Update scene visible rect only once

            // Update flicker speed
            m_flickerSpeedX += delta.x() >> 1;
            m_flickerSpeedY += delta.y();
            if (!m_flickerTimer.isActive())
            {
                // If flicker timer is not started, then start it
                m_flickerTimer.start(FLICKER_INTERVAL);
            }
        }

        needUpdate = true;
    }

    if (m_mouseButtons == 0)
    {
        if (m_selectionItem->isVisible())
        {
            auto prevValue = m_selectionItem->hoverIndicator();
            m_selectionItem->setHoverIndicator(m_selectionItem->indicatorContains(mouseScenePos));
            needUpdate = needUpdate || (prevValue != m_selectionItem->hoverIndicator());

            prevValue = m_selectionItem->hoverLeft();
            m_selectionItem->setHoverLeft(m_selectionItem->hoverLeft(mouseScenePos.x()));
            needUpdate = needUpdate || (prevValue != m_selectionItem->hoverLeft());

            if (!m_selectionItem->hoverLeft())
            {
                prevValue = m_selectionItem->hoverRight();
                m_selectionItem->setHoverRight(m_selectionItem->hoverRight(mouseScenePos.x()));
                needUpdate = needUpdate || (prevValue != m_selectionItem->hoverRight());
            }
        }

        if (m_rulerItem->isVisible())
        {
            auto prevValue = m_rulerItem->hoverLeft();
            m_rulerItem->setHoverLeft(m_rulerItem->hoverLeft(mouseScenePos.x()));
            needUpdate = needUpdate || (prevValue != m_rulerItem->hoverLeft());

            if (!m_rulerItem->hoverLeft())
            {
                prevValue = m_rulerItem->hoverRight();
                m_rulerItem->setHoverRight(m_rulerItem->hoverRight(mouseScenePos.x()));
                needUpdate = needUpdate || (prevValue != m_rulerItem->hoverRight());
            }
        }
    }

    if (needUpdate)
    {
        repaintScene(); // repaint scene
    }

    event->accept();
}

//////////////////////////////////////////////////////////////////////////

void BlocksGraphicsView::keyPressEvent(QKeyEvent* event)
{
    static const int KeyStep = 100;

    const int key = event->key();

    switch (key)
    {
        case Qt::Key_Right:
        case Qt::Key_6:
        {
            notifyVisibleRegionPosChange(m_offset + KeyStep / m_scale);
            break;
        }

        case Qt::Key_Left:
        case Qt::Key_4:
        {
            notifyVisibleRegionPosChange(m_offset - KeyStep / m_scale);
            break;
        }

        case Qt::Key_Up:
        case Qt::Key_8:
        {
            auto vbar = verticalScrollBar();
            vbar->setValue(vbar->value() - KeyStep);
            break;
        }

        case Qt::Key_Down:
        case Qt::Key_2:
        {
            auto vbar = verticalScrollBar();
            vbar->setValue(vbar->value() + KeyStep);
            break;
        }

        case Qt::Key_Plus:
        case Qt::Key_Equal:
        {
            onWheel(mapToDiagram(mapToScene(mapFromGlobal(QCursor::pos())).x()), KeyStep);
            break;
        }

        case Qt::Key_Minus:
        {
            onWheel(mapToDiagram(mapToScene(mapFromGlobal(QCursor::pos())).x()), -KeyStep);
            break;
        }
    }

    m_idleTime = 0;
    event->accept();
}

//////////////////////////////////////////////////////////////////////////

void BlocksGraphicsView::resizeEvent(QResizeEvent* event)
{
    Parent::resizeEvent(event);

    const QRectF previousRect = m_visibleSceneRect;
    const int vbar_width = updateVisibleSceneRect(); // Update scene visible rect only once    

    // Update slider width for scrollbar
    const auto windowWidth = (m_visibleSceneRect.width() + vbar_width) / m_scale;
    notifyVisibleRegionSizeChange(windowWidth);

    // Calculate new offset to save old screen center
    const auto deltaWidth = m_visibleSceneRect.width() - previousRect.width();
    m_offset = clamp(0., m_offset - deltaWidth * 0.5 / m_scale, m_sceneWidth - windowWidth);

    // Update slider position
    profiler_gui::BoolFlagGuard guard(m_bUpdatingRect, true); // To be sure that updateVisibleSceneRect will not be called by scrollbar change
    notifyVisibleRegionPosChange();
    guard.restore();

    repaintScene(); // repaint scene
}

//////////////////////////////////////////////////////////////////////////

void BlocksGraphicsView::initMode()
{
    // TODO: find mode with least number of bugs :)
    // There are always some display bugs...

    setCacheMode(QGraphicsView::CacheNone);
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    setOptimizationFlag(QGraphicsView::DontSavePainterState, true);

    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    connect(verticalScrollBar(), &QScrollBar::valueChanged, this, &This::onScrollbarValueChange);
    connect(&m_flickerTimer, &QTimer::timeout, this, &This::onFlickerTimeout);
    connect(&m_idleTimer, &QTimer::timeout, this, &This::onIdleTimeout);

    m_idleTimer.setInterval(IDLE_TIMER_INTERVAL);

    using profiler_gui::GlobalSignals;
    auto globalSignals = &EASY_GLOBALS.events;
    connect(globalSignals, &GlobalSignals::hierarchyFlagChanged, this, &This::onHierarchyFlagChange);
    connect(globalSignals, &GlobalSignals::selectedThreadChanged, this, &This::onSelectedThreadChange);
    connect(globalSignals, &GlobalSignals::selectedBlockChanged, this, &This::onSelectedBlockChange);
    connect(globalSignals, &GlobalSignals::itemsExpandStateChanged, this, &This::onRefreshRequired);
    connect(globalSignals, &GlobalSignals::refreshRequired, this, &This::onRefreshRequired);
    connect(globalSignals, &GlobalSignals::allDataGoingToBeDeleted, this, &This::clear);

    connect(globalSignals, &GlobalSignals::fileOpened, [this] {
        setTree(EASY_GLOBALS.profiler_blocks);
    });

    connect(globalSignals, &GlobalSignals::selectedBlockIdChanged, [this](profiler::block_id_t)
    {
        if (profiler_gui::is_max(EASY_GLOBALS.selected_block_id))
        {
            if (EASY_GLOBALS.selected_thread != 0)
            {
                for (auto item : m_items)
                {
                    if (item->threadId() == EASY_GLOBALS.selected_thread)
                    {
                        m_pScrollbar->setHistogramSource(EASY_GLOBALS.selected_thread, item->items(0));
                        break;
                    }
                }
            }
            else
            {
                m_pScrollbar->setHistogramSource(0, nullptr);
            }
        }
        else
            m_pScrollbar->setHistogramSource(EASY_GLOBALS.selected_thread, EASY_GLOBALS.selected_block_id);
        onRefreshRequired();
    });

    connect(globalSignals, &profiler_gui::GlobalSignals::threadNameDecorationChanged, this, &This::onThreadViewChanged);
    connect(globalSignals, &profiler_gui::GlobalSignals::hexThreadIdChanged, this, &This::onThreadViewChanged);

    connect(globalSignals, &profiler_gui::GlobalSignals::blocksTreeModeChanged, [this]()
    {
        if (!m_selectedBlocks.empty())
            emit intervalChanged(
                m_selectedBlocks,
                m_beginTime,
                position2time(m_selectionItem->left()),
                position2time(m_selectionItem->right()),
                m_selectionItem->strict()
            );
    });

    connect(globalSignals, &profiler_gui::GlobalSignals::chartSliderChanged, this, &This::onGraphicsScrollbarValueChange);
    connect(globalSignals, &profiler_gui::GlobalSignals::chartWheeled, this, &This::onGraphicsScrollbarWheel);
}

//////////////////////////////////////////////////////////////////////////

void BlocksGraphicsView::onThreadViewChanged()
{
    if (m_bEmpty)
        return;

    for (auto item : m_items)
        item->validateName();

    updateVisibleSceneRect();
    emit treeChanged();

    onHierarchyFlagChange(EASY_GLOBALS.only_current_thread_hierarchy);

    repaintScene();
}

//////////////////////////////////////////////////////////////////////////

void BlocksGraphicsView::onScrollbarValueChange(int)
{
    if (!m_bUpdatingRect && !m_bEmpty)
        updateVisibleSceneRect();
}

void BlocksGraphicsView::onGraphicsScrollbarValueChange(qreal value)
{
    if (!m_bEmpty)
    {
        m_offset = value;
        if (!m_bUpdatingRect)
        {
            updateVisibleSceneRect();
            repaintScene();
        }
    }
}

//////////////////////////////////////////////////////////////////////////

void BlocksGraphicsView::onFlickerTimeout()
{
    ++m_flickerCounterX;
    ++m_flickerCounterY;

    if (m_mouseButtons & Qt::LeftButton)
    {
        // Fast slow-down and stop if mouse button is pressed, no flicking.
        m_flickerSpeedX >>= 1;
        m_flickerSpeedY >>= 1;
        if (m_flickerSpeedX == -1) m_flickerSpeedX = 0;
        if (m_flickerSpeedY == -1) m_flickerSpeedY = 0;
    }
    else
    {
        // Flick when mouse button is not pressed

        using estd::sign;
        using estd::absmin;

        auto vbar = verticalScrollBar();

        profiler_gui::BoolFlagGuard guard(m_bUpdatingRect, true); // Block scrollbars from updating scene rect to make it possible to do it only once
        notifyVisibleRegionPosChange(m_offset - m_flickerSpeedX / m_scale);
        vbar->setValue(vbar->value() - m_flickerSpeedY);
        guard.restore();
        // Seems like an ugly stub, but QSignalBlocker is also a bad decision
        // because if scrollbar does not emit valueChanged signal then viewport does not move

        updateVisibleSceneRect(); // Update scene visible rect only once
        repaintScene(); // repaint scene

        const int dx = static_cast<int>(sign(m_flickerSpeedX) * m_flickerCounterX / FLICKER_FACTOR);
        const int dy = static_cast<int>(sign(m_flickerSpeedY) * m_flickerCounterY / FLICKER_FACTOR);

        if (abs(dx) > 0)
        {
            m_flickerSpeedX -= absmin(dx, m_flickerSpeedX);
            m_flickerCounterX = 0;
        }

        if (abs(dy) > 0)
        {
            m_flickerSpeedY -= absmin(dy, m_flickerSpeedY);
            m_flickerCounterY = 0;
        }
    }

    if (m_flickerSpeedX == 0 && m_flickerSpeedY == 0)
    {
        // Flicker stopped, no timer needed.
        m_flickerTimer.stop();
        m_flickerSpeedX = 0;
        m_flickerSpeedY = 0;
        m_flickerCounterX = 0;
        m_flickerCounterY = 0;
    }
}

//////////////////////////////////////////////////////////////////////////

void BlocksGraphicsView::onIdleTimeout()
{
    m_idleTime += IDLE_TIMER_INTERVAL;

    if (m_idleTime < IDLE_TIME)
    {
        removePopup();
        return;
    }

    if (m_popupWidget != nullptr)
        return;

    if (window() == nullptr || !window()->isActiveWindow())
        return;

    auto focusWidget = qApp->focusWidget();
    while (focusWidget != nullptr && !focusWidget->property("stayVisible").toBool())
        focusWidget = focusWidget->parentWidget();

    if (focusWidget != nullptr)
        return;

    m_isArbitraryValueTooltip = false;
    auto scenePos = mapToScene(mapFromGlobal(QCursor::pos()));

    if (scenePos.x() < m_visibleSceneRect.left() || scenePos.x() > m_visibleSceneRect.right())
        return;

    if (scenePos.y() < m_visibleSceneRect.top() || scenePos.y() > m_visibleSceneRect.bottom())
        return;

    decltype(scenePos) pos(m_offset + scenePos.x() / m_scale, scenePos.y());

    // Try to select one of context switches or items
    for (auto item : m_items)
    {
        auto cse = item->intersectEvent(pos);
        if (cse != nullptr)
        {
            const auto& itemBlock = cse->tree;

            auto widget = new QWidget(this, Qt::ToolTip | Qt::WindowTransparentForInput);
            if (widget == nullptr)
                return;

            widget->setObjectName(QStringLiteral("DiagramPopup"));
            widget->setAttribute(Qt::WA_ShowWithoutActivating, true);
            widget->setFocusPolicy(Qt::NoFocus);

            auto lay = new QGridLayout(widget);
            if (lay == nullptr)
                return;

            int row = 0;
            lay->addWidget(new BoldLabel("Context switch event", widget), row, 0, 1, 3, Qt::AlignHCenter);
            ++row;

            lay->addWidget(new QLabel("Thread:", widget), row, 0, Qt::AlignRight);

            const char* process_name = "";
            profiler::thread_id_t tid = 0;
            if (EASY_GLOBALS.version < profiler_gui::V130)
            {
                tid = cse->tree.node->id();
                process_name = cse->tree.node->name();
            }
            else
            {
                tid = cse->tree.cs->tid();
                process_name = cse->tree.cs->name();
            }

            auto it = EASY_GLOBALS.profiler_blocks.find(tid);

            if (it != EASY_GLOBALS.profiler_blocks.end())
            {
                if (EASY_GLOBALS.hex_thread_id)
                    lay->addWidget(new QLabel(QString("0x%1 %2").arg(tid, 0, 16).arg(it->second.name()), widget), row, 1, 1, 2, Qt::AlignLeft);
                else
                    lay->addWidget(new QLabel(QString("%1 %2").arg(tid).arg(it->second.name()), widget), row, 1, 1, 2, Qt::AlignLeft);
            }
            else if (EASY_GLOBALS.hex_thread_id)
                lay->addWidget(new QLabel(QString("0x%1").arg(tid, 0, 16), widget), row, 1, 1, 2, Qt::AlignLeft);
            else
                lay->addWidget(new QLabel(QString::number(tid), widget), row, 1, 1, 2, Qt::AlignLeft);
            ++row;

            lay->addWidget(new QLabel("Process:", widget), row, 0, Qt::AlignRight);
            lay->addWidget(new QLabel(process_name, widget), row, 1, 1, 2, Qt::AlignLeft);
            ++row;

            const auto duration = itemBlock.node->duration();
            lay->addWidget(new QLabel("Time:", widget), row, 0, Qt::AlignRight);
            lay->addWidget(new QLabel(profiler_gui::timeStringRealNs(EASY_GLOBALS.time_units, duration, 3), widget), row, 1, 1, 2, Qt::AlignLeft);
            ++row;

            if (itemBlock.per_thread_stats)
            {
                lay->addWidget(new QLabel("Total:", widget), row, 0, Qt::AlignRight);
                lay->addWidget(new QLabel(profiler_gui::timeStringRealNs(EASY_GLOBALS.time_units, itemBlock.per_thread_stats->total_duration, 3), widget), row, 1, 1, 2, Qt::AlignLeft);
                ++row;

                lay->addWidget(new BoldLabel("-------- Statistics --------", widget), row, 0, 1, 3, Qt::AlignHCenter);
                lay->addWidget(new QLabel("per ", widget), row + 1, 0, Qt::AlignRight);
                lay->addWidget(new QLabel("This %:", widget), row + 2, 0, Qt::AlignRight);
                lay->addWidget(new QLabel("Total %:", widget), row + 3, 0, Qt::AlignRight);
                lay->addWidget(new QLabel("N Calls:", widget), row + 4, 0, Qt::AlignRight);

                lay->addWidget(new QLabel("Thread", widget), row + 1, 1, Qt::AlignHCenter);

                auto percent = profiler_gui::percentReal(duration, item->root().profiled_time);
                lay->addWidget(new QLabel(0.005 < percent && percent < 0.5001 ? QString::number(percent, 'f', 2) : QString::number(static_cast<int>(0.5 + percent)), widget), row + 2, 1, Qt::AlignHCenter);

                lay->addWidget(new QLabel(QString::number(profiler_gui::percent(itemBlock.per_thread_stats->total_duration, item->root().profiled_time)), widget), row + 3, 1, Qt::AlignHCenter);

                lay->addWidget(new QLabel(QString::number(itemBlock.per_thread_stats->calls_number), widget), row + 4, 1, Qt::AlignHCenter);

                if (itemBlock.per_frame_stats && !profiler_gui::is_max(itemBlock.per_frame_stats->parent_block))
                {
                    int col = 2;
                    auto frame_duration = easyBlocksTree(itemBlock.per_frame_stats->parent_block).node->duration();

                    lay->addWidget(new QLabel("Frame", widget), row + 1, col, Qt::AlignHCenter);

                    percent = profiler_gui::percentReal(duration, frame_duration);
                    lay->addWidget(new QLabel(0.005 < percent && percent < 0.5001 ? QString::number(percent, 'f', 2) : QString::number(static_cast<int>(0.5 + percent)), widget), row + 2, col, Qt::AlignHCenter);

                    percent = profiler_gui::percentReal(itemBlock.per_frame_stats->total_duration, frame_duration);
                    lay->addWidget(new QLabel(0.005 < percent && percent < 0.5001 ? QString::number(percent, 'f', 2) : QString::number(static_cast<int>(0.5 + percent)), widget), row + 3, col, Qt::AlignHCenter);

                    lay->addWidget(new QLabel(QString::number(itemBlock.per_frame_stats->calls_number), widget), row + 4, col, Qt::AlignHCenter);
                }
            }

            m_popupWidget = widget;

            break;
        }

        profiler::block_index_t i = ~0U;
        auto block = item->intersect(pos, i);
        if (block != nullptr)
        {
            const auto& itemBlock = block->tree;
            const auto& itemDesc = easyDescriptor(itemBlock.node->id());

            if (itemDesc.type() == profiler::BlockType::Value)
            {
                m_isArbitraryValueTooltip = true;
                m_popupWidget = new ArbitraryValueToolTip(itemDesc.name(), itemBlock, this);
                break;
            }

            auto widget = new QWidget(this, Qt::ToolTip | Qt::WindowTransparentForInput);
            if (widget == nullptr)
                return;

            widget->setObjectName(QStringLiteral("DiagramPopup"));
            widget->setAttribute(Qt::WA_ShowWithoutActivating, true);
            widget->setFocusPolicy(Qt::NoFocus);

            auto lay = new QGridLayout(widget);
            if (lay == nullptr)
                return;

            lay->setSpacing(2);

            int row = 0;
            switch (itemDesc.type())
            {
                case profiler::BlockType::Block:
                {
                    const auto name = *itemBlock.node->name() != 0 ? itemBlock.node->name() : itemDesc.name();

                    //lay->addWidget(new QLabel("Name:", widget), row, 0, Qt::AlignRight);
                    lay->addWidget(new BoldLabel(profiler_gui::toUnicode(name), widget), row, 0, 1, 5,
                                   Qt::AlignHCenter);
                    ++row;

                    const auto duration = itemBlock.node->duration();
                    lay->addWidget(new QLabel("Time:", widget), row, 0, Qt::AlignRight);
                    lay->addWidget(new QLabel(profiler_gui::timeStringRealNs(EASY_GLOBALS.time_units, duration, 3),
                                              widget), row, 1, 1, 3, Qt::AlignLeft);
                    ++row;

                    profiler::timestamp_t children_duration = 0;
                    for (auto child : itemBlock.children)
                        children_duration += easyBlock(child).tree.node->duration();

                    const auto self_duration = duration - children_duration;
                    const auto self_percent =
                        duration == 0 ? 100. : profiler_gui::percentReal(self_duration, duration);
                    lay->addWidget(new QLabel("Self:", widget), row, 0, Qt::AlignRight);
                    lay->addWidget(new QLabel(QString("%1 (%2%)")
                                                  .arg(profiler_gui::timeStringRealNs(EASY_GLOBALS.time_units,
                                                                                    self_duration, 3))
                                                  .arg(QString::number(self_percent, 'g', 3)), widget),
                                   row, 1, 1, 3, Qt::AlignLeft);
                    ++row;

                    break;
                }

                case profiler::BlockType::Event:
                {
                    const auto name = *itemBlock.node->name() != 0 ? itemBlock.node->name() : itemDesc.name();

                    lay->addWidget(new BoldLabel("User defined event", widget), row, 0, 1, 2, Qt::AlignHCenter);
                    ++row;

                    lay->addWidget(new QLabel("Name:", widget), row, 0, Qt::AlignRight);
                    lay->addWidget(new QLabel(profiler_gui::toUnicode(name), widget), row, 1, Qt::AlignLeft);
                    ++row;

                    break;
                }

                case profiler::BlockType::Value:
                {
                    lay->addWidget(new BoldLabel("Arbitrary Value", widget), row, 0, 1, 2, Qt::AlignHCenter);
                    ++row;

                    lay->addWidget(new QLabel("Name:", widget), row, 0, Qt::AlignRight);
                    lay->addWidget(new QLabel(profiler_gui::toUnicode(itemDesc.name()), widget), row, 1, Qt::AlignLeft);
                    ++row;

                    lay->addWidget(new QLabel("Value:", widget), row, 0, Qt::AlignRight);
                    lay->addWidget(new QLabel(profiler_gui::shortValueString(*itemBlock.value), widget), row, 1, Qt::AlignLeft);
                    ++row;

                    lay->addWidget(new QLabel("VIN:", widget), row, 0, Qt::AlignRight);
                    lay->addWidget(new QLabel(QString("0x%1").arg(itemBlock.value->value_id(), 0, 16), widget), row, 1, Qt::AlignLeft);
                    ++row;

                    break;
                }

                default:
                {
                    delete widget;
                    return;
                }
            }

            if (itemBlock.per_thread_stats != nullptr)
            {
                if (itemDesc.type() == profiler::BlockType::Block)
                {
                    const auto duration = itemBlock.node->duration();

                    lay->addWidget(new QLabel("Avg:", widget), row, 0, Qt::AlignRight);
                    lay->addWidget(new QLabel(profiler_gui::timeStringRealNs(EASY_GLOBALS.time_units, itemBlock.per_thread_stats->average_duration(), 3), widget), row, 1, 1, 3, Qt::AlignLeft);
                    ++row;

                    lay->addWidget(new QLabel("Median:", widget), row, 0, Qt::AlignRight);
                    lay->addWidget(new QLabel(profiler_gui::timeStringRealNs(EASY_GLOBALS.time_units, itemBlock.per_thread_stats->median_duration, 3), widget), row, 1, 1, 3, Qt::AlignLeft);
                    ++row;

                    // Calculate idle/active time
                    {
                        const auto& threadRoot = item->root();

                        profiler::block_index_t ind = 0;
                        auto it = std::lower_bound(threadRoot.sync.begin(), threadRoot.sync.end(), itemBlock.node->begin(),
                                                     [](profiler::block_index_t _cs_index, profiler::timestamp_t _val)
                        {
                            return EASY_GLOBALS.gui_blocks[_cs_index].tree.node->begin() < _val;
                        });

                        if (it != threadRoot.sync.end())
                        {
                            ind = static_cast<profiler::block_index_t>(it - threadRoot.sync.begin());
                            if (ind > 0)
                                --ind;
                        }
                        else
                        {
                            ind = static_cast<profiler::block_index_t>(threadRoot.sync.size());
                        }

                        profiler::timestamp_t idleTime = 0;
                        for (auto ncs = static_cast<profiler::block_index_t>(threadRoot.sync.size()); ind < ncs; ++ind)
                        {
                            auto cs_index = threadRoot.sync[ind];
                            const auto cs = EASY_GLOBALS.gui_blocks[cs_index].tree.node;

                            if (cs->begin() > itemBlock.node->end())
                                break;

                            if (itemBlock.node->begin() <= cs->begin() && cs->end() <= itemBlock.node->end())
                                idleTime += cs->duration();
                        }

                        const auto active_time = duration - idleTime;
                        const auto active_percent = duration == 0 ? 100. : profiler_gui::percentReal(active_time, duration);
                        lay->addWidget(new QLabel("Active time:", widget), row, 0, Qt::AlignRight);
                        lay->addWidget(new QLabel(QString("%1 (%2%)").arg(profiler_gui::timeStringRealNs(EASY_GLOBALS.time_units, active_time, 3)).arg(QString::number(active_percent, 'g', 3)), widget), row, 1, 1, 3, Qt::AlignLeft);
                        ++row;
                    }

                    lay->addWidget(new BoldLabel("-------- Statistics --------", widget), row, 0, 1, 5, Qt::AlignHCenter);
                    lay->addWidget(new QLabel("per ", widget), row + 1, 0, Qt::AlignRight);
                    lay->addWidget(new QLabel("This %:", widget), row + 2, 0, Qt::AlignRight);
                    lay->addWidget(new QLabel("Total %:", widget), row + 3, 0, Qt::AlignRight);
                    lay->addWidget(new QLabel("Total self %:", widget), row + 4, 0, Qt::AlignRight);
                    lay->addWidget(new QLabel("N Calls:", widget), row + 5, 0, Qt::AlignRight);

                    lay->addWidget(new QLabel("Thread", widget), row + 1, 1, Qt::AlignHCenter);

                    auto percent = profiler_gui::percentReal(duration, item->root().profiled_time);
                    lay->addWidget(new QLabel(0.005 < percent && percent < 0.5001 ? QString::number(percent, 'f', 2) : QString::number(static_cast<int>(0.5 + percent)), widget), row + 2, 1, Qt::AlignHCenter);

                    lay->addWidget(new QLabel(QString::number(profiler_gui::percent(itemBlock.per_thread_stats->total_duration, item->root().profiled_time)), widget), row + 3, 1, Qt::AlignHCenter);

                    lay->addWidget(new QLabel(QString::number(profiler_gui::percent(itemBlock.per_thread_stats->total_duration - itemBlock.per_thread_stats->total_children_duration, item->root().profiled_time)), widget), row + 4, 1, Qt::AlignHCenter);

                    lay->addWidget(new QLabel(QString::number(itemBlock.per_thread_stats->calls_number), widget), row + 5, 1, Qt::AlignHCenter);

                    int col = 1;

                    if (itemBlock.per_frame_stats->parent_block != i && !profiler_gui::is_max(itemBlock.per_frame_stats->parent_block))
                    {
                        ++col;
                        auto frame_duration = easyBlocksTree(itemBlock.per_frame_stats->parent_block).node->duration();

                        lay->addWidget(new QLabel("Frame", widget), row + 1, col, Qt::AlignHCenter);

                        percent = profiler_gui::percentReal(duration, frame_duration);
                        lay->addWidget(new QLabel(0.005 < percent && percent < 0.5001 ? QString::number(percent, 'f', 2) : QString::number(static_cast<int>(0.5 + percent)), widget), row + 2, col, Qt::AlignHCenter);

                        percent = profiler_gui::percentReal(itemBlock.per_frame_stats->total_duration, frame_duration);
                        lay->addWidget(new QLabel(0.005 < percent && percent < 0.5001 ? QString::number(percent, 'f', 2) : QString::number(static_cast<int>(0.5 + percent)), widget), row + 3, col, Qt::AlignHCenter);

                        percent = profiler_gui::percentReal(itemBlock.per_frame_stats->total_duration - itemBlock.per_frame_stats->total_children_duration, frame_duration);
                        lay->addWidget(new QLabel(0.005 < percent && percent < 0.5001 ? QString::number(percent, 'f', 2) : QString::number(static_cast<int>(0.5 + percent)), widget), row + 4, col, Qt::AlignHCenter);

                        lay->addWidget(new QLabel(QString::number(itemBlock.per_frame_stats->calls_number), widget), row + 5, col, Qt::AlignHCenter);
                    }

                    if (!profiler_gui::is_max(itemBlock.per_parent_stats->parent_block))
                    {
                        ++col;
                        auto parent_duration = easyBlocksTree(itemBlock.per_parent_stats->parent_block).node->duration();

                        lay->addWidget(new QLabel("Parent", widget), row + 1, col, Qt::AlignHCenter);

                        percent = profiler_gui::percentReal(duration, parent_duration);
                        lay->addWidget(new QLabel(0.005 < percent && percent < 0.5001 ? QString::number(percent, 'f', 2) : QString::number(static_cast<int>(0.5 + percent)), widget), row + 2, col, Qt::AlignHCenter);

                        percent = profiler_gui::percentReal(itemBlock.per_parent_stats->total_duration, parent_duration);
                        lay->addWidget(new QLabel(0.005 < percent && percent < 0.5001 ? QString::number(percent, 'f', 2) : QString::number(static_cast<int>(0.5 + percent)), widget), row + 3, col, Qt::AlignHCenter);

                        percent = profiler_gui::percentReal(itemBlock.per_parent_stats->total_duration - itemBlock.per_parent_stats->total_children_duration, parent_duration);
                        lay->addWidget(new QLabel(0.005 < percent && percent < 0.5001 ? QString::number(percent, 'f', 2) : QString::number(static_cast<int>(0.5 + percent)), widget), row + 4, col, Qt::AlignHCenter);

                        lay->addWidget(new QLabel(QString::number(itemBlock.per_parent_stats->calls_number), widget), row + 5, col, Qt::AlignHCenter);

                        ++col;
                    }
                }
                else
                {
                    lay->addWidget(new QLabel("N calls/Thread:", widget), row, 0, Qt::AlignRight);
                    lay->addWidget(new QLabel(QString::number(itemBlock.per_thread_stats->calls_number), widget), row, 1, Qt::AlignLeft);
                }
            }

            m_popupWidget = widget;

            break;
        }
    }

    if (m_popupWidget != nullptr)
    {
        m_popupWidget->move(QCursor::pos());
        m_popupWidget->show();

        const auto w = std::min(m_popupWidget->width(), (int)m_visibleSceneRect.width());
        const auto h = std::min(m_popupWidget->height(), (int)m_visibleSceneRect.height());
        m_popupWidget->setFixedSize(w, h);

        const auto prevPos = scenePos;

        auto br = m_popupWidget->rect();
        if (scenePos.y() + br.height() > m_visibleSceneRect.bottom())
            scenePos.setY(std::max(scenePos.y() - br.height() + 1, m_visibleSceneRect.top()));

        if (scenePos.x() + br.width() > m_visibleSceneRect.right())
            scenePos.setX(std::max(scenePos.x() - br.width(), m_visibleSceneRect.left()));

        if ((scenePos - prevPos).manhattanLength() != 0)
            m_popupWidget->move(mapToGlobal(mapFromScene(scenePos)));
    }
}

//////////////////////////////////////////////////////////////////////////

void BlocksGraphicsView::onHierarchyFlagChange(bool)
{
    bool changedSelection = false;

    if (!m_selectedBlocks.empty())
    {
        changedSelection = true;
        m_selectedBlocks.clear();
    }

    if (m_selectionItem->isVisible())
    {
        for (auto item : m_items)
        {
            if (!EASY_GLOBALS.only_current_thread_hierarchy || item->threadId() == EASY_GLOBALS.selected_thread)
                item->getBlocks(m_selectionItem->left(), m_selectionItem->right(), m_selectedBlocks);
        }

        if (!m_selectedBlocks.empty())
        {
            changedSelection = true;
        }
    }

    if (changedSelection)
    {
        emit intervalChanged(
            m_selectedBlocks,
            m_beginTime,
            position2time(m_selectionItem->left()),
            position2time(m_selectionItem->right()),
            m_selectionItem->strict()
        );
    }
}

void BlocksGraphicsView::onSelectedThreadChange(profiler::thread_id_t id)
{
    if (m_pScrollbar == nullptr || m_pScrollbar->histThread() == id)
    {
        return;
    }

    if (id == 0)
    {
        m_pScrollbar->setHistogramSource(0, nullptr);
        return;
    }

    for (auto item : m_items)
    {
        if (item->threadId() == id)
        {
            m_pScrollbar->setHistogramSource(id, item->items(0));

            bool changedSelection = false;
            if (EASY_GLOBALS.only_current_thread_hierarchy)
            {
                if (!m_selectedBlocks.empty())
                {
                    changedSelection = true;
                    m_selectedBlocks.clear();
                }

                if (m_selectionItem->isVisible())
                {
                    item->getBlocks(m_selectionItem->left(), m_selectionItem->right(), m_selectedBlocks);
                    if (!m_selectedBlocks.empty())
                        changedSelection = true;
                }
            }

            if (changedSelection)
            {
                emit intervalChanged(
                    m_selectedBlocks,
                    m_beginTime,
                    position2time(m_selectionItem->left()),
                    position2time(m_selectionItem->right()),
                    m_selectionItem->strict()
                );
            }

            repaintScene();
            return;
        }
    }

    m_pScrollbar->setHistogramSource(0, nullptr);
    repaintScene();
}

//////////////////////////////////////////////////////////////////////////

void BlocksGraphicsView::onSelectedBlockChange(unsigned int _block_index)
{
    if (!m_bUpdatingRect)
    {
        if (_block_index < EASY_GLOBALS.gui_blocks.size())
        {
            // Scroll to item

            const auto& guiblock = EASY_GLOBALS.gui_blocks[_block_index];
            const auto thread_item = m_items[guiblock.graphics_item];
            const auto& item = thread_item->items(guiblock.graphics_item_level)[guiblock.graphics_item_index];

            m_flickerSpeedX = m_flickerSpeedY = 0;

            const profiler_gui::BoolFlagGuard guard(m_bUpdatingRect, true);
            verticalScrollBar()->setValue(static_cast<int>(thread_item->levelY(guiblock.graphics_item_level) - m_visibleSceneRect.height() * 0.5));
            notifyVisibleRegionPosChange(item.left() + (item.width() - m_visibleRegionWidth) * 0.5);

            if (EASY_GLOBALS.selecting_block_changes_thread && EASY_GLOBALS.selected_thread != thread_item->threadId())
            {
                EASY_GLOBALS.selected_thread = thread_item->threadId();

                emit EASY_GLOBALS.events.lockCharts();
                emit EASY_GLOBALS.events.selectedThreadChanged(EASY_GLOBALS.selected_thread);
                emit EASY_GLOBALS.events.unlockCharts();
            }

            m_pScrollbar->setHistogramSource(EASY_GLOBALS.selected_thread, guiblock.tree.node->id());
        }
        else if (EASY_GLOBALS.selected_thread != 0)
        {
            for (auto item : m_items)
            {
                if (item->threadId() == EASY_GLOBALS.selected_thread)
                {
                    m_pScrollbar->setHistogramSource(EASY_GLOBALS.selected_thread, item->items(0));
                    break;
                }
            }
        }
        else
        {
            m_pScrollbar->setHistogramSource(0, nullptr);
        }

        updateVisibleSceneRect();
        repaintScene();
    }
}

//////////////////////////////////////////////////////////////////////////

void BlocksGraphicsView::onRefreshRequired()
{
    if (!m_bUpdatingRect)
    {
        repaintScene();
    }
}

//////////////////////////////////////////////////////////////////////////

DiagramWidget::DiagramWidget(QWidget* _parent)
    : QWidget(_parent)
    , m_splitter(new QSplitter(Qt::Vertical, this))
    , m_scrollbar(new GraphicsScrollbar(px(85) + 2 + (EASY_GLOBALS.size.font_height << 1), this))
    , m_view(new BlocksGraphicsView(this))
    , m_threadNamesWidget(new ThreadNamesWidget(m_view, m_scrollbar->height(), this))
{
    initWidget();
}

void DiagramWidget::initWidget()
{
    m_splitter->setHandleWidth(1);
    m_splitter->setContentsMargins(0, 0, 0, 0);
    m_splitter->addWidget(m_view);
    m_splitter->addWidget(m_scrollbar);
    m_splitter->setStretchFactor(0, 500);
    m_splitter->setStretchFactor(1, 1);

    auto lay = new QHBoxLayout(this);
    lay->setContentsMargins(0, 0, 0, 0);
    lay->setSpacing(1);
    lay->addWidget(m_threadNamesWidget);
    lay->addWidget(m_splitter);

    m_view->setScrollbar(m_scrollbar);
}

DiagramWidget::~DiagramWidget()
{

}

BlocksGraphicsView* DiagramWidget::view()
{
    return m_view;
}

ThreadNamesWidget* DiagramWidget::threadsView()
{
    return m_threadNamesWidget;
}

void DiagramWidget::clear()
{
    m_scrollbar->clear();
    m_threadNamesWidget->clear();
    m_view->clear();
}

void DiagramWidget::save(QSettings& settings)
{
    settings.setValue("diagram/vsplitter/geometry", m_splitter->saveGeometry());
    settings.setValue("diagram/vsplitter/state", m_splitter->saveState());
}

void DiagramWidget::restore(QSettings& settings)
{
    auto geometry = settings.value("diagram/vsplitter/geometry").toByteArray();
    if (!geometry.isEmpty())
        m_splitter->restoreGeometry(geometry);

    auto state = settings.value("diagram/vsplitter/state").toByteArray();
    if (!state.isEmpty())
        m_splitter->restoreState(state);
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

void ThreadNameItem::paint(QPainter* _painter, const QStyleOptionGraphicsItem*, QWidget*)
{
    auto const parentView = static_cast<ThreadNamesWidget*>(scene()->parent());
    const auto view = parentView->view();
    const auto& items = view->getItems();
    if (items.empty())
        return;

    const auto visibleSceneRect = view->visibleSceneRect();
    const auto h = visibleSceneRect.height() + EASY_GLOBALS.size.timeline_height - 2;
    const auto w = parentView->width();//parentView->sceneRect().width();

    const auto overlap = EASY_GLOBALS.size.threads_row_spacing >> 1;
    static const QBrush brushes[2] = {QColor::fromRgb(BACKGROUND_1), QColor::fromRgb(BACKGROUND_2)};
    int i = -1;

    QRectF rect;

    _painter->resetTransform();

    // Draw thread names
    auto default_font = _painter->font();
    _painter->setFont(EASY_GLOBALS.font.background);
    for (auto item : items)
    {
        ++i;

        auto br = item->boundingRect();
        auto top = item->y() + br.top() - visibleSceneRect.top() - overlap;
        auto hgt = br.height() + EASY_GLOBALS.size.threads_row_spacing;
        auto bottom = top + hgt;

        if (top > h || bottom < 0)
            continue;

        if (item->threadId() == EASY_GLOBALS.selected_thread)
            _painter->setBrush(QBrush(QColor::fromRgb(profiler_gui::SELECTED_THREAD_BACKGROUND)));
        else
            _painter->setBrush(brushes[i & 1]);

        if (top < 0)
        {
            hgt += top;
            top = 0;
        }

        const auto dh = top + hgt - h;
        if (dh > 0)
            hgt -= dh;

        rect.setRect(0, top, w, hgt);

        _painter->setPen(profiler_gui::SYSTEM_BORDER_COLOR);
        _painter->drawRect(rect);

        rect.translate(-5, 0);
        _painter->setPen(profiler_gui::TEXT_COLOR);
        _painter->drawText(rect, Qt::AlignRight | Qt::AlignVCenter, item->threadName());
    }

    const auto rect_bottom = rect.bottom();
    if (rect_bottom < h)
    {
        ++i;
        rect.translate(5, rect.height());
        rect.setHeight(h - rect_bottom);
        _painter->setBrush(brushes[i & 1]);
        _painter->setPen(profiler_gui::SYSTEM_BORDER_COLOR);
        _painter->drawRect(rect);
    }

    if (h + 2 >= parentView->height())
        return;

    // Draw separator between thread names area and information area
    _painter->setPen(profiler_gui::SYSTEM_BORDER_COLOR);
    _painter->drawLine(QLineF(0, h, w, h));
    _painter->drawLine(QLineF(0, h + 2, w, h + 2));

    // Draw information
    _painter->setFont(EASY_GLOBALS.font.ruler);
    QFontMetricsF fm(EASY_GLOBALS.font.ruler, parentView);
    const qreal th = fm.height(); // Calculate displayed text height
    const qreal time1 = view->chronoTime();
    const qreal time2 = view->chronoTimeAux();

    auto y = h + 2;

    auto drawTimeText = [&rect, &w, &y, &fm, &_painter](qreal time, qreal th, QRgb color)
    {
        if (time > 0)
        {
            const QString text = profiler_gui::autoTimeStringReal(time); // Displayed text
            rect.setRect(0, y, w, th);

            _painter->setPen(color);
            _painter->drawText(rect, Qt::AlignCenter, text);

            y += th;
        }
    };

    drawTimeText(time1, th, profiler_gui::RULER_COLOR.rgb() & 0x00ffffff);
    drawTimeText(time2, th, profiler_gui::RULER_COLOR2.rgb() & 0x00ffffff);
}

//////////////////////////////////////////////////////////////////////////

ThreadNamesWidget::ThreadNamesWidget(BlocksGraphicsView* _view, int _additionalHeight, QWidget* _parent)
    : Parent(_parent)
    , m_idleTime(0)
    , m_view(_view)
    , m_popupWidget(nullptr)
    , m_maxLength(100)
    , m_bHovered(false)
    , m_additionalHeight(_additionalHeight + 1)
{
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Ignored);

    setScene(new QGraphicsScene(this));

    setMouseTracking(true);
    setCacheMode(QGraphicsView::CacheNone);
    setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setFixedWidth(m_maxLength);

    connect(&EASY_GLOBALS.events, &profiler_gui::GlobalSignals::selectedThreadChanged, [this](profiler::thread_id_t){ repaintScene(); });
    connect(&EASY_GLOBALS.events, &profiler_gui::GlobalSignals::allDataGoingToBeDeleted, this, &This::clear);

    connect(m_view, &BlocksGraphicsView::treeChanged, this, &This::onTreeChange);
    connect(m_view, &BlocksGraphicsView::sceneUpdated, this, &This::repaintScene);
    connect(m_view->verticalScrollBar(), &QScrollBar::valueChanged, verticalScrollBar(), &QScrollBar::setValue, Qt::QueuedConnection);
    connect(m_view->verticalScrollBar(), &QScrollBar::rangeChanged, this, &This::setVerticalScrollbarRange, Qt::QueuedConnection);

    m_idleTimer.setInterval(IDLE_TIMER_INTERVAL);
    connect(&m_idleTimer, &QTimer::timeout, this, &This::onIdleTimeout);
}

ThreadNamesWidget::~ThreadNamesWidget()
{
    removePopup();
}

void ThreadNamesWidget::onWindowActivationChanged()
{
    if (!window()->isActiveWindow() && qApp->activeWindow() != m_popupWidget)
    {
        removePopup();
    }
}

void ThreadNamesWidget::removePopup()
{
    delete m_popupWidget;
    m_popupWidget = nullptr;
}

void ThreadNamesWidget::clear()
{
    const QSignalBlocker b(this);
    removePopup();
    scene()->clear();

    m_maxLength = 100;
    setFixedWidth(m_maxLength);

    m_idleTimer.stop();
    m_idleTime = 0;
}

void ThreadNamesWidget::setVerticalScrollbarRange(int _minValue, int _maxValue)
{
    verticalScrollBar()->setRange(_minValue, _maxValue + m_additionalHeight);
}

void ThreadNamesWidget::onTreeChange()
{
    const QSignalBlocker b(this);
    removePopup();
    scene()->clear();

    m_idleTimer.stop();
    m_idleTime = 0;

    QFontMetricsF fm(EASY_GLOBALS.font.background, this);
    qreal maxLength = 100;
    const auto& graphicsItems = m_view->getItems();
    for (auto graphicsItem : graphicsItems)
        maxLength = std::max(maxLength, (10 + fm.width(graphicsItem->threadName())) * profiler_gui::FONT_METRICS_FACTOR);

    auto vbar = verticalScrollBar();
    auto viewBar = m_view->verticalScrollBar();

    auto r = m_view->sceneRect();
    setSceneRect(0, r.top(), maxLength, r.height() + m_additionalHeight);

    setVerticalScrollbarRange(viewBar->minimum(), viewBar->maximum());
    vbar->setSingleStep(viewBar->singleStep());
    vbar->setPageStep(viewBar->pageStep());
    vbar->setValue(viewBar->value());

    auto item = new ThreadNameItem();
    item->setPos(0, 0);
    item->setBoundingRect(sceneRect());
    scene()->addItem(item);

    m_maxLength = static_cast<int>(maxLength);
    setFixedWidth(m_maxLength);

    m_idleTimer.start();
}

void ThreadNamesWidget::onIdleTimeout()
{
    m_idleTime += IDLE_TIMER_INTERVAL;

    if (m_idleTime < IDLE_TIME)
    {
        removePopup();
        return;
    }

    if (m_popupWidget != nullptr)
        return;

    if (window() == nullptr || !window()->isActiveWindow())
        return;

    auto focusWidget = qApp->focusWidget();
    while (focusWidget != nullptr && !focusWidget->property("stayVisible").toBool())
        focusWidget = focusWidget->parentWidget();

    if (focusWidget != nullptr)
        return;

    const auto localPos = mapFromGlobal(QCursor::pos());
    auto visibleSceneRect = mapToScene(rect()).boundingRect();
    visibleSceneRect.setTop(m_view->visibleSceneRect().top());

    auto scenePos = QPointF(mapToScene(localPos).x(), m_view->mapToScene(localPos).y());

    if (scenePos.x() < visibleSceneRect.left() || scenePos.x() > visibleSceneRect.right())
        return;

    if (scenePos.y() < visibleSceneRect.top() || scenePos.y() > visibleSceneRect.bottom())
        return;

    const qreal y = scenePos.y() - visibleSceneRect.top();

    const auto& items = m_view->getItems();
    if (items.empty())
        return;

    const auto overlap = EASY_GLOBALS.size.threads_row_spacing >> 1;

    GraphicsBlockItem* intersectingItem = nullptr;
    for (auto item : items)
    {
        auto br = item->boundingRect();
        auto top = item->y() + br.top() - visibleSceneRect.top() - overlap;
        auto hgt = br.height() + EASY_GLOBALS.size.threads_row_spacing;
        auto bottom = top + hgt;

        if (bottom < y || y < top)
            continue;

        intersectingItem = item;

        break;
    }

    if (intersectingItem != nullptr)
    {
        auto widget = new QWidget(this, Qt::ToolTip | Qt::WindowTransparentForInput);
        if (widget == nullptr)
            return;

        widget->setObjectName(QStringLiteral("ThreadsPopup"));
        widget->setAttribute(Qt::WA_ShowWithoutActivating, true);
        widget->setFocusPolicy(Qt::NoFocus);

        auto lay = new QGridLayout(widget);
        if (lay == nullptr)
            return;

        int row = 0;

        lay->setSpacing(2);
        lay->addWidget(new BoldLabel(intersectingItem->threadName(), widget), row, 0, 1, 2, Qt::AlignHCenter);
        ++row;

        profiler::timestamp_t duration = 0;
        const auto& root = intersectingItem->root();
        if (!root.children.empty())
            duration = easyBlock(root.children.back()).tree.node->end() - easyBlock(root.children.front()).tree.node->begin();

        lay->addWidget(new QLabel("Time:", widget), row, 0, Qt::AlignRight);
        lay->addWidget(new QLabel(profiler_gui::timeStringRealNs(EASY_GLOBALS.time_units, duration, 3), widget), row, 1, Qt::AlignLeft);
        ++row;

        lay->addWidget(new QLabel("Profiled:", widget), row, 0, Qt::AlignRight);
        if (duration != 0)
        {
            lay->addWidget(new QLabel(QString("%1 (%2%)").arg(profiler_gui::timeStringRealNs(EASY_GLOBALS.time_units, root.profiled_time, 3))
                .arg(QString::number(100. * (double)root.profiled_time / (double)duration, 'f', 2)), widget), row, 1, Qt::AlignLeft);
        }
        else
        {
            lay->addWidget(new QLabel(profiler_gui::timeStringRealNs(EASY_GLOBALS.time_units, root.profiled_time, 3), widget), row, 1, Qt::AlignLeft);
        }
        ++row;

        lay->addWidget(new QLabel("Wait:", widget), row, 0, Qt::AlignRight);
        if (duration != 0)
        {
            lay->addWidget(new QLabel(QString("%1 (%2%)").arg(profiler_gui::timeStringRealNs(EASY_GLOBALS.time_units, root.wait_time, 3))
                .arg(QString::number(100. * (double)root.wait_time / (double)duration, 'f', 2)), widget), row, 1, Qt::AlignLeft);
        }
        else
        {
            lay->addWidget(new QLabel(profiler_gui::timeStringRealNs(EASY_GLOBALS.time_units, root.wait_time, 3), widget), row, 1, Qt::AlignLeft);
        }
        ++row;

        const auto eventsSize = root.events.size();

        lay->addWidget(new QLabel("Frames:", widget), row, 0, Qt::AlignRight);
        lay->addWidget(new QLabel(QString::number(root.frames_number), widget), row, 1, Qt::AlignLeft);
        ++row;

        lay->addWidget(new QLabel("Blocks:", widget), row, 0, Qt::AlignRight);
        lay->addWidget(new QLabel(QString::number(root.blocks_number - eventsSize), widget), row, 1, Qt::AlignLeft);
        ++row;

        lay->addWidget(new QLabel("Markers:", widget), row, 0, Qt::AlignRight);
        lay->addWidget(new QLabel(QString::number(eventsSize), widget), row, 1, Qt::AlignLeft);
        ++row;

        m_popupWidget = widget;
        if (m_popupWidget != nullptr)
        {
            focusWidget = qApp->focusWidget();

            m_popupWidget->move(QCursor::pos());
            m_popupWidget->show();

            if (focusWidget != nullptr && focusWidget->property("stayVisible").toBool())
                focusWidget->raise();
        }
    }
}

void ThreadNamesWidget::repaintScene()
{
    scene()->update();
}

void ThreadNamesWidget::enterEvent(QEvent* _event)
{
    Parent::enterEvent(_event);
    m_bHovered = true;
    if (!m_idleTimer.isActive())
        m_idleTimer.start();
    m_idleTime = 0;
}

void ThreadNamesWidget::leaveEvent(QEvent* _event)
{
    Parent::leaveEvent(_event);
    m_bHovered = false;
    m_idleTime = 0;
}

void ThreadNamesWidget::mousePressEvent(QMouseEvent* _event)
{
    m_idleTime = 0;

    QMouseEvent e(_event->type(), _event->pos() - QPointF(sceneRect().width(), 0), _event->button(), _event->buttons() & ~Qt::RightButton, _event->modifiers());
    m_view->mousePressEvent(&e);
    _event->accept();
}

void ThreadNamesWidget::mouseDoubleClickEvent(QMouseEvent* _event)
{
    const auto overlap = EASY_GLOBALS.size.threads_row_spacing >> 1;

    m_idleTime = 0;

    auto y = m_view->mapToScene(mapFromGlobal(QCursor::pos())).y();
    const auto& items = m_view->getItems();
    for (auto item : items)
    {
        auto br = item->boundingRect();
        auto top = item->y() + br.top() - overlap;
        auto bottom = top + br.height() + overlap;

        if (y < top || y > bottom)
            continue;

        const auto thread_id = item->threadId();
        if (thread_id != EASY_GLOBALS.selected_thread)
        {
            EASY_GLOBALS.selected_thread = thread_id;
            emit EASY_GLOBALS.events.selectedThreadChanged(thread_id);
        }

        break;
    }

    _event->accept();
}

void ThreadNamesWidget::mouseReleaseEvent(QMouseEvent* _event)
{
    m_idleTime = 0;

    QMouseEvent e(_event->type(), _event->pos() - QPointF(sceneRect().width(), 0), _event->button(), _event->buttons() & ~Qt::RightButton, _event->modifiers());
    m_view->mouseReleaseEvent(&e);
    _event->accept();
}

void ThreadNamesWidget::mouseMoveEvent(QMouseEvent* _event)
{
    m_idleTime = 0;

    QMouseEvent e(_event->type(), _event->pos() - QPointF(sceneRect().width(), 0), _event->button(), _event->buttons() & ~Qt::RightButton, _event->modifiers());
    m_view->mouseMoveEvent(&e);
    _event->accept();
}

void ThreadNamesWidget::keyPressEvent(QKeyEvent* _event)
{
    m_idleTime = 0;
    m_view->keyPressEvent(_event);
}

void ThreadNamesWidget::keyReleaseEvent(QKeyEvent* _event)
{
    m_idleTime = 0;
    _event->accept();
}

void ThreadNamesWidget::wheelEvent(QWheelEvent* _event)
{
    m_idleTime = 0;

    auto vbar = m_view->verticalScrollBar();
    if (vbar != nullptr)
    {
        _event->accept();

        const auto prev = vbar->value();
        vbar->setValue(vbar->value() - _event->delta());

        if (prev != vbar->value())
        {
            verticalScrollBar()->setValue(vbar->value());
            repaintScene();
        }
    }
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

