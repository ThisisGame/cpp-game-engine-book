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

#ifndef EASY_PROFILER_SERIALIZED_BLOCK_H
#define EASY_PROFILER_SERIALIZED_BLOCK_H

#include <easy/details/profiler_public_types.h>
#include <easy/details/arbitrary_value_public_types.h>

class CSwitchBlock;

namespace profiler {

    template <DataType dataType, bool isArray>
    struct Value;

    template <bool isArray>
    struct Value<DataType::TypesCount, isArray>;

    //////////////////////////////////////////////////////////////////////////

    class PROFILER_API SerializedBlock EASY_FINAL : public BaseBlockData
    {
        friend ::ProfileManager;
        friend ::ThreadStorage;

    public:

        inline const char* data() const { return reinterpret_cast<const char*>(this); }

        ///< Run-time block name is stored right after main BaseBlockData data
        inline const char* name() const { return data() + sizeof(BaseBlockData); }

        SerializedBlock(const SerializedBlock&)              = delete;
        SerializedBlock& operator = (const SerializedBlock&) = delete;
        SerializedBlock(SerializedBlock&&)                   = delete;
        SerializedBlock& operator = (SerializedBlock&&)      = delete;
        ~SerializedBlock()                                   = delete;

    private:

        explicit SerializedBlock(const Block& block, uint16_t name_length);

    }; // END of SerializedBlock.

    //////////////////////////////////////////////////////////////////////////

#pragma pack(push, 1)
    class PROFILER_API CSwitchEvent : public Event
    {
        thread_id_t m_thread_id;

    public:

        CSwitchEvent() = default;
        CSwitchEvent(const CSwitchEvent&) = default;
        explicit CSwitchEvent(timestamp_t _begin_time, thread_id_t _tid) EASY_NOEXCEPT;

        inline thread_id_t tid() const EASY_NOEXCEPT { return m_thread_id; }

    }; // END of class CSwitchEvent.
#pragma pack(pop)

    class PROFILER_API SerializedCSwitch EASY_FINAL : public CSwitchEvent
    {
        friend ::ProfileManager;
        friend ::ThreadStorage;

    public:

        inline const char* data() const { return reinterpret_cast<const char*>(this); }

        ///< Run-time block name is stored right after main CSwitchEvent data
        inline const char* name() const { return data() + sizeof(CSwitchEvent); }

        SerializedCSwitch(const SerializedCSwitch&)              = delete;
        SerializedCSwitch& operator = (const SerializedCSwitch&) = delete;
        SerializedCSwitch(SerializedCSwitch&&)                   = delete;
        SerializedCSwitch& operator = (SerializedCSwitch&&)      = delete;
        ~SerializedCSwitch()                                     = delete;

    private:

        explicit SerializedCSwitch(const CSwitchBlock& block, uint16_t name_length);

    }; // END of SerializedCSwitch.

    //////////////////////////////////////////////////////////////////////////

#pragma pack(push, 1)
    class PROFILER_API SerializedBlockDescriptor EASY_FINAL : public BaseBlockDescriptor
    {
        uint16_t m_nameLength; ///< Length of the name including trailing '\0' sybmol

    public:

        inline const char* data() const {
            return reinterpret_cast<const char*>(this);
        }

        ///< Name is stored right after m_nameLength
        inline const char* name() const {
            static const auto shift = sizeof(BaseBlockDescriptor) + sizeof(decltype(m_nameLength));
            return data() + shift;
        }

        ///< File name is stored right after the name
        inline const char* file() const {
            return name() + m_nameLength;
        }

        inline void setStatus(EasyBlockStatus _status) EASY_NOEXCEPT {
            m_status = _status;
        }

        // Instances of this class can not be created or destroyed directly
        SerializedBlockDescriptor()                                              = delete;
        SerializedBlockDescriptor(const SerializedBlockDescriptor&)              = delete;
        SerializedBlockDescriptor& operator = (const SerializedBlockDescriptor&) = delete;
        SerializedBlockDescriptor(SerializedBlockDescriptor&&)                   = delete;
        SerializedBlockDescriptor& operator = (SerializedBlockDescriptor&&)      = delete;
        ~SerializedBlockDescriptor()                                             = delete;

    }; // END of SerializedBlockDescriptor.
//#pragma pack(pop)

    //////////////////////////////////////////////////////////////////////////

//#pragma pack(push, 1)
    class PROFILER_API ArbitraryValue : protected BaseBlockData
    {
        friend ::ThreadStorage;

    protected:

        char     m_nameStub; ///< Artificial padding which is used to imitate SerializedBlock::name() == 0 behavior
        char      m_padding; ///< Padding to the bound of 2 bytes
        uint16_t     m_size;
        DataType     m_type;
        bool      m_isArray;
        vin_t    m_value_id;

        explicit ArbitraryValue(timestamp_t _timestamp, vin_t _vin, block_id_t _id,
                                uint16_t _size, DataType _type, bool _isArray)
            : BaseBlockData(_timestamp, _timestamp, _id)
            , m_nameStub(0)
            , m_padding(0)
            , m_size(_size)
            , m_type(_type)
            , m_isArray(_isArray)
            , m_value_id(_vin)
        {
        }

    public:

        using BaseBlockData::id;
        using Event::begin;

        ~ArbitraryValue() = delete;

        const char* data() const {
            return reinterpret_cast<const char*>(this) + sizeof(ArbitraryValue);
        }

        uint16_t data_size() const {
            return m_size;
        }

        vin_t value_id() const {
            return m_value_id;
        }

        DataType type() const {
            return m_type;
        }

        bool isArray() const {
            return m_isArray;
        }

        template <DataType dataType>
        const Value<dataType, false>* toValue() const {
            return m_type == dataType ? static_cast<const Value<dataType, false>*>(this) : nullptr;
        }

        template <class T>
        const Value<StdToDataType<T>::data_type, false>* toValue() const {
            static_assert(StdToDataType<T>::data_type != DataType::TypesCount,
                          "You should use standard builtin scalar types as profiler::Value type!");
            return toValue<StdToDataType<T>::data_type>();
        }

        template <DataType dataType>
        const Value<dataType, true>* toArray() const {
            return m_isArray && m_type == dataType ? static_cast<const Value<dataType, true>*>(this) : nullptr;
        }

        template <class T>
        const Value<StdToDataType<T>::data_type, true>* toArray() const {
            static_assert(StdToDataType<T>::data_type != DataType::TypesCount,
                          "You should use standard builtin scalar types as profiler::Value type!");
            return toArray<StdToDataType<T>::data_type>();
        }
    }; // end of class ArbitraryValue.
#pragma pack(pop)

    //////////////////////////////////////////////////////////////////////////

    template <DataType dataType>
    struct Value<dataType, false> EASY_FINAL : public ArbitraryValue {
        using value_type = typename StdType<dataType>::value_type;
        value_type value() const { return *reinterpret_cast<const value_type*>(data()); }
        ~Value() = delete;
    };


    template <DataType dataType>
    struct Value<dataType, true> EASY_FINAL : public ArbitraryValue {
        using value_type = typename StdType<dataType>::value_type;
        const value_type* value() const { return reinterpret_cast<const value_type*>(data()); }
        uint16_t size() const { return m_size / sizeof(value_type); }
        value_type operator [] (int i) const { return value()[i]; }
        value_type at(int i) const { return value()[i]; }
        ~Value() = delete;
    };


    template <>
    struct Value<DataType::String, true> EASY_FINAL : public ArbitraryValue {
        using value_type = char;
        const char* value() const { return data(); }
        uint16_t size() const { return m_size; }
        char operator [] (int i) const { return data()[i]; }
        char at(int i) const { return data()[i]; }
        const char* c_str() const { return data(); }
        ~Value() = delete;
    };

    //////////////////////////////////////////////////////////////////////////

    template <DataType dataType>
    using SingleValue = Value<dataType, false>;

    template <DataType dataType>
    using ArrayValue = Value<dataType, true>;

    using StringValue = Value<DataType::String, true>;

    //////////////////////////////////////////////////////////////////////////

} // END of namespace profiler.

#endif // EASY_PROFILER_SERIALIZED_BLOCK_H
