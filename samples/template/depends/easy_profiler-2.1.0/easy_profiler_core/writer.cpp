/************************************************************************
* file name         : writer.cpp
* ----------------- :
* creation time     : 2018/05/08
* authors           : Sergey Yagovtsev, Victor Zarubkin
* emails            : yse.sey@gmail.com, v.s.zarubkin@gmail.com
* ----------------- :
* description       : The file contains implementation of fillTreesFromFile function
*                   : which reads profiler file and fill profiler blocks tree.
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

#include <fstream>
#include <iterator>
#include <algorithm>

#include <easy/writer.h>
#include <easy/profiler.h>

#include "alignment_helpers.h"

//////////////////////////////////////////////////////////////////////////

extern const uint32_t EASY_PROFILER_SIGNATURE;
extern const uint32_t EASY_PROFILER_VERSION;

EASY_CONSTEXPR auto BaseCSwitchSize = sizeof(profiler::SerializedCSwitch) + 1;

//////////////////////////////////////////////////////////////////////////

struct BlocksRange
{
    profiler::block_index_t begin;
    profiler::block_index_t   end;

    BlocksRange(profiler::block_index_t size = 0)
        : begin(0), end(size)
    {
    }

    BlocksRange(profiler::block_index_t beginIndex, profiler::block_index_t endIndex)
        : begin(beginIndex), end(endIndex)
    {
    }
};

struct BlocksMemoryAndCount
{
    uint64_t usedMemorySize = 0; // memory size used by profiler blocks
    profiler::block_index_t blocksCount = 0;

    BlocksMemoryAndCount() = default;

    BlocksMemoryAndCount& operator += (const BlocksMemoryAndCount& another)
    {
        usedMemorySize += another.usedMemorySize;
        blocksCount += another.blocksCount;
        return *this;
    }
};

struct BlocksAndCSwitchesRange
{
    BlocksMemoryAndCount    blocksMemoryAndCount;
    BlocksMemoryAndCount cswitchesMemoryAndCount;
    BlocksRange                           blocks;
    BlocksRange                        cswitches;
};

//////////////////////////////////////////////////////////////////////////

template <typename T>
static void write(std::ostream& _stream, const char* _data, T _size)
{
    _stream.write(_data, _size);
}

template <class T>
static void write(std::ostream& _stream, const T& _data)
{
    _stream.write((const char*)&_data, sizeof(T));
}

static bool update_progress_write(std::atomic<int>& progress, int new_value, std::ostream& _log)
{
    auto oldprogress = progress.exchange(new_value, std::memory_order_release);
    if (oldprogress < 0)
    {
        _log << "Writing was interrupted";
        return false;
    }

    return true;
}

//////////////////////////////////////////////////////////////////////////

static BlocksRange findRange(const profiler::BlocksTree::children_t& children, profiler::timestamp_t beginTime,
                             profiler::timestamp_t endTime, const profiler::block_getter_fn& getter)
{
    const auto size = static_cast<profiler::block_index_t>(children.size());
    BlocksRange range(size);

    if (size == 0)
        return range;

    if (beginTime <= getter(children.front()).node->begin() && getter(children.back()).node->end() <= endTime)
        return range; // All blocks inside range

    auto first_it = std::lower_bound(children.begin(), children.end(), beginTime,
                                     [&](profiler::block_index_t element, profiler::timestamp_t value)
    {
        return getter(element).node->end() < value;
    });

    for (; first_it != children.end(); ++first_it)
    {
        const auto& child = getter(*first_it);
        if (child.node->begin() >= beginTime || child.node->end() > beginTime)
            break;
    }

    if (first_it != children.end() && getter(*first_it).node->begin() <= endTime)
    {
        auto last_it = std::lower_bound(children.begin(), children.end(), endTime,
                                        [&](profiler::block_index_t element, profiler::timestamp_t value)
        {
            return getter(element).node->begin() <= value;
        });

        if (last_it != children.end() && getter(*last_it).node->end() >= beginTime)
        {
            const auto begin = static_cast<profiler::block_index_t>(std::distance(children.begin(), first_it));
            const auto end = static_cast<profiler::block_index_t>(std::distance(children.begin(), last_it));

            if (begin <= end)
            {
                range.begin = begin;
                range.end = end;
            }
        }
    }

    return range;
}

static BlocksRange findRange(const profiler::bookmarks_t& bookmarks, profiler::timestamp_t beginTime, profiler::timestamp_t endTime)
{
    const auto size = static_cast<profiler::block_index_t>(bookmarks.size());
    BlocksRange range(size);

    if (size == 0)
        return range;

    if (beginTime <= bookmarks.front().pos && bookmarks.back().pos <= endTime)
        return range; // All blocks inside range

    auto first_it = std::lower_bound(bookmarks.begin(), bookmarks.end(), beginTime,
                                     [](const profiler::Bookmark& element, profiler::timestamp_t value)
    {
        return element.pos < value;
    });

    for (; first_it != bookmarks.end(); ++first_it)
    {
        const auto& bookmark = *first_it;
        if (bookmark.pos >= beginTime)
            break;
    }

    if (first_it != bookmarks.end() && first_it->pos <= endTime)
    {
        auto last_it = std::lower_bound(bookmarks.begin(), bookmarks.end(), endTime,
                                        [](const profiler::Bookmark& element, profiler::timestamp_t value)
        {
            return element.pos <= value;
        });

        if (last_it != bookmarks.end() && last_it->pos >= beginTime)
        {
            const auto begin = static_cast<profiler::block_index_t>(std::distance(bookmarks.begin(), first_it));
            const auto end = static_cast<profiler::block_index_t>(std::distance(bookmarks.begin(), last_it));

            if (begin <= end)
            {
                range.begin = begin;
                range.end = end;
            }
        }
    }

    return range;
}

static BlocksMemoryAndCount calculateUsedMemoryAndBlocksCount(const profiler::BlocksTree::children_t& children,
                                                              const BlocksRange& range,
                                                              const profiler::block_getter_fn& getter,
                                                              const profiler::descriptors_list_t& descriptors,
                                                              bool contextSwitches)
{
    BlocksMemoryAndCount memoryAndCount;

    if (!contextSwitches)
    {
        for (auto i = range.begin; i < range.end; ++i)
        {
            const auto& child = getter(children[i]);

            // Calculate self memory consumption
            const auto& desc = *descriptors[child.node->id()];
            uint64_t usedMemorySize = 0;

            if (desc.type() == profiler::BlockType::Value)
                usedMemorySize = sizeof(profiler::ArbitraryValue) + child.value->data_size();
            else
                usedMemorySize = sizeof(profiler::SerializedBlock) + strlen(child.node->name()) + 1;

            // Calculate children memory consumption
            const BlocksRange childRange(0, static_cast<profiler::block_index_t>(child.children.size()));
            const auto childrenMemoryAndCount = calculateUsedMemoryAndBlocksCount(child.children, childRange,
                                                                                  getter, descriptors,
                                                                                  false);

            // Accumulate memory and count
            memoryAndCount += childrenMemoryAndCount;
            memoryAndCount.usedMemorySize += usedMemorySize;
            ++memoryAndCount.blocksCount;
        }
    }
    else
    {
        for (auto i = range.begin; i < range.end; ++i)
        {
            const auto& child = getter(children[i]);
            const uint64_t usedMemorySize = BaseCSwitchSize + strlen(child.cs->name());
            memoryAndCount.usedMemorySize += usedMemorySize;
            ++memoryAndCount.blocksCount;
        }
    }

    return memoryAndCount;
}

//////////////////////////////////////////////////////////////////////////

static void serializeBlocks(std::ostream& output, std::vector<char>& buffer,
                            const profiler::BlocksTree::children_t& children, const BlocksRange& range,
                            const profiler::block_getter_fn& getter, const profiler::descriptors_list_t& descriptors)
{
    for (auto i = range.begin; i < range.end; ++i)
    {
        const auto& child = getter(children[i]);

        // Serialize children
        const BlocksRange childRange(0, static_cast<profiler::block_index_t>(child.children.size()));
        serializeBlocks(output, buffer, child.children, childRange, getter, descriptors);

        // Serialize self
        const auto& desc = *descriptors[child.node->id()];
        uint16_t usedMemorySize = 0;

        if (desc.type() == profiler::BlockType::Value)
        {
            usedMemorySize = static_cast<uint16_t>(sizeof(profiler::ArbitraryValue)) + child.value->data_size();
            buffer.resize(usedMemorySize + sizeof(uint16_t));
            unaligned_store16(buffer.data(), usedMemorySize);
            memcpy(buffer.data() + sizeof(uint16_t), child.value, static_cast<size_t>(usedMemorySize));
        }
        else
        {
            usedMemorySize = static_cast<uint16_t>(sizeof(profiler::SerializedBlock)
                                                   + strlen(child.node->name()) + 1);

            buffer.resize(usedMemorySize + sizeof(uint16_t));
            unaligned_store16(buffer.data(), usedMemorySize);
            memcpy(buffer.data() + sizeof(uint16_t), child.node, static_cast<size_t>(usedMemorySize));

            if (child.node->id() != desc.id())
            {
                // This block id is dynamic. Restore it's value like it was before in the input .prof file
                auto block = reinterpret_cast<profiler::SerializedBlock*>(buffer.data() + sizeof(uint16_t));
                block->setId(desc.id());
            }
        }

        write(output, buffer.data(), buffer.size());
    }
}

static void serializeContextSwitches(std::ostream& output, std::vector<char>& buffer,
                                     const profiler::BlocksTree::children_t& children, const BlocksRange& range,
                                     const profiler::block_getter_fn& getter)
{
    for (auto i = range.begin; i < range.end; ++i)
    {
        const auto& child = getter(children[i]);

        const auto usedMemorySize = static_cast<uint16_t>(BaseCSwitchSize + strlen(child.cs->name()));

        buffer.resize(usedMemorySize + sizeof(uint16_t));
        unaligned_store16(buffer.data(), usedMemorySize);
        memcpy(buffer.data() + sizeof(uint16_t), child.cs, static_cast<size_t>(usedMemorySize));

        write(output, buffer.data(), buffer.size());
    }
}

static void serializeDescriptors(std::ostream& output, std::vector<char>& buffer,
                                 const profiler::descriptors_list_t& descriptors,
                                 profiler::block_id_t descriptors_count)
{
    const size_t size = std::min(descriptors.size(), static_cast<size_t>(descriptors_count));
    for (size_t i = 0; i < size; ++i)
    {
        const auto& desc = *descriptors[i];
        if (desc.id() != i)
            break;

        const auto usedMemorySize = static_cast<uint16_t>(sizeof(profiler::SerializedBlockDescriptor)
                                                          + strlen(desc.name()) + strlen(desc.file()) + 2);

        buffer.resize(usedMemorySize + sizeof(uint16_t));
        unaligned_store16(buffer.data(), usedMemorySize);
        memcpy(buffer.data() + sizeof(uint16_t), &desc, static_cast<size_t>(usedMemorySize));

        write(output, buffer.data(), buffer.size());
    }
}

static void serializeBookmarks(std::ostream& output, const profiler::bookmarks_t& bookmarks, const BlocksRange& range)
{
    for (auto i = range.begin; i < range.end; ++i)
    {
        const auto& bookmark = bookmarks[i];

        const auto usedMemorySize = static_cast<uint16_t>(profiler::Bookmark::BaseSize + bookmark.text.size());
        write(output, usedMemorySize);
        write(output, bookmark.pos);
        write(output, bookmark.color);
        write(output, bookmark.text.c_str(), bookmark.text.size() + 1);
    }
}

//////////////////////////////////////////////////////////////////////////

extern "C" PROFILER_API profiler::block_index_t writeTreesToFile(std::atomic<int>& progress, const char* filename,
                                                                 const profiler::SerializedData& serialized_descriptors,
                                                                 const profiler::descriptors_list_t& descriptors,
                                                                 profiler::block_id_t descriptors_count,
                                                                 const profiler::thread_blocks_tree_t& trees,
                                                                 const profiler::bookmarks_t& bookmarks,
                                                                 profiler::block_getter_fn block_getter,
                                                                 profiler::timestamp_t begin_time,
                                                                 profiler::timestamp_t end_time,
                                                                 profiler::processid_t pid,
                                                                 std::ostream& log)
{
    if (!update_progress_write(progress, 0, log))
        return 0;

    std::ofstream outFile(filename, std::fstream::binary);
    if (!outFile.is_open())
    {
        log << "Can not open file " << filename;
        return 0;
    }

    // Write data to file
    auto result = writeTreesToStream(progress, outFile, serialized_descriptors, descriptors, descriptors_count, trees,
                                     bookmarks, std::move(block_getter), begin_time, end_time, pid, log);

    return result;
}

//////////////////////////////////////////////////////////////////////////

extern "C" PROFILER_API profiler::block_index_t writeTreesToStream(std::atomic<int>& progress, std::ostream& str,
                                                                   const profiler::SerializedData& serialized_descriptors,
                                                                   const profiler::descriptors_list_t& descriptors,
                                                                   profiler::block_id_t descriptors_count,
                                                                   const profiler::thread_blocks_tree_t& trees,
                                                                   const profiler::bookmarks_t& bookmarks,
                                                                   profiler::block_getter_fn block_getter,
                                                                   profiler::timestamp_t begin_time,
                                                                   profiler::timestamp_t end_time,
                                                                   profiler::processid_t pid,
                                                                   std::ostream& log)
{
    if (trees.empty() || serialized_descriptors.empty() || descriptors_count == 0)
    {
        log << "Nothing to save";
        return 0;
    }

    BlocksMemoryAndCount total;

    using ranges_t = std::unordered_map<profiler::thread_id_t, BlocksAndCSwitchesRange, estd::hash<profiler::thread_id_t> >;
    ranges_t block_ranges;

    // Calculate block ranges and used memory (for serialization)
    profiler::timestamp_t beginTime = begin_time, endTime = end_time;
    size_t i = 0;
    for (const auto& kv : trees)
    {
        const auto id = kv.first;
        const auto& tree = kv.second;

        BlocksAndCSwitchesRange range;

        range.blocks = findRange(tree.children, begin_time, end_time, block_getter);
        range.cswitches = findRange(tree.sync, begin_time, end_time, block_getter);

        range.blocksMemoryAndCount = calculateUsedMemoryAndBlocksCount(tree.children, range.blocks, block_getter,
                                                                       descriptors, false);
        total += range.blocksMemoryAndCount;

        if (range.blocksMemoryAndCount.blocksCount != 0)
        {
            beginTime = std::min(beginTime, block_getter(tree.children[range.blocks.begin]).node->begin());
            endTime = std::max(endTime, block_getter(tree.children[range.blocks.end - 1]).node->end());
        }

        range.cswitchesMemoryAndCount = calculateUsedMemoryAndBlocksCount(tree.sync, range.cswitches, block_getter,
                                                                          descriptors, true);
        total += range.cswitchesMemoryAndCount;

        if (range.cswitchesMemoryAndCount.blocksCount != 0)
        {
            beginTime = std::min(beginTime, block_getter(tree.children[range.cswitches.begin]).cs->begin());
            endTime = std::max(endTime, block_getter(tree.children[range.cswitches.end - 1]).cs->end());
        }

        block_ranges[id] = range;

        if (!update_progress_write(progress, 15 / static_cast<int>(trees.size() - i), log))
            return 0;

        ++i;
    }

    BlocksRange bookmarksRange;
    uint16_t bookmarksCount = 0;
    if (!bookmarks.empty())
    {
        bookmarksRange = findRange(bookmarks, begin_time, end_time);
        bookmarksCount = static_cast<uint16_t>(bookmarksRange.end - bookmarksRange.begin);
        if (bookmarksCount != 0)
        {
            beginTime = std::min(beginTime, bookmarks[bookmarksRange.begin].pos);
            endTime = std::max(endTime, bookmarks[bookmarksRange.end - 1].pos);
        }
    }

    if (total.blocksCount == 0)
    {
        log << "Nothing to save";
        return 0;
    }

    const uint64_t usedMemorySizeDescriptors = serialized_descriptors.size() + descriptors_count * sizeof(uint16_t);

    // Write data to stream
    write(str, EASY_PROFILER_SIGNATURE);
    write(str, EASY_PROFILER_VERSION);
    write(str, pid);

    // write 0 because we do not need to convert time from ticks to nanoseconds (it's already converted)
    write<int64_t>(str, 0LL); // CPU frequency

    write(str, beginTime);
    write(str, endTime);

    write(str, total.usedMemorySize);
    write(str, usedMemorySizeDescriptors);
    write(str, total.blocksCount);
    write(str, descriptors_count);
    write(str, static_cast<uint32_t>(trees.size()));
    write(str, bookmarksCount);
    write(str, static_cast<uint16_t>(0)); // padding

    std::vector<char> buffer;

    // Serialize all descriptors
    serializeDescriptors(str, buffer, descriptors, descriptors_count);

    // Serialize all blocks
    i = 0;
    for (const auto& kv : trees)
    {
        const auto id = kv.first;
        const auto& tree = kv.second;
        const auto& range = block_ranges.at(id);

        const auto nameSize = static_cast<uint16_t>(tree.thread_name.size() + 1);
        write(str, id);
        write(str, nameSize);
        write(str, tree.name(), nameSize);

        // Serialize context switches
        write(str, range.cswitchesMemoryAndCount.blocksCount);
        if (range.cswitchesMemoryAndCount.blocksCount != 0)
            serializeContextSwitches(str, buffer, tree.sync, range.cswitches, block_getter);

        // Serialize blocks
        write(str, range.blocksMemoryAndCount.blocksCount);
        if (range.blocksMemoryAndCount.blocksCount != 0)
            serializeBlocks(str, buffer, tree.children, range.blocks, block_getter, descriptors);

        if (!update_progress_write(progress, 40 + 57 / static_cast<int>(trees.size() - i), log))
            return 0;
    }

    write(str, EASY_PROFILER_SIGNATURE);

    // Serialize bookmarks
    if (bookmarksCount != 0)
    {
        serializeBookmarks(str, bookmarks, bookmarksRange);
        write(str, EASY_PROFILER_SIGNATURE);
    }

    return total.blocksCount;
}

//////////////////////////////////////////////////////////////////////////
