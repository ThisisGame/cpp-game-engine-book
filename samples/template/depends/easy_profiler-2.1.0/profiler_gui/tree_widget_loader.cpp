/************************************************************************
* file name         : tree_widget_loader.h
* ----------------- :
* creation time     : 2016/08/18
* author            : Victor Zarubkin
* email             : v.s.zarubkin@gmail.com
* ----------------- :
* description       : The file contains implementation of TreeWidgetLoader which aim is
*                   : to load EasyProfiler blocks hierarchy in separate thread.
* ----------------- :
* change log        : * 2016/08/18 Victor Zarubkin: moved sources from blocks_tree_widget.h/.cpp
*                   :       and renamed Prof* to Easy*.
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

#include <map>

#include "globals.h"
#include "thread_pool.h"
#include "tree_widget_item.h"

#include "tree_widget_loader.h"

#ifdef max
#undef max
#endif

#ifdef min
#undef min
#endif

//////////////////////////////////////////////////////////////////////////
#define EASY_INIT_ATOMIC(v) v

#if defined (_WIN32)
#undef EASY_INIT_ATOMIC
#define EASY_INIT_ATOMIC(v) {v}
#endif

namespace {

struct ThreadData
{
    StatsMap stats;
    IdItems iditems;
    TreeWidgetItem* item = nullptr;
};

using ThreadDataMap = std::unordered_map<profiler::thread_id_t, ThreadData, estd::hash<profiler::thread_id_t> >;

void calculateMedians(StatsMap::iterator begin, StatsMap::iterator end)
{
    for (auto it = begin; it != end; ++it)
    {
        auto& durations = it->second.durations;
        if (durations.empty())
        {
            continue;
        }

        it->second.stats.median_duration = profiler_gui::calculateMedian(durations);

        decltype(it->second.durations) dummy;
        dummy.swap(durations);
    }
}

} // end of namespace <noname>.

static void fillStatsColumns(
    TreeWidgetItem* item,
    const profiler::BlockStatistics* stats,
    profiler_gui::TimeUnits units,
    int min_column,
    int max_column,
    int avg_column,
    int median_column,
    int total_column,
    int n_calls_column
) {
    item->setData(n_calls_column, Qt::UserRole, stats->calls_number);
    item->setText(n_calls_column, QString::number(stats->calls_number));

    if (min_column == COL_MIN_PER_AREA)
    {
        item->setData(min_column, MinMaxBlockIndexRole, stats->min_duration_block);
        item->setData(max_column, MinMaxBlockIndexRole, stats->max_duration_block);
    }

    if (stats->calls_number < 2)
    {
        return;
    }

    const auto min_duration = easyBlock(stats->min_duration_block).tree.node->duration();
    const auto max_duration = easyBlock(stats->max_duration_block).tree.node->duration();
    const auto avg_duration = stats->average_duration();
    const auto tot_duration = stats->total_duration;
    const auto median_duration = stats->median_duration;

    item->setTimeSmart(min_column, units, min_duration);
    item->setTimeSmart(max_column, units, max_duration);
    item->setTimeSmart(avg_column, units, avg_duration);
    item->setTimeSmart(median_column, units, median_duration);
    item->setTimeSmart(total_column, units, tot_duration);

    if (stats->calls_number > 1 && tot_duration != 0)
    {
        if (max_duration >= (tot_duration >> 1))
        {
            item->setForeground(max_column, QColor::fromRgb(profiler::colors::RedA700));
        }
        else if (max_duration >= (tot_duration >> 2))
        {
            item->setForeground(max_column, QColor::fromRgb(profiler::colors::OrangeA400));
        }
    }
}

inline void fillStatsColumnsThread(TreeWidgetItem* item, const profiler::BlockStatistics* stats, profiler_gui::TimeUnits units)
{
    fillStatsColumns(
        item,
        stats,
        units,
        COL_MIN_PER_THREAD,
        COL_MAX_PER_THREAD,
        COL_AVG_PER_THREAD,
        COL_MEDIAN_PER_THREAD,
        COL_TOTAL_TIME_PER_THREAD,
        COL_NCALLS_PER_THREAD
    );
}

inline void fillStatsColumnsFrame(TreeWidgetItem* item, const profiler::BlockStatistics* stats, profiler_gui::TimeUnits units)
{
    fillStatsColumns(
        item,
        stats,
        units,
        COL_MIN_PER_FRAME,
        COL_MAX_PER_FRAME,
        COL_AVG_PER_FRAME,
        COL_MEDIAN_PER_FRAME,
        COL_TOTAL_TIME_PER_FRAME,
        COL_NCALLS_PER_FRAME
    );
}

inline void fillStatsColumnsParent(TreeWidgetItem* item, const profiler::BlockStatistics* stats, profiler_gui::TimeUnits units)
{
    fillStatsColumns(
        item,
        stats,
        units,
        COL_MIN_PER_PARENT,
        COL_MAX_PER_PARENT,
        COL_AVG_PER_PARENT,
        COL_MEDIAN_PER_PARENT,
        COL_TOTAL_TIME_PER_PARENT,
        COL_NCALLS_PER_PARENT
    );
}

inline void fillStatsColumnsSelection(TreeWidgetItem* item, const profiler::BlockStatistics* stats, profiler_gui::TimeUnits units)
{
    fillStatsColumns(
        item,
        stats,
        units,
        COL_MIN_PER_AREA,
        COL_MAX_PER_AREA,
        COL_AVG_PER_AREA,
        COL_MEDIAN_PER_AREA,
        COL_TOTAL_TIME_PER_AREA,
        COL_NCALLS_PER_AREA
    );
}

TreeWidgetLoader::TreeWidgetLoader()
    : m_worker(true)
    , m_bDone(EASY_INIT_ATOMIC(false))
    , m_bInterrupt(EASY_INIT_ATOMIC(false))
    , m_progress(EASY_INIT_ATOMIC(0))
    , m_mode(TreeMode::Full)
{
}

TreeWidgetLoader::~TreeWidgetLoader()
{
    interrupt(true);
}

bool TreeWidgetLoader::done() const
{
    return m_bDone.load(std::memory_order_acquire);
}

void TreeWidgetLoader::setDone()
{
    m_bDone.store(true, std::memory_order_release);
    //m_progress.store(100);
}

void TreeWidgetLoader::setProgress(int _progress)
{
    m_progress.store(_progress, std::memory_order_release);
}

bool TreeWidgetLoader::interrupted() const volatile
{
    return m_bInterrupt.load(std::memory_order_acquire);
}

int TreeWidgetLoader::progress() const
{
    return m_progress.load(std::memory_order_acquire);
}

void TreeWidgetLoader::takeTopLevelItems(ThreadedItems& _output)
{
    if (done())
    {
        _output = std::move(m_topLevelItems);
        m_topLevelItems.clear();
    }
}

void TreeWidgetLoader::takeItems(Items& _output)
{
    if (done())
    {
        _output = std::move(m_items);
        m_items.clear();
    }
}

QString TreeWidgetLoader::error() const
{
    return done() ? m_error : QString();
}

void TreeWidgetLoader::interrupt(bool _wait)
{
    m_worker.dequeue();
    m_bDone.store(false, std::memory_order_release);
    m_progress.store(0, std::memory_order_release);

    if (!m_topLevelItems.empty())
    {
        if (!_wait)
        {
            auto topLevelItems = std::move(m_topLevelItems);

#ifdef EASY_LAMBDA_MOVE_CAPTURE
            ThreadPool::instance().backgroundJob([items = std::move(topLevelItems)] {
                for (auto item : items)
#else
            ThreadPool::instance().backgroundJob([=] {
                for (auto item : topLevelItems)
#endif
                    profiler_gui::deleteTreeItem(item.second);
            });
        }
        else
        {
            for (auto item : m_topLevelItems)
                profiler_gui::deleteTreeItem(item.second);
        }
    }

    m_items.clear();
    m_topLevelItems.clear();
    m_error.clear();
}

void TreeWidgetLoader::fillTreeBlocks(
    const profiler_gui::TreeBlocks& _blocks,
    profiler::timestamp_t _beginTime,
    profiler::timestamp_t _left,
    profiler::timestamp_t _right,
    bool _strict,
    TreeMode _mode
) {
    interrupt();
    m_mode = _mode;

    const auto zeroBlocks = EASY_GLOBALS.add_zero_blocks_to_hierarchy;
    const auto decoratedNames = EASY_GLOBALS.use_decorated_thread_name;
    const auto hexThreadIds = EASY_GLOBALS.hex_thread_id;
    const auto timeUnits = EASY_GLOBALS.time_units;
    const auto maxCount = EASY_GLOBALS.max_rows_count;
    const auto blocks = std::ref(_blocks);
    const auto mode = m_mode;
    m_worker.enqueue([=] {
        switch (mode)
        {
            case TreeMode::Full:
            {
                setTreeInternalTop(
                    _beginTime,
                    blocks,
                    _left,
                    _right,
                    _strict,
                    zeroBlocks,
                    decoratedNames,
                    hexThreadIds,
                    timeUnits,
                    maxCount
                );
                break;
            }
            case TreeMode::Plain:
            {
                setTreeInternalPlainTop(
                    _beginTime,
                    blocks,
                    _left,
                    _right,
                    _strict,
                    zeroBlocks,
                    decoratedNames,
                    hexThreadIds,
                    timeUnits
                );
                break;
            }
            case TreeMode::SelectedArea:
            {
                setTreeInternalAggregateTop(
                    _beginTime,
                    blocks,
                    _left,
                    _right,
                    _strict,
                    zeroBlocks,
                    decoratedNames,
                    hexThreadIds,
                    timeUnits
                );
                break;
            }
        }
    }, m_bInterrupt);
}

//////////////////////////////////////////////////////////////////////////

using BeginEndIndicesMap = std::unordered_map<profiler::thread_id_t, profiler::block_index_t,
    ::estd::hash<profiler::thread_id_t> >;

void TreeWidgetLoader::setTreeInternalTop(
    const profiler::timestamp_t& _beginTime,
    const profiler_gui::TreeBlocks& _blocks,
    profiler::timestamp_t _left,
    profiler::timestamp_t _right,
    bool _strict,
    bool _addZeroBlocks,
    bool _decoratedThreadNames,
    bool _hexThreadId,
    profiler_gui::TimeUnits _units,
    size_t _maxCount
) {
    BeginEndIndicesMap beginEndMap;
    ThreadDataMap threadsMap;

    auto total = static_cast<int>(_blocks.size());

    uint32_t total_count = 0;
    int i = 0;
    for (const auto& block : _blocks)
    {
        if (interrupted())
        {
            setDone();
            return;
        }

        const auto& gui_block = easyBlock(block.tree);
        const auto& tree = gui_block.tree;
        const auto startTime = tree.node->begin();
        const auto endTime = tree.node->end();

        if (startTime > _right || endTime < _left)
        {
            setProgress((3 * ++i) / total);
            continue;
        }

        const profiler::timestamp_t duration = endTime - startTime;
        const bool partial = _strict && (startTime < _left || endTime > _right);
        if (partial && duration != 0 && (startTime == _right || endTime == _left))
        {
            setProgress((3 * ++i) / total);
            continue;
        }

        total_count += calculateChildrenCountRecursive(tree.children, startTime, endTime, _strict, partial, _addZeroBlocks) + 1;

        setProgress((3 * ++i) / total);
    }

    if (total_count > _maxCount)
    {
        m_error = QString(
            "Exceeded maximum rows count = %1.\n"
            "Actual rows count: %2 (%3%).\n"
            "Please, reduce selected area width\n"
            "or increase maximum count in settings\n"
            "or change the tree mode."
        ).arg(profiler_gui::shortenCountString(_maxCount))
            .arg(profiler_gui::shortenCountString(total_count))
            .arg(profiler_gui::percent(total_count, _maxCount));
        setDone();
        return;
    }

    const auto u_thread = profiler_gui::toUnicode("thread");

    i = 0;
    for (const auto& block : _blocks)
    {
        if (interrupted())
            break;

        auto& gui_block = easyBlock(block.tree);
        auto& tree = gui_block.tree;
        const auto startTime = tree.node->begin();
        const auto endTime = tree.node->end();

        if (startTime > _right || endTime < _left)
        {
            setProgress(3 + (92 * ++i) / total);
            continue;
        }

        const profiler::timestamp_t duration = endTime - startTime;

        const bool partial = _strict && (startTime < _left || endTime > _right);
        if (partial && duration != 0 && (startTime == _right || endTime == _left))
        {
            setProgress(3 + (92 * ++i) / total);
            continue;
        }

        auto& thread_data = threadsMap[block.root->thread_id];
        auto thread_item = thread_data.item;
        profiler::block_index_t& firstCswitch = beginEndMap[block.root->thread_id];
        StatsMap& stats = thread_data.stats;
        IdItems& iditems = thread_data.iditems;

        if (thread_item == nullptr)
        {
            thread_item = new TreeWidgetItem();
            thread_data.item = thread_item;

            thread_item->setText(COL_NAME, profiler_gui::decoratedThreadName(_decoratedThreadNames, *block.root, u_thread, _hexThreadId));
            thread_item->setData(COL_NAME, Qt::UserRole, static_cast<quint64>(block.root->thread_id));

            profiler::timestamp_t thread_duration = 0;
            if (!block.root->children.empty())
                thread_duration = easyBlocksTree(block.root->children.back()).node->end()
                                  - easyBlocksTree(block.root->children.front()).node->begin();

            thread_item->setTimeSmart(COL_TIME, _units, thread_duration);
            thread_item->setBackgroundColor(profiler_gui::SELECTED_THREAD_BACKGROUND);

            // Sum of all children durations:
            thread_item->setTimeSmart(COL_SELF_TIME, _units, block.root->profiled_time);

            firstCswitch = 0;
            auto it = std::lower_bound(block.root->sync.begin(), block.root->sync.end(), _left, [](profiler::block_index_t ind, decltype(_left) _val)
            {
                return EASY_GLOBALS.gui_blocks[ind].tree.node->begin() < _val;
            });

            if (it != block.root->sync.end())
            {
                firstCswitch = static_cast<profiler::block_index_t>(std::distance(block.root->sync.begin(), it));
                if (firstCswitch > 0)
                    --firstCswitch;
            }
            else
            {
                firstCswitch = static_cast<profiler::block_index_t>(block.root->sync.size());
            }
        }

        // Variable firstCswitch modified on each call to calculateIdleTime().
        // Just because all blocks are sorted by startTime, it's OK to modify firstCswitch - this will speed-up future calculateIdleTime calls
        const auto idleTime = calculateIdleTime(*block.root, firstCswitch, startTime, endTime);

        auto item = new TreeWidgetItem(block.tree, thread_item);
        item->setPartial(partial);

        auto name = *tree.node->name() != 0 ? tree.node->name() : easyDescriptor(tree.node->id()).name();
        item->setText(COL_NAME, profiler_gui::toUnicode(name));
        item->setTimeSmart(COL_TIME, _units, duration);

        auto active_time = duration - idleTime;
        auto active_percent = duration == 0 ? 100. : profiler_gui::percentReal(active_time, duration);
        item->setTimeSmart(COL_ACTIVE_TIME, _units, active_time);
        item->setText(COL_ACTIVE_PERCENT, QString::number(active_percent, 'g', 3));
        item->setData(COL_ACTIVE_PERCENT, Qt::UserRole, active_percent);

        item->setTimeMs(COL_BEGIN, startTime - _beginTime);
        item->setTimeMs(COL_END, endTime - _beginTime);

        item->setData(COL_PERCENT_PER_FRAME, Qt::UserRole, 0);

        auto percentage_per_thread = profiler_gui::percent(duration, block.root->profiled_time);
        item->setData(COL_PERCENT_PER_PARENT, Qt::UserRole, percentage_per_thread);
        item->setText(COL_PERCENT_PER_PARENT, QString::number(percentage_per_thread));

        if (tree.per_thread_stats != nullptr) // if there is per_thread_stats then there are other stats also
        {
            const auto per_thread_stats = tree.per_thread_stats;
            const auto per_parent_stats = tree.per_parent_stats;
            const auto per_frame_stats  = tree.per_frame_stats;

            fillStatsColumnsThread(item, per_thread_stats, _units);
            fillStatsColumnsParent(item, per_parent_stats, _units);
            fillStatsColumnsFrame(item, per_frame_stats, _units);

            percentage_per_thread = profiler_gui::percent(per_thread_stats->total_duration, block.root->profiled_time);
            item->setData(COL_PERCENT_SUM_PER_THREAD, Qt::UserRole, percentage_per_thread);
            item->setText(COL_PERCENT_SUM_PER_THREAD, QString::number(percentage_per_thread));
        }
        else
        {
            item->setData(COL_PERCENT_SUM_PER_THREAD, Qt::UserRole, 0);
            item->setText(COL_PERCENT_SUM_PER_THREAD, "");
        }

        const auto color = easyDescriptor(gui_block.tree.node->id()).color();
        item->setBackgroundColor(color);

        size_t children_items_number = 0;
        profiler::timestamp_t children_duration = 0;
        if (!gui_block.tree.children.empty())
        {
            iditems.clear();

            children_items_number = setTreeInternal(*block.root, iditems, stats, firstCswitch, _beginTime, tree.children,
                item, item, _left, _right, _strict, partial, children_duration, _addZeroBlocks, _units, 1);

            if (interrupted())
                break;

            if (partial && children_items_number == 0)
            {
                delete item;
                setProgress(3 + (92 * ++i) / total);
                continue;
            }
        }

        int percentage = 100;
        auto self_duration = duration - children_duration;
        if (children_duration > 0 && duration > 0)
        {
            percentage = static_cast<int>(0.5 + 100. * static_cast<double>(self_duration) / static_cast<double>(duration));
        }

        item->setTimeSmart(COL_SELF_TIME, _units, self_duration);
        item->setData(COL_SELF_TIME_PERCENT, Qt::UserRole, percentage);
        item->setText(COL_SELF_TIME_PERCENT, QString::number(percentage));

        updateStats(stats, tree.node->id(), block.tree, duration, children_duration);

        if (gui_block.expanded)
            item->setExpanded(true);

        m_items.insert(std::make_pair(block.tree, item));

        setProgress(3 + (92 * ++i) / total);
    }

    i = 0;
    total = static_cast<int>(threadsMap.size());
    for (auto& it : threadsMap)
    {
        auto& thread_data = it.second;
        auto item = thread_data.item;

        if (item->childCount() > 0)
        {
            m_topLevelItems.emplace_back(it.first, item);
            fillStatsForTree(item, thread_data.stats, _units, _right - _left);
        }
        else
        {
            delete item;
        }

        setProgress(95 + (5 * ++i) / total);
    }

    setDone();
}

//////////////////////////////////////////////////////////////////////////

void TreeWidgetLoader::setTreeInternalPlainTop(
    const profiler::timestamp_t& _beginTime,
    const profiler_gui::TreeBlocks& _blocks,
    profiler::timestamp_t _left,
    profiler::timestamp_t _right,
    bool _strict,
    bool _addZeroBlocks,
    bool _decoratedThreadNames,
    bool _hexThreadId,
    profiler_gui::TimeUnits _units
) {
    BeginEndIndicesMap beginEndMap;
    ThreadDataMap threadsMap;

    const auto u_thread = profiler_gui::toUnicode("thread");
    int i = 0, total = static_cast<int>(_blocks.size());
    for (const auto& block : _blocks)
    {
        if (interrupted())
            break;

        const auto block_index = block.tree;
        auto& gui_block = easyBlock(block_index);
        auto& tree = gui_block.tree;
        const auto startTime = tree.node->begin();
        const auto endTime = tree.node->end();

        if (startTime > _right || endTime < _left)
        {
            setProgress((95 * ++i) / total);
            continue;
        }

        const profiler::timestamp_t duration = endTime - startTime;

        const bool partial = _strict && (startTime < _left || endTime > _right);
        if (partial && duration != 0 && (startTime == _right || endTime == _left))
        {
            setProgress((95 * ++i) / total);
            continue;
        }

        auto& thread_data = threadsMap[block.root->thread_id];
        auto thread_item = thread_data.item;
        profiler::block_index_t& firstCswitch = beginEndMap[block.root->thread_id];
        StatsMap& stats = thread_data.stats;
        IdItems& iditems = thread_data.iditems;

        if (thread_item == nullptr)
        {
            thread_item = new TreeWidgetItem();
            thread_data.item = thread_item;

            thread_item->setText(COL_NAME, profiler_gui::decoratedThreadName(_decoratedThreadNames, *block.root, u_thread, _hexThreadId));
            thread_item->setData(COL_NAME, Qt::UserRole, static_cast<quint64>(block.root->thread_id));

            profiler::timestamp_t thread_duration = 0;
            if (!block.root->children.empty())
                thread_duration = easyBlocksTree(block.root->children.back()).node->end()
                                  - easyBlocksTree(block.root->children.front()).node->begin();

            thread_item->setTimeSmart(COL_TIME, _units, thread_duration);
            thread_item->setBackgroundColor(profiler_gui::SELECTED_THREAD_BACKGROUND);

            // Sum of all children durations:
            thread_item->setTimeSmart(COL_SELF_TIME, _units, block.root->profiled_time);

            firstCswitch = 0;
            auto it = std::lower_bound(block.root->sync.begin(), block.root->sync.end(), _left, [] (profiler::block_index_t ind, decltype(_left) _val)
            {
                return EASY_GLOBALS.gui_blocks[ind].tree.node->begin() < _val;
            });

            if (it != block.root->sync.end())
            {
                firstCswitch = static_cast<profiler::block_index_t>(std::distance(block.root->sync.begin(), it));
                if (firstCswitch > 0)
                    --firstCswitch;
            }
            else
            {
                firstCswitch = static_cast<profiler::block_index_t>(block.root->sync.size());
            }
        }

        // Variable firstCswitch modified on each call to calculateIdleTime().
        // Just because all blocks are sorted by startTime, it's OK to modify firstCswitch - this will speed-up future calculateIdleTime calls
        const auto idleTime = calculateIdleTime(*block.root, firstCswitch, startTime, endTime);

        auto it = iditems.find(tree.node->id());
        if (it != iditems.end())
        {
            auto item = it->second.first;
            size_t children_items_count = 0;
            profiler::timestamp_t children_duration = 0;

            if (!tree.children.empty())
            {
                children_items_count = setTreeInternalPlain(*block.root, iditems, stats, firstCswitch, _beginTime, tree.children,
                    item, item, _left, _right, _strict, partial, children_duration, _addZeroBlocks, _units, 1);

                if (interrupted())
                    break;

                if (partial && children_items_count == 0)
                {
                    setProgress((95 * ++i) / total);
                    continue;
                }
            }

            if (tree.per_thread_stats != nullptr)
            {
                // stats for current selection
                updateStats(stats, tree.node->id(), block_index, duration, children_duration);

                // stats
                if (children_duration != 0)
                {
                    const auto self_duration = duration - children_duration + item->data(COL_SELF_TIME, Qt::UserRole).toULongLong();

                    int percentage = 100;
                    if (tree.per_thread_stats->total_duration > 0)
                    {
                        percentage = profiler_gui::percent(self_duration, tree.per_thread_stats->total_duration);
                    }

                    item->setTimeSmart(COL_SELF_TIME, _units, self_duration);
                    item->setData(COL_SELF_TIME_PERCENT, Qt::UserRole, percentage);
                    item->setText(COL_SELF_TIME_PERCENT, QString::number(percentage));
                }

                auto active_time = duration - idleTime + item->data(COL_ACTIVE_TIME, Qt::UserRole).toULongLong();
                auto active_percent = tree.per_thread_stats->total_duration == 0 ? 100. : profiler_gui::percentReal(active_time, tree.per_thread_stats->total_duration);
                item->setTimeSmart(COL_ACTIVE_TIME, _units, active_time);
                item->setText(COL_ACTIVE_PERCENT, QString::number(active_percent, 'g', 3));
                item->setData(COL_ACTIVE_PERCENT, Qt::UserRole, active_percent);
            }

            const auto total_duration = item->data(COL_TIME, Qt::UserRole).toULongLong() + duration;
            auto percentage_per_thread = profiler_gui::percent(total_duration, block.root->profiled_time);
            item->setData(COL_PERCENT_PER_PARENT, Qt::UserRole, percentage_per_thread);
            item->setText(COL_PERCENT_PER_PARENT, QString::number(percentage_per_thread));

            setProgress((95 * ++i) / total);

            continue;
        }

        auto item = new TreeWidgetItem(block.tree, thread_item);
        item->setPartial(partial);

        iditems[tree.node->id()] = std::make_pair(item, 0);

        auto name = *tree.node->name() != 0 ? tree.node->name() : easyDescriptor(tree.node->id()).name();
        item->setText(COL_NAME, profiler_gui::toUnicode(name));
        item->setTimeSmart(COL_TIME, _units, duration);

        auto active_time = duration - idleTime;
        auto active_percent = duration == 0 ? 100. : profiler_gui::percentReal(active_time, duration);
        item->setTimeSmart(COL_ACTIVE_TIME, _units, active_time);
        item->setText(COL_ACTIVE_PERCENT, QString::number(active_percent, 'g', 3));
        item->setData(COL_ACTIVE_PERCENT, Qt::UserRole, active_percent);

        item->setTimeMs(COL_BEGIN, startTime - _beginTime);
        item->setTimeMs(COL_END, endTime - _beginTime);

        item->setData(COL_PERCENT_PER_FRAME, Qt::UserRole, 100);

        auto percentage_per_thread = profiler_gui::percent(duration, block.root->profiled_time);
        item->setData(COL_PERCENT_PER_PARENT, Qt::UserRole, percentage_per_thread);
        item->setText(COL_PERCENT_PER_PARENT, QString::number(percentage_per_thread));

        if (tree.per_thread_stats != nullptr) // if there is per_thread_stats then there are other stats also
        {
            const auto per_thread_stats = tree.per_thread_stats;
            const auto per_parent_stats = tree.per_parent_stats;
            const auto per_frame_stats = tree.per_frame_stats;

            fillStatsColumnsThread(item, per_thread_stats, _units);
            fillStatsColumnsParent(item, per_parent_stats, _units);
            fillStatsColumnsFrame(item, per_frame_stats, _units);

            percentage_per_thread = profiler_gui::percent(per_thread_stats->total_duration, block.root->profiled_time);
            item->setData(COL_PERCENT_SUM_PER_THREAD, Qt::UserRole, percentage_per_thread);
            item->setText(COL_PERCENT_SUM_PER_THREAD, QString::number(percentage_per_thread));
        }
        else
        {
            item->setData(COL_PERCENT_SUM_PER_THREAD, Qt::UserRole, 0);
            item->setText(COL_PERCENT_SUM_PER_THREAD, "");
        }

        const auto color = easyDescriptor(tree.node->id()).color();
        item->setBackgroundColor(color);

        size_t children_items_number = 0;
        profiler::timestamp_t children_duration = 0;
        if (!tree.children.empty())
        {
            children_items_number = setTreeInternalPlain(*block.root, iditems, stats, firstCswitch, _beginTime,
                tree.children, item, item, _left, _right, _strict, partial, children_duration, _addZeroBlocks, _units, 1);

            if (interrupted())
                break;

            if (partial && children_items_number == 0)
            {
                delete item;
                iditems.clear();
                setProgress((95 * ++i) / total);
                continue;
            }
        }

        iditems.clear();

        int percentage = 100;
        auto self_duration = duration - children_duration;
        if (children_duration > 0 && duration > 0)
        {
            percentage = static_cast<int>(0.5 + 100. * static_cast<double>(self_duration) / static_cast<double>(duration));
        }

        item->setTimeSmart(COL_SELF_TIME, _units, self_duration);
        item->setData(COL_SELF_TIME_PERCENT, Qt::UserRole, percentage);
        item->setText(COL_SELF_TIME_PERCENT, QString::number(percentage));

        updateStats(stats, tree.node->id(), block_index, duration, children_duration);

        if (gui_block.expanded)
            item->setExpanded(true);

        m_items.insert(std::make_pair(block.tree, item));

        setProgress((95 * ++i) / total);
    }

    i = 0;
    total = static_cast<int>(threadsMap.size());
    for (auto& it : threadsMap)
    {
        auto& thread_data = it.second;
        auto item = thread_data.item;

        if (item->childCount() > 0)
        {
            m_topLevelItems.emplace_back(it.first, item);
            fillStatsForTree(item, thread_data.stats, _units, _right - _left);
        }
        else
        {
            delete item;
        }

        setProgress(95 + (5 * ++i) / total);
    }

    setDone();
}

//////////////////////////////////////////////////////////////////////////

void TreeWidgetLoader::setTreeInternalAggregateTop(
    const profiler::timestamp_t& _beginTime,
    const profiler_gui::TreeBlocks& _blocks,
    profiler::timestamp_t _left,
    profiler::timestamp_t _right,
    bool _strict,
    bool _addZeroBlocks,
    bool _decoratedThreadNames,
    bool _hexThreadId,
    profiler_gui::TimeUnits _units
) {
    BeginEndIndicesMap beginEndMap;
    ThreadDataMap threadsMap;

    const auto u_thread = profiler_gui::toUnicode("thread");
    int i = 0, total = static_cast<int>(_blocks.size());
    for (const auto& block : _blocks)
    {
        if (interrupted())
            break;

        const auto block_index = block.tree;
        auto& gui_block = easyBlock(block_index);
        auto& tree = gui_block.tree;
        const auto startTime = tree.node->begin();
        const auto endTime = tree.node->end();

        if (startTime > _right || endTime < _left)
        {
            setProgress((95 * ++i) / total);
            continue;
        }

        const profiler::timestamp_t duration = endTime - startTime;

        const bool partial = _strict && (startTime < _left || endTime > _right);
        if (partial && duration != 0 && (startTime == _right || endTime == _left))
        {
            setProgress((95 * ++i) / total);
            continue;
        }

        auto& thread_data = threadsMap[block.root->thread_id];
        auto thread_item = thread_data.item;
        profiler::block_index_t& firstCswitch = beginEndMap[block.root->thread_id];
        StatsMap& stats = thread_data.stats;
        IdItems& iditems = thread_data.iditems;

        if (thread_item == nullptr)
        {
            thread_item = new TreeWidgetItem();
            thread_data.item = thread_item;

            thread_item->setText(COL_NAME, profiler_gui::decoratedThreadName(_decoratedThreadNames, *block.root, u_thread, _hexThreadId));
            thread_item->setData(COL_NAME, Qt::UserRole, static_cast<quint64>(block.root->thread_id));

            profiler::timestamp_t thread_duration = 0;
            if (!block.root->children.empty())
                thread_duration = easyBlocksTree(block.root->children.back()).node->end()
                                  - easyBlocksTree(block.root->children.front()).node->begin();

            thread_item->setTimeSmart(COL_TIME, _units, thread_duration);
            thread_item->setBackgroundColor(profiler_gui::SELECTED_THREAD_BACKGROUND);

            // Sum of all children durations:
            thread_item->setTimeSmart(COL_SELF_TIME, _units, block.root->profiled_time);

            firstCswitch = 0;
            auto it = std::lower_bound(block.root->sync.begin(), block.root->sync.end(), _left, [] (profiler::block_index_t ind, decltype(_left) _val)
            {
                return EASY_GLOBALS.gui_blocks[ind].tree.node->begin() < _val;
            });

            if (it != block.root->sync.end())
            {
                firstCswitch = static_cast<profiler::block_index_t>(std::distance(block.root->sync.begin(), it));
                if (firstCswitch > 0)
                    --firstCswitch;
            }
            else
            {
                firstCswitch = static_cast<profiler::block_index_t>(block.root->sync.size());
            }
        }

        // Variable firstCswitch modified on each call to calculateIdleTime().
        // Just because all blocks are sorted by startTime, it's OK to modify firstCswitch - this will speed-up future calculateIdleTime calls
        const auto idleTime = calculateIdleTime(*block.root, firstCswitch, startTime, endTime);

        auto it = iditems.find(tree.node->id());
        if (it != iditems.end())
        {
            auto item = it->second.first;
            size_t children_items_count = 0;
            profiler::timestamp_t children_duration = 0;

            if (!tree.children.empty())
            {
                children_items_count = setTreeInternalAggregate(*block.root, iditems, stats, firstCswitch, _beginTime,
                    tree.children, thread_item, _left, _right, _strict, partial, children_duration, _addZeroBlocks, _units, 1);

                if (interrupted())
                    break;

                if (partial && children_items_count == 0)
                {
                    setProgress((95 * ++i) / total);
                    continue;
                }
            }

            const auto total_duration = item->data(COL_TIME, Qt::UserRole).toULongLong() + duration;
            item->setTimeSmart(COL_TIME, _units, total_duration);

            if (tree.per_thread_stats != nullptr)
            {
                // stats for current selection
                updateStats(stats, tree.node->id(), block_index, duration, children_duration);
            }

            // stats
            if (children_duration != 0)
            {
                const auto self_duration = duration - children_duration + item->data(COL_SELF_TIME, Qt::UserRole).toULongLong();

                int percentage = 100;
                if (total_duration > 0)
                {
                    percentage = profiler_gui::percent(self_duration, total_duration);
                }

                item->setTimeSmart(COL_SELF_TIME, _units, self_duration);
                item->setData(COL_SELF_TIME_PERCENT, Qt::UserRole, percentage);
                item->setText(COL_SELF_TIME_PERCENT, QString::number(percentage));
            }

            auto active_time = duration - idleTime + item->data(COL_ACTIVE_TIME, Qt::UserRole).toULongLong();
            auto active_percent = total_duration == 0 ? 100. : profiler_gui::percentReal(active_time, total_duration);
            item->setTimeSmart(COL_ACTIVE_TIME, _units, active_time);
            item->setText(COL_ACTIVE_PERCENT, QString::number(active_percent, 'g', 3));
            item->setData(COL_ACTIVE_PERCENT, Qt::UserRole, active_percent);

            setProgress((95 * ++i) / total);

            continue;
        }

        auto item = new TreeWidgetItem(block.tree, thread_item);
        item->setPartial(partial);

        iditems[tree.node->id()] = std::make_pair(item, 0);

        auto name = *tree.node->name() != 0 ? tree.node->name() : easyDescriptor(tree.node->id()).name();
        item->setText(COL_NAME, profiler_gui::toUnicode(name));
        item->setTimeSmart(COL_TIME, _units, duration);

        auto active_time = duration - idleTime;
        auto active_percent = duration == 0 ? 100. : profiler_gui::percentReal(active_time, duration);
        item->setTimeSmart(COL_ACTIVE_TIME, _units, active_time);
        item->setText(COL_ACTIVE_PERCENT, QString::number(active_percent, 'g', 3));
        item->setData(COL_ACTIVE_PERCENT, Qt::UserRole, active_percent);

        const auto per_thread_stats = gui_block.tree.per_thread_stats;
        if (per_thread_stats != nullptr)
        {
            fillStatsColumnsThread(item, per_thread_stats, _units);
            auto percent_per_thread = profiler_gui::percent(per_thread_stats->total_duration, block.root->profiled_time);
            item->setData(COL_PERCENT_SUM_PER_THREAD, Qt::UserRole, percent_per_thread);
            item->setText(COL_PERCENT_SUM_PER_THREAD, QString::number(percent_per_thread));
        }
        else
        {
            item->setData(COL_PERCENT_SUM_PER_THREAD, Qt::UserRole, 0);
        }

        const auto color = easyDescriptor(gui_block.tree.node->id()).color();
        item->setBackgroundColor(color);

        size_t children_items_number = 0;
        profiler::timestamp_t children_duration = 0;
        if (!tree.children.empty())
        {
            children_items_number = setTreeInternalAggregate(*block.root, iditems, stats, firstCswitch, _beginTime,
                gui_block.tree.children, thread_item, _left, _right, _strict, partial, children_duration, _addZeroBlocks, _units, 1);

            if (interrupted())
                break;

            if (partial && children_items_number == 0)
            {
                delete item;
                iditems.erase(tree.node->id());
                setProgress((95 * ++i) / total);
                continue;
            }
        }

        int percent_value = 100;
        auto self_duration = duration - children_duration;
        if (children_duration > 0 && duration > 0)
        {
            percent_value = static_cast<int>(0.5 + 100. * static_cast<double>(self_duration) / static_cast<double>(duration));
        }

        item->setTimeSmart(COL_SELF_TIME, _units, self_duration);
        item->setData(COL_SELF_TIME_PERCENT, Qt::UserRole, percent_value);
        item->setText(COL_SELF_TIME_PERCENT, QString::number(percent_value));

        updateStats(stats, tree.node->id(), block_index, duration, children_duration);

        if (gui_block.expanded)
            item->setExpanded(true);

        m_items.insert(std::make_pair(block.tree, item));

        setProgress((95 * ++i) / total);
    }

    i = 0;
    total = static_cast<int>(threadsMap.size());
    for (auto& it : threadsMap)
    {
        auto& thread_data = it.second;
        auto item = thread_data.item;

        if (item->childCount() > 0)
        {
            m_topLevelItems.emplace_back(it.first, item);
            fillStatsForTree(item, thread_data.stats, _units, _right - _left);
        }
        else
        {
            delete item;
        }

        setProgress(95 + (5 * ++i) / total);
    }

    setDone();
}

//////////////////////////////////////////////////////////////////////////

size_t TreeWidgetLoader::setTreeInternal(
    const profiler::BlocksTreeRoot& _threadRoot,
    IdItems& _iditems,
    StatsMap& _statsMap,
    profiler::block_index_t _firstCswitch,
    profiler::timestamp_t _beginTime,
    const profiler::BlocksTree::children_t& _children,
    TreeWidgetItem* _parent,
    TreeWidgetItem* _frame,
    profiler::timestamp_t left,
    profiler::timestamp_t right,
    bool strict,
    bool partial_parent,
    profiler::timestamp_t& _duration,
    bool _addZeroBlocks,
    profiler_gui::TimeUnits _units,
    int _depth
) {
    size_t total_items = 0;
    for (auto child_index : _children)
    {
        if (interrupted())
            break;

        auto& gui_block = easyBlock(child_index);
        const auto& child = gui_block.tree;
        const auto startTime = child.node->begin();
        const auto endTime = child.node->end();
        const auto duration = endTime - startTime;

        if (startTime > right || endTime < left)
            continue;

        const bool partial = strict && (startTime < left || endTime > right);
        if (partial && partial_parent && duration != 0 && (startTime == right || endTime == left))
            continue;

        const auto& desc = easyDescriptor(child.node->id());

        if (duration == 0 && !_addZeroBlocks && desc.type() == profiler::BlockType::Block)
            continue;

        const auto idleTime = calculateIdleTime(_threadRoot, _firstCswitch, startTime, endTime);

        auto item = new TreeWidgetItem(child_index, _parent);

        auto name = *child.node->name() != 0 ? child.node->name() : desc.name();
        item->setText(COL_NAME, profiler_gui::toUnicode(name));
        item->setTimeSmart(COL_TIME, _units, duration);

        auto active_time = duration - idleTime;
        auto active_percent = duration == 0 ? 100. : profiler_gui::percentReal(active_time, duration);
        item->setTimeSmart(COL_ACTIVE_TIME, _units, active_time);
        item->setText(COL_ACTIVE_PERCENT, QString::number(active_percent, 'g', 3));
        item->setData(COL_ACTIVE_PERCENT, Qt::UserRole, active_percent);

        item->setTimeMs(COL_BEGIN, startTime - _beginTime);
        item->setTimeMs(COL_END, endTime - _beginTime);
        item->setData(COL_PERCENT_SUM_PER_THREAD, Qt::UserRole, 0);

        if (child.per_thread_stats != nullptr) // if there is per_thread_stats then there are other stats also
        {
            const auto per_thread_stats = child.per_thread_stats;
            const auto per_parent_stats = child.per_parent_stats;
            const auto per_frame_stats  = child.per_frame_stats;

            auto parent_duration = _parent->data(COL_TIME, Qt::UserRole).toULongLong();
            auto percentage = duration == 0 ? 0 : profiler_gui::percent(duration, parent_duration);
            auto percentage_sum = profiler_gui::percent(per_parent_stats->total_duration, parent_duration);
            item->setData(COL_PERCENT_PER_PARENT, Qt::UserRole, percentage);
            item->setText(COL_PERCENT_PER_PARENT, QString::number(percentage));
            item->setData(COL_PERCENT_SUM_PER_PARENT, Qt::UserRole, percentage_sum);
            item->setText(COL_PERCENT_SUM_PER_PARENT, QString::number(percentage_sum));

            if (_frame != nullptr)
            {
                if (_parent != _frame)
                {
                    parent_duration = _frame->data(COL_TIME, Qt::UserRole).toULongLong();
                    percentage = duration == 0 ? 0 : profiler_gui::percent(duration, parent_duration);
                    percentage_sum = profiler_gui::percent(per_frame_stats->total_duration, parent_duration);
                }

                item->setData(COL_PERCENT_PER_FRAME, Qt::UserRole, percentage);
                item->setText(COL_PERCENT_PER_FRAME, QString::number(percentage));
                item->setData(COL_PERCENT_SUM_PER_FRAME, Qt::UserRole, percentage_sum);
                item->setText(COL_PERCENT_SUM_PER_FRAME, QString::number(percentage_sum));
            }
            else
            {
                item->setData(COL_PERCENT_PER_FRAME, Qt::UserRole, 0);
                item->setData(COL_PERCENT_SUM_PER_FRAME, Qt::UserRole, 0);

                auto percentage_per_thread = profiler_gui::percent(duration, _threadRoot.profiled_time);
                item->setData(COL_PERCENT_PER_PARENT, Qt::UserRole, percentage_per_thread);
                item->setText(COL_PERCENT_PER_PARENT, QString::number(percentage_per_thread));
            }

            fillStatsColumnsThread(item, per_thread_stats, _units);
            fillStatsColumnsParent(item, per_parent_stats, _units);
            fillStatsColumnsFrame(item, per_frame_stats, _units);

            auto percentage_per_thread = profiler_gui::percent(per_thread_stats->total_duration, _threadRoot.profiled_time);
            item->setData(COL_PERCENT_SUM_PER_THREAD, Qt::UserRole, percentage_per_thread);
            item->setText(COL_PERCENT_SUM_PER_THREAD, QString::number(percentage_per_thread));
        }
        else
        {
            if (_frame == nullptr)
            {
                auto percentage_per_thread = profiler_gui::percent(duration, _threadRoot.profiled_time);
                item->setData(COL_PERCENT_PER_PARENT, Qt::UserRole, percentage_per_thread);
                item->setText(COL_PERCENT_PER_PARENT, QString::number(percentage_per_thread));
            }
            else
            {
                item->setData(COL_PERCENT_PER_PARENT, Qt::UserRole, 0);
            }

            item->setData(COL_PERCENT_SUM_PER_PARENT, Qt::UserRole, 0);
            item->setData(COL_PERCENT_SUM_PER_THREAD, Qt::UserRole, 0);
        }

        item->setBackgroundColor(desc.color());

        size_t children_items_number = 0;
        profiler::timestamp_t children_duration = 0;
        if (!child.children.empty())
        {
            _iditems.clear();

            children_items_number = setTreeInternal(_threadRoot, _iditems, _statsMap, _firstCswitch, _beginTime, child.children,
                item, _frame ? _frame : item, left, right, strict, partial, children_duration, _addZeroBlocks, _units, _depth + 1);

            if (interrupted())
                break;

            if (partial && children_items_number == 0)
            {
                delete item;
                continue;
            }
        }

        _duration += duration;

        int percentage = 100;
        auto self_duration = duration - children_duration;
        if (children_duration > 0 && duration > 0)
        {
            percentage = profiler_gui::percent(self_duration, duration);
        }

        item->setTimeSmart(COL_SELF_TIME, _units, self_duration);
        item->setData(COL_SELF_TIME_PERCENT, Qt::UserRole, percentage);
        item->setText(COL_SELF_TIME_PERCENT, QString::number(percentage));

        total_items += children_items_number + 1;

        if (gui_block.expanded)
            item->setExpanded(true);

        m_items.insert(std::make_pair(child_index, item));

        updateStats(_statsMap, child.node->id(), child_index, duration, children_duration);
    }

    return total_items;
}

//////////////////////////////////////////////////////////////////////////

profiler::timestamp_t TreeWidgetLoader::calculateChildrenDurationRecursive(
    const profiler::BlocksTree::children_t& _children,
    profiler::block_id_t _id
) const {
    profiler::timestamp_t total_duration = 0;

    for (auto child_index : _children)
    {
        if (interrupted())
            break;

        const auto& gui_block = easyBlock(child_index);
        total_duration += gui_block.tree.node->duration();
        if (gui_block.tree.node->id() == _id)
            total_duration += calculateChildrenDurationRecursive(gui_block.tree.children, _id);
    }

    return total_duration;
}

uint32_t TreeWidgetLoader::calculateChildrenCountRecursive(
    const profiler::BlocksTree::children_t& children,
    profiler::timestamp_t left,
    profiler::timestamp_t right,
    bool strict,
    bool partial_parent,
    bool addZeroBlocks
) const {
    uint32_t count = 0;

    for (auto child_index : children)
    {
        if (interrupted())
            break;

        const auto& gui_block = easyBlock(child_index);
        const auto& child = gui_block.tree;
        const auto startTime = child.node->begin();
        const auto endTime = child.node->end();
        const auto duration = endTime - startTime;

        if (startTime > right || endTime < left)
            continue;

        const bool partial = strict && (startTime < left || endTime > right);
        if (partial && partial_parent && duration != 0 && (startTime == right || endTime == left))
            continue;

        const auto& desc = easyDescriptor(child.node->id());

        if (duration == 0 && !addZeroBlocks && desc.type() == profiler::BlockType::Block)
            continue;

        count += calculateChildrenCountRecursive(gui_block.tree.children, left, right, strict, partial, addZeroBlocks) + 1;
    }

    return count;
}

size_t TreeWidgetLoader::setTreeInternalPlain(
    const profiler::BlocksTreeRoot& threadRoot,
    IdItems& iditems,
    StatsMap& statsMap,
    profiler::block_index_t firstCswitch,
    profiler::timestamp_t beginTime,
    const profiler::BlocksTree::children_t& children,
    TreeWidgetItem* root,
    TreeWidgetItem* frame,
    profiler::timestamp_t left,
    profiler::timestamp_t right,
    bool strict,
    bool partial_parent,
    profiler::timestamp_t& total_duration,
    bool addZeroBlocks,
    profiler_gui::TimeUnits units,
    int depth
) {
    size_t total_items = 0;

    for (auto child_index : children)
    {
        if (interrupted())
            break;

        const auto& gui_block = easyBlock(child_index);
        const auto& child = gui_block.tree;
        const auto startTime = child.node->begin();
        const auto endTime = child.node->end();
        const auto duration = endTime - startTime;

        if (startTime > right || endTime < left)
            continue;

        const bool partial = strict && (startTime < left || endTime > right);
        if (partial && partial_parent && duration != 0 && (startTime == right || endTime == left))
            continue;

        const auto& desc = easyDescriptor(child.node->id());

        auto it = iditems.find(child.node->id());
        if (it != iditems.end())
        {
            size_t children_items_number = 0;
            profiler::timestamp_t children_duration = 0;
            if (!child.children.empty())
            {
                children_items_number = setTreeInternalPlain(threadRoot, iditems, statsMap, firstCswitch, beginTime,
                    child.children, root, frame, left, right, strict, partial, children_duration, addZeroBlocks, units, depth + 1);

                if (interrupted())
                    break;

                if (partial && children_items_number == 0)
                    continue;
            }

            total_duration += duration;
            ++total_items;

            updateStats(statsMap, child.node->id(), child_index, duration, children_duration);

            if (it->second.first != nullptr && child.per_frame_stats != nullptr)
            {
                auto item = it->second.first;

                //auto children_duration = calculateChildrenDurationRecursive(child.children, it->first);
                if (children_duration != 0)
                {
                    auto self_duration = duration + item->data(COL_SELF_TIME, Qt::UserRole).toULongLong() - children_duration;

                    int percentage = 100;
                    if (child.per_frame_stats->total_duration > 0)
                        percentage = profiler_gui::percent(self_duration, child.per_frame_stats->total_duration);

                    item->setTimeSmart(COL_SELF_TIME, units, self_duration);
                    item->setData(COL_SELF_TIME_PERCENT, Qt::UserRole, percentage);
                    item->setText(COL_SELF_TIME_PERCENT, QString::number(percentage));
                }

                const auto idleTime = calculateIdleTime(threadRoot, firstCswitch, startTime, endTime);
                const auto active_time = duration + item->data(COL_ACTIVE_TIME, Qt::UserRole).toULongLong() - idleTime;
                const auto active_percent = child.per_frame_stats->total_duration == 0 ? 100. : profiler_gui::percentReal(active_time, child.per_frame_stats->total_duration);
                item->setTimeSmart(COL_ACTIVE_TIME, units, active_time);
                item->setText(COL_ACTIVE_PERCENT, QString::number(active_percent, 'g', 3));
                item->setData(COL_ACTIVE_PERCENT, Qt::UserRole, active_percent);
            }

            continue;
        }

        const auto idleTime = calculateIdleTime(threadRoot, firstCswitch, startTime, endTime);

        auto item = new TreeWidgetItem(child_index, root);

        auto name = *child.node->name() != 0 ? child.node->name() : desc.name();
        item->setText(COL_NAME, profiler_gui::toUnicode(name));

        if (child.per_thread_stats != nullptr) // if there is per_thread_stats then there are other stats also
        {
            const auto per_thread_stats = child.per_thread_stats;
            const auto per_frame_stats = child.per_frame_stats;

            fillStatsColumnsThread(item, per_thread_stats, units);
            fillStatsColumnsFrame(item, per_frame_stats, units);
            
            auto percentage_per_thread = profiler_gui::percent(per_thread_stats->total_duration, threadRoot.profiled_time);
            item->setData(COL_PERCENT_SUM_PER_THREAD, Qt::UserRole, percentage_per_thread);
            item->setText(COL_PERCENT_SUM_PER_THREAD, QString::number(percentage_per_thread));

            const auto frame_duration = frame->data(COL_TIME, Qt::UserRole).toULongLong();
            const auto percentage_sum = profiler_gui::percent(per_frame_stats->total_duration, frame_duration);
            item->setData(COL_PERCENT_PER_FRAME, Qt::UserRole, percentage_sum);
            item->setText(COL_PERCENT_PER_FRAME, QString::number(percentage_sum));
            item->setTimeSmart(COL_TIME, units, per_frame_stats->total_duration);
        }
        else
        {
            item->setData(COL_PERCENT_SUM_PER_THREAD, Qt::UserRole, 0);
            item->setData(COL_PERCENT_PER_FRAME, Qt::UserRole, 0);
        }

        item->setBackgroundColor(desc.color());

        iditems[child.node->id()] = std::make_pair(nullptr, depth);
        item->setPartial(partial);

        size_t children_items_number = 0;
        profiler::timestamp_t children_duration = 0;
        if (!child.children.empty())
        {
            children_items_number = setTreeInternalPlain(threadRoot, iditems, statsMap, firstCswitch, beginTime, child.children, root,
                                                         frame, left, right, strict, partial, children_duration,
                                                         addZeroBlocks, units, depth + 1);

            if (interrupted())
                break;

            if (partial && children_items_number == 0)
            {
                delete item;
                iditems.erase(child.node->id());
                continue;
            }
        }

        iditems[child.node->id()] = std::make_pair(item, depth);
        total_duration += duration;

        if (child.per_frame_stats != nullptr)
        {
            int percentage = 100;
            auto self_duration = child.per_frame_stats->total_duration - children_duration;
            if (child.per_frame_stats->total_duration > 0)
                percentage = profiler_gui::percent(self_duration, child.per_frame_stats->total_duration);

            item->setTimeSmart(COL_SELF_TIME, units, self_duration);
            item->setData(COL_SELF_TIME_PERCENT, Qt::UserRole, percentage);
            item->setText(COL_SELF_TIME_PERCENT, QString::number(percentage));

            auto active_time = child.per_frame_stats->total_duration - idleTime;
            auto active_percent = child.per_frame_stats->total_duration == 0 ? 100. : profiler_gui::percentReal(active_time, child.per_frame_stats->total_duration);
            item->setTimeSmart(COL_ACTIVE_TIME, units, active_time);
            item->setText(COL_ACTIVE_PERCENT, QString::number(active_percent, 'g', 3));
            item->setData(COL_ACTIVE_PERCENT, Qt::UserRole, active_percent);
        }

        total_items += children_items_number + 1;

        if (gui_block.expanded)
            item->setExpanded(true);

        m_items.insert(std::make_pair(child_index, item));

        updateStats(statsMap, child.node->id(), child_index, duration, children_duration);
    }

    return total_items;
}

//////////////////////////////////////////////////////////////////////////

size_t TreeWidgetLoader::setTreeInternalAggregate(
    const profiler::BlocksTreeRoot& threadRoot,
    IdItems& iditems,
    StatsMap& statsMap,
    profiler::block_index_t firstCswitch,
    profiler::timestamp_t beginTime,
    const profiler::BlocksTree::children_t& children,
    TreeWidgetItem* root,
    profiler::timestamp_t left,
    profiler::timestamp_t right,
    bool strict,
    bool partial_parent,
    profiler::timestamp_t& total_duration,
    bool addZeroBlocks,
    profiler_gui::TimeUnits units,
    int depth
) {
    size_t total_items = 0;

    for (auto child_index : children)
    {
        if (interrupted())
            break;

        const auto& gui_block = easyBlock(child_index);
        const auto& child = gui_block.tree;
        const auto startTime = child.node->begin();
        const auto endTime = child.node->end();
        const auto duration = endTime - startTime;

        if (startTime > right || endTime < left)
            continue;

        const bool partial = strict && (startTime < left || endTime > right);
        if (partial && partial_parent && duration != 0 && (startTime == right || endTime == left))
            continue;

        const auto& desc = easyDescriptor(child.node->id());

        auto it = iditems.find(child.node->id());
        if (it != iditems.end())
        {
            size_t children_items_number = 0;
            profiler::timestamp_t children_duration = 0;
            if (!child.children.empty())
            {
                children_items_number = setTreeInternalAggregate(threadRoot, iditems, statsMap, firstCswitch, beginTime,
                    child.children, root, left, right, strict, partial, children_duration, addZeroBlocks, units, depth + 1);

                if (interrupted())
                    break;

                if (partial && children_items_number == 0)
                    continue;
            }

            auto item = it->second.first;

            total_duration += duration;
            ++total_items;

            updateStats(statsMap, child.node->id(), child_index, duration, children_duration);

            const auto total_block_duration = duration + item->data(COL_TIME, Qt::UserRole).toULongLong();

            int percentage = 100;
            auto self_duration = duration - children_duration + item->data(COL_SELF_TIME, Qt::UserRole).toULongLong();
            if (total_block_duration > 0)
                percentage = profiler_gui::percent(self_duration, total_block_duration);

            item->setTimeSmart(COL_TIME, units, total_block_duration);
            item->setTimeSmart(COL_SELF_TIME, units, self_duration);
            item->setData(COL_SELF_TIME_PERCENT, Qt::UserRole, percentage);
            item->setText(COL_SELF_TIME_PERCENT, QString::number(percentage));

            const auto idleTime = calculateIdleTime(threadRoot, firstCswitch, startTime, endTime);
            const auto active_time = duration + item->data(COL_ACTIVE_TIME, Qt::UserRole).toULongLong() - idleTime;
            const auto active_percent = total_block_duration == 0 ? 100. : profiler_gui::percentReal(active_time, total_block_duration);
            item->setTimeSmart(COL_ACTIVE_TIME, units, active_time);
            item->setText(COL_ACTIVE_PERCENT, QString::number(active_percent, 'g', 3));
            item->setData(COL_ACTIVE_PERCENT, Qt::UserRole, active_percent);

            continue;
        }

        const auto idleTime = calculateIdleTime(threadRoot, firstCswitch, startTime, endTime);

        auto item = new TreeWidgetItem(child_index, root);

        auto name = *child.node->name() != 0 ? child.node->name() : desc.name();
        item->setText(COL_NAME, profiler_gui::toUnicode(name));
        item->setTimeSmart(COL_TIME, units, duration);

        if (child.per_thread_stats != nullptr) // if there is per_thread_stats then there are other stats also
        {
            const auto per_thread_stats = child.per_thread_stats;

            fillStatsColumnsThread(item, per_thread_stats, units);

            auto percent_per_thread = profiler_gui::percent(per_thread_stats->total_duration, threadRoot.profiled_time);
            item->setData(COL_PERCENT_SUM_PER_THREAD, Qt::UserRole, percent_per_thread);
            item->setText(COL_PERCENT_SUM_PER_THREAD, QString::number(percent_per_thread));
        }
        else
        {
            item->setData(COL_PERCENT_SUM_PER_THREAD, Qt::UserRole, 0);
        }

        item->setBackgroundColor(desc.color());

        iditems[child.node->id()] = std::make_pair(item, depth);

        item->setPartial(partial);

        size_t children_items_number = 0;
        profiler::timestamp_t children_duration = 0;
        if (!child.children.empty())
        {
            children_items_number = setTreeInternalAggregate(threadRoot, iditems, statsMap, firstCswitch, beginTime,
                child.children, root, left, right, strict, partial, children_duration, addZeroBlocks, units, depth + 1);

            if (interrupted())
                break;

            if (partial && children_items_number == 0)
            {
                delete item;
                iditems.erase(child.node->id());
                continue;
            }
        }

        total_duration += duration;

        int percentage = 100;
        auto self_duration = duration - children_duration;
        if (duration > 0)
            percentage = profiler_gui::percent(self_duration, duration);

        item->setTimeSmart(COL_SELF_TIME, units, self_duration);
        item->setData(COL_SELF_TIME_PERCENT, Qt::UserRole, percentage);
        item->setText(COL_SELF_TIME_PERCENT, QString::number(percentage));

        auto active_time = duration - idleTime;
        auto active_percent = duration == 0 ? 100. : profiler_gui::percentReal(active_time, duration);
        item->setTimeSmart(COL_ACTIVE_TIME, units, active_time);
        item->setText(COL_ACTIVE_PERCENT, QString::number(active_percent, 'g', 3));
        item->setData(COL_ACTIVE_PERCENT, Qt::UserRole, active_percent);

        total_items += children_items_number + 1;

        if (gui_block.expanded)
            item->setExpanded(true);

        m_items.insert(std::make_pair(child_index, item));

        updateStats(statsMap, child.node->id(), child_index, duration, children_duration);
    }

    return total_items;
}

//////////////////////////////////////////////////////////////////////////

profiler::timestamp_t TreeWidgetLoader::calculateIdleTime(
    const profiler::BlocksTreeRoot& _threadRoot,
    profiler::block_index_t& _firstCSwitch,
    profiler::timestamp_t _begin,
    profiler::timestamp_t _end
) const {
    bool hasContextSwitch = false;
    profiler::timestamp_t idleTime = 0;
    for (profiler::block_index_t ind = _firstCSwitch, ncs = static_cast<profiler::block_index_t>(_threadRoot.sync.size()); ind < ncs; ++ind)
    {
        auto cs_index = _threadRoot.sync[ind];
        const auto cs = EASY_GLOBALS.gui_blocks[cs_index].tree.node;

        if (cs->begin() > _end)
        {
            if (!hasContextSwitch)
                _firstCSwitch = ind;
            break;
        }

        if (_begin <= cs->begin() && cs->end() <= _end)
        {
            if (!hasContextSwitch)
            {
                _firstCSwitch = ind;
                hasContextSwitch = true;
            }

            idleTime += cs->duration();
        }
    }
    return idleTime;
}

//////////////////////////////////////////////////////////////////////////

void TreeWidgetLoader::updateStats(
    StatsMap& statsMap,
    profiler::block_id_t id,
    profiler::block_index_t index,
    profiler::timestamp_t duration,
    profiler::timestamp_t children_duration
) const {
    auto stats_it = statsMap.find(id);
    if (stats_it == statsMap.end())
    {
        stats_it = statsMap.emplace(id, loader::Stats(duration, index, 0)).first;
        auto& stat = stats_it->second.stats;
        stat.total_children_duration = children_duration;
    }
    else
    {
        auto& stat = stats_it->second.stats;
        auto& durations = stats_it->second.durations;

        ++stat.calls_number;
        stat.total_duration += duration;
        stat.total_children_duration += children_duration;

        if (duration > easyBlock(stat.max_duration_block).tree.node->duration())
        {
            stat.max_duration_block = index;
        }

        if (duration < easyBlock(stat.min_duration_block).tree.node->duration())
        {
            stat.min_duration_block = index;
        }

        ++durations[duration].count;
    }
}

//////////////////////////////////////////////////////////////////////////

void TreeWidgetLoader::fillStatsForTree(TreeWidgetItem* root, StatsMap& stats, profiler_gui::TimeUnits _units, profiler::timestamp_t selectionDuration) const
{
    if (stats.empty())
    {
        return;
    }

    calculateMedians(stats.begin(), stats.end());

    std::deque<TreeWidgetItem*> queue;

    if (root->parent() != nullptr)
    {
        queue.push_back(root);
    }
    else
    {
        for (int i = 0, count = root->childCount(); i < count; ++i)
        {
            queue.push_back(static_cast<TreeWidgetItem*>(root->child(i)));
        }
    }

    while (!queue.empty() && !interrupted())
    {
        auto item = queue.front();

        for (int i = 0, count = item->childCount(); i < count; ++i)
        {
            queue.push_back(static_cast<TreeWidgetItem*>(item->child(i)));
        }

        auto stat_it = stats.find(item->block().node->id());
        if (stat_it != stats.end())
        {
            auto& stat = stat_it->second;

            fillStatsColumnsSelection(item, &stat.stats, _units);

            auto percent_per_selection = std::min(100, profiler_gui::percent(stat.stats.total_duration, selectionDuration));
            item->setData(COL_PERCENT_SUM_PER_AREA, Qt::UserRole, percent_per_selection);
            item->setText(COL_PERCENT_SUM_PER_AREA, QString::number(percent_per_selection));

            percent_per_selection = std::min(100, profiler_gui::percent(item->block().node->duration(), selectionDuration));
            item->setData(COL_PERCENT_PER_AREA, Qt::UserRole, percent_per_selection);
            item->setText(COL_PERCENT_PER_AREA, QString::number(percent_per_selection));
        }

        queue.pop_front();
    }
}

//////////////////////////////////////////////////////////////////////////
