/************************************************************************
* file name         : tree_widget_item.cpp
* ----------------- :
* creation time     : 2016/08/18
* author            : Victor Zarubkin
* email             : v.s.zarubkin@gmail.com
* ----------------- :
* description       : The file contains implementation of TreeWidgetItem.
* ----------------- :
* change log        : * 2016/08/18 Victor Zarubkin: Moved sources from blocks_tree_widget.cpp
*                   :       and renamed classes from Prof* to Easy*.
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

#include "tree_widget_item.h"

#include <QAbstractTextDocumentLayout>
#include <QFont>
#include <QPainter>
#include <QPoint>
#include <QBrush>
#include <QRect>
#include <QSize>
#include <QTextDocument>
#include <QVariant>

#include "globals.h"
#include "blocks_tree_widget.h"
#include "text_highlighter.h"

//////////////////////////////////////////////////////////////////////////

namespace {

EASY_CONSTEXPR bool HasToolTip[COL_COLUMNS_NUMBER] = {
      false // COL_NAME = 0,
    , true  // COL_BEGIN,
    , true  // COL_TIME,
    , true  // COL_SELF_TIME,
    , false // COL_SELF_TIME_PERCENT,
    , true  // COL_END,
    , false // COL_PERCENT_PER_FRAME,
    , true  // COL_TOTAL_TIME_PER_FRAME,
    , false // COL_PERCENT_SUM_PER_FRAME,
    , true  // COL_MIN_PER_FRAME,
    , true  // COL_MAX_PER_FRAME,
    , true  // COL_AVG_PER_FRAME,
    , true  // COL_MEDIAN_PER_FRAME,
    , false // COL_NCALLS_PER_FRAME,
    , true  // COL_TOTAL_TIME_PER_THREAD,
    , false // COL_PERCENT_SUM_PER_THREAD,
    , true  // COL_MIN_PER_THREAD,
    , true  // COL_MAX_PER_THREAD,
    , true  // COL_AVG_PER_THREAD,
    , true  // COL_MEDIAN_PER_THREAD,
    , false // COL_NCALLS_PER_THREAD,
    , false // COL_PERCENT_PER_PARENT,
    , true  // COL_TOTAL_TIME_PER_PARENT,
    , false // COL_PERCENT_SUM_PER_PARENT,
    , true  // COL_MIN_PER_PARENT,
    , true  // COL_MAX_PER_PARENT,
    , true  // COL_AVG_PER_PARENT,
    , true  // COL_MEDIAN_PER_PARENT,
    , false // COL_NCALLS_PER_PARENT,
    , true  // COL_ACTIVE_TIME,
    , false // COL_ACTIVE_PERCENT,
    , false // COL_PERCENT_PER_AREA,
    , true  // COL_TOTAL_TIME_PER_AREA,
    , false // COL_PERCENT_SUM_PER_AREA,
    , true  // COL_MIN_PER_AREA,
    , true  // COL_MAX_PER_AREA,
    , true  // COL_AVG_PER_AREA,
    , true  // COL_MEDIAN_PER_AREA,
    , false // COL_NCALLS_PER_AREA,
};

} // end of namespace <noname>.

//////////////////////////////////////////////////////////////////////////

TreeWidgetItem::TreeWidgetItem(profiler::block_index_t _treeBlock, Parent* _parent)
    : Parent(_parent, QTreeWidgetItem::UserType)
    , m_block(_treeBlock)
    , m_customBGColor(0)
    , m_bMain(false)
    , m_partial(false)
{

}

TreeWidgetItem::~TreeWidgetItem()
{
}

bool TreeWidgetItem::operator < (const Parent& _other) const
{
    const auto col = treeWidget()->sortColumn();

    switch (col)
    {
        //case COL_UNKNOWN:
        case COL_NAME:
        {
            if (parent() == nullptr)
                return false; // Do not sort topLevelItems by name
            return Parent::operator < (_other);
        }

        case COL_NCALLS_PER_THREAD:
        case COL_NCALLS_PER_PARENT:
        case COL_NCALLS_PER_FRAME:
        case COL_NCALLS_PER_AREA:
        {
            return data(col, Qt::UserRole).toUInt() < _other.data(col, Qt::UserRole).toUInt();
        }

        case COL_SELF_TIME_PERCENT:
        case COL_PERCENT_PER_PARENT:
        case COL_PERCENT_PER_FRAME:
        case COL_PERCENT_SUM_PER_PARENT:
        case COL_PERCENT_SUM_PER_FRAME:
        case COL_PERCENT_SUM_PER_THREAD:
        case COL_PERCENT_PER_AREA:
        case COL_PERCENT_SUM_PER_AREA:
        {
            return data(col, Qt::UserRole).toInt() < _other.data(col, Qt::UserRole).toInt();
        }

        case COL_ACTIVE_PERCENT:
        {
            return data(col, Qt::UserRole).toDouble() < _other.data(col, Qt::UserRole).toDouble();
        }

        default:
        {
            // durations min, max, average
            return data(col, Qt::UserRole).toULongLong() < _other.data(col, Qt::UserRole).toULongLong();
        }
    }
}

bool TreeWidgetItem::isPartial() const
{
    return m_partial;
}

QVariant TreeWidgetItem::data(int _column, int _role) const
{
    if (_column == COL_NAME)
    {
        if (_role == BlockColorRole)
        {
            if (parent() != nullptr || m_bMain)
                return QBrush(QColor::fromRgba(m_customBGColor));
            return QVariant();
        }
    }

    switch (_role)
    {
        case Qt::ForegroundRole:
        {
            if (m_bMain)
                return QVariant::fromValue(QColor::fromRgb(profiler_gui::SELECTED_THREAD_FOREGROUND));
            auto fg = Parent::data(_column, _role);
            if (!fg.isNull())
                return fg;
            if (m_partial)
                return partialForeground();
            break;
        }
        
        case Qt::ToolTipRole:
        {
            const bool hasToolTip = HasToolTip[_column];
            if (hasToolTip)
            {
                auto v = data(_column, Qt::UserRole);
                if (!v.isNull())
                    return QString("%1 ns").arg(v.toULongLong());
            }
            break;
        }

        case MinMaxBlockIndexRole:
        {
            auto v = Parent::data(_column, _role);
            if (!v.isNull() || parent() == nullptr)
                return v;
            return QVariant::fromValue(m_block);
        }

        default:
        {
            auto v = Parent::data(_column, _role);
            if (!v.isNull() || parent() == nullptr || (_role != Qt::UserRole && _role != Qt::DisplayRole))
                return v;
            return relevantData(_column, _role);
        }
    }

    return QVariant();
}

QVariant TreeWidgetItem::relevantData(int _column, int _role) const
{
    switch (_column)
    {
        case COL_NAME:
        case COL_BEGIN:
        case COL_END:
        case COL_NCALLS_PER_PARENT:
        case COL_NCALLS_PER_FRAME:
        case COL_NCALLS_PER_THREAD:
        case COL_NCALLS_PER_AREA:
        case COL_SELF_TIME:
        case COL_SELF_TIME_PERCENT:
        case COL_ACTIVE_TIME:
        case COL_ACTIVE_PERCENT:
        case COL_PERCENT_PER_PARENT:
        case COL_PERCENT_PER_FRAME:
        case COL_PERCENT_PER_AREA:
        case COL_PERCENT_SUM_PER_THREAD:
        {
            return QVariant();
        }

        default:
        {
            break;
        }
    }

    if (EASY_GLOBALS.hide_stats_for_single_blocks && _role == Qt::DisplayRole)
    {
        return QVariant();
    }

    switch (_column)
    {
        case COL_TOTAL_TIME_PER_PARENT:
        case COL_TOTAL_TIME_PER_FRAME:
        case COL_TOTAL_TIME_PER_THREAD:
        case COL_TOTAL_TIME_PER_AREA:
        case COL_MIN_PER_PARENT:
        case COL_MIN_PER_FRAME:
        case COL_MIN_PER_THREAD:
        case COL_MIN_PER_AREA:
        case COL_MAX_PER_PARENT:
        case COL_MAX_PER_FRAME:
        case COL_MAX_PER_THREAD:
        case COL_MAX_PER_AREA:
        case COL_AVG_PER_PARENT:
        case COL_AVG_PER_FRAME:
        case COL_AVG_PER_THREAD:
        case COL_AVG_PER_AREA:
        case COL_MEDIAN_PER_PARENT:
        case COL_MEDIAN_PER_FRAME:
        case COL_MEDIAN_PER_THREAD:
        case COL_MEDIAN_PER_AREA:
        {
            return data(COL_TIME, _role);
        }

        case COL_PERCENT_SUM_PER_PARENT:
        {
            return data(COL_PERCENT_PER_PARENT, _role);
        }

        case COL_PERCENT_SUM_PER_FRAME:
        {
            return data(COL_PERCENT_PER_FRAME, _role);
        }

        case COL_PERCENT_SUM_PER_AREA:
        {
            return data(COL_PERCENT_PER_AREA, _role);
        }

        default:
        {
            break;
        }
    }

    return QVariant();
}

QVariant TreeWidgetItem::partialForeground() const
{
    const auto mode = static_cast<const BlocksTreeWidget*>(treeWidget())->mode();
    profiler::calls_number_t ncalls = 0;

    switch (mode)
    {
        case TreeMode::Plain:
        {
            ncalls = data(COL_NCALLS_PER_FRAME, Qt::UserRole).toUInt();
            break;
        }

        case TreeMode::SelectedArea:
        {
            ncalls = data(COL_NCALLS_PER_AREA, Qt::UserRole).toUInt();
            break;
        }

        default:
            break;
    }

    if (ncalls < 3)
        return QVariant::fromValue(QColor::fromRgb(profiler::colors::Grey500));

    return QVariant::fromValue(QColor::fromRgb(profiler::colors::Grey700));
}

profiler::block_index_t TreeWidgetItem::block_index() const
{
    return m_block;
}

profiler_gui::EasyBlock& TreeWidgetItem::guiBlock()
{
    return easyBlock(m_block);
}

const profiler::BlocksTree& TreeWidgetItem::block() const
{
    return easyBlocksTree(m_block);
}

profiler::thread_id_t TreeWidgetItem::threadId() const
{
    const QTreeWidgetItem* parentItem = this;
    while (parentItem->parent() != nullptr)
    {
        parentItem = parent();
    }
    return static_cast<profiler::thread_id_t>(parentItem->data(COL_NAME, Qt::UserRole).toULongLong());
}

void TreeWidgetItem::setTimeSmart(int _column, profiler_gui::TimeUnits _units, const profiler::timestamp_t& _time, const QString& _prefix)
{
    const profiler::timestamp_t nanosecondsTime = PROF_NANOSECONDS(_time);

    setData(_column, Qt::UserRole, (quint64)nanosecondsTime);
    //setHasToolTip(_column);
    setText(_column, QString("%1%2").arg(_prefix).arg(profiler_gui::timeStringRealNs(_units, nanosecondsTime, 3)));
}

void TreeWidgetItem::setTimeSmart(int _column, profiler_gui::TimeUnits _units, const profiler::timestamp_t& _time)
{
    const profiler::timestamp_t nanosecondsTime = PROF_NANOSECONDS(_time);

    setData(_column, Qt::UserRole, (quint64)nanosecondsTime);
    //setHasToolTip(_column);
    setText(_column, profiler_gui::timeStringRealNs(_units, nanosecondsTime, 3));
}

void TreeWidgetItem::setTimeMs(int _column, const profiler::timestamp_t& _time)
{
    const profiler::timestamp_t nanosecondsTime = PROF_NANOSECONDS(_time);
    setData(_column, Qt::UserRole, (quint64)nanosecondsTime);
    //setHasToolTip(_column);
    setText(_column, QString::number(double(nanosecondsTime) * 1e-6, 'g', 9));
}

void TreeWidgetItem::setTimeMs(int _column, const profiler::timestamp_t& _time, const QString& _prefix)
{
    const profiler::timestamp_t nanosecondsTime = PROF_NANOSECONDS(_time);
    setData(_column, Qt::UserRole, (quint64)nanosecondsTime);
    //setHasToolTip(_column);
    setText(_column, QString("%1%2").arg(_prefix).arg(double(nanosecondsTime) * 1e-6, 0, 'g', 9));
}

void TreeWidgetItem::setBackgroundColor(QRgb _color)
{
    m_customBGColor = _color;
}

void TreeWidgetItem::setMain(bool _main)
{
    m_bMain = _main;
}

void TreeWidgetItem::setPartial(bool partial)
{
    m_partial = partial;
}

void TreeWidgetItem::collapseAll()
{
    for (int i = 0, childrenNumber = childCount(); i < childrenNumber; ++i)
    {
        static_cast<TreeWidgetItem*>(child(i))->collapseAll();
    }

    setExpanded(false);
    if (parent() != nullptr)
        guiBlock().expanded = false;
}

void TreeWidgetItem::expandAll()
{
    for (int i = 0, childrenNumber = childCount(); i < childrenNumber; ++i)
    {
        static_cast<TreeWidgetItem*>(child(i))->expandAll();
    }

    setExpanded(true);
    if (parent() != nullptr)
        guiBlock().expanded = true;
}

//////////////////////////////////////////////////////////////////////////

TreeWidgetItemDelegate::TreeWidgetItemDelegate(BlocksTreeWidget* parent)
    : QStyledItemDelegate(parent)
    , m_treeWidget(parent)
{

}

TreeWidgetItemDelegate::~TreeWidgetItemDelegate()
{

}

void TreeWidgetItemDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    const auto brushData = m_treeWidget->model()->data(index, BlockColorRole);
    if (brushData.isNull())
    {
        // Draw item as usual
        QStyledItemDelegate::paint(painter, option, index);
        highlightMatchingText(painter, option, index);
        return;
    }

    const auto padding = px(2);
    const auto colorBlockSize = option.rect.height() >> 1;
    const auto currentTreeIndex = m_treeWidget->currentIndex();
    if (index.parent() == currentTreeIndex.parent() && index.row() == currentTreeIndex.row())
    {
        // Draw selection background for selected row
        painter->save();
        painter->setBrush(m_treeWidget->palette().highlight());
        painter->setPen(Qt::NoPen);
        painter->drawRect(QRect(option.rect.left(), option.rect.top(), colorBlockSize + padding, option.rect.height()));
        painter->restore();
    }

    // Adjust rect size for drawing color marker
    QStyleOptionViewItem opt = option;
    opt.rect.adjust(colorBlockSize + padding, 0, 0, 0);

    // Draw item as usual
    QStyledItemDelegate::paint(painter, opt, index);
    highlightMatchingText(painter, opt, index);

    const auto colorBlockRest = option.rect.height() - colorBlockSize;

    painter->save();

    // Draw color marker with block color
    const auto brush = brushData.value<QBrush>();
    painter->setBrush(brush);
    painter->setPen(QColor::fromRgb(profiler::colors::Grey600));
    painter->drawRect(QRect(option.rect.left(), option.rect.top() + (colorBlockRest >> 1),
                            colorBlockSize, option.rect.height() - colorBlockRest));

    // Draw line under tree indicator
    const auto bottomLeft = opt.rect.bottomLeft();
    painter->setBrush(Qt::NoBrush);
    painter->setPen(profiler_gui::SYSTEM_BORDER_COLOR);
    painter->drawLine(QPoint(bottomLeft.x() - colorBlockSize - padding, bottomLeft.y()), bottomLeft);

    painter->restore();
}

void TreeWidgetItemDelegate::highlightMatchingText(
    QPainter* painter,
    const QStyleOptionViewItem& option,
    const QModelIndex& index
) const {
    if (m_treeWidget->lastFoundItem() != nullptr && !m_treeWidget->searchString().isEmpty())
    {
        // Highlight matching word
        auto displayData = m_treeWidget->model()->data(index);
        if (displayData.canConvert<QString>())
        {
            const auto text = displayData.toString();
            const auto caseSensitivity = m_treeWidget->caseSensitiveSearch() ? Qt::CaseSensitive : Qt::CaseInsensitive;
            if (text.contains(m_treeWidget->searchString(), caseSensitivity))
            {
                auto lastFoundIndex = m_treeWidget->indexFromItem(m_treeWidget->lastFoundItem(), index.column());
                highlightMatchingText(
                    painter,
                    option,
                    text,
                    m_treeWidget->searchString(),
                    caseSensitivity,
                    lastFoundIndex == index
                );
            }
        }
    }
}

void TreeWidgetItemDelegate::highlightMatchingText(
    QPainter* painter,
    const QStyleOptionViewItem& option,
    const QString& text,
    const QString& pattern,
    Qt::CaseSensitivity caseSensitivity,
    bool current
) const {
    const auto padding = px(2);

    QTextDocument doc;
    doc.setDefaultFont(painter->font());

    auto textOption = doc.defaultTextOption();
    textOption.setWrapMode(QTextOption::NoWrap);
    doc.setDefaultTextOption(textOption);

    doc.setTextWidth(option.rect.width() - padding);

    const auto elidedText = painter->fontMetrics().elidedText(text, Qt::ElideRight, std::max(option.rect.width() - padding, 0));
    doc.setHtml(elidedText);

    TextHighlighter highlighter(
        &doc,
        painter->pen().color(),
        QColor::fromRgb(profiler::colors::Grey100),
        pattern,
        caseSensitivity,
        current
    );

    painter->save();

#ifdef _WIN32
    EASY_CONSTEXPR int fixed_padding_x = -1;
    EASY_CONSTEXPR int fixed_padding_y = 0;
#else
    EASY_CONSTEXPR int fixed_padding_x = -1;
    EASY_CONSTEXPR int fixed_padding_y = -1;
#endif

    auto dh = std::max((option.rect.height() - doc.size().height()) * 0.5, 0.);
    painter->translate(option.rect.left() + fixed_padding_x, option.rect.top() + dh + fixed_padding_y);

    QRect clip(0, 0, option.rect.width(), option.rect.height());
    painter->setClipRect(clip);

    QAbstractTextDocumentLayout::PaintContext ctx;
    ctx.clip = clip;
    ctx.palette.setColor(QPalette::Text, Qt::transparent);
    doc.documentLayout()->draw(painter, ctx);

    painter->restore();
}

QSize TreeWidgetItemDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    auto* tree = m_treeWidget;
    if (tree == nullptr)
        return QStyledItemDelegate::sizeHint(option, index);

    auto* model = tree->model();
    if (model == nullptr)
        return QStyledItemDelegate::sizeHint(option, index);

    auto displayData = m_treeWidget->model()->data(index);
    if (!displayData.canConvert<QString>())
    {
        return QStyledItemDelegate::sizeHint(option, index);
    }

    // unfortunately, Qt does not take padding into account, so have to add it manually...
    const auto padding = px(15);
    auto text = displayData.toString();
    const auto width = static_cast<int>((m_treeWidget->fontMetrics().width(text) + padding) * 1.05);

    const auto brushData = m_treeWidget->model()->data(index, BlockColorRole);
    if (brushData.isNull())
    {
        return QSize(width, option.rect.height());
    }

    const auto colorBlockSize = option.rect.height() >> 1;
    return QSize(width + colorBlockSize + px(2), option.rect.height());
}
