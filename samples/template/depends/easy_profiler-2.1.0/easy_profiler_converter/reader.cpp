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

#include <algorithm>
#include <fstream>
#include <deque>
#include <functional>

#include "reader.h"

//////////////////////////////////////////////////////////////////////////

namespace profiler
{

namespace reader
{

profiler::block_index_t FileReader::readFile(const std::string& filename)
{
    profiler::SerializedData serialized_blocks, serialized_descriptors;
    profiler::descriptors_list_t descriptors;
    profiler::blocks_t blocks;
    profiler::thread_blocks_tree_t threaded_trees;
    profiler::bookmarks_t bookmarks;
    profiler::BeginEndTime beginEndTime;

    profiler::processid_t pid = 0;
    uint32_t total_descriptors_number = 0;

    EASY_CONSTEXPR bool DoNotGatherStats = false;
    const auto blocks_number = ::fillTreesFromFile(filename.c_str(), beginEndTime, serialized_blocks, serialized_descriptors,
        descriptors, blocks, threaded_trees, bookmarks, total_descriptors_number, m_version, pid, DoNotGatherStats,
        m_errorMessage);

    if (blocks_number == 0)
        return 0;

    m_blockDescriptors.reserve(descriptors.size());
    uint32_t descId = 0;
    for (auto descriptor : descriptors)
    {
        BlockDescriptor desc;
        desc.parentId = descriptor->id();
        desc.id = descId++;
        desc.lineNumber = descriptor->line();
        desc.argbColor = descriptor->color();
        desc.blockType = static_cast<decltype(desc.blockType)>(descriptor->type());
        desc.status = descriptor->status();

        if (desc.parentId == desc.id) // compile time descriptor and name
            desc.blockName = descriptor->name();

        desc.fileName = descriptor->file();
        m_blockDescriptors.push_back(std::move(desc));
    }

    descriptors.clear();
    serialized_descriptors.clear();

    for (auto& kv : threaded_trees)
    {
        auto& thread = kv.second;
        auto& root = m_blocksTree[kv.first];
        auto& cswitches = m_contextSwitches[kv.first];

        root.info.descriptor = nullptr;
        if (!thread.children.empty())
        {
            root.info.beginTime = blocks[thread.children.front()].node->begin();
            root.info.endTime = blocks[thread.children.back()].node->end();
        }
        else
        {
            root.info.beginTime = 0;
            root.info.endTime = 0;
        }

        m_threadNames[kv.first] = std::move(thread.thread_name);

        cswitches.reserve(thread.sync.size());
        for (auto i : thread.sync)
        {
            auto baseData = blocks[i].cs;
            cswitches.emplace_back(ContextSwitchEvent {baseData->begin(), baseData->end(), baseData->tid(), baseData->name()});
        }

        using child_t = std::pair<profiler::block_index_t, std::reference_wrapper<BlocksTreeNode> >;
        using children_queue_t = std::deque<child_t>;
        children_queue_t queue;

        root.children.reserve(thread.children.size());
        for (auto i : thread.children)
            queue.emplace_back(i, std::ref(root));

        while (!queue.empty())
        {
            auto current = queue.front();
            queue.pop_front();

            const profiler::BlocksTree& block = blocks[current.first];
            BlocksTreeNode& parent = current.second;

            parent.children.emplace_back();
            auto& child = parent.children.back();

            child.children.reserve(block.children.size());
            for (auto i : block.children)
                queue.emplace_back(i, std::ref(child));

            auto& descriptor = m_blockDescriptors[block.node->id()];
            if (descriptor.parentId != descriptor.id && descriptor.blockName.empty())
                descriptor.blockName = block.node->name(); // runtime name

            auto& info = child.info;
            info.beginTime = block.node->begin();
            info.endTime = block.node->end();
            info.descriptor = &descriptor;
            info.blockIndex = current.first;
        }
    }

    m_bookmarks.swap(bookmarks);

    return blocks_number;
}

const thread_blocks_tree_t& FileReader::getBlocksTree() const
{
    return m_blocksTree;
}

const descriptors_list_t& FileReader::getBlockDescriptors() const
{
    return m_blockDescriptors;
}

const profiler::bookmarks_t& FileReader::getBookmarks() const
{
    return m_bookmarks;
}

const std::string& FileReader::getThreadName(uint64_t threadId) const
{
    auto it = m_threadNames.find(threadId);
    if (it == m_threadNames.end())
        return m_emptyString;
    return it->second;
}

uint32_t FileReader::getVersion() const
{
    return m_version;
}

::std::string FileReader::getVersionString() const
{
    std::stringstream stream;
    stream << ((m_version & 0xff000000) >> 24) << "." << ((m_version & 0x00ff0000) >> 16) << "." << (m_version & 0x0000ffff);
    return stream.str();
}

const context_switches_t& FileReader::getContextSwitches() const
{
    return m_contextSwitches;
}

} // end of namespace reader.

} // end of namespace profiler.
