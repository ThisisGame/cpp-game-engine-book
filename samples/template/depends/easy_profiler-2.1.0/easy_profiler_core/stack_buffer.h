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

#ifndef EASY_PROFILER_STACK_BUFFER_H
#define EASY_PROFILER_STACK_BUFFER_H

#include <algorithm>
#include <cstdlib>
#include <list>

#include "nonscoped_block.h"

#ifdef max
#undef max
#endif

template <class T>
inline void destroy_elem(T*)
{

}

inline void destroy_elem(NonscopedBlock* _elem)
{
    _elem->destroy();
}

template <class T>
class StackBuffer
{
    struct chunk { int8_t data[sizeof(T)]; };

    std::list<chunk> m_overflow; ///< List of additional stack elements if current capacity of buffer is not enough
    T*                 m_buffer; ///< Contiguous buffer used for stack
    uint32_t             m_size; ///< Current size of stack
    uint32_t         m_capacity; ///< Current capacity of m_buffer
    uint32_t      m_maxcapacity; ///< Maximum used capacity including m_buffer and m_overflow

public:

    StackBuffer() = delete;
    StackBuffer(const StackBuffer&) = delete;
    StackBuffer(StackBuffer&&) = delete;

    explicit StackBuffer(uint32_t N)
        : m_buffer(static_cast<T*>(malloc(N * sizeof(T))))
        , m_size(0)
        , m_capacity(N)
        , m_maxcapacity(N)
    {
    }

    ~StackBuffer()
    {
        for (uint32_t i = 0; i < m_size; ++i)
            destroy_elem(m_buffer + i);

        free(m_buffer);

        for (auto& elem : m_overflow)
            destroy_elem(reinterpret_cast<T*>(elem.data + 0));
    }

    template <class ... TArgs>
    T& push(TArgs ... _args)
    {
        if (m_size < m_capacity)
            return *(::new (m_buffer + m_size++) T(_args...));

        m_overflow.emplace_back();
        const uint32_t cap = m_capacity + static_cast<uint32_t>(m_overflow.size());
        if (m_maxcapacity < cap)
            m_maxcapacity = cap;

        return *(::new (m_overflow.back().data + 0) T(_args...));
    }

    void pop()
    {
        if (m_overflow.empty())
        {
            // m_size should not be equal to 0 here because ProfileManager behavior does not allow such situation
            destroy_elem(m_buffer + --m_size);

            if (m_size == 0 && m_maxcapacity > m_capacity)
            {
                // When stack gone empty we can resize buffer to use enough space in the future
                free(m_buffer);
                m_maxcapacity = m_capacity = std::max(m_maxcapacity, m_capacity << 1);
                m_buffer = static_cast<T*>(malloc(m_capacity * sizeof(T)));
            }

            return;
        }

        destroy_elem(reinterpret_cast<T*>(m_overflow.back().data + 0));
        m_overflow.pop_back();
    }

}; // END of class StackBuffer.

#endif // EASY_PROFILER_STACK_BUFFER_H
