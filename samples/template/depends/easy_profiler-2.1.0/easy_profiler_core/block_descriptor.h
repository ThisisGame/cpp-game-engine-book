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

#ifndef EASY_PROFILER_BLOCK_DESCRIPTOR_H
#define EASY_PROFILER_BLOCK_DESCRIPTOR_H

#include <string>
#include <easy/details/profiler_public_types.h>

#ifndef EASY_BLOCK_DESC_FULL_COPY
# define EASY_BLOCK_DESC_FULL_COPY 1
#endif

class BlockDescriptor : public profiler::BaseBlockDescriptor
{
    friend ProfileManager;

    using Parent = profiler::BaseBlockDescriptor;

#if EASY_BLOCK_DESC_FULL_COPY == 0
    using string_t = const char*;
#else
    using string_t = std::string;
#endif

    string_t m_filename; ///< Source file name where this block is declared
    string_t     m_name; ///< Static name of all blocks of the same type (blocks can have dynamic name) which is, in pair with descriptor id, a unique block identifier

public:

    BlockDescriptor() = delete;
    BlockDescriptor(const BlockDescriptor&) = delete;
    BlockDescriptor& operator = (const BlockDescriptor&) = delete;

    BlockDescriptor(profiler::block_id_t _id, profiler::EasyBlockStatus _status, const char* _name,
                    const char* _filename, int _line, profiler::block_type_t _block_type, profiler::color_t _color);

    const char*      name() const;
    const char*  filename() const;
    uint16_t     nameSize() const;
    uint16_t filenameSize() const;

    static void destroy(BlockDescriptor* instance);

}; // END of class BlockDescriptor.

#endif //EASY_PROFILER_BLOCK_DESCRIPTOR_H
