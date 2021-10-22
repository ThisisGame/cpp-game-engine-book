/************************************************************************
* file name         : tree_widget_item.h
* ----------------- : 
* creation time     : 2016/08/18
* author            : Victor Zarubkin
* email             : v.s.zarubkin@gmail.com
* ----------------- : 
* description       : The file contains declaration of TreeWidgetItem
*                   : for displyaing EasyProfiler blocks tree.
* ----------------- : 
* change log        : * 2016/08/18 Victor Zarubkin: moved sources from blocks_tree_widget.h
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

#ifndef EASY_TREE_WIDGET_ITEM_H
#define EASY_TREE_WIDGET_ITEM_H

#include <stdlib.h>
#include <QTreeWidget>
#include <QStyledItemDelegate>
#include <easy/reader.h>

#include "common_functions.h"

class BlocksTreeWidget;

//////////////////////////////////////////////////////////////////////////

EASY_CONSTEXPR int COLUMNS_VERSION = 3;
EASY_CONSTEXPR int BlockColorRole = Qt::UserRole + 1;
EASY_CONSTEXPR int MinMaxBlockIndexRole = Qt::UserRole + 2;

enum EasyColumnsIndexes
{
    COL_UNKNOWN = -1,

    COL_NAME = 0,

    COL_BEGIN,

    COL_TIME,
    COL_SELF_TIME,
    COL_SELF_TIME_PERCENT,

    COL_END,

    COL_PERCENT_PER_FRAME,
    COL_TOTAL_TIME_PER_FRAME,
    COL_PERCENT_SUM_PER_FRAME,
    COL_MIN_PER_FRAME,
    COL_MAX_PER_FRAME,
    COL_AVG_PER_FRAME,
    COL_MEDIAN_PER_FRAME,
    COL_NCALLS_PER_FRAME,

    COL_TOTAL_TIME_PER_THREAD,
    COL_PERCENT_SUM_PER_THREAD,
    COL_MIN_PER_THREAD,
    COL_MAX_PER_THREAD,
    COL_AVG_PER_THREAD,
    COL_MEDIAN_PER_THREAD,
    COL_NCALLS_PER_THREAD,

    COL_PERCENT_PER_PARENT,
    COL_TOTAL_TIME_PER_PARENT,
    COL_PERCENT_SUM_PER_PARENT,
    COL_MIN_PER_PARENT,
    COL_MAX_PER_PARENT,
    COL_AVG_PER_PARENT,
    COL_MEDIAN_PER_PARENT,
    COL_NCALLS_PER_PARENT,

    COL_ACTIVE_TIME,
    COL_ACTIVE_PERCENT,

    COL_PERCENT_PER_AREA,
    COL_TOTAL_TIME_PER_AREA,
    COL_PERCENT_SUM_PER_AREA,
    COL_MIN_PER_AREA,
    COL_MAX_PER_AREA,
    COL_AVG_PER_AREA,
    COL_MEDIAN_PER_AREA,
    COL_NCALLS_PER_AREA,

    COL_COLUMNS_NUMBER
};

//////////////////////////////////////////////////////////////////////////

class TreeWidgetItem : public QTreeWidgetItem
{
    using Parent = QTreeWidgetItem;
    using This = TreeWidgetItem;

    const profiler::block_index_t           m_block;
    QRgb                            m_customBGColor;
    bool                                    m_bMain;
    bool                                  m_partial;

public:

    explicit TreeWidgetItem(profiler::block_index_t _treeBlock = profiler_gui::numeric_max<decltype(m_block)>()
        , Parent* _parent = nullptr);

    ~TreeWidgetItem() override;

    bool operator < (const Parent& _other) const override;
    QVariant data(int _column, int _role) const override;

public:

    bool isPartial() const;
    profiler::block_index_t block_index() const;
    profiler_gui::EasyBlock& guiBlock();
    const profiler::BlocksTree& block() const;

    profiler::thread_id_t threadId() const;

    void setTimeSmart(int _column, profiler_gui::TimeUnits _units, const profiler::timestamp_t& _time, const QString& _prefix);
    void setTimeSmart(int _column, profiler_gui::TimeUnits _units, const profiler::timestamp_t& _time);

    void setTimeMs(int _column, const profiler::timestamp_t& _time);
    void setTimeMs(int _column, const profiler::timestamp_t& _time, const QString& _prefix);

    void setBackgroundColor(QRgb _color);

    void setMain(bool _main);
    void setPartial(bool partial);

    void collapseAll();

    void expandAll();

private:

    //void setHasToolTip(int _column);
    QVariant relevantData(int _column, int _role) const;
    QVariant partialForeground() const;

}; // END of class TreeWidgetItem.

//////////////////////////////////////////////////////////////////////////

class TreeWidgetItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT
    BlocksTreeWidget* m_treeWidget;

public:

    explicit TreeWidgetItemDelegate(BlocksTreeWidget* parent = nullptr);
    ~TreeWidgetItemDelegate() override;
    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
    QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override;

private:

    void highlightMatchingText(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;

    void highlightMatchingText(
        QPainter* painter,
        const QStyleOptionViewItem& option,
        const QString& text,
        const QString& pattern,
        Qt::CaseSensitivity caseSensitivity,
        bool current
    ) const;

}; // END of class TreeWidgetItemDelegate.

//////////////////////////////////////////////////////////////////////////

#endif // EASY_TREE_WIDGET_ITEM_H
