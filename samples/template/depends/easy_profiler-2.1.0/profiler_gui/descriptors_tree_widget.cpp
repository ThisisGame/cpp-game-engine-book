/************************************************************************
* file name         : descriptors_tree_widget.cpp
* ----------------- :
* creation time     : 2016/09/17
* author            : Victor Zarubkin
* email             : v.s.zarubkin@gmail.com
* ----------------- :
* description       : The file contains implementation of DescriptorsTreeWidget and it's auxiliary classes
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

#include <QAbstractTextDocumentLayout>
#include <QAction>
#include <QActionGroup>
#include <QApplication>
#include <QClipboard>
#include <QContextMenuEvent>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QKeyEvent>
#include <QLabel>
#include <QLineEdit>
#include <QMenu>
#include <QSettings>
#include <QSignalBlocker>
#include <QSplitter>
#include <QString>
#include <QTextDocument>
#include <QTimer>
#include <QToolBar>
#include <QVariant>
#include <QVBoxLayout>

#include "descriptors_tree_widget.h"

#include "globals.h"
#include "arbitrary_value_inspector.h"
#include "text_highlighter.h"
#include "thread_pool.h"

#ifdef max
#undef max
#endif

#ifdef min
#undef min
#endif

//////////////////////////////////////////////////////////////////////////

::profiler::EasyBlockStatus nextStatus(::profiler::EasyBlockStatus _status)
{
    switch (_status)
    {
        case ::profiler::OFF:
            return ::profiler::ON;

        case ::profiler::ON:
            return ::profiler::FORCE_ON;

        case ::profiler::FORCE_ON:
            return ::profiler::OFF_RECURSIVE;

        case ::profiler::OFF_RECURSIVE:
            return ::profiler::ON_WITHOUT_CHILDREN;

        case ::profiler::ON_WITHOUT_CHILDREN:
            return ::profiler::FORCE_ON_WITHOUT_CHILDREN;

        case ::profiler::FORCE_ON_WITHOUT_CHILDREN:
            return ::profiler::OFF;
    }

    return ::profiler::OFF;
}

const char* statusText(::profiler::EasyBlockStatus _status)
{
    switch (_status)
    {
        case ::profiler::OFF:
            return "OFF";

        case ::profiler::ON:
            return "ON";

        case ::profiler::FORCE_ON:
            return "FORCE_ON";

        case ::profiler::OFF_RECURSIVE:
            return "OFF_RECURSIVE";

        case ::profiler::ON_WITHOUT_CHILDREN:
            return "ON_WITHOUT_CHILDREN";

        case ::profiler::FORCE_ON_WITHOUT_CHILDREN:
            return "FORCE_ON_WITHOUT_CHILDREN";
    }

    return "";
}

::profiler::color_t statusColor(::profiler::EasyBlockStatus _status)
{
    switch (_status)
    {
        case ::profiler::OFF:
            return ::profiler::colors::Red900;

        case ::profiler::ON:
            return ::profiler::colors::LightGreen900;

        case ::profiler::FORCE_ON:
            return ::profiler::colors::LightGreen900;

        case ::profiler::OFF_RECURSIVE:
            return ::profiler::colors::Red900;

        case ::profiler::ON_WITHOUT_CHILDREN:
            return ::profiler::colors::Lime900;

        case ::profiler::FORCE_ON_WITHOUT_CHILDREN:
            return ::profiler::colors::Lime900;
    }

    return ::profiler::colors::Black;
}

//////////////////////////////////////////////////////////////////////////

DescriptorsTreeItem::DescriptorsTreeItem(::profiler::block_id_t _desc, Parent* _parent)
    : Parent(_parent, QTreeWidgetItem::UserType)
    , m_desc(_desc)
    , m_type(DescriptorsTreeItem::Type::File)
{

}

DescriptorsTreeItem::~DescriptorsTreeItem()
{

}

bool DescriptorsTreeItem::operator < (const Parent& _other) const
{
    const auto col = treeWidget()->sortColumn();

    switch (col)
    {
        case DESC_COL_FILE_LINE:
        {
            if (parent() != nullptr)
                return data(col, Qt::UserRole).toInt() < _other.data(col, Qt::UserRole).toInt();
        }
    }

    return Parent::operator < (_other);
}

QVariant DescriptorsTreeItem::data(int _column, int _role) const
{
    switch (_column)
    {
        case DESC_COL_TYPE:
        {
            if (_role == Qt::ToolTipRole)
            {
                switch (m_type)
                {
                    case Type::File:  return QStringLiteral("File");
                    case Type::Event: return QStringLiteral("Event");
                    case Type::Block: return QStringLiteral("Block");
                    case Type::Value: return QStringLiteral("Arbitrary Value");
                }
            }
            else if (_role == Qt::DisplayRole)
            {
                switch (m_type)
                {
                    case Type::File:  return QStringLiteral("F");
                    case Type::Event: return QStringLiteral("E");
                    case Type::Block: return QStringLiteral("B");
                    case Type::Value: return QStringLiteral("V");
                }
            }

            break;
        }

        case DESC_COL_FILE_LINE:
        {
            if (parent() != nullptr)
            {
                if (_role == Qt::ToolTipRole)
                {
                    const int row = data(_column, Qt::UserRole).toInt();
                    return QString("%1:%2").arg(parent()->data(_column, Qt::UserRole).toString()).arg(row);
                }
                else if (_role == Qt::DisplayRole)
                {
                    const int row = data(_column, Qt::UserRole).toInt();
                    return QString("%1:%2").arg(parent()->text(_column)).arg(row);
                }
            }
            else if (_role == Qt::ToolTipRole)
            {
                return data(_column, Qt::UserRole).toString();
            }
            break;
        }

        default:
        {
            break;
        }
    }

    return Parent::data(_column, _role);
}

//////////////////////////////////////////////////////////////////////////

DescriptorsTreeWidget::DescriptorsTreeWidget(QWidget* _parent)
    : Parent(_parent)
    , m_lastFound(nullptr)
    , m_lastFoundIndex(0)
    , m_lastSearchColumn(-1)
    , m_searchColumn(DESC_COL_NAME)
    , m_bLocked(false)
    , m_bCaseSensitiveSearch(false)
    , m_bInitialized(false)
{
    memset(m_columnsMinimumWidth, 0, sizeof(m_columnsMinimumWidth));

    setAutoFillBackground(false);
    setAlternatingRowColors(true);
    setItemsExpandable(true);
    setAnimated(true);
    setSortingEnabled(false);
    setColumnCount(DESC_COL_COLUMNS_NUMBER);
    setSelectionBehavior(QAbstractItemView::SelectRows);

    auto header_item = new QTreeWidgetItem();
    header_item->setText(DESC_COL_FILE_LINE, "File/Line");
    header_item->setText(DESC_COL_TYPE, "Type");
    header_item->setText(DESC_COL_NAME, "Name");
    header_item->setText(DESC_COL_STATUS, "Status");
    setHeaderItem(header_item);

    connect(&EASY_GLOBALS.events, &::profiler_gui::GlobalSignals::selectedBlockChanged, this, &This::onSelectedBlockChange);
    connect(&EASY_GLOBALS.events, &::profiler_gui::GlobalSignals::blockStatusChanged, this, &This::onBlockStatusChange);
    connect(this, &Parent::itemExpanded, this, &This::onItemExpand);
    connect(this, &Parent::itemDoubleClicked, this, &This::onDoubleClick);
    connect(this, &Parent::currentItemChanged, this, &This::onCurrentItemChange);
    connect(header(), &QHeaderView::sectionResized, this, &This::onHeaderSectionResized);

    loadSettings();

    setItemDelegateForColumn(m_searchColumn, new DescWidgetItemDelegate(this));
}

DescriptorsTreeWidget::~DescriptorsTreeWidget()
{
    if (::profiler_gui::is_max(EASY_GLOBALS.selected_block) && !::profiler_gui::is_max(EASY_GLOBALS.selected_block_id))
    {
        ::profiler_gui::set_max(EASY_GLOBALS.selected_block_id);
        emit EASY_GLOBALS.events.refreshRequired();
    }

    saveSettings();
}

void DescriptorsTreeWidget::showEvent(QShowEvent* event)
{
    Parent::showEvent(event);

    if (!m_bInitialized)
    {
#if !defined(_WIN32) && !defined(__APPLE__)
        const auto padding = px(9);
#else
        const auto padding = px(6);
#endif

        auto header = this->header();
        auto headerItem = this->headerItem();

        auto f = header->font();
#if !defined(_WIN32) && !defined(__APPLE__)
        f.setBold(true);
#endif
        QFontMetrics fm(f);

        const auto indicatorSize = header->isSortIndicatorShown() ? px(11) : 0;
        for (int i = 0; i < DESC_COL_COLUMNS_NUMBER; ++i)
        {
            auto minSize = static_cast<int>(fm.width(headerItem->text(i)) * profiler_gui::FONT_METRICS_FACTOR + padding);
            m_columnsMinimumWidth[i] = minSize;

            if (header->isSortIndicatorShown() && header->sortIndicatorSection() == i)
            {
                minSize += indicatorSize;
            }

            if (header->sectionSize(i) < minSize)
            {
                header->resizeSection(i, minSize);
            }
        }

        m_bInitialized = true;
    }
}

//////////////////////////////////////////////////////////////////////////

void DescriptorsTreeWidget::resetSearch(bool repaint)
{
    if (m_lastSearch.isEmpty())
    {
        return;
    }

    m_lastSearchColumn = m_searchColumn;
    m_bCaseSensitiveSearch = false;
    m_lastSearch.clear();
    m_lastFound = nullptr;
    m_lastFoundIndex = 0;

    if (repaint)
    {
        viewport()->update();
    }
}

void DescriptorsTreeWidget::setSearchColumn(int column)
{
    const int prevColumn = m_searchColumn;
    m_searchColumn = column;

    if (m_searchColumn != prevColumn)
    {
        auto delegate = itemDelegateForColumn(prevColumn);
        setItemDelegateForColumn(prevColumn, nullptr);
        delete delegate;

        setItemDelegateForColumn(m_searchColumn, new DescWidgetItemDelegate(this));
    }

    emit searchColumnChanged(column);
}

int DescriptorsTreeWidget::searchColumn() const
{
    return m_searchColumn;
}

QTreeWidgetItem* DescriptorsTreeWidget::lastFoundItem() const
{
    return m_lastFound;
}

bool DescriptorsTreeWidget::caseSensitiveSearch() const
{
    return m_bCaseSensitiveSearch;
}

const QString& DescriptorsTreeWidget::searchString() const
{
    return m_lastSearch;
}

int DescriptorsTreeWidget::lastFoundIndex() const
{
    return m_lastFoundIndex;
}

//////////////////////////////////////////////////////////////////////////

void DescriptorsTreeWidget::contextMenuEvent(QContextMenuEvent* _event)
{
    _event->accept();

    QMenu menu;
    menu.setToolTipsVisible(true);
    auto action = menu.addAction("Expand All");
    action->setIcon(QIcon(imagePath("expand")));
    connect(action, &QAction::triggered, this, &This::expandAll);

    action = menu.addAction("Collapse All");
    action->setIcon(QIcon(imagePath("collapse")));
    connect(action, &QAction::triggered, this, &This::collapseAll);

    auto item = currentItem();
    if (item != nullptr && item->parent() != nullptr && currentColumn() >= DESC_COL_TYPE)
    {
        const auto& desc = easyDescriptor(static_cast<DescriptorsTreeItem*>(item)->desc());

        menu.addSeparator();
        auto submenu = menu.addMenu("Change Status");
        submenu->setToolTipsVisible(true);

#define ADD_STATUS_ACTION(NameValue, StatusValue, ToolTipValue)\
        action = submenu->addAction(NameValue);\
        action->setCheckable(true);\
        action->setChecked(desc.status() == StatusValue);\
        action->setData(static_cast<quint32>(StatusValue));\
        action->setToolTip(ToolTipValue);\
        connect(action, &QAction::triggered, this, &This::onBlockStatusChangeClicked)

        ADD_STATUS_ACTION("Off", ::profiler::OFF, "Do not profile this block.");
        ADD_STATUS_ACTION("On", ::profiler::ON, "Profile this block\nif parent enabled children.");
        ADD_STATUS_ACTION("Force-On", ::profiler::FORCE_ON, "Always profile this block even\nif it's parent disabled children.");
        ADD_STATUS_ACTION("Off-recursive", ::profiler::OFF_RECURSIVE, "Do not profile neither this block\nnor it's children.");
        ADD_STATUS_ACTION("On-without-children", ::profiler::ON_WITHOUT_CHILDREN, "Profile this block, but\ndo not profile it's children.");
        ADD_STATUS_ACTION("Force-On-without-children", ::profiler::FORCE_ON_WITHOUT_CHILDREN, "Always profile this block, but\ndo not profile it's children.");
#undef ADD_STATUS_ACTION

        submenu->setEnabled(EASY_GLOBALS.connected);
        if (!EASY_GLOBALS.connected)
            submenu->setTitle(QString("%1 (connection needed)").arg(submenu->title()));
    }

    if (item != nullptr)
    {
        menu.addSeparator();
        action = menu.addAction(QStringLiteral("Copy Full Path"));
        connect(action, &QAction::triggered, [this, item] (bool) {
            auto fileItem = item->parent() == nullptr ? item : item->parent();
            auto fullName = fileItem->data(DESC_COL_FILE_LINE, Qt::UserRole).toString();
            qApp->clipboard()->setText(fullName);
        });
    }

    menu.exec(QCursor::pos());
}

//////////////////////////////////////////////////////////////////////////

void DescriptorsTreeWidget::clearSilent(bool _global)
{
    const QSignalBlocker b(this);

    setSortingEnabled(false);
    resetSearch(false);

    m_items.clear();

    if (topLevelItemCount() != 0)
    {
        ::std::vector<QTreeWidgetItem*> topLevelItems;
        topLevelItems.reserve(static_cast<size_t>(topLevelItemCount()));

        for (int i = topLevelItemCount() - 1; i >= 0; --i)
        {
            const bool expanded = !_global && topLevelItem(i)->isExpanded();
            auto item = takeTopLevelItem(i);
            if (expanded)
                m_expandedFilesTemp.insert(item->text(DESC_COL_FILE_LINE).toStdString());
            topLevelItems.push_back(item);
        }

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
}

//////////////////////////////////////////////////////////////////////////

struct FileItems
{
    using Items = ::std::unordered_map<int, DescriptorsTreeItem*, ::estd::hash<int> >;
    Items children;
    QTreeWidgetItem* item = nullptr;
};

void DescriptorsTreeWidget::build()
{
    auto f = font();
    f.setBold(true);

    typedef ::std::unordered_map<::std::string, FileItems> Files;
    Files fileItems;

    m_items.resize(EASY_GLOBALS.descriptors.size());
    memset(m_items.data(), 0, sizeof(void*) * m_items.size());

    const QSignalBlocker b(this);
    profiler::block_id_t id = 0, count = 0;

    QString commonDir;
    for (auto desc : EASY_GLOBALS.descriptors)
    {
        if (desc != nullptr)
        {
            ++count;

            auto& p = fileItems[desc->file()];
            if (p.item == nullptr)
            {
                auto item = new DescriptorsTreeItem(0);
                auto fullName = QString(desc->file()).remove(QRegExp("^(\\.{2}\\\\+)+")); // without leading "..\"
                auto fileName = QString(desc->file()).remove(QRegExp("^(.+(\\\\|\\/)+)+"));
                auto dir = fullName.left(fullName.length() - fileName.length());

                if (count == 1)
                {
                    commonDir = dir;
                }
                else if (!commonDir.isEmpty())
                {
                    if (dir.length() < commonDir.length())
                    {
                        commonDir.truncate(dir.length());
                    }

                    int i = 0;
                    for (; i < commonDir.length(); ++i)
                    {
                        if (commonDir[i] != dir[i])
                        {
                            break;
                        }
                    }

                    commonDir.truncate(i);
                }

                item->setData(DESC_COL_FILE_LINE, Qt::UserRole, fullName);
                item->setType(DescriptorsTreeItem::Type::File);
                p.item = item;
            }

            auto it = p.children.find(desc->line());
            if (it == p.children.end())
            {
                auto item = new DescriptorsTreeItem(desc->id(), p.item);
                item->setData(DESC_COL_FILE_LINE, Qt::UserRole, desc->line());
                item->setText(DESC_COL_NAME, desc->name());

                switch (desc->type())
                {
                    case ::profiler::BlockType::Block:
                        item->setType(DescriptorsTreeItem::Type::Block);
                        break;

                    case ::profiler::BlockType::Event:
                        item->setType(DescriptorsTreeItem::Type::Event);
                        break;

                    case ::profiler::BlockType::Value:
                        item->setType(DescriptorsTreeItem::Type::Value);
                        break;

                    case ::profiler::BlockType::TypesCount: break;
                }

                item->setFont(DESC_COL_STATUS, f);
                item->setText(DESC_COL_STATUS, statusText(desc->status()));
                item->setForeground(DESC_COL_STATUS, QColor::fromRgba(statusColor(desc->status())));

                m_items[id] = item;
                p.children.insert(::std::make_pair(desc->line(), item));
            }
            else
            {
                m_items[id] = it->second;
            }
        }

        ++id;
    }

    for (auto& p : fileItems)
    {
        auto fullName = p.second.item->data(DESC_COL_FILE_LINE, Qt::UserRole).toString();
        p.second.item->setText(DESC_COL_FILE_LINE, fullName.right(fullName.length() - commonDir.length()));
        addTopLevelItem(p.second.item);
        if (m_expandedFilesTemp.find(p.first) != m_expandedFilesTemp.end())
            p.second.item->setExpanded(true);
    }

    m_expandedFilesTemp.clear();
    setSortingEnabled(true);
    sortByColumn(DESC_COL_FILE_LINE, Qt::AscendingOrder);
    resizeColumnsToContents();

    QTimer::singleShot(100, [this] { onSelectedBlockChange(EASY_GLOBALS.selected_block); });
}

//////////////////////////////////////////////////////////////////////////

void DescriptorsTreeWidget::onHeaderSectionResized(int logicalIndex, int /*oldSize*/, int newSize)
{
    const auto indicatorSize = header()->isSortIndicatorShown() && header()->sortIndicatorSection() == logicalIndex ? px(11) : 0;
    const auto minSize = m_columnsMinimumWidth[logicalIndex] + indicatorSize;

    if (!m_bInitialized || newSize >= minSize)
    {
        return;
    }

    header()->resizeSection(logicalIndex, minSize);
}

//////////////////////////////////////////////////////////////////////////

void DescriptorsTreeWidget::onItemExpand(QTreeWidgetItem*)
{
    resizeColumnsToContents();
}

//////////////////////////////////////////////////////////////////////////

void DescriptorsTreeWidget::onDoubleClick(QTreeWidgetItem* _item, int _column)
{
    if (!EASY_GLOBALS.connected)
        return;

    if (_column >= DESC_COL_TYPE && _item->parent() != nullptr)
    {
        auto item = static_cast<DescriptorsTreeItem*>(_item);
        auto& desc = easyDescriptor(item->desc());
        desc.setStatus(nextStatus(desc.status()));

        item->setText(DESC_COL_STATUS, statusText(desc.status()));
        item->setForeground(DESC_COL_STATUS, QColor::fromRgba(statusColor(desc.status())));

        m_bLocked = true;
        emit EASY_GLOBALS.events.blockStatusChanged(desc.id(), desc.status());
        m_bLocked = false;
    }
}

//////////////////////////////////////////////////////////////////////////

void DescriptorsTreeWidget::onCurrentItemChange(QTreeWidgetItem* _item, QTreeWidgetItem* _prev)
{
    if (_item != nullptr)
    {
        if (::profiler_gui::is_max(EASY_GLOBALS.selected_block) && _item->parent() != nullptr)
        {
            const auto id = static_cast<DescriptorsTreeItem*>(_item)->desc();
            if (EASY_GLOBALS.selected_block_id != id)
            {
                EASY_GLOBALS.selected_block_id = id;
                emit EASY_GLOBALS.events.selectedBlockIdChanged(id);
            }
        }
    }
    else if (::profiler_gui::is_max(EASY_GLOBALS.selected_block) && !::profiler_gui::is_max(EASY_GLOBALS.selected_block_id))
    {
        ::profiler_gui::set_max(EASY_GLOBALS.selected_block_id);
        emit EASY_GLOBALS.events.selectedBlockIdChanged(EASY_GLOBALS.selected_block_id);
    }
}

//////////////////////////////////////////////////////////////////////////

void DescriptorsTreeWidget::onBlockStatusChangeClicked(bool _checked)
{
    if (!_checked || !EASY_GLOBALS.connected)
        return;

    auto item = currentItem();
    if (item == nullptr || item->parent() == nullptr)
        return;

    auto action = qobject_cast<QAction*>(sender());
    if (action != nullptr)
    {
        auto& desc = easyDescriptor(static_cast<DescriptorsTreeItem*>(item)->desc());
        desc.setStatus(static_cast<::profiler::EasyBlockStatus>(action->data().toUInt()));
        item->setText(DESC_COL_STATUS, statusText(desc.status()));
        item->setForeground(DESC_COL_STATUS, QColor::fromRgba(statusColor(desc.status())));

        m_bLocked = true;
        emit EASY_GLOBALS.events.blockStatusChanged(desc.id(), desc.status());
        m_bLocked = false;
    }
}

void DescriptorsTreeWidget::onBlockStatusChange(::profiler::block_id_t _id, ::profiler::EasyBlockStatus _status)
{
    if (m_bLocked)
        return;

    auto item = m_items[_id];
    if (item == nullptr)
        return;

    auto& desc = easyDescriptor(item->desc());
    item->setText(DESC_COL_STATUS, statusText(desc.status()));
    item->setForeground(DESC_COL_STATUS, QColor::fromRgba(statusColor(desc.status())));
}

//////////////////////////////////////////////////////////////////////////

void DescriptorsTreeWidget::resizeColumnsToContents()
{
    for (int i = 0; i < DESC_COL_COLUMNS_NUMBER; ++i)
        resizeColumnToContents(i);
}

//////////////////////////////////////////////////////////////////////////

void DescriptorsTreeWidget::onSelectedBlockChange(uint32_t _block_index)
{
    if (::profiler_gui::is_max(_block_index))
    {
        setCurrentItem(nullptr);
        return;
    }

    auto item = m_items[easyBlocksTree(_block_index).node->id()];
    if (item == nullptr)
        return;

    scrollToItem(item, QAbstractItemView::PositionAtCenter);
    setCurrentItem(item);
}

//////////////////////////////////////////////////////////////////////////

void DescriptorsTreeWidget::loadSettings()
{
    QSettings settings(::profiler_gui::ORGANAZATION_NAME, ::profiler_gui::APPLICATION_NAME);
    settings.beginGroup("desc_tree_widget");

    auto val = settings.value("searchColumn");
    if (!val.isNull())
        m_searchColumn = val.toInt();

    settings.endGroup();
}

void DescriptorsTreeWidget::saveSettings()
{
    QSettings settings(::profiler_gui::ORGANAZATION_NAME, ::profiler_gui::APPLICATION_NAME);
    settings.beginGroup("desc_tree_widget");

    settings.setValue("searchColumn", m_searchColumn);

    settings.endGroup();
}

//////////////////////////////////////////////////////////////////////////

int DescriptorsTreeWidget::findNext(const QString& _str, Qt::MatchFlags _flags)
{
    if (_str.isEmpty())
    {
        resetSearch();
        return 0;
    }

    const bool isNewSearch = (m_lastSearchColumn != m_searchColumn || m_lastSearch != _str);
    auto itemsList = findItems(_str, Qt::MatchContains | Qt::MatchRecursive | _flags, m_searchColumn);
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
        m_lastSearchColumn = m_searchColumn;
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

int DescriptorsTreeWidget::findPrev(const QString& _str, Qt::MatchFlags _flags)
{
    if (_str.isEmpty())
    {
        resetSearch();
        return 0;
    }

    const bool isNewSearch = (m_lastSearchColumn != m_searchColumn || m_lastSearch != _str);
    auto itemsList = findItems(_str, Qt::MatchContains | Qt::MatchRecursive | _flags, m_searchColumn);
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
        m_lastSearchColumn = m_searchColumn;
        m_lastSearch = _str;
        if (!itemsList.empty())
        {
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

BlockDescriptorsWidget::BlockDescriptorsWidget(QWidget* _parent) : Parent(_parent)
    , m_splitter(new QSplitter(Qt::Vertical, this))
    , m_tree(new DescriptorsTreeWidget(this))
    , m_values(new ArbitraryValuesWidget(this))
    , m_searchBox(new QLineEdit(this))
    , m_foundNumber(new QLabel(QStringLiteral("<font color=\"red\">0</font> matches"), this))
    , m_searchButton(nullptr)
    , m_bCaseSensitiveSearch(false)
{
    m_splitter->setHandleWidth(1);
    m_splitter->setContentsMargins(0, 0, 0, 0);
    m_splitter->addWidget(m_tree);
    m_splitter->addWidget(m_values);
    m_splitter->setStretchFactor(0, 1);
    m_splitter->setStretchFactor(1, 1);

    m_searchBox->setContentsMargins(5, 0, 0, 0);
    m_searchBox->setClearButtonEnabled(true);
    m_searchBox->setPlaceholderText("Search");

    auto tb = new QToolBar(this);
    tb->setIconSize(applicationIconsSize());
    auto refreshButton = tb->addAction(QIcon(imagePath("reload")), tr("Refresh blocks list"));
    refreshButton->setEnabled(EASY_GLOBALS.connected);
    refreshButton->setToolTip(tr("Refresh blocks list.\nConnection needed."));
    connect(refreshButton, &QAction::triggered, &EASY_GLOBALS.events, &::profiler_gui::GlobalSignals::blocksRefreshRequired);



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
    QAction* caseSensitiveSwitch = a;

    menu->addSeparator();
    auto headerItem = m_tree->headerItem();
    actionGroup = new QActionGroup(this);
    actionGroup->setExclusive(true);
    for (int i = 0; i < DESC_COL_STATUS; ++i)
    {
        if (i == DESC_COL_TYPE)
            continue;

        a = new QAction(QStringLiteral("Search by ") + headerItem->text(i), actionGroup);
        a->setData(i);
        a->setCheckable(true);
        if (i == m_tree->searchColumn())
            a->setChecked(true);
        connect(a, &QAction::triggered, this, &This::onSearchColumnChange);

        menu->addAction(a);
    }

    tb->addSeparator();
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
    lay->addWidget(m_splitter);

    connect(m_searchBox, &QLineEdit::returnPressed, this, &This::onSeachBoxReturnPressed);
    connect(m_searchBox, &QLineEdit::textChanged, this, &This::onSearchBoxTextChanged);

    connect(m_tree, &DescriptorsTreeWidget::searchColumnChanged, this, &This::onSearchColumnChanged);

    connect(&EASY_GLOBALS.events, &::profiler_gui::GlobalSignals::connectionChanged, refreshButton, &QAction::setEnabled);
    connect(&EASY_GLOBALS.events, &profiler_gui::GlobalSignals::allDataGoingToBeDeleted, this, &This::clear);
    connect(&EASY_GLOBALS.events, &profiler_gui::GlobalSignals::fileOpened, this, &This::build);

    loadSettings();
    caseSensitiveSwitch->setChecked(m_bCaseSensitiveSearch);

    onSearchColumnChanged(m_tree->searchColumn());
    m_foundNumber->hide();
}

BlockDescriptorsWidget::~BlockDescriptorsWidget()
{
    saveSettings();
}

void BlockDescriptorsWidget::loadSettings()
{
    QSettings settings(::profiler_gui::ORGANAZATION_NAME, ::profiler_gui::APPLICATION_NAME);
    settings.beginGroup("BlockDescriptorsWidget");

    auto val = settings.value("case_sensitive");
    if (!val.isNull())
        m_bCaseSensitiveSearch = val.toBool();

    auto geometry = settings.value("vsplitter/geometry").toByteArray();
    if (!geometry.isEmpty())
        m_splitter->restoreGeometry(geometry);

    auto state = settings.value("vsplitter/state").toByteArray();
    if (!state.isEmpty())
        m_splitter->restoreState(state);

    settings.endGroup();
}

void BlockDescriptorsWidget::saveSettings()
{
    QSettings settings(::profiler_gui::ORGANAZATION_NAME, ::profiler_gui::APPLICATION_NAME);
    settings.beginGroup("BlockDescriptorsWidget");
    settings.setValue("case_sensitive", m_bCaseSensitiveSearch);
    settings.setValue("vsplitter/geometry", m_splitter->saveGeometry());
    settings.setValue("vsplitter/state", m_splitter->saveState());
    settings.endGroup();
}

void BlockDescriptorsWidget::keyPressEvent(QKeyEvent* _event)
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

void BlockDescriptorsWidget::contextMenuEvent(QContextMenuEvent* _event)
{
    m_tree->contextMenuEvent(_event);
}

void BlockDescriptorsWidget::showEvent(QShowEvent* event)
{
    Parent::showEvent(event);
    m_searchBox->setFixedWidth(px(300));
}

void BlockDescriptorsWidget::build()
{
    m_tree->clearSilent(false);
    m_foundNumber->setText(QStringLiteral("<font color=\"red\">0</font> matches"));
    m_foundNumber->hide();
    m_tree->build();
    m_values->rebuild();
}

void BlockDescriptorsWidget::clear()
{
    m_tree->clearSilent(true);
    m_foundNumber->setText(QStringLiteral("<font color=\"red\">0</font> matches"));
    m_foundNumber->hide();
    m_values->clear();
}

ArbitraryValuesWidget* BlockDescriptorsWidget::dataViewer() const
{
    return m_values;
}

void BlockDescriptorsWidget::onSeachBoxReturnPressed()
{
    if (m_searchButton->data().toBool())
        findNext(true);
    else
        findPrev(true);
}

void BlockDescriptorsWidget::onSearchBoxTextChanged(const QString& _text)
{
    if (_text.isEmpty())
    {
        m_foundNumber->hide();
        m_tree->resetSearch();
    }
}

void BlockDescriptorsWidget::onSearchColumnChanged(int column)
{
    switch (column)
    {
        case DESC_COL_NAME:
            m_searchBox->setPlaceholderText("Search by name");
            break;

        case DESC_COL_FILE_LINE:
            m_searchBox->setPlaceholderText("Search by filename");
            break;

        default:
            m_searchBox->setPlaceholderText("Search");
            break;
    }

    onSeachBoxReturnPressed();
}

void BlockDescriptorsWidget::onSearchColumnChange(bool)
{
    auto action = qobject_cast<QAction*>(sender());
    if (action != nullptr)
        m_tree->setSearchColumn(action->data().toInt());
}

void BlockDescriptorsWidget::findNext(bool)
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

void BlockDescriptorsWidget::findPrev(bool)
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

void BlockDescriptorsWidget::findNextFromMenu(bool _checked)
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

void BlockDescriptorsWidget::findPrevFromMenu(bool _checked)
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

DescWidgetItemDelegate::DescWidgetItemDelegate(DescriptorsTreeWidget* parent)
    : QStyledItemDelegate(parent)
    , m_treeWidget(parent)
{

}

DescWidgetItemDelegate::~DescWidgetItemDelegate()
{

}

void DescWidgetItemDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    QStyledItemDelegate::paint(painter, option, index);
    highlightMatchingText(painter, option, index);
}

void DescWidgetItemDelegate::highlightMatchingText(
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

void DescWidgetItemDelegate::highlightMatchingText(
    QPainter* painter,
    const QStyleOptionViewItem& option,
    const QString& text,
    const QString& pattern,
    Qt::CaseSensitivity caseSensitivity,
    bool current
) const {
    const auto str = pattern.toStdString();
    (void)str;

    QTextDocument doc;
    doc.setDefaultFont(painter->font());

    auto textOption = doc.defaultTextOption();
    textOption.setWrapMode(QTextOption::NoWrap);
    doc.setDefaultTextOption(textOption);

    doc.setTextWidth(option.rect.width());

    const auto elidedText = painter->fontMetrics().elidedText(text, Qt::ElideRight, option.rect.width());
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

//////////////////////////////////////////////////////////////////////////
