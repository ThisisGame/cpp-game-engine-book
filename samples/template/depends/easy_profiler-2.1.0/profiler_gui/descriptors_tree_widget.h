/************************************************************************
* file name         : descriptors_tree_widget.h
* ----------------- : 
* creation time     : 2016/09/17
* author            : Victor Zarubkin
* email             : v.s.zarubkin@gmail.com
* ----------------- : 
* description       : The file contains declaration of DescriptorsTreeWidget and it's auxiliary classes
*                   : for displyaing EasyProfiler blocks descriptors tree.
* ----------------- : 
* change log        : * 2016/09/17 Victor Zarubkin: initial commit.
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

#ifndef EASY_DESCRIPTORS_WIDGET_H
#define EASY_DESCRIPTORS_WIDGET_H

#include <QTreeWidget>
#include <QStyledItemDelegate>
#include <QString>

#include <vector>
#include <unordered_set>

#include <easy/details/profiler_public_types.h>

//////////////////////////////////////////////////////////////////////////

enum DescColumns
{
    DESC_COL_FILE_LINE = 0,
    DESC_COL_TYPE,
    DESC_COL_NAME,
    DESC_COL_STATUS,

    DESC_COL_COLUMNS_NUMBER
};

//////////////////////////////////////////////////////////////////////////

class DescriptorsTreeItem : public QTreeWidgetItem
{
    using Parent = QTreeWidgetItem;
    using This = DescriptorsTreeItem;

public:

    enum class Type : uint8_t
    {
        File,
        Event,
        Block,
        Value
    };

private:

    ::profiler::block_id_t m_desc;
    Type m_type;

public:

    explicit DescriptorsTreeItem(::profiler::block_id_t _desc, Parent* _parent = nullptr);
    ~DescriptorsTreeItem() override;

    bool operator < (const Parent& _other) const override;
    QVariant data(int _column, int _role) const override;

public:

    // Public inline methods

    ::profiler::block_id_t desc() const
    {
        return m_desc;
    }

    void setType(Type _type)
    {
        m_type = _type;
    }

}; // END of class DescriptorsTreeItem.

//////////////////////////////////////////////////////////////////////////

class DescriptorsTreeWidget : public QTreeWidget
{
    Q_OBJECT

    using Parent = QTreeWidget;
    using This = DescriptorsTreeWidget;

    using Items = ::std::vector<DescriptorsTreeItem*>;
    using ExpandedFiles = ::std::unordered_set<::std::string>;

protected:

    ExpandedFiles    m_expandedFilesTemp;
    Items                        m_items;
    QString                 m_lastSearch;
    QTreeWidgetItem*         m_lastFound;
    int  m_columnsMinimumWidth[DESC_COL_COLUMNS_NUMBER];
    int                 m_lastFoundIndex;
    int               m_lastSearchColumn;
    int                   m_searchColumn;
    bool                       m_bLocked;
    bool          m_bCaseSensitiveSearch;
    bool                  m_bInitialized;

public:

    // Public virtual methods

    explicit DescriptorsTreeWidget(QWidget* _parent = nullptr);
    ~DescriptorsTreeWidget() override;

    void contextMenuEvent(QContextMenuEvent* _event) override;
    void showEvent(class QShowEvent* event) override;

public:

    using Parent::indexFromItem;

    // Public non-virtual methods

    int findNext(const QString& _str, Qt::MatchFlags _flags);
    int findPrev(const QString& _str, Qt::MatchFlags _flags);

    void resetSearch(bool repaint = true);

    void setSearchColumn(int column);
    int searchColumn() const;

    QTreeWidgetItem* lastFoundItem() const;
    const QString& searchString() const;
    bool caseSensitiveSearch() const;
    int lastFoundIndex() const;

signals:

    void searchColumnChanged(int column);

public slots:

    void clearSilent(bool _global = false);
    void build();

private slots:

    void onHeaderSectionResized(int logicalIndex, int oldSize, int newSize);
    void onBlockStatusChangeClicked(bool);
    void onCurrentItemChange(QTreeWidgetItem* _item, QTreeWidgetItem* _prev);
    void onItemExpand(QTreeWidgetItem* _item);
    void onDoubleClick(QTreeWidgetItem* _item, int _column);
    void onSelectedBlockChange(uint32_t _block_index);
    void onBlockStatusChange(::profiler::block_id_t _id, ::profiler::EasyBlockStatus _status);
    void resizeColumnsToContents();

private:

    // Private methods

    void loadSettings();
    void saveSettings();

}; // END of class DescriptorsTreeWidget.

//////////////////////////////////////////////////////////////////////////

class BlockDescriptorsWidget : public QWidget
{
    Q_OBJECT

    using Parent = QWidget;
    using This = BlockDescriptorsWidget;

private:

    class QSplitter*   m_splitter;
    DescriptorsTreeWidget*    m_tree;
    class ArbitraryValuesWidget* m_values;
    class QLineEdit*  m_searchBox;
    class QLabel*   m_foundNumber;
    class QAction* m_searchButton;
    bool   m_bCaseSensitiveSearch;

public:

    // Public virtual methods

    explicit BlockDescriptorsWidget(QWidget* _parent = nullptr);
    ~BlockDescriptorsWidget() override;
    void keyPressEvent(QKeyEvent* _event) override;
    void contextMenuEvent(QContextMenuEvent* _event) override;

protected:

    // Protected virtual methods

    void showEvent(QShowEvent* event) override;

public:

    // Public non-virtual methods

    void build();
    void clear();
    class ArbitraryValuesWidget* dataViewer() const;

private slots:

    void onSeachBoxReturnPressed();
    void findNext(bool);
    void findPrev(bool);
    void findNextFromMenu(bool);
    void findPrevFromMenu(bool);
    void onSearchColumnChange(bool);
    void onSearchBoxTextChanged(const QString& _text);
    void onSearchColumnChanged(int column);

private:

    // Private non-virtual slots

    void loadSettings();
    void saveSettings();

}; // END of class BlockDescriptorsWidget.


//////////////////////////////////////////////////////////////////////////

class DescWidgetItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT
    DescriptorsTreeWidget* m_treeWidget;

public:

    explicit DescWidgetItemDelegate(DescriptorsTreeWidget* parent = nullptr);
    ~DescWidgetItemDelegate() override;
    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;

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

}; // END of class DescWidgetItemDelegate.

//////////////////////////////////////////////////////////////////////////

#endif // EASY_DESCRIPTORS_WIDGET_H
