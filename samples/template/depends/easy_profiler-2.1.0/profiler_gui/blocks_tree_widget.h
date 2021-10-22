/************************************************************************
* file name         : blocks_tree_widget.h
* ----------------- : 
* creation time     : 2016/06/26
* author            : Victor Zarubkin
* email             : v.s.zarubkin@gmail.com
* ----------------- : 
* description       : The file contains declaration of BlocksTreeWidget and it's auxiliary classes
*                   : for displyaing EasyProfiler blocks tree.
* ----------------- : 
* change log        : * 2016/06/26 Victor Zarubkin: moved sources from tree_view.h
*                   :       and renamed classes from My* to Prof*.
*                   :
*                   : * 2016/06/27 Victor Zarubkin: Added possibility to colorize rows
*                   :       with profiler blocks' colors.
*                   :
*                   : * 2016/06/29 Victor Zarubkin: Added clearSilent() method.
*                   :
*                   : * 2016/08/18 Victor Zarubkin: Added loading blocks hierarchy in separate thread;
*                   :       Moved sources of TreeWidgetItem into tree_widget_item.h/.cpp
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

#ifndef EASY_TREE_WIDGET_H
#define EASY_TREE_WIDGET_H

#include <QTreeWidget>
#include <QTimer>

#include "tree_widget_loader.h"
#include "tree_widget_item.h"

#include <easy/reader.h>

//////////////////////////////////////////////////////////////////////////

class BlocksTreeWidget : public QTreeWidget
{
    Q_OBJECT

    using Parent = QTreeWidget;
    using This = BlocksTreeWidget;

protected:

    TreeWidgetLoader           m_treeBuilder;
    Items                            m_items;
    RootsMap                         m_roots;
    ::profiler_gui::TreeBlocks m_inputBlocks;
    QTimer                       m_fillTimer;
    QTimer                       m_idleTimer;
    QString                     m_lastSearch;
    QTreeWidgetItem*             m_lastFound;
    ::profiler::timestamp_t      m_beginTime;
    class RoundProgressDialog*    m_progress;
    class QLabel*                m_hintLabel;
    class ArbitraryValueToolTip* m_valueTooltip;
    TreeMode                          m_mode;
    int                     m_lastFoundIndex;
    bool                           m_bLocked;
    bool             m_bSilentExpandCollapse;
    bool              m_bCaseSensitiveSearch;
    bool                      m_bInitialized;
    char m_columnsHiddenStatus[COL_COLUMNS_NUMBER];
    int  m_columnsMinimumWidth[COL_COLUMNS_NUMBER];

public:

    using Parent::indexFromItem;

    explicit BlocksTreeWidget(QWidget* _parent = nullptr);
    ~BlocksTreeWidget() override;

    bool eventFilter(QObject* object, QEvent* event) override;
    void mousePressEvent(QMouseEvent* _event) override;
    void contextMenuEvent(QContextMenuEvent* _event) override;
    void dragEnterEvent(QDragEnterEvent*) override {}

    void clearSilent(bool _global = false);
    int findNext(const QString& _str, Qt::MatchFlags _flags);
    int findPrev(const QString& _str, Qt::MatchFlags _flags);

    void resetSearch(bool repaint = true);

    TreeMode mode() const
    {
        return m_mode;
    }

    QTreeWidgetItem* lastFoundItem() const;
    const QString& searchString() const;
    bool caseSensitiveSearch() const;
    int lastFoundIndex() const;

public slots:

    void setTreeBlocks(const ::profiler_gui::TreeBlocks& _blocks, ::profiler::timestamp_t _session_begin_time, ::profiler::timestamp_t _left, ::profiler::timestamp_t _right, bool _strict);
    void updateHintLabelOnHover(bool hover);

protected:

    void resizeEvent(QResizeEvent* _event) override;
    void moveEvent(QMoveEvent* _event) override;

private slots:

    void onHeaderSectionResized(int logicalIndex, int oldSize, int newSize);

    void onJumpToItemClicked(bool);

    void onCollapseAllClicked(bool);

    void onExpandAllClicked(bool);

    void onCollapseAllChildrenClicked(bool);

    void onExpandAllChildrenClicked(bool);

    void onItemExpand(QTreeWidgetItem* _item);
    void onItemCollapse(QTreeWidgetItem* _item);
    void onCurrentItemChange(QTreeWidgetItem* _item, QTreeWidgetItem*);
    void onItemDoubleClicked(QTreeWidgetItem* _item, int _column);

    void onSelectedThreadChange(::profiler::thread_id_t _id);

    void onSelectedBlockChange(uint32_t _block_index);

    void onBlockStatusChangeClicked(bool);

    void resizeColumnsToContents();

    void onHideShowColumn(bool);
    void onModeChange(bool);

    void onFillTimerTimeout();
    void onIdleTimeout();

protected:

    void loadSettings();
    void saveSettings();
    void alignProgressBar();

private:

    void destroyProgressDialog();
    void createProgressDialog();

}; // END of class BlocksTreeWidget.

//////////////////////////////////////////////////////////////////////////

class StatsWidget : public QWidget
{
    Q_OBJECT

    using Parent = QWidget;
    using This = StatsWidget;

private:

    BlocksTreeWidget*               m_tree;
    class QLineEdit*           m_searchBox;
    class QLabel*            m_foundNumber;
    class QAction*          m_searchButton;
    bool            m_bCaseSensitiveSearch;

public:

    // Public virtual methods

    explicit StatsWidget(QWidget* _parent = nullptr);
    ~StatsWidget() override;

    void enterEvent(QEvent* event) override;
    void leaveEvent(QEvent* event) override;
    void keyPressEvent(QKeyEvent* _event) override;
    void contextMenuEvent(QContextMenuEvent* _event) override;
    void dragEnterEvent(QDragEnterEvent*) override {}

protected:

    // Protected virtual methods

    void showEvent(QShowEvent* event) override;

public:

    // Public non-virtual methods

    BlocksTreeWidget* tree();
    void clear(bool _global = false);

private slots:

    // Private slots

    void onSeachBoxReturnPressed();
    void onSearchBoxTextChanged(const QString& _text);
    void findNext(bool);
    void findPrev(bool);
    void findNextFromMenu(bool);
    void findPrevFromMenu(bool);

private:

    // Private non-virtual methods

    void loadSettings();
    void saveSettings();

}; // END of class HierarchyWidget.


//////////////////////////////////////////////////////////////////////////

#endif // EASY_TREE_WIDGET_H
