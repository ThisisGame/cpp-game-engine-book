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

#include "nonscoped_block.h"
#include <cstring>
#include <cstdlib>

NonscopedBlock::NonscopedBlock(const profiler::BaseBlockDescriptor* _desc, const char* _runtimeName, bool)
    : profiler::Block(_desc, _runtimeName, false), m_runtimeName(nullptr)
{

}

NonscopedBlock::~NonscopedBlock()
{
    // Actually destructor should not be invoked because StackBuffer do manual memory management

    m_end = m_begin; // to restrict profiler::Block to invoke profiler::endBlock() on destructor.
    free(m_runtimeName);
}

void NonscopedBlock::copyname()
{
    // Here we need to copy m_name to m_runtimeName to ensure that
    // it would be alive to the moment we will serialize the block

    if ((m_status & profiler::ON) == 0)
        return;

    if (*m_name != 0)
    {
        auto len = strlen(m_name);
        m_runtimeName = static_cast<char*>(malloc(len + 1));

        // memcpy should be faster than strncpy because we know
        // actual bytes number and both strings have the same size
        memcpy(m_runtimeName, m_name, len);

        m_runtimeName[len] = 0;
        m_name = m_runtimeName;
    }
    else
    {
        m_name = "";
    }
}

void NonscopedBlock::destroy()
{
    // free memory used by m_runtimeName
    free(m_runtimeName);
    m_name = "";
}
