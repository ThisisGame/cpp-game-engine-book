/**
Lightweight profiler library for c++
Copyright(C) 2016-2019  Sergey Yagovtsev, Victor Zarubkin

Licensed under either of
    * MIT license (LICENSE.MIT or http://opensource.org/licenses/MIT)
    * Apache License, Version 2.0, (LICENSE.APACHE or http://www.apache.org/licenses/LICENSE-2.0)
at your option.

The MIT License
    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights 
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies 
    of the Software, and to permit persons to whom the Software is furnished 
    to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all 
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
    INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR 
    PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE 
    LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, 
    TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE 
    USE OR OTHER DEALINGS IN THE SOFTWARE.


The Apache License, Version 2.0 (the "License");
    You may not use this file except in compliance with the License.
    You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.

**/

#ifndef EASY_PROFILER_READER_H
#define EASY_PROFILER_READER_H

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <atomic>
#include <functional>
#include <iostream>
#include <unordered_map>
#include <string>
#include <vector>

#include <easy/serialized_block.h>
#include <easy/details/arbitrary_value_public_types.h>
#include <easy/utility.h>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace profiler {

    using processid_t    = uint64_t;
    using calls_number_t = uint32_t;
    using block_index_t  = uint32_t;

#pragma pack(push, 1)
    struct BlockStatistics EASY_FINAL
    {
        profiler::timestamp_t          total_duration; ///< Total duration of all block calls
        profiler::timestamp_t         median_duration; ///< Median duration of all block calls
        profiler::timestamp_t total_children_duration; ///< Total duration of all children of all block calls
        profiler::block_index_t    min_duration_block; ///< Will be used in GUI to jump to the block with min duration
        profiler::block_index_t    max_duration_block; ///< Will be used in GUI to jump to the block with max duration
        profiler::block_index_t          parent_block; ///< Index of block which is "parent" for "per_parent_stats" or "frame" for "per_frame_stats" or thread-id for "per_thread_stats"
        profiler::calls_number_t         calls_number; ///< Block calls number

        explicit BlockStatistics(profiler::timestamp_t _duration, profiler::block_index_t _block_index, profiler::block_index_t _parent_index)
            : total_duration(_duration)
            , median_duration(0)
            , total_children_duration(0)
            , min_duration_block(_block_index)
            , max_duration_block(_block_index)
            , parent_block(_parent_index)
            , calls_number(1)
        {
        }

        //BlockStatistics() = default;

        inline profiler::timestamp_t average_duration() const
        {
            return total_duration / calls_number;
        }

    }; // END of struct BlockStatistics.
#pragma pack(pop)

    extern "C" PROFILER_API void release_stats(BlockStatistics*& _stats);

    //////////////////////////////////////////////////////////////////////////

    class BlocksTree EASY_FINAL
    {
        using This = BlocksTree;

    public:

        using blocks_t = std::vector<This>;
        using children_t = std::vector<profiler::block_index_t>;

        children_t children; ///< List of children blocks. May be empty.

        union {
            profiler::SerializedBlock*    node; ///< Pointer to serialized data for regular block (id, name, begin, end etc.)
            profiler::SerializedCSwitch*    cs; ///< Pointer to serialized data for context switch (thread_id, name, begin, end etc.)
            profiler::ArbitraryValue*    value; ///< Pointer to serialized data for arbitrary value
        };

        profiler::BlockStatistics* per_parent_stats; ///< Pointer to statistics for this block within the parent (may be nullptr for top-level blocks)
        profiler::BlockStatistics*  per_frame_stats; ///< Pointer to statistics for this block within the frame (may be nullptr for top-level blocks)
        profiler::BlockStatistics* per_thread_stats; ///< Pointer to statistics for this block within the bounds of all frames per current thread
        uint8_t                               depth; ///< Maximum number of sublevels (maximum children depth)

        BlocksTree(const This&) = delete;
        This& operator = (const This&) = delete;

        BlocksTree() EASY_NOEXCEPT
            : node(nullptr)
            , per_parent_stats(nullptr)
            , per_frame_stats(nullptr)
            , per_thread_stats(nullptr)
            , depth(0)
        {

        }

        BlocksTree(This&& that) EASY_NOEXCEPT
            : BlocksTree()
        {
            make_move(std::forward<This&&>(that));
        }

        This& operator = (This&& that) EASY_NOEXCEPT
        {
            make_move(std::forward<This&&>(that));
            return *this;
        }

        ~BlocksTree() EASY_NOEXCEPT
        {
            release_stats(per_thread_stats);
            release_stats(per_parent_stats);
            release_stats(per_frame_stats);
        }

        bool operator < (const This& other) const EASY_NOEXCEPT
        {
            if (node == nullptr || other.node == nullptr)
                return false;
            return node->begin() < other.node->begin();
        }

        void shrink_to_fit() EASY_NOEXCEPT
        {
            //for (auto& child : children)
            //    child.shrink_to_fit();

            // shrink version 1:
            //children.shrink_to_fit();

            // shrink version 2:
            //children_t new_children;
            //new_children.reserve(children.size());
            //std::move(children.begin(), children.end(), std::back_inserter(new_children));
            //new_children.swap(children);
        }

    private:

        void make_move(This&& that) EASY_NOEXCEPT
        {
            if (per_thread_stats != that.per_thread_stats)
                release_stats(per_thread_stats);

            if (per_parent_stats != that.per_parent_stats)
                release_stats(per_parent_stats);

            if (per_frame_stats != that.per_frame_stats)
                release_stats(per_frame_stats);

            children = std::move(that.children);
            node = that.node;
            per_parent_stats = that.per_parent_stats;
            per_frame_stats = that.per_frame_stats;
            per_thread_stats = that.per_thread_stats;
            depth = that.depth;

            that.node = nullptr;
            that.per_parent_stats = nullptr;
            that.per_frame_stats = nullptr;
            that.per_thread_stats = nullptr;
        }

    }; // END of class BlocksTree.

    //////////////////////////////////////////////////////////////////////////

    struct Bookmark EASY_FINAL
    {
        EASY_STATIC_CONSTEXPR size_t BaseSize = sizeof(profiler::timestamp_t) +
            sizeof(profiler::color_t) + 1;

        std::string            text;
        profiler::timestamp_t   pos;
        profiler::color_t     color;
    };

    using bookmarks_t = std::vector<Bookmark>;

    //////////////////////////////////////////////////////////////////////////

    class BlocksTreeRoot EASY_FINAL
    {
        using This = BlocksTreeRoot;

    public:

        BlocksTree::children_t       children; ///< List of children indexes
        BlocksTree::children_t           sync; ///< List of context-switch events
        BlocksTree::children_t         events; ///< List of events indexes
        std::string               thread_name; ///< Name of this thread
        profiler::timestamp_t   profiled_time; ///< Profiled time of this thread (sum of all children duration)
        profiler::timestamp_t       wait_time; ///< Wait time of this thread (sum of all context switches)
        profiler::thread_id_t       thread_id; ///< System Id of this thread
        profiler::block_index_t frames_number; ///< Total frames number (top-level blocks)
        profiler::block_index_t blocks_number; ///< Total blocks number including their children
        uint8_t                         depth; ///< Maximum stack depth (number of levels)

        BlocksTreeRoot(const This&) = delete;
        This& operator = (const This&) = delete;

        BlocksTreeRoot() EASY_NOEXCEPT
            : profiled_time(0), wait_time(0), thread_id(0), frames_number(0), blocks_number(0), depth(0)
        {
        }

        BlocksTreeRoot(This&& that) EASY_NOEXCEPT
            : children(std::move(that.children))
            , sync(std::move(that.sync))
            , events(std::move(that.events))
            , thread_name(std::move(that.thread_name))
            , profiled_time(that.profiled_time)
            , wait_time(that.wait_time)
            , thread_id(that.thread_id)
            , frames_number(that.frames_number)
            , blocks_number(that.blocks_number)
            , depth(that.depth)
        {
        }

        This& operator = (This&& that) EASY_NOEXCEPT
        {
            children = std::move(that.children);
            sync = std::move(that.sync);
            events = std::move(that.events);
            thread_name = std::move(that.thread_name);
            profiled_time = that.profiled_time;
            wait_time = that.wait_time;
            thread_id = that.thread_id;
            frames_number = that.frames_number;
            blocks_number = that.blocks_number;
            depth = that.depth;
            return *this;
        }

        inline bool got_name() const EASY_NOEXCEPT
        {
            return !thread_name.empty();
        }

        inline const char* name() const EASY_NOEXCEPT
        {
            return thread_name.c_str();
        }

        bool operator < (const This& other) const EASY_NOEXCEPT
        {
            return thread_id < other.thread_id;
        }

    }; // END of class BlocksTreeRoot.

    struct BeginEndTime
    {
        profiler::timestamp_t beginTime;
        profiler::timestamp_t endTime;
    };

    using blocks_t = profiler::BlocksTree::blocks_t;
    using thread_blocks_tree_t = std::unordered_map<profiler::thread_id_t, profiler::BlocksTreeRoot, ::estd::hash<profiler::thread_id_t> >;
    using block_getter_fn = std::function<const profiler::BlocksTree&(profiler::block_index_t)>;

    //////////////////////////////////////////////////////////////////////////

    class PROFILER_API SerializedData EASY_FINAL
    {
        uint64_t m_size;
        char*    m_data;

    public:

        SerializedData(const SerializedData&) = delete;
        SerializedData& operator = (const SerializedData&) = delete;

        SerializedData();

        SerializedData(SerializedData&& that);

        ~SerializedData();

        void set(uint64_t _size);

        void extend(uint64_t _size);

        SerializedData& operator = (SerializedData&& that);

        char* operator [] (uint64_t i);

        const char* operator [] (uint64_t i) const;

        bool empty() const;

        uint64_t size() const;

        char* data();

        const char* data() const;

        void clear();

        void swap(SerializedData& other);

    private:

        void set(char* _data, uint64_t _size);

    }; // END of class SerializedData.

    //////////////////////////////////////////////////////////////////////////

    using descriptors_list_t = std::vector<SerializedBlockDescriptor*>;

} // END of namespace profiler.

extern "C" {

    PROFILER_API profiler::block_index_t fillTreesFromFile(std::atomic<int>& progress, const char* filename,
                                                           profiler::BeginEndTime& begin_end_time,
                                                           profiler::SerializedData& serialized_blocks,
                                                           profiler::SerializedData& serialized_descriptors,
                                                           profiler::descriptors_list_t& descriptors,
                                                           profiler::blocks_t& _blocks,
                                                           profiler::thread_blocks_tree_t& threaded_trees,
                                                           profiler::bookmarks_t& bookmarks,
                                                           uint32_t& descriptors_count,
                                                           uint32_t& version,
                                                           profiler::processid_t& pid,
                                                           bool gather_statistics,
                                                           std::ostream& _log);

    PROFILER_API profiler::block_index_t fillTreesFromStream(std::atomic<int>& progress, std::istream& str,
                                                             profiler::BeginEndTime& begin_end_time,
                                                             profiler::SerializedData& serialized_blocks,
                                                             profiler::SerializedData& serialized_descriptors,
                                                             profiler::descriptors_list_t& descriptors,
                                                             profiler::blocks_t& _blocks,
                                                             profiler::thread_blocks_tree_t& threaded_trees,
                                                             profiler::bookmarks_t& bookmarks,
                                                             uint32_t& descriptors_count,
                                                             uint32_t& version,
                                                             profiler::processid_t& pid,
                                                             bool gather_statistics,
                                                             std::ostream& _log);

    PROFILER_API bool readDescriptionsFromStream(std::atomic<int>& progress, std::istream& str,
                                                 profiler::SerializedData& serialized_descriptors,
                                                 profiler::descriptors_list_t& descriptors,
                                                 std::ostream& _log);

}

inline profiler::block_index_t fillTreesFromFile(const char* filename, profiler::BeginEndTime& begin_end_time,
                                                 profiler::SerializedData& serialized_blocks,
                                                 profiler::SerializedData& serialized_descriptors,
                                                 profiler::descriptors_list_t& descriptors, profiler::blocks_t& _blocks,
                                                 profiler::thread_blocks_tree_t& threaded_trees,
                                                 profiler::bookmarks_t& bookmarks,
                                                 uint32_t& descriptors_count,
                                                 uint32_t& version,
                                                 profiler::processid_t& pid,
                                                 bool gather_statistics,
                                                 std::ostream& _log)
{
    std::atomic<int> progress(0);
    return fillTreesFromFile(progress, filename, begin_end_time, serialized_blocks, serialized_descriptors,
                             descriptors, _blocks, threaded_trees, bookmarks, descriptors_count, version, pid,
                             gather_statistics, _log);
}

inline bool readDescriptionsFromStream(std::istream& str,
                                       profiler::SerializedData& serialized_descriptors,
                                       profiler::descriptors_list_t& descriptors,
                                       std::ostream& _log)
{
    std::atomic<int> progress(0);
    return readDescriptionsFromStream(progress, str, serialized_descriptors, descriptors, _log);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // EASY_PROFILER_READER_H
