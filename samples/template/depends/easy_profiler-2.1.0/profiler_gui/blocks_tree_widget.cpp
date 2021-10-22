/************************************************************************
* file name         : blocks_tree_widget.cpp
* ----------------- :
* creation time     : 2016/06/26
* author            : Victor Zarubkin
* email             : v.s.zarubkin@gmail.com
* ----------------- :
* description       : The file contains implementation of BlocksTreeWidget and it's auxiliary classes
*                   : for displyaing easy_profiler blocks tree.
* ----------------- :
* change log        : * 2016/06/26 Victor Zarubkin: Moved sources from tree_view.h
*                   :       and renamed classes from My* to Prof*.
*                   :
*                   : * 2016/06/27 Victor Zarubkin: Added possibility to colorize rows
*                   :       with profiler blocks' colors.
*                   :       Also added displaying frame statistics for blocks.
*                   :       Disabled sorting by name to save order of threads displayed on graphics view.
*                   :
*                   : * 2016/06/29 Victor Zarubkin: Added clearSilent() method.
*                   :
*                   : * 2016/08/18 Victor Zarubkin: Moved sources of TreeWidgetItem into tree_widget_item.h/.cpp
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

#include <QAction>
#include <QActionGroup>
#include <QApplication>
#include <QByteArray>
#include <QContextMenuEvent>
#include <QDebug>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QMenu>
#include <QMessageBox>
#include <QMoveEvent>
#include <QResizeEvent>
#include <QScrollBar>
#include <QSettings>
#include <QSignalBlocker>
#include <QToolBar>
#include <QVBoxLayout>

#include "blocks_tree_widget.h"
#include "arbitrary_value_tooltip.h"
#include "round_progress_widget.h"
#include "globals.h"
#include "thread_pool.h"

#ifdef max
#undef max
#endif

#ifdef min
#undef min
#endif

//////////////////////////////////////////////////////////////////////////

namespace {

const int TREE_BUILDER_TIMER_INTERVAL = 40;

const bool PLAIN_MODE_COLUMNS[COL_COLUMNS_NUMBER] = {
      true  // COL_NAME = 0,
    , true  // COL_BEGIN,
    , true  // COL_TIME,
    , true  // COL_SELF_TIME,
    , true  // COL_SELF_TIME_PERCENT,
    , true  // COL_END,
    , true  // COL_PERCENT_PER_FRAME,
    , false // COL_TOTAL_TIME_PER_FRAME,
    , false // COL_PERCENT_SUM_PER_FRAME,
    , true  // COL_MIN_PER_FRAME,
    , true  // COL_MAX_PER_FRAME,
    , true  // COL_AVG_PER_FRAME,
    , true  // COL_MEDIAN_PER_FRAME,
    , true  // COL_NCALLS_PER_FRAME,
    , true  // COL_TOTAL_TIME_PER_THREAD,
    , true  // COL_PERCENT_SUM_PER_THREAD,
    , true  // COL_MIN_PER_THREAD,
    , true  // COL_MAX_PER_THREAD,
    , true  // COL_AVG_PER_THREAD,
    , true  // COL_MEDIAN_PER_THREAD,
    , true  // COL_NCALLS_PER_THREAD,
    , false // COL_PERCENT_PER_PARENT,
    , false // COL_TOTAL_TIME_PER_PARENT,
    , false // COL_PERCENT_SUM_PER_PARENT,
    , false // COL_MIN_PER_PARENT,
    , false // COL_MAX_PER_PARENT,
    , false // COL_AVG_PER_PARENT,
    , false // COL_MEDIAN_PER_PARENT,
    , false // COL_NCALLS_PER_PARENT,
    , true  // COL_ACTIVE_TIME,
    , true  // COL_ACTIVE_PERCENT,
    , true  // COL_PERCENT_PER_AREA,
    , true  // COL_TOTAL_TIME_PER_AREA,
    , true  // COL_PERCENT_SUM_PER_AREA,
    , true  // COL_MIN_PER_AREA,
    , true  // COL_MAX_PER_AREA,
    , true  // COL_AVG_PER_AREA,
    , true  // COL_MEDIAN_PER_AREA,
    , true  // COL_NCALLS_PER_AREA,
};

const bool SELECTION_MODE_COLUMNS[COL_COLUMNS_NUMBER] = {
      true  // COL_NAME = 0,
    , false // COL_BEGIN,
    , true  // COL_TIME,
    , true  // COL_SELF_TIME,
    , true  // COL_SELF_TIME_PERCENT,
    , false // COL_END,
    , false // COL_PERCENT_PER_FRAME,
    , false // COL_TOTAL_TIME_PER_FRAME,
    , false // COL_PERCENT_SUM_PER_FRAME,
    , false // COL_MIN_PER_FRAME,
    , false // COL_MAX_PER_FRAME,
    , false // COL_AVG_PER_FRAME,
    , false // COL_MEDIAN_PER_FRAME,
    , false // COL_NCALLS_PER_FRAME,
    , true  // COL_TOTAL_TIME_PER_THREAD,
    , true  // COL_PERCENT_SUM_PER_THREAD,
    , true  // COL_MIN_PER_THREAD,
    , true  // COL_MAX_PER_THREAD,
    , true  // COL_AVG_PER_THREAD,
    , true  // COL_MEDIAN_PER_THREAD,
    , true  // COL_NCALLS_PER_THREAD,
    , false // COL_PERCENT_PER_PARENT,
    , false // COL_TOTAL_TIME_PER_PARENT,
    , false // COL_PERCENT_SUM_PER_PARENT,
    , false // COL_MIN_PER_PARENT,
    , false // COL_MAX_PER_PARENT,
    , false // COL_AVG_PER_PARENT,
    , false // COL_MEDIAN_PER_PARENT,
    , false // COL_NCALLS_PER_PARENT,
    , true  // COL_ACTIVE_TIME,
    , true  // COL_ACTIVE_PERCENT,
    , true  // COL_PERCENT_PER_AREA,
    , true  // COL_TOTAL_TIME_PER_AREA,
    , true  // COL_PERCENT_SUM_PER_AREA,
    , true  // COL_MIN_PER_AREA,
    , true  // COL_MAX_PER_AREA,
    , true  // COL_AVG_PER_AREA,
    , true  // COL_MEDIAN_PER_AREA,
    , true  // COL_NCALLS_PER_AREA,
};

} // end of namespace <noname>.

//////////////////////////////////////////////////////////////////////////

BlocksTreeWidget::BlocksTreeWidget(QWidget* _parent)
    : Parent(_parent)
    , m_beginTime(::std::numeric_limits<decltype(m_beginTime)>::max())
    , m_lastFound(nullptr)
    , m_progress(nullptr)
    , m_hintLabel(nullptr)
    , m_valueTooltip(nullptr)
    , m_mode(TreeMode::Plain)
    , m_lastFoundIndex(0)
    , m_bLocked(false)
    , m_bSilentExpandCollapse(false)
    , m_bCaseSensitiveSearch(false)
    , m_bInitialized(false)
{
    installEventFilter(this);
    memset(m_columnsHiddenStatus, 0, sizeof(m_columnsHiddenStatus));
    memset(m_columnsMinimumWidth, 0, sizeof(m_columnsMinimumWidth));

    setAutoFillBackground(false);
    setAlternatingRowColors(true);
    setItemsExpandable(true);
    setAnimated(true);
    setSortingEnabled(false);
    setColumnCount(COL_COLUMNS_NUMBER);
    setSelectionBehavior(QAbstractItemView::SelectRows);

    header()->setSectionResizeMode(QHeaderView::Interactive);

    auto header_item = new QTreeWidgetItem();

    header_item->setText(COL_NAME, "Name");

    header_item->setText(COL_BEGIN, "Begin, ms");

    header_item->setText(COL_TIME, "Time");
    header_item->setText(COL_SELF_TIME, "SelfTime");
    //header_item->setToolTip(COL_SELF_TIME, "");
    header_item->setText(COL_TOTAL_TIME_PER_PARENT, "Total/parent");
    header_item->setText(COL_TOTAL_TIME_PER_FRAME,  "Total/frame");
    header_item->setText(COL_TOTAL_TIME_PER_THREAD, "Total/thread");

    header_item->setText(COL_SELF_TIME_PERCENT, "Self%");
    header_item->setText(COL_PERCENT_PER_PARENT,     "%/parent");
    header_item->setText(COL_PERCENT_PER_FRAME,      "%/frame");
    header_item->setText(COL_PERCENT_SUM_PER_FRAME,  "Total%/frame");
    header_item->setText(COL_PERCENT_SUM_PER_PARENT, "Total%/parent");
    header_item->setText(COL_PERCENT_SUM_PER_THREAD, "Total%/thread");

    header_item->setText(COL_END, "End, ms");

    header_item->setText(COL_MIN_PER_FRAME,    "Min/frame");
    header_item->setText(COL_MAX_PER_FRAME,    "Max/frame");
    header_item->setText(COL_AVG_PER_FRAME,    "Avg/frame");
    header_item->setText(COL_MEDIAN_PER_FRAME, "Mdn/frame");
    header_item->setText(COL_NCALLS_PER_FRAME, "N/frame");

    header_item->setText(COL_MIN_PER_PARENT,    "Min/parent");
    header_item->setText(COL_MAX_PER_PARENT,    "Max/parent");
    header_item->setText(COL_AVG_PER_PARENT,    "Avg/parent");
    header_item->setText(COL_MEDIAN_PER_PARENT, "Mdn/parent");
    header_item->setText(COL_NCALLS_PER_PARENT, "N/parent");

    header_item->setText(COL_MIN_PER_THREAD,    "Min/thread");
    header_item->setText(COL_MAX_PER_THREAD,    "Max/thread");
    header_item->setText(COL_AVG_PER_THREAD,    "Avg/thread");
    header_item->setText(COL_MEDIAN_PER_THREAD, "Mdn/thread");
    header_item->setText(COL_NCALLS_PER_THREAD, "N/thread");

    header_item->setText(COL_ACTIVE_TIME,    "WorkTime");
    header_item->setText(COL_ACTIVE_PERCENT, "Work%");

    header_item->setText(COL_PERCENT_PER_AREA,     "%/area");
    header_item->setText(COL_TOTAL_TIME_PER_AREA,  "Total/area");
    header_item->setText(COL_PERCENT_SUM_PER_AREA, "Total%/area");
    header_item->setText(COL_MIN_PER_AREA,         "Min/area");
    header_item->setText(COL_MAX_PER_AREA,         "Max/area");
    header_item->setText(COL_AVG_PER_AREA,         "Avg/area");
    header_item->setText(COL_MEDIAN_PER_AREA,      "Mdn/area");
    header_item->setText(COL_NCALLS_PER_AREA,      "N/area");

    auto color = QColor::fromRgb(profiler::colors::DeepOrange900);
    header_item->setForeground(COL_MIN_PER_THREAD, color);
    header_item->setForeground(COL_MAX_PER_THREAD, color);
    header_item->setForeground(COL_AVG_PER_THREAD, color);
    header_item->setForeground(COL_MEDIAN_PER_THREAD, color);
    header_item->setForeground(COL_NCALLS_PER_THREAD, color);
    header_item->setForeground(COL_PERCENT_SUM_PER_THREAD, color);
    header_item->setForeground(COL_TOTAL_TIME_PER_THREAD, color);

    color = QColor::fromRgb(profiler::colors::Purple800);
    header_item->setForeground(COL_MIN_PER_FRAME, color);
    header_item->setForeground(COL_MAX_PER_FRAME, color);
    header_item->setForeground(COL_AVG_PER_FRAME, color);
    header_item->setForeground(COL_MEDIAN_PER_FRAME, color);
    header_item->setForeground(COL_NCALLS_PER_FRAME, color);
    header_item->setForeground(COL_PERCENT_SUM_PER_FRAME, color);
    header_item->setForeground(COL_TOTAL_TIME_PER_FRAME, color);
    header_item->setForeground(COL_PERCENT_PER_FRAME, color);

    color = QColor::fromRgb(profiler::colors::Teal900);
    header_item->setForeground(COL_MIN_PER_PARENT, color);
    header_item->setForeground(COL_MAX_PER_PARENT, color);
    header_item->setForeground(COL_AVG_PER_PARENT, color);
    header_item->setForeground(COL_MEDIAN_PER_PARENT, color);
    header_item->setForeground(COL_NCALLS_PER_PARENT, color);
    header_item->setForeground(COL_PERCENT_SUM_PER_PARENT, color);
    header_item->setForeground(COL_TOTAL_TIME_PER_PARENT, color);
    header_item->setForeground(COL_PERCENT_PER_PARENT, color);

    color = QColor::fromRgb(profiler::colors::Blue900);
    header_item->setForeground(COL_PERCENT_PER_AREA, color);
    header_item->setForeground(COL_TOTAL_TIME_PER_AREA, color);
    header_item->setForeground(COL_PERCENT_SUM_PER_AREA, color);
    header_item->setForeground(COL_MIN_PER_AREA, color);
    header_item->setForeground(COL_MAX_PER_AREA, color);
    header_item->setForeground(COL_AVG_PER_AREA, color);
    header_item->setForeground(COL_MEDIAN_PER_AREA, color);
    header_item->setForeground(COL_NCALLS_PER_AREA, color);

    setHeaderItem(header_item);

    connect(&EASY_GLOBALS.events, &profiler_gui::GlobalSignals::selectedThreadChanged,
            this, &This::onSelectedThreadChange, Qt::QueuedConnection);
    connect(&EASY_GLOBALS.events, &profiler_gui::GlobalSignals::selectedBlockChanged,
            this, &This::onSelectedBlockChange, Qt::QueuedConnection);
    connect(&m_fillTimer, &QTimer::timeout, this, &This::onFillTimerTimeout);
    connect(&m_idleTimer, &QTimer::timeout, this, &This::onIdleTimeout);
    m_idleTimer.setInterval(500);
    m_idleTimer.setSingleShot(true);

    loadSettings();

    m_columnsHiddenStatus[0] = 0;
    setColumnHidden(0, false);

    switch (m_mode)
    {
        case TreeMode::Full:
        {
            for (int i = 1; i < COL_COLUMNS_NUMBER; ++i)
            {
                m_columnsHiddenStatus[i] = static_cast<char>(isColumnHidden(i) ? 1 : 0);
            }
            break;
        }

        case TreeMode::Plain:
        {
            for (int i = 1; i < COL_COLUMNS_NUMBER; ++i)
            {
                if (PLAIN_MODE_COLUMNS[i])
                {
                    if (isColumnHidden(i))
                        m_columnsHiddenStatus[i] = 1;
                }
                else if (!isColumnHidden(i))
                {
                    setColumnHidden(i, true);
                }
            }
            break;
        }

        case TreeMode::SelectedArea:
        {
            for (int i = 1; i < COL_COLUMNS_NUMBER; ++i)
            {
                if (SELECTION_MODE_COLUMNS[i])
                {
                    if (isColumnHidden(i))
                        m_columnsHiddenStatus[i] = 1;
                }
                else if (!isColumnHidden(i))
                {
                    setColumnHidden(i, true);
                }
            }
            break;
        }
    }

    m_hintLabel = new QLabel("Use Right Mouse Button on the Diagram to build a tree...\n"
                             "Way 1: Press the button >> Move mouse >> Release the button\n"
                             "Way 2: Just click the right mouse button on any block", this);
    m_hintLabel->setObjectName(QStringLiteral("BlocksTreeWidget_HintLabel"));
    m_hintLabel->setProperty("hovered", false);
    m_hintLabel->setAlignment(Qt::AlignCenter);

    QTimer::singleShot(1500, this, &This::alignProgressBar);

    setItemDelegateForColumn(0, new TreeWidgetItemDelegate(this));

    connect(header(), &QHeaderView::sectionResized, this, &This::onHeaderSectionResized);
}

BlocksTreeWidget::~BlocksTreeWidget()
{
    saveSettings();
    delete m_valueTooltip;
}

//////////////////////////////////////////////////////////////////////////

bool BlocksTreeWidget::eventFilter(QObject* _object, QEvent* _event)
{
    if (_object != this)
        return false;

    const auto eventType = _event->type();
    switch (eventType)
    {
        case QEvent::MouseMove:
        case QEvent::HoverMove:
        {
            if (m_idleTimer.isActive())
                m_idleTimer.stop();

            if (m_valueTooltip != nullptr)
            {
                const int size = std::min(m_valueTooltip->width(), m_valueTooltip->height()) >> 1;
                const auto rect = m_valueTooltip->rect().adjusted(-size, -size, size, size);
                const auto pos = m_valueTooltip->mapFromGlobal(QCursor::pos());

                if (rect.contains(pos))
                {
                    if (!m_valueTooltip->rect().contains(pos))
                        m_idleTimer.start();
                    return false;
                }

                delete m_valueTooltip;
                m_valueTooltip = nullptr;
            }

            m_idleTimer.start();

            break;
        }

        case QEvent::Show:
        {
            if (!m_bInitialized)
            {
#if !defined(_WIN32) && !defined(__APPLE__)
                const auto padding = px(9);
#else
                const auto padding = px(6);
#endif

                auto f = header()->font();
#if !defined(_WIN32) && !defined(__APPLE__)
                f.setBold(true);
#endif
                QFontMetrics fm(f);

                for (int i = 0; i < COL_COLUMNS_NUMBER; ++i)
                {
                    m_columnsMinimumWidth[i] = static_cast<int>(fm.width(headerItem()->text(i)) * profiler_gui::FONT_METRICS_FACTOR + padding);
                }

                EASY_CONSTEXPR int Margins = 20;
                setMinimumSize(m_hintLabel->width() + Margins, m_hintLabel->height() + header()->height() + Margins);
                m_bInitialized = true;
            }

            break;
        }

        default: break;
    }

    return false;
}

void BlocksTreeWidget::updateHintLabelOnHover(bool hover)
{
    profiler_gui::updateProperty(m_hintLabel, "hovered", hover);
}

void BlocksTreeWidget::onHeaderSectionResized(int logicalIndex, int /*oldSize*/, int newSize)
{
    const auto indicatorSize = header()->isSortIndicatorShown() && header()->sortIndicatorSection() == logicalIndex ? px(11) : 0;
    const auto minSize = m_columnsMinimumWidth[logicalIndex] + indicatorSize;

    if (!m_bInitialized || newSize >= minSize)
    {
        return;
    }

    header()->resizeSection(logicalIndex, minSize);
}

void BlocksTreeWidget::mousePressEvent(QMouseEvent* _event)
{
    delete m_valueTooltip;
    m_valueTooltip = nullptr;
    if (m_idleTimer.isActive())
        m_idleTimer.stop();
    m_idleTimer.start();

    Parent::mousePressEvent(_event);
}

//////////////////////////////////////////////////////////////////////////

void BlocksTreeWidget::onFillTimerTimeout()
{
    if (m_treeBuilder.done())
    {
        m_fillTimer.stop();

        ThreadedItems toplevelitems;
        m_treeBuilder.takeItems(m_items);
        m_treeBuilder.takeTopLevelItems(toplevelitems);
        auto error = m_treeBuilder.error();
        m_treeBuilder.interrupt();
        {
            const QSignalBlocker b(this);
            for (auto& item : toplevelitems)
            {
                addTopLevelItem(item.second);
                m_roots[item.first] = item.second;
            }
        }

        destroyProgressDialog();

        m_bLocked = false;
        m_inputBlocks.clear();

        setSortingEnabled(true);

        switch (m_mode)
        {
            case TreeMode::Full:
            {
                sortByColumn(COL_BEGIN, Qt::AscendingOrder); // sort by begin time
                break;
            }

            case TreeMode::Plain:
            {
                sortByColumn(COL_BEGIN, Qt::AscendingOrder); // sort by begin time
                sortByColumn(COL_PERCENT_PER_FRAME, Qt::DescendingOrder); // and after that, sort by frame %
                break;
            }

            case TreeMode::SelectedArea:
            {
                sortByColumn(COL_PERCENT_SUM_PER_AREA, Qt::DescendingOrder);
                break;
            }
        }

        //resizeColumnToContents(COL_NAME);
        resizeColumnsToContents();

        connect(this, &Parent::itemExpanded, this, &This::onItemExpand);
        connect(this, &Parent::itemCollapsed, this, &This::onItemCollapse);
        connect(this, &Parent::currentItemChanged, this, &This::onCurrentItemChange);
        connect(this, &Parent::itemDoubleClicked, this, &This::onItemDoubleClicked);
        onSelectedThreadChange(EASY_GLOBALS.selected_thread);
        onSelectedBlockChange(EASY_GLOBALS.selected_block);

        if (!error.isEmpty())
        {
            QMessageBox::warning(this, "Warning", error, QMessageBox::Close);
            clearSilent();
        }
    }
    else if (m_progress != nullptr)
    {
        m_progress->setValue(m_treeBuilder.progress());
    }
}

void BlocksTreeWidget::onIdleTimeout()
{
    // Close old tooltip
    delete m_valueTooltip;
    m_valueTooltip = nullptr;

    const auto pos = viewport()->mapFromGlobal(QCursor::pos());
    auto itemUnderCursor = itemAt(pos);
    if (itemUnderCursor == nullptr)
        return;

    auto item = static_cast<TreeWidgetItem*>(itemUnderCursor);
    if (profiler_gui::is_max(item->block_index()))
        return;

    const auto& block = item->block();
    const auto& desc = easyDescriptor(block.node->id());
    if (desc.type() != profiler::BlockType::Value)
        return;

    const int column = columnAt(pos.x());
    if (!item->data(column, Qt::ToolTipRole).isNull())
        return;

    auto focusWidget = qApp->focusWidget();
    while (focusWidget != nullptr && !focusWidget->property("stayVisible").toBool())
        focusWidget = focusWidget->parentWidget();

    if (focusWidget != nullptr)
        return;

    m_valueTooltip = new ArbitraryValueToolTip(itemUnderCursor->text(COL_NAME), block, this);
    m_valueTooltip->move(QCursor::pos());
    m_valueTooltip->show();

    // Actual size becomes valid only after show()
    m_valueTooltip->setFixedSize(m_valueTooltip->size());
}

void BlocksTreeWidget::setTreeBlocks(const profiler_gui::TreeBlocks& _blocks, profiler::timestamp_t _session_begin_time, profiler::timestamp_t _left, profiler::timestamp_t _right, bool _strict)
{
    clearSilent();

    m_beginTime = _session_begin_time;
    _left += m_beginTime;// - ::std::min(m_beginTime, 1000ULL);
    _right += m_beginTime;// + 1000;

    m_inputBlocks = _blocks;
    if (!m_inputBlocks.empty())
    {
        m_bLocked = true;
        m_hintLabel->hide();
        createProgressDialog();
        m_treeBuilder.fillTreeBlocks(m_inputBlocks, _session_begin_time, _left, _right, _strict, m_mode);
        m_fillTimer.start(TREE_BUILDER_TIMER_INTERVAL);
    }

    //StubLocker l;
    //ThreadedItems toplevelitems;
    //FillTreeClass<StubLocker>::setTreeInternal2(l, m_items, toplevelitems, m_beginTime, _blocks, _left, _right, _strict, m_bColorRows);
    //{
    //    const QSignalBlocker b(this);
    //    for (auto& item : toplevelitems)
    //    {
    //        addTopLevelItem(item.second);
    //        m_roots[item.first] = item.second;
    //        if (item.first == EASY_GLOBALS.selected_thread)
    //            item.second->setMain(true);
    //    }
    //}

    //setSortingEnabled(true);
    //sortByColumn(COL_BEGIN, Qt::AscendingOrder);
    //resizeColumnToContents(COL_NAME);

    //connect(this, &Parent::itemExpanded, this, &This::onItemExpand);
    //connect(this, &Parent::itemCollapsed, this, &This::onItemCollapse);
    //onSelectedBlockChange(EASY_GLOBALS.selected_block);
}

//////////////////////////////////////////////////////////////////////////

void BlocksTreeWidget::clearSilent(bool _global)
{
    const QSignalBlocker b(this);

    m_treeBuilder.interrupt();
    destroyProgressDialog();
    m_hintLabel->show();

    m_bLocked = false;
    m_beginTime = ::std::numeric_limits<decltype(m_beginTime)>::max();

    setSortingEnabled(false);
    disconnect(this, &Parent::itemExpanded, this, &This::onItemExpand);
    disconnect(this, &Parent::itemCollapsed, this, &This::onItemCollapse);
    disconnect(this, &Parent::currentItemChanged, this, &This::onCurrentItemChange);
    disconnect(this, &Parent::itemDoubleClicked, this, &This::onItemDoubleClicked);
    resetSearch(false);

    if (!_global && EASY_GLOBALS.collapse_items_on_tree_close)
    {
        for (auto& item : m_items)
        {
            item.second->guiBlock().expanded = false;
        }
    }

    m_items.clear();
    m_roots.clear();

    if (topLevelItemCount() != 0)
    {
        std::vector<QTreeWidgetItem*> topLevelItems;
        topLevelItems.reserve(static_cast<size_t>(topLevelItemCount()));
        for (int i = topLevelItemCount() - 1; i >= 0; --i)
            topLevelItems.push_back(takeTopLevelItem(i));

#ifdef EASY_LAMBDA_MOVE_CAPTURE
        ThreadPool::instance().backgroundJob([items = std::move(topLevelItems)] {
            for (auto item : items)
#else
        ThreadPool::instance().backgroundJob([topLevelItems] {
            for (auto item : topLevelItems)
#endif
                profiler_gui::deleteTreeItem(item);
        });
    }

    //clear();

    if (!_global)
        emit EASY_GLOBALS.events.itemsExpandStateChanged();
}

//////////////////////////////////////////////////////////////////////////

void BlocksTreeWidget::resetSearch(bool repaint)
{
    if (m_lastSearch.isEmpty())
    {
        return;
    }

    m_bCaseSensitiveSearch = false;
    m_lastSearch.clear();
    m_lastFound = nullptr;
    m_lastFoundIndex = 0;

    if (repaint)
    {
        viewport()->update();
    }
}

QTreeWidgetItem* BlocksTreeWidget::lastFoundItem() const
{
    return m_lastFound;
}

bool BlocksTreeWidget::caseSensitiveSearch() const
{
    return m_bCaseSensitiveSearch;
}

const QString& BlocksTreeWidget::searchString() const
{
    return m_lastSearch;
}

int BlocksTreeWidget::lastFoundIndex() const
{
    return m_lastFoundIndex;
}

int BlocksTreeWidget::findNext(const QString& _str, Qt::MatchFlags _flags)
{
    if (m_bLocked)
    {
        return 0;
    }

    if (_str.isEmpty())
    {
        resetSearch();
        return 0;
    }

    const bool isNewSearch = (m_lastSearch != _str);
    auto itemsList = findItems(_str, Qt::MatchContains | Qt::MatchRecursive | _flags, COL_NAME);

    m_bCaseSensitiveSearch = _flags.testFlag(Qt::MatchCaseSensitive);

    if (!isNewSearch)
    {
        if (!itemsList.empty())
        {
            bool stop = false;
            int i = 0;
            decltype(m_lastFound) next = nullptr;
            for (auto item : itemsList)
            {
                if (stop)
                {
                    next = item;
                    break;
                }

                stop = item == m_lastFound;
                ++i;
            }

            m_lastFound = next == nullptr ? itemsList.front() : next;
            m_lastFoundIndex = next == nullptr ? 0 : i;
        }
        else
        {
            m_lastFound = nullptr;
            m_lastFoundIndex = 0;
        }
    }
    else
    {
        m_lastSearch = _str;
        m_lastFound = !itemsList.empty() ? itemsList.front() : nullptr;
        m_lastFoundIndex = 0;
    }

    if (m_lastFound != nullptr)
    {
        scrollToItem(m_lastFound, QAbstractItemView::PositionAtCenter);
        setCurrentItem(m_lastFound);
    }

    viewport()->update();

    return itemsList.size();
}

int BlocksTreeWidget::findPrev(const QString& _str, Qt::MatchFlags _flags)
{
    if (m_bLocked)
    {
        return 0;
    }

    if (_str.isEmpty())
    {
        resetSearch();
        return 0;
    }

    const bool isNewSearch = (m_lastSearch != _str);
    auto itemsList = findItems(_str, Qt::MatchContains | Qt::MatchRecursive | _flags, COL_NAME);

    m_bCaseSensitiveSearch = _flags.testFlag(Qt::MatchCaseSensitive);

    if (!isNewSearch)
    {
        if (!itemsList.empty())
        {
            int i = 0;
            decltype(m_lastFound) prev = nullptr;
            for (auto item : itemsList)
            {
                if (item == m_lastFound)
                {
                    --i;
                    break;
                }

                prev = item;
                ++i;
            }

            m_lastFound = prev == nullptr ? itemsList.back() : prev;
            m_lastFoundIndex = prev == nullptr ? itemsList.length() - 1 : i;
        }
        else
        {
            m_lastFound = nullptr;
            m_lastFoundIndex = 0;
        }
    }
    else
    {
        m_lastSearch = _str;
        if (!itemsList.empty()) {
            m_lastFound = itemsList.back();
            m_lastFoundIndex = itemsList.length() - 1;
        }
        else
        {
            m_lastFound = nullptr;
            m_lastFoundIndex = 0;
        }
    }

    if (m_lastFound != nullptr)
    {
        scrollToItem(m_lastFound, QAbstractItemView::PositionAtCenter);
        setCurrentItem(m_lastFound);
    }

    viewport()->update();

    return itemsList.size();
}

//////////////////////////////////////////////////////////////////////////

void BlocksTreeWidget::contextMenuEvent(QContextMenuEvent* _event)
{
    if (m_bLocked)
    {
        _event->accept();
        return;
    }

    delete m_valueTooltip;
    m_valueTooltip = nullptr;
    if (m_idleTimer.isActive())
        m_idleTimer.stop();

    const auto col = currentColumn();
    auto item = static_cast<TreeWidgetItem*>(currentItem());
    QMenu menu;
    menu.setToolTipsVisible(true);
    QAction* action = nullptr;

    if (!m_items.empty())
    {
        action = menu.addAction("Expand All");
        connect(action, &QAction::triggered, this, &This::onExpandAllClicked);
        action->setIcon(QIcon(imagePath("expand")));

        action = menu.addAction("Collapse All");
        connect(action, &QAction::triggered, this, &This::onCollapseAllClicked);
        action->setIcon(QIcon(imagePath("collapse")));

        if (item != nullptr && col >= 0)
        {
            menu.addSeparator();

            action = menu.addAction("Expand All Children");
            connect(action, &QAction::triggered, this, &This::onExpandAllChildrenClicked);
            action->setIcon(QIcon(imagePath("expand")));

            action = menu.addAction("Collapse All Children");
            connect(action, &QAction::triggered, this, &This::onCollapseAllChildrenClicked);
            action->setIcon(QIcon(imagePath("collapse")));
        }

        menu.addSeparator();
    }

    auto actionGroup = new QActionGroup(&menu);
    actionGroup->setExclusive(true);

    auto actionHierarchy = new QAction("Call-Stack", actionGroup);
    actionHierarchy->setCheckable(true);
    actionHierarchy->setChecked(m_mode == TreeMode::Full);
    actionHierarchy->setToolTip("Display Full Call Stack");
    actionHierarchy->setData((quint32)TreeMode::Full);
    menu.addAction(actionHierarchy);

    auto actionPlain = new QAction("Per-Frame Stats", actionGroup);
    actionPlain->setCheckable(true);
    actionPlain->setChecked(m_mode == TreeMode::Plain);
    actionPlain->setToolTip("Display plain list of blocks per frame.\nSome columns are disabled with this mode.");
    actionPlain->setData((quint32)TreeMode::Plain);
    menu.addAction(actionPlain);

    auto actionSelectedArea = new QAction("Aggregate Stats", actionGroup);
    actionSelectedArea->setCheckable(true);
    actionSelectedArea->setChecked(m_mode == TreeMode::SelectedArea);
    actionSelectedArea->setToolTip("Display aggregate stats for selected area.\nSome columns are disabled with this mode.");
    actionSelectedArea->setData((quint32)TreeMode::SelectedArea);
    menu.addAction(actionSelectedArea);

    connect(actionHierarchy, &QAction::triggered, this, &This::onModeChange);
    connect(actionPlain, &QAction::triggered, this, &This::onModeChange);
    connect(actionSelectedArea, &QAction::triggered, this, &This::onModeChange);

    menu.addSeparator();

    if (item != nullptr && item->parent() != nullptr)
    {
        if (col >= 0)
        {
            switch (col)
            {
                case COL_MIN_PER_THREAD:
                case COL_MIN_PER_PARENT:
                case COL_MIN_PER_FRAME:
                case COL_MIN_PER_AREA:
                case COL_MAX_PER_THREAD:
                case COL_MAX_PER_PARENT:
                case COL_MAX_PER_FRAME:
                case COL_MAX_PER_AREA:
                {
                    auto& block = item->block();
                    auto i = profiler_gui::numeric_max<uint32_t>();
                    QString name;
                    switch (col)
                    {
                        case COL_MIN_PER_THREAD: name = QStringLiteral("Min"); i = block.per_thread_stats->min_duration_block; break;
                        case COL_MIN_PER_PARENT: name = QStringLiteral("Min"); i = block.per_parent_stats->min_duration_block; break;
                        case COL_MIN_PER_FRAME:  name = QStringLiteral("Min"); i = block.per_frame_stats->min_duration_block; break;
                        case COL_MAX_PER_THREAD: name = QStringLiteral("Max"); i = block.per_thread_stats->max_duration_block; break;
                        case COL_MAX_PER_PARENT: name = QStringLiteral("Max"); i = block.per_parent_stats->max_duration_block; break;
                        case COL_MAX_PER_FRAME:  name = QStringLiteral("Max"); i = block.per_frame_stats->max_duration_block; break;

                        case COL_MIN_PER_AREA:
                        {
                            name = QStringLiteral("Min");
                            auto data = item->data(COL_MIN_PER_AREA, MinMaxBlockIndexRole);
                            if (!data.isNull())
                                i = data.toUInt();
                            break;
                        }

                        case COL_MAX_PER_AREA:
                        {
                            name = QStringLiteral("Max");
                            auto data = item->data(COL_MAX_PER_AREA, MinMaxBlockIndexRole);
                            if (!data.isNull())
                                i = data.toUInt();
                            break;
                        }
                    }

                    if (i != profiler_gui::numeric_max(i))
                    {
                        menu.addSeparator();
                        auto itemAction = new QAction(QString("Jump To %1 Item").arg(name), nullptr);
                        itemAction->setData(i);
                        itemAction->setToolTip(QString("Jump to item with %1 duration").arg(name.toLower()));
                        connect(itemAction, &QAction::triggered, this, &This::onJumpToItemClicked);
                        menu.addAction(itemAction);
                    }

                    break;
                }

                default:
                    break;
            }
        }

        const auto& desc = easyDescriptor(item->block().node->id());
        auto submenu = menu.addMenu("Block Status");
        submenu->setToolTipsVisible(true);

#define ADD_STATUS_ACTION(NameValue, StatusValue, ToolTipValue)\
        action = submenu->addAction(NameValue);\
        action->setCheckable(true);\
        action->setChecked(desc.status() == StatusValue);\
        action->setData(static_cast<quint32>(StatusValue));\
        action->setToolTip(ToolTipValue);\
        connect(action, &QAction::triggered, this, &This::onBlockStatusChangeClicked)

        ADD_STATUS_ACTION("Off", profiler::OFF, "Do not profile this block.");
        ADD_STATUS_ACTION("On", profiler::ON, "Profile this block\nif parent enabled children.");
        ADD_STATUS_ACTION("Force-On", profiler::FORCE_ON, "Always profile this block even\nif it's parent disabled children.");
        ADD_STATUS_ACTION("Off-recursive", profiler::OFF_RECURSIVE, "Do not profile neither this block\nnor it's children.");
        ADD_STATUS_ACTION("On-without-children", profiler::ON_WITHOUT_CHILDREN, "Profile this block, but\ndo not profile it's children.");
        ADD_STATUS_ACTION("Force-On-without-children", profiler::FORCE_ON_WITHOUT_CHILDREN, "Always profile this block, but\ndo not profile it's children.");
#undef ADD_STATUS_ACTION

        submenu->setEnabled(EASY_GLOBALS.connected);
        if (!EASY_GLOBALS.connected)
            submenu->setTitle(QString("%1 (connection needed)").arg(submenu->title()));
    }

    auto hidemenu = menu.addMenu("Select Columns");
    auto hdr = headerItem();

#define ADD_COLUMN_ACTION(i) \
    { \
        auto columnAction = new QAction(hdr->text(i), nullptr); \
        columnAction->setData(i); \
        columnAction->setCheckable(true); \
        columnAction->setChecked(m_columnsHiddenStatus[i] == 0); \
        if ((m_mode == TreeMode::Full || (m_mode == TreeMode::Plain && PLAIN_MODE_COLUMNS[i]) || (m_mode == TreeMode::SelectedArea && SELECTION_MODE_COLUMNS[i]))) \
            connect(columnAction, &QAction::triggered, this, &This::onHideShowColumn); \
        else \
            columnAction->setEnabled(false); \
        hidemenu->addAction(columnAction); \
    }

    ADD_COLUMN_ACTION(COL_BEGIN);
    ADD_COLUMN_ACTION(COL_END);
    ADD_COLUMN_ACTION(COL_TIME);
    ADD_COLUMN_ACTION(COL_SELF_TIME);
    ADD_COLUMN_ACTION(COL_SELF_TIME_PERCENT);

    hidemenu->addSeparator();

    ADD_COLUMN_ACTION(COL_TOTAL_TIME_PER_FRAME);
    ADD_COLUMN_ACTION(COL_PERCENT_SUM_PER_FRAME);
    ADD_COLUMN_ACTION(COL_PERCENT_PER_FRAME);
    ADD_COLUMN_ACTION(COL_MIN_PER_FRAME);
    ADD_COLUMN_ACTION(COL_MAX_PER_FRAME);
    ADD_COLUMN_ACTION(COL_AVG_PER_FRAME);
    ADD_COLUMN_ACTION(COL_MEDIAN_PER_FRAME);
    ADD_COLUMN_ACTION(COL_NCALLS_PER_FRAME);

    hidemenu->addSeparator();

    ADD_COLUMN_ACTION(COL_TOTAL_TIME_PER_THREAD);
    ADD_COLUMN_ACTION(COL_PERCENT_SUM_PER_THREAD);
    ADD_COLUMN_ACTION(COL_MIN_PER_THREAD);
    ADD_COLUMN_ACTION(COL_MAX_PER_THREAD);
    ADD_COLUMN_ACTION(COL_AVG_PER_THREAD);
    ADD_COLUMN_ACTION(COL_MEDIAN_PER_THREAD);
    ADD_COLUMN_ACTION(COL_NCALLS_PER_THREAD);

    hidemenu->addSeparator();

    ADD_COLUMN_ACTION(COL_TOTAL_TIME_PER_PARENT);
    ADD_COLUMN_ACTION(COL_PERCENT_SUM_PER_PARENT);
    ADD_COLUMN_ACTION(COL_PERCENT_PER_PARENT);
    ADD_COLUMN_ACTION(COL_MIN_PER_PARENT);
    ADD_COLUMN_ACTION(COL_MAX_PER_PARENT);
    ADD_COLUMN_ACTION(COL_AVG_PER_PARENT);
    ADD_COLUMN_ACTION(COL_MEDIAN_PER_PARENT);
    ADD_COLUMN_ACTION(COL_NCALLS_PER_PARENT);

    hidemenu->addSeparator();

    ADD_COLUMN_ACTION(COL_ACTIVE_TIME);
    ADD_COLUMN_ACTION(COL_ACTIVE_PERCENT);

    hidemenu->addSeparator();

    ADD_COLUMN_ACTION(COL_TOTAL_TIME_PER_AREA);
    ADD_COLUMN_ACTION(COL_PERCENT_SUM_PER_AREA);
    ADD_COLUMN_ACTION(COL_PERCENT_PER_AREA);
    ADD_COLUMN_ACTION(COL_MIN_PER_AREA);
    ADD_COLUMN_ACTION(COL_MAX_PER_AREA);
    ADD_COLUMN_ACTION(COL_AVG_PER_AREA);
    ADD_COLUMN_ACTION(COL_MEDIAN_PER_AREA);
    ADD_COLUMN_ACTION(COL_NCALLS_PER_AREA);

#undef ADD_STATUS_ACTION

    menu.exec(QCursor::pos());

    _event->accept();
}

//////////////////////////////////////////////////////////////////////////

void BlocksTreeWidget::onItemDoubleClicked(QTreeWidgetItem* _item, int _column)
{
    delete m_valueTooltip;
    m_valueTooltip = nullptr;
    if (m_idleTimer.isActive())
        m_idleTimer.stop();

    if (_item == nullptr || _column != COL_NAME || _item == headerItem())
    {
        return;
    }

    auto item = static_cast<TreeWidgetItem*>(_item);
    if (item->parent() != nullptr)
    {
        if (easyDescriptor(item->block()).type() == profiler::BlockType::Value)
        {
            // TODO: open dialog to view values
        }
    }
}

//////////////////////////////////////////////////////////////////////////

void BlocksTreeWidget::resizeEvent(QResizeEvent* _event)
{
    Parent::resizeEvent(_event);
    alignProgressBar();
}

void BlocksTreeWidget::moveEvent(QMoveEvent* _event)
{
    Parent::moveEvent(_event);
    alignProgressBar();
}

void BlocksTreeWidget::alignProgressBar()
{
    const auto scrollbarHeight = verticalScrollBar()->isVisible() ? verticalScrollBar()->height() : 0;
    const auto center = rect().adjusted(0, header()->height(), 0, -scrollbarHeight - 6).center();

    if (m_progress != nullptr)
    {
        const auto& pos = center;
        m_progress->move(pos.x() - (m_progress->width() >> 1),
                         std::max(pos.y() - (m_progress->height() >> 1), header()->height()));
        m_progress->update();
    }

    m_hintLabel->move(center.x() - (m_hintLabel->width() >> 1),
                      std::max(center.y() - (m_hintLabel->height() >> 1), header()->height()));
}

void BlocksTreeWidget::destroyProgressDialog()
{
    if (m_progress != nullptr)
    {
        m_progress->setValue(100);
        m_progress->deleteLater();
        m_progress = nullptr;
    }
}

void BlocksTreeWidget::createProgressDialog()
{
    destroyProgressDialog();

    m_progress = new RoundProgressDialog(QStringLiteral("Building tree..."), this);
    m_progress->setValue(0);
    m_progress->show();

    alignProgressBar();
}

//////////////////////////////////////////////////////////////////////////

void BlocksTreeWidget::onJumpToItemClicked(bool)
{
    auto action = qobject_cast<QAction*>(sender());
    if (action == nullptr)
        return;

    auto block_index = action->data().toUInt();
    EASY_GLOBALS.selected_block = block_index;
    if (block_index < EASY_GLOBALS.gui_blocks.size())
        EASY_GLOBALS.selected_block_id = easyBlock(block_index).tree.node->id();
    else
        profiler_gui::set_max(EASY_GLOBALS.selected_block_id);
    emit EASY_GLOBALS.events.selectedBlockChanged(block_index);
}

void BlocksTreeWidget::onCollapseAllClicked(bool)
{
    const QSignalBlocker b(this);

    m_bSilentExpandCollapse = true;
    collapseAll();
    m_bSilentExpandCollapse = false;

    if (EASY_GLOBALS.bind_scene_and_tree_expand_status)
    {
        for (auto& item : m_items)
            item.second->guiBlock().expanded = false;
        emit EASY_GLOBALS.events.itemsExpandStateChanged();
    }
}

void BlocksTreeWidget::onExpandAllClicked(bool)
{
    const QSignalBlocker blocker(this);

    m_bSilentExpandCollapse = true;
    expandAll();
    resizeColumnsToContents();
    m_bSilentExpandCollapse = false;

    if (EASY_GLOBALS.bind_scene_and_tree_expand_status)
    {
        for (auto& item : m_items)
        {
            auto& b = item.second->guiBlock();
            b.expanded = !b.tree.children.empty();
        }

        emit EASY_GLOBALS.events.itemsExpandStateChanged();
    }
}

void BlocksTreeWidget::onCollapseAllChildrenClicked(bool)
{
    auto current = static_cast<TreeWidgetItem*>(currentItem());
    if (current != nullptr)
    {
        const QSignalBlocker b(this);

        m_bSilentExpandCollapse = true;
        current->collapseAll();
        m_bSilentExpandCollapse = false;

        emit EASY_GLOBALS.events.itemsExpandStateChanged();
    }
}

void BlocksTreeWidget::onExpandAllChildrenClicked(bool)
{
    auto current = static_cast<TreeWidgetItem*>(currentItem());
    if (current != nullptr)
    {
        const QSignalBlocker b(this);

        m_bSilentExpandCollapse = true;
        current->expandAll();
        resizeColumnsToContents();
        m_bSilentExpandCollapse = false;

        emit EASY_GLOBALS.events.itemsExpandStateChanged();
    }
}

//////////////////////////////////////////////////////////////////////////

void BlocksTreeWidget::onBlockStatusChangeClicked(bool _checked)
{
    if (!_checked)
        return;

    auto item = static_cast<TreeWidgetItem*>(currentItem());
    if (item == nullptr)
        return;

    auto action = qobject_cast<QAction*>(sender());
    if (action != nullptr)
    {
        auto& desc = easyDescriptor(item->block().node->id());
        desc.setStatus(static_cast<profiler::EasyBlockStatus>(action->data().toUInt()));
        emit EASY_GLOBALS.events.blockStatusChanged(desc.id(), desc.status());
    }
}

//////////////////////////////////////////////////////////////////////////

void BlocksTreeWidget::onItemExpand(QTreeWidgetItem* _item)
{
    if (!EASY_GLOBALS.bind_scene_and_tree_expand_status || _item->parent() == nullptr)
    {
        resizeColumnsToContents();
        return;
    }

    static_cast<TreeWidgetItem*>(_item)->guiBlock().expanded = true;

    if (!m_bSilentExpandCollapse)
    {
        resizeColumnsToContents();
        emit EASY_GLOBALS.events.itemsExpandStateChanged();
    }
}

void BlocksTreeWidget::onItemCollapse(QTreeWidgetItem* _item)
{
    if (!EASY_GLOBALS.bind_scene_and_tree_expand_status || _item->parent() == nullptr)
        return;

    static_cast<TreeWidgetItem*>(_item)->guiBlock().expanded = false;

    if (!m_bSilentExpandCollapse)
        emit EASY_GLOBALS.events.itemsExpandStateChanged();
}

//////////////////////////////////////////////////////////////////////////

void BlocksTreeWidget::onCurrentItemChange(QTreeWidgetItem* _item, QTreeWidgetItem* _previous)
{
    (void)_previous;

    if (_item == nullptr)
    {
        profiler_gui::set_max(EASY_GLOBALS.selected_block);
        profiler_gui::set_max(EASY_GLOBALS.selected_block_id);
    }
    else
    {
        auto item = static_cast<TreeWidgetItem*>(_item);

        EASY_GLOBALS.selected_block = item->block_index();
        if (EASY_GLOBALS.selected_block < EASY_GLOBALS.gui_blocks.size())
            EASY_GLOBALS.selected_block_id = easyBlock(EASY_GLOBALS.selected_block).tree.node->id();
        else
            profiler_gui::set_max(EASY_GLOBALS.selected_block_id);
    }

    disconnect(&EASY_GLOBALS.events, &profiler_gui::GlobalSignals::selectedBlockChanged, this, &This::onSelectedBlockChange);
    emit EASY_GLOBALS.events.selectedBlockChanged(EASY_GLOBALS.selected_block);
    connect(&EASY_GLOBALS.events, &profiler_gui::GlobalSignals::selectedBlockChanged, this, &This::onSelectedBlockChange);
}

//////////////////////////////////////////////////////////////////////////

void BlocksTreeWidget::onSelectedThreadChange(profiler::thread_id_t _id)
{
    for (auto& it : m_roots)
    {
        auto item = it.second;
        item->setMain(it.first == _id);
    }

    // Calling update() or repaint() (or both!) does not work even if setUpdatesEnabled(true) have been set in constructor.
    // Have to set focus to this widget to force update/repaint. :(
    // TODO: Find valid solution instead of this workaround.
    auto f = qApp->focusWidget();
    setFocus();
    if (f != nullptr)
        f->setFocus();
}

void BlocksTreeWidget::onSelectedBlockChange(uint32_t _block_index)
{
    disconnect(this, &Parent::currentItemChanged, this, &This::onCurrentItemChange);

    TreeWidgetItem* item = nullptr;

    if (_block_index < EASY_GLOBALS.gui_blocks.size())
    {
        auto it = m_items.find(_block_index);
        if (it != m_items.end())
        {
            item = it->second;
        }
        else if (m_mode != TreeMode::Full)
        {
            const auto currentThread = EASY_GLOBALS.selected_thread;
            for (auto& itemPair : m_items)
            {
                auto blockItem = itemPair.second;
                if (blockItem->parent() != nullptr && blockItem->threadId() == currentThread)
                {
                    const auto id = blockItem->block().node->id();
                    if (id == EASY_GLOBALS.selected_block_id || easyDescriptor(id).id() == EASY_GLOBALS.selected_block_id)
                    {
                        item = blockItem;
                        break;
                    }
                }
            }
        }
    }

    if (item != nullptr)
    {
        //const QSignalBlocker b(this);

        if (EASY_GLOBALS.bind_scene_and_tree_expand_status)
        {
            m_bSilentExpandCollapse = true;
            setCurrentItem(item);
            scrollToItem(item, QAbstractItemView::PositionAtCenter);
            if (item->guiBlock().expanded)
                expandItem(item);
            else
                collapseItem(item);
            resizeColumnsToContents();
            m_bSilentExpandCollapse = false;

            emit EASY_GLOBALS.events.itemsExpandStateChanged();
        }
        else
        {
            disconnect(this, &Parent::itemExpanded, this, &This::onItemExpand);
            setCurrentItem(item);
            scrollToItem(item, QAbstractItemView::PositionAtCenter);
            resizeColumnsToContents();
            connect(this, &Parent::itemExpanded, this, &This::onItemExpand);
        }
    }
    else
    {
        setCurrentItem(item);
    }

    connect(this, &Parent::currentItemChanged, this, &This::onCurrentItemChange);
}

//////////////////////////////////////////////////////////////////////////

void BlocksTreeWidget::resizeColumnsToContents()
{
    for (int i = 0; i < COL_COLUMNS_NUMBER; ++i)
    {
        if (!isColumnHidden(i))
            resizeColumnToContents(i);
    }
}

//////////////////////////////////////////////////////////////////////////

void BlocksTreeWidget::onHideShowColumn(bool)
{
    auto action = qobject_cast<QAction*>(sender());
    if (action == nullptr)
        return;

    const auto col = action->data().toInt();
    const bool hideCol = m_columnsHiddenStatus[col] == 0;
    setColumnHidden(col, hideCol);
    m_columnsHiddenStatus[col] = static_cast<char>(hideCol ? 1 : 0);
}

void BlocksTreeWidget::onModeChange(bool)
{
    auto action = qobject_cast<QAction*>(sender());
    if (action == nullptr)
        return;

    const auto prev = m_mode;
    m_mode = static_cast<TreeMode>(action->data().toUInt());

    if (m_mode == prev)
        return;

    if (m_mode == TreeMode::Full)
    {
        for (int i = 1; i < COL_COLUMNS_NUMBER; ++i)
        {
            setColumnHidden(i, m_columnsHiddenStatus[i] != 0);
        }
    }
    else if (m_mode == TreeMode::Plain)
    {
        for (int i = 1; i < COL_COLUMNS_NUMBER; ++i)
        {
            setColumnHidden(i, m_columnsHiddenStatus[i] != 0 || !PLAIN_MODE_COLUMNS[i]);
        }
    }
    else
    {
        for (int i = 1; i < COL_COLUMNS_NUMBER; ++i)
        {
            setColumnHidden(i, m_columnsHiddenStatus[i] != 0 || !SELECTION_MODE_COLUMNS[i]);
        }
    }

    emit EASY_GLOBALS.events.blocksTreeModeChanged();
}

//////////////////////////////////////////////////////////////////////////

void BlocksTreeWidget::loadSettings()
{
    QSettings settings(profiler_gui::ORGANAZATION_NAME, profiler_gui::APPLICATION_NAME);
    settings.beginGroup("tree_widget");

    auto val = settings.value("regime");
    if (!val.isNull())
    {
        m_mode = static_cast<TreeMode>(val.toUInt());
    }

    val = settings.value("columns_version");
    if (!val.isNull() && val.toInt() == COLUMNS_VERSION)
    {
        val = settings.value("columns");
        if (!val.isNull())
        {
            auto byteArray = val.toByteArray();
            memcpy(
                m_columnsHiddenStatus, byteArray.constData(), std::min(sizeof(m_columnsHiddenStatus), (size_t)byteArray.size())
            );
        }

        auto state = settings.value("headerState").toByteArray();
        if (!state.isEmpty())
        {
            header()->restoreState(state);
        }
    }

    settings.endGroup();
}

void BlocksTreeWidget::saveSettings()
{
    QSettings settings(profiler_gui::ORGANAZATION_NAME, profiler_gui::APPLICATION_NAME);
    settings.beginGroup("tree_widget");
    settings.setValue("regime", static_cast<uint8_t>(m_mode));
    settings.setValue("columns_version", COLUMNS_VERSION);
    settings.setValue("columns", QByteArray(m_columnsHiddenStatus, COL_COLUMNS_NUMBER));
    settings.setValue("headerState", header()->saveState());
    settings.endGroup();
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

StatsWidget::StatsWidget(QWidget* _parent) : Parent(_parent)
    , m_tree(new BlocksTreeWidget(this))
    , m_searchBox(new QLineEdit(this))
    , m_foundNumber(new QLabel(QStringLiteral("<font color=\"red\">0</font> matches"), this))
    , m_searchButton(nullptr)
    , m_bCaseSensitiveSearch(false)
{
    loadSettings();

    m_searchBox->setContentsMargins(5, 0, 0, 0);
    m_searchBox->setClearButtonEnabled(true);
    m_searchBox->setPlaceholderText("Search by name");

    auto menu = new QMenu(this);
    m_searchButton = menu->menuAction();
    m_searchButton->setText("Find next");
    m_searchButton->setIcon(QIcon(imagePath("find-next")));
    m_searchButton->setData(true);
    connect(m_searchButton, &QAction::triggered, this, &This::findNext);

    auto actionGroup = new QActionGroup(this);
    actionGroup->setExclusive(true);

    auto a = new QAction(tr("Find next"), actionGroup);
    a->setCheckable(true);
    a->setChecked(true);
    connect(a, &QAction::triggered, this, &This::findNextFromMenu);
    menu->addAction(a);

    a = new QAction(tr("Find previous"), actionGroup);
    a->setCheckable(true);
    connect(a, &QAction::triggered, this, &This::findPrevFromMenu);
    menu->addAction(a);

    a = menu->addAction("Case sensitive");
    a->setCheckable(true);
    a->setChecked(m_bCaseSensitiveSearch);
    connect(a, &QAction::triggered, [this](bool _checked){ m_bCaseSensitiveSearch = _checked; });
    menu->addAction(a);

    auto tb = new QToolBar(this);
    tb->setIconSize(applicationIconsSize());
    tb->setContentsMargins(0, 0, 0, 0);
    tb->addAction(m_searchButton);
    tb->addWidget(m_searchBox);

    auto searchbox = new QHBoxLayout();
    searchbox->setContentsMargins(0, 0, 5, 0);
    searchbox->addWidget(tb);
    searchbox->addSpacing(5);
    searchbox->addWidget(m_foundNumber);
    searchbox->addStretch(100);

    auto lay = new QVBoxLayout(this);
    lay->setContentsMargins(1, 1, 1, 1);
    lay->addLayout(searchbox);
    lay->addWidget(m_tree);

    connect(m_searchBox, &QLineEdit::returnPressed, this, &This::onSeachBoxReturnPressed);
    connect(m_searchBox, &QLineEdit::textChanged, this, &This::onSearchBoxTextChanged);

    connect(&EASY_GLOBALS.events, &profiler_gui::GlobalSignals::allDataGoingToBeDeleted, [this] {
        clear(true);
    });

    m_foundNumber->hide();
}

StatsWidget::~StatsWidget()
{
    saveSettings();
}

void StatsWidget::loadSettings()
{
    QSettings settings(profiler_gui::ORGANAZATION_NAME, profiler_gui::APPLICATION_NAME);
    settings.beginGroup("HierarchyWidget");

    auto val = settings.value("case_sensitive");
    if (!val.isNull())
        m_bCaseSensitiveSearch = val.toBool();

    settings.endGroup();
}

void StatsWidget::saveSettings()
{
    QSettings settings(profiler_gui::ORGANAZATION_NAME, profiler_gui::APPLICATION_NAME);
    settings.beginGroup("HierarchyWidget");
    settings.setValue("case_sensitive", m_bCaseSensitiveSearch);
    settings.endGroup();
}

void StatsWidget::enterEvent(QEvent* event)
{
    Parent::enterEvent(event);
    m_tree->updateHintLabelOnHover(true);
}

void StatsWidget::leaveEvent(QEvent* event)
{
    Parent::leaveEvent(event);
    m_tree->updateHintLabelOnHover(false);
}

void StatsWidget::keyPressEvent(QKeyEvent* _event)
{
    switch (_event->key())
    {
        case Qt::Key_F3:
        {
            if (_event->modifiers() & Qt::ShiftModifier)
                findPrev(true);
            else
                findNext(true);
            break;
        }

        case Qt::Key_Escape:
        {
            m_searchBox->clear();
            break;
        }

        default: break;
    }

    _event->accept();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void StatsWidget::contextMenuEvent(QContextMenuEvent* _event)
{
    m_tree->contextMenuEvent(_event);
}

void StatsWidget::showEvent(QShowEvent* event)
{
    Parent::showEvent(event);
    m_searchBox->setFixedWidth(px(300));
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

BlocksTreeWidget* StatsWidget::tree()
{
    return m_tree;
}

void StatsWidget::clear(bool _global)
{
    m_tree->clearSilent(_global);
    m_foundNumber->setText(QStringLiteral("<font color=\"red\">0</font> matches"));
    m_foundNumber->hide();
}

void StatsWidget::onSeachBoxReturnPressed()
{
    if (m_searchButton->data().toBool())
        findNext(true);
    else
        findPrev(true);
}

void StatsWidget::onSearchBoxTextChanged(const QString& _text)
{
    if (_text.isEmpty())
    {
        m_foundNumber->hide();
        m_tree->resetSearch();
    }
}

void StatsWidget::findNext(bool)
{
    auto text = m_searchBox->text();
    if (text.isEmpty())
    {
        if (m_foundNumber->isVisible())
            m_foundNumber->hide();
        m_tree->resetSearch();
        return;
    }

    auto matches = m_tree->findNext(text, m_bCaseSensitiveSearch ? Qt::MatchCaseSensitive : Qt::MatchFlags());

    if (matches == 0)
    {
        m_foundNumber->setText(QStringLiteral("<font color=\"red\">0</font> matches"));
    }
    else if (matches == 1)
    {
        m_foundNumber->setText(QStringLiteral("<font color=\"#f5f5f5\" style=\"background:#e040fb\">&nbsp;1&nbsp;</font> match"));
    }
    else
    {
        auto i = m_tree->lastFoundIndex() + 1;
        m_foundNumber->setText(QString("<font color=\"#f5f5f5\" style=\"background:#e040fb\">&nbsp;%1&nbsp;</font> of "
                                       "<font style=\"background:#ffeb3b\">&nbsp;%2&nbsp;</font> matches")
                                       .arg(i).arg(matches));
    }

    if (!m_foundNumber->isVisible())
        m_foundNumber->show();
}

void StatsWidget::findPrev(bool)
{
    auto text = m_searchBox->text();
    if (text.isEmpty())
    {
        if (m_foundNumber->isVisible())
            m_foundNumber->hide();
        m_tree->resetSearch();
        return;
    }

    auto matches = m_tree->findPrev(text, m_bCaseSensitiveSearch ? Qt::MatchCaseSensitive : Qt::MatchFlags());

    if (matches == 0)
    {
        m_foundNumber->setText(QStringLiteral("<font color=\"red\">0</font> matches"));
    }
    else if (matches == 1)
    {
        m_foundNumber->setText(QStringLiteral("<font color=\"#f5f5f5\" style=\"background:#e040fb\">&nbsp;1&nbsp;</font> match"));
    }
    else
    {
        auto i = m_tree->lastFoundIndex() + 1;
        m_foundNumber->setText(QString("<font color=\"#f5f5f5\" style=\"background:#e040fb\">&nbsp;%1&nbsp;</font> of "
                                       "<font style=\"background:#ffeb3b\">&nbsp;%2&nbsp;</font> matches")
                                   .arg(i).arg(matches));
    }

    if (!m_foundNumber->isVisible())
        m_foundNumber->show();
}

void StatsWidget::findNextFromMenu(bool _checked)
{
    if (!_checked)
        return;

    if (!m_searchButton->data().toBool())
    {
        m_searchButton->setData(true);
        m_searchButton->setText(tr("Find next"));
        m_searchButton->setIcon(QIcon(imagePath("find-next")));
        disconnect(m_searchButton, &QAction::triggered, this, &This::findPrev);
        connect(m_searchButton, &QAction::triggered, this, &This::findNext);
    }

    findNext(true);
}

void StatsWidget::findPrevFromMenu(bool _checked)
{
    if (!_checked)
        return;

    if (m_searchButton->data().toBool())
    {
        m_searchButton->setData(false);
        m_searchButton->setText(tr("Find prev"));
        m_searchButton->setIcon(QIcon(imagePath("find-prev")));
        disconnect(m_searchButton, &QAction::triggered, this, &This::findNext);
        connect(m_searchButton, &QAction::triggered, this, &This::findPrev);
    }

    findPrev(true);
}

//////////////////////////////////////////////////////////////////////////
