/************************************************************************
* file name         : tree_widget_loader.h
* ----------------- : 
* creation time     : 2016/08/18
* author            : Victor Zarubkin
* email             : v.s.zarubkin@gmail.com
* ----------------- : 
* description       : The file contains declaration of TreeWidgetLoader which aim is
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

#ifndef EASY_TREE_WIDGET_LOADER_H
#define EASY_TREE_WIDGET_LOADER_H

#include <stdlib.h>
#include <vector>
#include <atomic>
#include <easy/reader.h>
#include "common_types.h"
#include "thread_pool_task.h"

//////////////////////////////////////////////////////////////////////////

class TreeWidgetItem;

namespace loader {

struct Stats
{
    profiler::BlockStatistics stats;
    profiler_gui::DurationsCountMap durations;

    Stats(profiler::timestamp_t duration, profiler::block_index_t block_index, profiler::block_index_t parent_index)
        : stats(duration, block_index, parent_index)
    {
        durations[duration].count = 1;
    }
};

} // end of namespace loader.

using Items = ::std::unordered_map<profiler::block_index_t, TreeWidgetItem*, ::estd::hash<profiler::block_index_t> >;
using ThreadedItems = std::vector<std::pair<profiler::thread_id_t, TreeWidgetItem*> >;
using RootsMap = std::unordered_map<profiler::thread_id_t, TreeWidgetItem*, estd::hash<profiler::thread_id_t> >;
using IdItems = std::unordered_map<profiler::block_id_t, std::pair<TreeWidgetItem*, int>, estd::hash<profiler::block_index_t> >;
using StatsMap = std::unordered_map<profiler::block_id_t, loader::Stats, estd::hash<profiler::block_id_t> >;

//////////////////////////////////////////////////////////////////////////

enum class TreeMode : uint8_t
{
    Full,
    Plain,
    SelectedArea
};

//////////////////////////////////////////////////////////////////////////

class TreeWidgetLoader Q_DECL_FINAL
{
    ThreadedItems   m_topLevelItems; ///< 
    Items                   m_items; ///< 
    ThreadPoolTask         m_worker; ///<
    QString                 m_error; ///<
    std::atomic_bool        m_bDone; ///<
    std::atomic_bool   m_bInterrupt; ///<
    std::atomic<int>     m_progress; ///<
    TreeMode                 m_mode; ///<

public:

    TreeWidgetLoader();
    ~TreeWidgetLoader();

    int progress() const;
    bool done() const;

    void takeTopLevelItems(ThreadedItems& _output);
    void takeItems(Items& _output);

    QString error() const;

    void interrupt(bool _wait = false);
    void fillTreeBlocks(
        const::profiler_gui::TreeBlocks& _blocks,
        profiler::timestamp_t _beginTime,
        profiler::timestamp_t _left,
        profiler::timestamp_t _right,
        bool _strict, TreeMode _mode
    );

private:

    bool interrupted() const volatile;
    void setDone();
    void setProgress(int _progress);

    void setTreeInternalTop(
        const profiler::timestamp_t& _beginTime,
        const ::profiler_gui::TreeBlocks& _blocks,
        profiler::timestamp_t _left,
        profiler::timestamp_t _right,
        bool _strict,
        bool _addZeroBlocks,
        bool _decoratedThreadNames,
        bool _hexThreadId,
        ::profiler_gui::TimeUnits _units,
        size_t _maxCount
    );

    size_t setTreeInternal(
        const profiler::BlocksTreeRoot& _threadRoot,
        IdItems& iditems,
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
        ::profiler_gui::TimeUnits _units,
        int _depth
    );

    void setTreeInternalPlainTop(
        const profiler::timestamp_t& _beginTime,
        const ::profiler_gui::TreeBlocks& _blocks,
        profiler::timestamp_t _left,
        profiler::timestamp_t _right,
        bool _strict,
        bool _addZeroBlocks,
        bool _decoratedThreadNames,
        bool _hexThreadId,
        ::profiler_gui::TimeUnits _units
    );

    size_t setTreeInternalPlain(
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
        bool _addZeroBlocks,
        ::profiler_gui::TimeUnits units,
        int depth
    );

    void setTreeInternalAggregateTop(
        const profiler::timestamp_t& _beginTime,
        const ::profiler_gui::TreeBlocks& _blocks,
        profiler::timestamp_t _left,
        profiler::timestamp_t _right,
        bool _strict,
        bool _addZeroBlocks,
        bool _decoratedThreadNames,
        bool _hexThreadId,
        ::profiler_gui::TimeUnits _units
    );

    size_t setTreeInternalAggregate(
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
        ::profiler_gui::TimeUnits units,
        int depth
    );

    profiler::timestamp_t calculateChildrenDurationRecursive(
        const profiler::BlocksTree::children_t& _children,
        profiler::block_id_t _id
    ) const;

    uint32_t calculateChildrenCountRecursive(
        const profiler::BlocksTree::children_t& children,
        profiler::timestamp_t left,
        profiler::timestamp_t right,
        bool strict,
        bool partial_parent,
        bool addZeroBlocks
    ) const;

    profiler::timestamp_t calculateIdleTime(const profiler::BlocksTreeRoot& _threadRoot, profiler::block_index_t& _firstCSwitch, profiler::timestamp_t _begin, profiler::timestamp_t _end) const;
    void updateStats(StatsMap& stats, profiler::block_id_t id, profiler::block_index_t index, profiler::timestamp_t duration, profiler::timestamp_t children_duration) const;
    void fillStatsForTree(TreeWidgetItem* root, StatsMap& stats, profiler_gui::TimeUnits _units, profiler::timestamp_t selectionDuration) const;

}; // END of class TreeWidgetLoader.

//////////////////////////////////////////////////////////////////////////

#endif // EASY_TREE_WIDGET_LOADER_H
