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

#ifndef EASY_PROFILER_CHUNK_ALLOCATOR_H
#define EASY_PROFILER_CHUNK_ALLOCATOR_H

#include <easy/details/easy_compiler_support.h>
#include <cstring>
#include <ostream>
#include "alignment_helpers.h"

//////////////////////////////////////////////////////////////////////////

#ifndef EASY_ENABLE_ALIGNMENT 
# define EASY_ENABLE_ALIGNMENT 0
#endif

#ifndef EASY_ALIGNMENT_SIZE
# define EASY_ALIGNMENT_SIZE EASY_ALIGNOF(std::max_align_t)
#endif

EASY_CONSTEXPR auto EASY_ALIGN_SIZE = EASY_ALIGNMENT_SIZE;

#if EASY_ENABLE_ALIGNMENT == 0
# define EASY_ALIGNED(TYPE, VAR, A) TYPE VAR
# define EASY_MALLOC(MEMSIZE, A) malloc(MEMSIZE)
# define EASY_FREE(MEMPTR) free(MEMPTR)
#else
// MSVC and GNUC aligned versions of malloc are defined in malloc.h
# include <malloc.h>
# if defined(_MSC_VER)
#  define EASY_ALIGNED(TYPE, VAR, A) __declspec(align(A)) TYPE VAR
#  define EASY_MALLOC(MEMSIZE, A) _aligned_malloc(MEMSIZE, A)
#  define EASY_FREE(MEMPTR) _aligned_free(MEMPTR)
# elif defined(__GNUC__)
#  define EASY_ALIGNED(TYPE, VAR, A) TYPE VAR __attribute__((aligned(A)))
#  define EASY_MALLOC(MEMSIZE, A) memalign(A, MEMSIZE)
#  define EASY_FREE(MEMPTR) free(MEMPTR)
# else
#  define EASY_ALIGNED(TYPE, VAR, A) TYPE VAR
#  define EASY_MALLOC(MEMSIZE, A) malloc(MEMSIZE)
#  define EASY_FREE(MEMPTR) free(MEMPTR)
# endif
#endif

//////////////////////////////////////////////////////////////////////////

template <const uint16_t N>
class chunk_allocator
{
    static_assert(N != 0, "chunk_allocator<N> N must be a positive value");

    struct chunk { EASY_ALIGNED(char, data[N], EASY_ALIGNMENT_SIZE); chunk* prev = nullptr; };

    struct chunk_list
    {
        chunk* last;

        chunk_list(const chunk_list&) = delete;
        chunk_list(chunk_list&&) = delete;

        chunk_list() : last(nullptr)
        {
            static_assert(sizeof(char) == 1, "easy_profiler logic error: sizeof(char) != 1 for this platform! Please, contact easy_profiler authors to resolve your problem.");
            emplace_back();
        }

        ~chunk_list()
        {
            do free_last(); while (last != nullptr);
        }

        void clear_all_except_last()
        {
            while (last->prev != nullptr)
                free_last();
            zero_last_chunk_size();
        }

        void emplace_back()
        {
            auto prev = last;
            last = ::new (EASY_MALLOC(sizeof(chunk), EASY_ALIGNMENT_SIZE)) chunk();
            last->prev = prev;
            zero_last_chunk_size();
        }

        /** Invert current chunks list to enable to iterate over chunks list in direct order.

        This method is used by serialize().
        */
        void invert()
        {
            chunk* next = nullptr;

            while (last->prev != nullptr) {
                auto p = last->prev;
                last->prev = next;
                next = last;
                last = p;
            }

            last->prev = next;
        }

    private:

        void free_last()
        {
            auto p = last;
            last = last->prev;
            EASY_FREE(p);
        }

        void zero_last_chunk_size()
        {
            // Although there is no need for unaligned access stuff b/c a new chunk will
            // usually be at least 8 byte aligned (and we only need 2 byte alignment),
            // this is the only way I have been able to get rid of the GCC strict-aliasing warning
            // without using std::memset. It's an extra line, but is just as fast as *(uint16_t*)last->data = 0;
            char* const data = last->data;
            *(uint16_t*)data = (uint16_t)0;
        }
    };

    // Used in serialize(): workaround for no constexpr support in MSVC 2013.
    EASY_STATIC_CONSTEXPR int_fast32_t MaxChunkOffset = N - sizeof(uint16_t);
    EASY_STATIC_CONSTEXPR uint16_t OneBeforeN = static_cast<uint16_t>(N - 1);

    chunk_list          m_chunks; ///< List of chunks.
    chunk*         m_markedChunk; ///< Chunk marked by last closed frame
    uint32_t              m_size; ///< Number of elements stored(# of times allocate() has been called.)
    uint32_t        m_markedSize; ///< Number of elements to the moment when put_mark() has been called.
    uint16_t       m_chunkOffset; ///< Number of bytes used in the current chunk.
    uint16_t m_markedChunkOffset; ///< Last byte in marked chunk for serializing.

public:

    chunk_allocator(const chunk_allocator&) = delete;
    chunk_allocator(chunk_allocator&&) = delete;

    chunk_allocator() : m_markedChunk(nullptr), m_size(0), m_markedSize(0), m_chunkOffset(0), m_markedChunkOffset(0)
    {
    }

    /** Allocate n bytes.

    Automatically checks if there is enough preserved memory to store additional n bytes
    and allocates additional buffer if needed.
    */
    void* allocate(uint16_t n)
    {
        ++m_size;

        if (!need_expand(n))
        {
            // Temp to avoid extra load due to this* aliasing.
            uint16_t chunkOffset = m_chunkOffset;
            char* data = m_chunks.last->data + chunkOffset;
            chunkOffset += n + sizeof(uint16_t);
            m_chunkOffset = chunkOffset;

            unaligned_store16(data, n);
            data += sizeof(uint16_t);

            // If there is enough space for at least another payload size,
            // set it to zero.
            if (chunkOffset < OneBeforeN)
                unaligned_zero16(data + n);

            return data;
        }

        m_chunkOffset = n + sizeof(uint16_t);
        m_chunks.emplace_back();

        char* data = m_chunks.last->data;
        unaligned_store16(data, n);
        data += sizeof(uint16_t);
        
        // We assume here that it takes more than one element to fill a chunk.
        unaligned_zero16(data + n);

        return data;
    }

    /** Check if current storage is not enough to store additional n bytes.
    */
    bool need_expand(uint16_t n) const
    {
        return (m_chunkOffset + n + sizeof(uint16_t)) > N;
    }

    uint32_t size() const
    {
        return m_size;
    }

    bool empty() const
    {
        return m_size == 0;
    }

    uint32_t markedSize() const
    {
        return m_markedSize;
    }

    bool markedEmpty() const
    {
        return m_markedSize == 0;
    }

    void clear()
    {
        m_size = 0;
        m_markedSize = 0;
        m_chunkOffset = 0;
        m_markedChunk = nullptr;
        m_chunks.clear_all_except_last(); // There is always at least one chunk
    }

    /** Serialize data to stream.

    \warning Data will be cleared after serialization.
    */
    void serialize(std::ostream& _outputStream)
    {
        // Chunks are stored in reversed order (stack).
        // To be able to iterate them in direct order we have to invert the chunks list.
        m_chunks.invert();

        // Each chunk is an array of N bytes that can hold between
        // 1(if the list isn't empty) and however many elements can fit in a chunk,
        // where an element consists of a payload size + a payload as follows:
        // elementStart[0..1]: size as a uint16_t
        // elementStart[2..size-1]: payload.
        
        // The maximum chunk offset is N-sizeof(uint16_t) b/c, if we hit that (or go past),
        // there is either no space left, 1 byte left, or 2 bytes left, all of which are
        // too small to cary more than a zero-sized element.

        chunk* current = m_chunks.last;
        bool isMarked;
        do {

            isMarked = (current == m_markedChunk);
            const char* data = current->data;

            const int_fast32_t maxOffset = isMarked ? m_markedChunkOffset : MaxChunkOffset;
            int_fast32_t chunkOffset = 0; // signed int so overflow is not checked.
            auto payloadSize = unaligned_load16<uint16_t>(data);

            while (chunkOffset < maxOffset && payloadSize != 0)
            {
                const uint16_t chunkSize = sizeof(uint16_t) + payloadSize;
                _outputStream.write(data, chunkSize);
                data += chunkSize;
                chunkOffset += chunkSize;
                unaligned_load16(data, &payloadSize);
            }

            current = current->prev;

        } while (current != nullptr && !isMarked);

        clear();
    }

    void put_mark()
    {
        m_markedChunk = m_chunks.last;
        m_markedSize = m_size;
        m_markedChunkOffset = m_chunkOffset;
    }

    void* marked_allocate(uint16_t n)
    {
        chunk* marked = m_markedChunk;
        if (marked == nullptr || (marked == m_chunks.last && m_markedSize == m_size))
        {
            auto data = allocate(n);
            put_mark();
            return data;
        }

        ++m_markedSize;

        uint16_t chunkOffset = m_markedChunkOffset;
        if ((chunkOffset + n + sizeof(uint16_t)) <= N)
        {
            // Temp to avoid extra load due to this* aliasing.
            char* data = marked->data + chunkOffset;
            chunkOffset += n + sizeof(uint16_t);
            m_markedChunkOffset = chunkOffset;

            unaligned_store16(data, n);
            data += sizeof(uint16_t);

            // If there is enough space for at least another payload size,
            // set it to zero.
            if (chunkOffset < OneBeforeN)
                unaligned_zero16(data + n);

            if (marked == m_chunks.last && chunkOffset > m_chunkOffset)
                m_chunkOffset = chunkOffset;

            return data;
        }

        chunkOffset = n + sizeof(uint16_t);
        m_markedChunkOffset = chunkOffset;

        chunk* last = m_chunks.last;
        if (marked == last)
        {
            m_chunks.emplace_back();
            last = m_chunks.last;
            m_chunkOffset = chunkOffset;
            m_size = m_markedSize;
        }
        else
        {
            do last = last->prev; while (last->prev != m_markedChunk);
        }

        m_markedChunk = last;
        char* data = last->data;
        unaligned_store16(data, n);
        data += sizeof(uint16_t);

        // We assume here that it takes more than one element to fill a chunk.
        unaligned_zero16(data + n);

        return data;
    }

}; // END of class chunk_allocator.

//////////////////////////////////////////////////////////////////////////

template <const uint16_t N, bool, bool>
struct aligned_size;

template <const uint16_t N, bool dummy>
struct aligned_size<N, true, dummy> {
    EASY_STATIC_CONSTEXPR uint16_t Size = N;
};

template <const uint16_t N>
struct aligned_size<N, false, true> {
    EASY_STATIC_CONSTEXPR uint16_t Size = static_cast<uint16_t>(N - (N % EASY_ALIGN_SIZE));
};

template <const uint16_t N>
struct aligned_size<N, false, false> {
    EASY_STATIC_CONSTEXPR uint16_t Size = static_cast<uint16_t>(N + EASY_ALIGN_SIZE - (N % EASY_ALIGN_SIZE));
};

template <const size_t N>
struct get_aligned_size {
    EASY_STATIC_CONSTEXPR uint16_t Size =
        aligned_size<static_cast<uint16_t>(N), (N % EASY_ALIGN_SIZE) == 0, (N > (65536 - EASY_ALIGN_SIZE))>::Size;
};

static_assert(get_aligned_size<EASY_ALIGN_SIZE - 3>::Size == EASY_ALIGN_SIZE, "wrong get_aligned_size");
static_assert(get_aligned_size<2 * EASY_ALIGN_SIZE - 3>::Size == 2 * EASY_ALIGN_SIZE, "wrong get_aligned_size");
static_assert(get_aligned_size<65530>::Size == 65536 - EASY_ALIGN_SIZE, "wrong get_aligned_size");
static_assert(get_aligned_size<65526>::Size == 65536 - EASY_ALIGN_SIZE, "wrong get_aligned_size");
static_assert(get_aligned_size<65536 - EASY_ALIGN_SIZE>::Size == 65536 - EASY_ALIGN_SIZE, "wrong get_aligned_size");
static_assert(get_aligned_size<65536 + 3 - EASY_ALIGN_SIZE * 2>::Size == 65536 - EASY_ALIGN_SIZE, "wrong get_aligned_size");

//////////////////////////////////////////////////////////////////////////

#endif // EASY_PROFILER_CHUNK_ALLOCATOR_H
