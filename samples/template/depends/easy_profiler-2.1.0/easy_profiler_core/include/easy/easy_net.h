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

#ifndef EASY_NET_H
#define EASY_NET_H

#include <easy/details/easy_compiler_support.h>
#include <stdint.h>

namespace profiler { namespace net {

EASY_CONSTEXPR uint32_t EASY_MESSAGE_SIGN = 20160909;

#pragma pack(push,1)

enum class MessageType : uint8_t
{
    Undefined = 0,

    Request_Start_Capture,
    Reply_Capturing_Started,
    Request_Stop_Capture,

    Reply_Blocks,
    Reply_Blocks_End,

    Connection_Accepted,

    Request_Blocks_Description,
    Reply_Blocks_Description,
    Reply_Blocks_Description_End,

    Change_Block_Status,
    Change_Event_Tracing_Status,
    Change_Event_Tracing_Priority,

    Ping,

    Request_MainThread_FPS,
    Reply_MainThread_FPS,
};

struct Message
{
    uint32_t magic_number = EASY_MESSAGE_SIGN;
    MessageType type = MessageType::Undefined;

    bool isEasyNetMessage() const EASY_NOEXCEPT {
        return EASY_MESSAGE_SIGN == magic_number;
    }

    explicit Message(MessageType _t) EASY_NOEXCEPT : type(_t) { }

    Message() = default;
};

struct DataMessage : public Message
{
    uint32_t size = 0; // bytes

    explicit DataMessage(MessageType _t = MessageType::Reply_Blocks) : Message(_t) {}
    explicit DataMessage(uint32_t _s, MessageType _t = MessageType::Reply_Blocks) : Message(_t), size(_s) {}

    const char* data() const { return reinterpret_cast<const char*>(this) + sizeof(DataMessage); }
};

struct BlockStatusMessage : public Message
{
    uint32_t    id;
    uint8_t status;

    explicit BlockStatusMessage(uint32_t _id, uint8_t _status)
        : Message(MessageType::Change_Block_Status), id(_id), status(_status) { }

    BlockStatusMessage() = delete;
};

struct EasyProfilerStatus : public Message
{
    bool         isProfilerEnabled;
    bool     isEventTracingEnabled;
    bool isLowPriorityEventTracing;

    explicit EasyProfilerStatus(bool _enabled, bool _ETenabled, bool _ETlowp)
        : Message(MessageType::Connection_Accepted)
        , isProfilerEnabled(_enabled)
        , isEventTracingEnabled(_ETenabled)
        , isLowPriorityEventTracing(_ETlowp)
    {
    }

    EasyProfilerStatus() = delete;
};

struct BoolMessage : public Message
{
    bool flag = false;

    explicit BoolMessage(MessageType _t, bool _flag = false)
        : Message(_t), flag(_flag) { }

    BoolMessage() = default;
};

struct TimestampMessage : public Message
{
    uint32_t maxValue = 0;
    uint32_t avgValue = 0;

    explicit TimestampMessage(MessageType _t, uint32_t _maxValue, uint32_t _avgValue)
        : Message(_t), maxValue(_maxValue), avgValue(_avgValue) { }

    TimestampMessage() = default;
};

#pragma pack(pop)

}//net

}//profiler

#endif // EASY_NET_H
