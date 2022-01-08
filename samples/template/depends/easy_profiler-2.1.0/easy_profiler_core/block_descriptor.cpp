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

#include <memory.h>
#include "block_descriptor.h"

#if EASY_BLOCK_DESC_FULL_COPY == 0
# define EASY_BLOCK_DESC_STRING_LEN(s) static_cast<uint16_t>(strlen(s) + 1)
# define EASY_BLOCK_DESC_STRING_VAL(s) s
#else
# define EASY_BLOCK_DESC_STRING_LEN(s) static_cast<uint16_t>(s.size() + 1)
# define EASY_BLOCK_DESC_STRING_VAL(s) s.c_str()
#endif

BlockDescriptor::BlockDescriptor(profiler::block_id_t _id, profiler::EasyBlockStatus _status, const char* _name,
                                 const char* _filename, int _line, profiler::block_type_t _block_type,
                                 profiler::color_t _color)
    : Parent(_id, _status, _line, _block_type, _color)
    , m_filename(_filename)
    , m_name(_name)
{
}

const char* BlockDescriptor::name() const
{
    return EASY_BLOCK_DESC_STRING_VAL(m_name);
}

const char* BlockDescriptor::filename() const
{
    return EASY_BLOCK_DESC_STRING_VAL(m_filename);
}

uint16_t BlockDescriptor::nameSize() const
{
    return EASY_BLOCK_DESC_STRING_LEN(m_name);
}

uint16_t BlockDescriptor::filenameSize() const
{
    return EASY_BLOCK_DESC_STRING_LEN(m_filename);
}

void BlockDescriptor::destroy(BlockDescriptor* instance)
{
#if EASY_BLOCK_DESC_FULL_COPY == 0
    if (instance != nullptr)
        instance->~BlockDescriptor();
    free(instance);
#else
    delete instance;
#endif
}
