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

#ifndef EASY_PROFILER_PUBLIC_TYPES_H
#define EASY_PROFILER_PUBLIC_TYPES_H

#include <easy/details/profiler_aux.h>

class NonscopedBlock;
class ProfileManager;
struct ThreadStorage;

namespace profiler {

    using timestamp_t = uint64_t;
    using thread_id_t = uint64_t;
    using block_id_t  = uint32_t;

    enum class BlockType : uint8_t
    {
        Event = 0,
        Block,
        Value,

        TypesCount
    };
    using block_type_t = BlockType;

    enum Duration : uint8_t
    {
        TICKS = 0, ///< CPU ticks
        MICROSECONDS ///< Microseconds
    };

    //***********************************************

#pragma pack(push,1)
    class PROFILER_API BaseBlockDescriptor
    {
        friend ::ProfileManager;
        friend ::ThreadStorage;

    protected:

        block_id_t          m_id; ///< This descriptor id (We can afford this spending because there are much more blocks than descriptors)
        int32_t           m_line; ///< Line number in the source file
        color_t          m_color; ///< Color of the block packed into 1-byte structure
        block_type_t      m_type; ///< Type of the block (See BlockType)
        EasyBlockStatus m_status; ///< If false then blocks with such id() will not be stored by profiler during profile session

        explicit BaseBlockDescriptor(block_id_t _id, EasyBlockStatus _status, int _line, block_type_t _block_type, color_t _color) EASY_NOEXCEPT;

    public:

        BaseBlockDescriptor() = delete;

        inline block_id_t id() const EASY_NOEXCEPT { return m_id; }
        inline int32_t line() const EASY_NOEXCEPT { return m_line; }
        inline color_t color() const EASY_NOEXCEPT { return m_color; }
        inline block_type_t type() const EASY_NOEXCEPT { return m_type; }
        inline EasyBlockStatus status() const EASY_NOEXCEPT { return m_status; }

    }; // END of class BaseBlockDescriptor.

    //***********************************************

    class PROFILER_API Event
    {
        friend ::ProfileManager;

    protected:

        timestamp_t m_begin;
        timestamp_t   m_end;

    public:

        Event() = delete;

        Event(const Event&) = default;
        explicit Event(timestamp_t _begin_time) EASY_NOEXCEPT;
        explicit Event(timestamp_t _begin_time, timestamp_t _end_time) EASY_NOEXCEPT;

        inline timestamp_t begin() const EASY_NOEXCEPT { return m_begin; }
        inline timestamp_t end() const EASY_NOEXCEPT { return m_end; }
        inline timestamp_t duration() const EASY_NOEXCEPT { return m_end - m_begin; }

    }; // END class Event.

    class PROFILER_API BaseBlockData : public Event
    {
        friend ::ProfileManager;

    protected:

        block_id_t m_id;

    public:

        BaseBlockData() = delete;

        BaseBlockData(const BaseBlockData&) = default;
        explicit BaseBlockData(timestamp_t _begin_time, block_id_t _id) EASY_NOEXCEPT;
        explicit BaseBlockData(timestamp_t _begin_time, timestamp_t _end_time, block_id_t _id) EASY_NOEXCEPT;

        inline block_id_t id() const EASY_NOEXCEPT { return m_id; }
        inline void setId(block_id_t _id) EASY_NOEXCEPT { m_id = _id; }

    }; // END of class BaseBlockData.
#pragma pack(pop)

    //***********************************************

    class PROFILER_API Block : public BaseBlockData
    {
        friend ::ProfileManager;
        friend ::ThreadStorage;
        friend ::NonscopedBlock;

        const char*       m_name;
        EasyBlockStatus m_status;
        bool          m_isScoped;

    private:

        void start();
        void start(timestamp_t _time) EASY_NOEXCEPT;
        void finish();
        void finish(timestamp_t _time) EASY_NOEXCEPT;
        inline bool finished() const EASY_NOEXCEPT { return m_end >= m_begin; }
        inline EasyBlockStatus status() const EASY_NOEXCEPT { return m_status; }
        inline void setStatus(EasyBlockStatus _status) EASY_NOEXCEPT { m_status = _status; }

    public:

        Block(const Block&)              = delete;
        Block& operator = (const Block&) = delete;

        Block(Block&& that) EASY_NOEXCEPT;
        Block(const BaseBlockDescriptor* _desc, const char* _runtimeName, bool _scoped = true) EASY_NOEXCEPT;
        Block(timestamp_t _begin_time, block_id_t _id, const char* _runtimeName) EASY_NOEXCEPT;
        Block(timestamp_t _begin_time, timestamp_t _end_time, block_id_t _id, const char* _runtimeName) EASY_NOEXCEPT;
        ~Block();

        inline const char* name() const EASY_NOEXCEPT { return m_name; }

    }; // END of class Block.

    //***********************************************

    class PROFILER_API ThreadGuard EASY_FINAL
    {
        friend ::ProfileManager;
        thread_id_t m_id = 0;

    public:

        ~ThreadGuard();

    }; // END of class ThreadGuard.

    EASY_CONSTEXPR uint16_t MAX_BLOCK_DATA_SIZE = 2048 + 512 + 256; ///< Estimated maximum size of block dynamic name or EASY_VALUE data size

} // END of namespace profiler.

#endif // EASY_PROFILER_PUBLIC_TYPES_H
