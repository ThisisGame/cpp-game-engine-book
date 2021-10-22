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

#include "converter.h"
#include <fstream>

void JsonExporter::convert(const profiler::reader::BlocksTreeNode& node, nlohmann::json& json) const
{
    if (node.info.descriptor != nullptr)
    {
        json = {{"id", node.info.blockIndex},
                {"name", node.info.descriptor->blockName},
                {"start", node.info.beginTime},
                {"stop", node.info.endTime},
                {"descriptor", node.info.descriptor->id}};
    }

    convertChildren(node, json);
}

void JsonExporter::convertChildren(const profiler::reader::BlocksTreeNode& node, nlohmann::json& json) const
{
    if (node.children.empty())
        return;

    auto children = nlohmann::json::array();
    for (const auto& child : node.children)
    {
        children.emplace_back();
        convert(child, children.back());
    }

    json["children"] = children;
}

void JsonExporter::convert(const ::std::string& inputFile, const ::std::string& outputFile) const
{
    profiler::reader::FileReader fr;
    if (fr.readFile(inputFile) == 0)
        return;

    nlohmann::json json = {{"version", fr.getVersionString()}, {"timeUnits", "ns"}};

    // convert descriptors
    {
        auto descriptors = nlohmann::json::array();
        const auto& block_descriptors = fr.getBlockDescriptors();
        for (const auto& descriptor : block_descriptors)
        {
            descriptors.emplace_back();

            std::stringstream stream;
            stream << "0x" << std::hex << descriptor.argbColor;

            auto& desc = descriptors.back();

            desc["id"] = descriptor.id;
            if (descriptor.parentId != descriptor.id)
                desc["parentId"] = descriptor.parentId;

            desc["name"] = descriptor.blockName;
            desc["type"] = descriptor.blockType;
            desc["color"] = stream.str();
            desc["sourceFile"] = descriptor.fileName;
            desc["sourceLine"] = descriptor.lineNumber;
        }

        json["blockDescriptors"] = descriptors;
    }

    // convert threads and blocks
    {
        auto threads = nlohmann::json::array();
        const auto& blocks_tree = fr.getBlocksTree();
        for (const auto& kv : blocks_tree)
        {
            threads.emplace_back();

            auto& thread = threads.back();
            thread["threadId"] = kv.first;
            thread["threadName"] = fr.getThreadName(kv.first);

            convertChildren(kv.second, thread);
        }

        json["threads"] = threads;
    }

    // convert bookmarks
    {
        auto bookmarks = nlohmann::json::array();
        const auto& bmarks = fr.getBookmarks();
        for (const auto& mark : bmarks)
        {
            std::stringstream stream;
            stream << "0x" << std::hex << mark.color;

            bookmarks.emplace_back();

            auto& bookmark = bookmarks.back();
            bookmark["timestamp"] = mark.pos;
            bookmark["color"] = stream.str();
            bookmark["text"] = mark.text;
        }

        json["bookmarks"] = bookmarks;
    }

    try
    {
        if (!outputFile.empty())
        {
            ::std::ofstream file(outputFile);
            json.dump(file, true, 1);
        }
        else
        {
            json.dump(::std::cout, true, 1);
        }
    }
    catch (...)
    {
        ::std::cout << "json.dump() error...";
    }
}
