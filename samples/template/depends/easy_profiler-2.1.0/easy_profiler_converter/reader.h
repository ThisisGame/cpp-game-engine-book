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

#ifndef EASY_PROFILER_CONVERTER_READER_H
#define EASY_PROFILER_CONVERTER_READER_H

#include <vector>
#include <sstream>
#include <string>
#include <unordered_map>

#include <easy/easy_protocol.h>
#include <easy/reader.h>
#include <easy/utility.h>

namespace profiler {

namespace reader {

class BlocksTreeNode;
using thread_blocks_tree_t = ::std::unordered_map<::profiler::thread_id_t, BlocksTreeNode, ::estd::hash<::profiler::thread_id_t> >;
using thread_names_t = ::std::unordered_map<::profiler::thread_id_t, ::std::string>;
using context_switches_list_t = ::std::vector<ContextSwitchEvent>;
using context_switches_t = ::std::unordered_map<::profiler::thread_id_t, context_switches_list_t, ::estd::hash<::profiler::thread_id_t> >;
using descriptors_list_t = ::std::vector<BlockDescriptor>;

class BlocksTreeNode EASY_FINAL
{
public:

    using children_t = ::std::vector<BlocksTreeNode>;

    BlockInfo                   info;
    children_t              children;
    BlocksTreeNode* parent = nullptr;

    BlocksTreeNode(BlocksTreeNode&& other) EASY_NOEXCEPT
        : info(other.info)
        , children(::std::move(other.children))
        , parent(other.parent)
    {
    }

    BlocksTreeNode& operator = (BlocksTreeNode&& other) EASY_NOEXCEPT
    {
        info = other.info;
        children = ::std::move(other.children);
        parent = other.parent;
        return *this;
    }

    BlocksTreeNode() = default;
    ~BlocksTreeNode() = default;

    BlocksTreeNode(const BlocksTreeNode&) = delete;
    BlocksTreeNode& operator = (const BlocksTreeNode&) = delete;

}; // end of class BlocksTreeNode.

class FileReader EASY_FINAL
{
public:

    ///< initial read file with RAW data
    ::profiler::block_index_t readFile(const ::std::string& filename);

    ///< get blocks tree
    const thread_blocks_tree_t& getBlocksTree() const;

    const descriptors_list_t& getBlockDescriptors() const;

    const profiler::bookmarks_t& getBookmarks() const;

    /*! get thread name by Id
    \param threadId thread Id
    \return Name of thread
    */
    const std::string& getThreadName(uint64_t threadId) const;

    /*! get file version
    \return data file version
    */
    uint32_t getVersion() const;

    ::std::string getVersionString() const;

    ///< get context switches
    const context_switches_t& getContextSwitches() const;

private:

    ::std::string             m_emptyString;
    ::std::stringstream      m_errorMessage; ///< error log stream
    thread_blocks_tree_t       m_blocksTree; ///< thread's blocks hierarchy
    thread_names_t            m_threadNames; ///< [thread_id, thread_name]
    context_switches_t    m_contextSwitches; ///< context switches info
    descriptors_list_t   m_blockDescriptors; ///< block descriptors
    profiler::bookmarks_t       m_bookmarks; ///< User bookmarks
    uint32_t                      m_version; ///< .prof file version

}; // end of class FileReader.

} // end of namespace reader.

} // end of namespace profiler.

#endif // EASY_PROFILER_CONVERTER_READER_H
