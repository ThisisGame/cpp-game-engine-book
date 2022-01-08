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

#include <QDebug>

#include <easy/easy_net.h>

#include "common_functions.h"
#include "socket_listener.h"

#ifdef min
#undef min
#endif

#ifdef max
#undef max
#endif

SocketListener::SocketListener() : m_receivedSize(0), m_port(0), m_regime(ListenerRegime::Idle)
{
    m_bInterrupt = false;
    m_bConnected = false;
    m_bStopReceive = false;
    m_bFrameTimeReady = false;
    m_bCaptureReady = false;
    m_frameMax = 0;
    m_frameAvg = 0;
}

SocketListener::~SocketListener()
{
    m_bInterrupt.store(true, std::memory_order_release);
    if (m_thread.joinable())
    {
        m_thread.join();
    }
}

bool SocketListener::connected() const
{
    return m_bConnected.load(std::memory_order_acquire);
}

bool SocketListener::captured() const
{
    return m_bCaptureReady.load(std::memory_order_acquire);
}

ListenerRegime SocketListener::regime() const
{
    return m_regime;
}

uint64_t SocketListener::size() const
{
    return m_receivedSize;
}

std::stringstream& SocketListener::data()
{
    return m_receivedData;
}

const std::string& SocketListener::address() const
{
    return m_address;
}

uint16_t SocketListener::port() const
{
    return m_port;
}

void SocketListener::clearData()
{
    profiler_gui::clear_stream(m_receivedData);
    m_receivedSize = 0;
}

void SocketListener::disconnect()
{
    if (connected())
    {
        m_bInterrupt.store(true, std::memory_order_release);
        if (m_thread.joinable())
        {
            m_thread.join();
        }

        m_bConnected.store(false, std::memory_order_release);
        m_bInterrupt.store(false, std::memory_order_release);
        m_bCaptureReady.store(false, std::memory_order_release);
        m_bStopReceive.store(false, std::memory_order_release);
    }

    m_address.clear();
    m_port = 0;

    closeSocket();
}

void SocketListener::closeSocket()
{
    m_easySocket.flush();
    m_easySocket.init();
}

bool SocketListener::connect(
    const char* _ipaddress,
    uint16_t _port,
    profiler::net::EasyProfilerStatus& _reply,
    bool _disconnectFirst
) {
    if (connected())
    {
        m_bInterrupt.store(true, std::memory_order_release);
        if (m_thread.joinable())
        {
            m_thread.join();
        }

        m_bConnected.store(false, std::memory_order_release);
        m_bInterrupt.store(false, std::memory_order_release);
        m_bCaptureReady.store(false, std::memory_order_release);
        m_bStopReceive.store(false, std::memory_order_release);
    }

    m_address.clear();
    m_port = 0;

    if (_disconnectFirst)
    {
        closeSocket();
    }

    int res = m_easySocket.setAddress(_ipaddress, _port);
    res = m_easySocket.connect();

    const bool isConnected = res == 0;
    if (isConnected)
    {
        EASY_CONSTEXPR size_t buffer_size = sizeof(profiler::net::EasyProfilerStatus) << 1;
        char buffer[buffer_size] = {};
        int bytes = 0;

        while (true)
        {
            bytes = m_easySocket.receive(buffer, buffer_size);

            if (bytes == -1)
            {
                if (m_easySocket.isDisconnected())
                {
                    return false;
                }

                bytes = 0;

                continue;
            }

            break;
        }

        if (bytes == 0)
        {
            m_address = _ipaddress;
            m_port = _port;
            m_bConnected.store(isConnected, std::memory_order_release);
            return isConnected;
        }

        size_t seek = bytes;
        while (seek < sizeof(profiler::net::EasyProfilerStatus))
        {
            bytes = m_easySocket.receive(buffer + seek, buffer_size - seek);

            if (bytes == -1)
            {
                if (m_easySocket.isDisconnected())
                {
                    return false;
                }

                break;
            }

            seek += bytes;
        }

        auto message = reinterpret_cast<const profiler::net::EasyProfilerStatus*>(buffer);
        if (message->isEasyNetMessage() && message->type == profiler::net::MessageType::Connection_Accepted)
        {
            _reply = *message;
        }

        m_address = _ipaddress;
        m_port = _port;
    }

    m_bConnected.store(isConnected, std::memory_order_release);

    return isConnected;
}

bool SocketListener::reconnect(const char* _ipaddress, uint16_t _port, profiler::net::EasyProfilerStatus& _reply)
{
      return connect(_ipaddress, _port, _reply, true);
}

bool SocketListener::startCapture()
{
    //if (m_thread.joinable())
    //{
    //    m_bInterrupt.store(true, std::memory_order_release);
    //    m_thread.join();
    //    m_bInterrupt.store(false, std::memory_order_release);
    //}

    clearData();

    profiler::net::Message request(profiler::net::MessageType::Request_Start_Capture);
    m_easySocket.send(&request, sizeof(request));

    if (m_easySocket.isDisconnected())
    {
        m_bConnected.store(false, std::memory_order_release);
        return false;
    }

    m_regime = ListenerRegime::Capture;
    m_bCaptureReady.store(false, std::memory_order_release);
    //m_thread = std::thread(&SocketListener::listenCapture, this);

    return true;
}

void SocketListener::stopCapture()
{
    //if (!m_thread.joinable() || m_regime != ListenerRegime::Capture)
    //    return;

    if (m_regime != ListenerRegime::Capture)
    {
        return;
    }

    //m_bStopReceive.store(true, std::memory_order_release);
    profiler::net::Message request(profiler::net::MessageType::Request_Stop_Capture);
    m_easySocket.send(&request, sizeof(request));

    //m_thread.join();

    if (m_easySocket.isDisconnected())
    {
        m_bConnected.store(false, std::memory_order_release);
        m_bStopReceive.store(false, std::memory_order_release);
        m_regime = ListenerRegime::Idle;
        m_bCaptureReady.store(true, std::memory_order_release);
        return;
    }

    m_regime = ListenerRegime::Capture_Receive;
    if (m_thread.joinable())
    {
        m_bInterrupt.store(true, std::memory_order_release);
        m_thread.join();
        m_bInterrupt.store(false, std::memory_order_release);
    }

      m_thread = std::thread(&SocketListener::listenCapture, this);

    //m_regime = ListenerRegime::Idle;
    //m_bStopReceive.store(false, std::memory_order_release);
}

void SocketListener::finalizeCapture()
{
    if (m_thread.joinable())
    {
        m_bInterrupt.store(true, std::memory_order_release);
        m_thread.join();
        m_bInterrupt.store(false, std::memory_order_release);
    }

    m_regime = ListenerRegime::Idle;
    m_bCaptureReady.store(false, std::memory_order_release);
    m_bStopReceive.store(false, std::memory_order_release);
}

void SocketListener::requestBlocksDescription()
{
    if (m_thread.joinable())
    {
        m_bInterrupt.store(true, std::memory_order_release);
        m_thread.join();
        m_bInterrupt.store(false, std::memory_order_release);
    }

    clearData();

    profiler::net::Message request(profiler::net::MessageType::Request_Blocks_Description);
    m_easySocket.send(&request, sizeof(request));

    if (m_easySocket.isDisconnected())
    {
        m_bConnected.store(false, std::memory_order_release);
    }

    m_regime = ListenerRegime::Descriptors;
    listenDescription();
    m_regime = ListenerRegime::Idle;
}

bool SocketListener::frameTime(uint32_t& _maxTime, uint32_t& _avgTime)
{
    if (m_bFrameTimeReady.exchange(false, std::memory_order_acquire))
    {
        _maxTime = m_frameMax.load(std::memory_order_acquire);
        _avgTime = m_frameAvg.load(std::memory_order_acquire);
        return true;
    }

    return false;
}

bool SocketListener::requestFrameTime()
{
    if (m_regime != ListenerRegime::Idle && m_regime != ListenerRegime::Capture)
    {
        return false;
    }

    if (m_thread.joinable())
    {
        m_bInterrupt.store(true, std::memory_order_release);
        m_thread.join();
        m_bInterrupt.store(false, std::memory_order_release);
    }

    profiler::net::Message request(profiler::net::MessageType::Request_MainThread_FPS);
    m_easySocket.send(&request, sizeof(request));

    if (m_easySocket.isDisconnected())
    {
        m_bConnected.store(false, std::memory_order_release);
        return false;
    }

    m_bFrameTimeReady.store(false, std::memory_order_release);
    m_thread = std::thread(&SocketListener::listenFrameTime, this);

    return true;
}

//////////////////////////////////////////////////////////////////////////

void SocketListener::listenCapture()
{
    EASY_CONSTEXPR int buffer_size = 8 * 1024 * 1024;

    char* buffer = new char[buffer_size];
    int seek = 0, bytes = 0;
    auto timeBegin = std::chrono::system_clock::now();

    bool isListen = true, disconnected = false;
    while (isListen && !m_bInterrupt.load(std::memory_order_acquire))
    {
        if (m_bStopReceive.load(std::memory_order_acquire))
        {
            profiler::net::Message request(profiler::net::MessageType::Request_Stop_Capture);
            m_easySocket.send(&request, sizeof(request));
            m_bStopReceive.store(false, std::memory_order_release);
        }

        if (bytes < 1)
        {
            bytes = 0;
            seek = 0;
        }
        else if (seek > 0)
        {
            if (bytes < seek)
            {
                memcpy(buffer, buffer + seek, static_cast<size_t>(bytes));
            }
            else
            {
                memcpy(buffer, buffer + seek, static_cast<size_t>(seek));
                for (int i = seek; i < bytes; ++i)
                {
                    buffer[i] = buffer[seek + i];
                }
            }

            seek = 0;
        }

        while (bytes < sizeof(profiler::net::Message))
        {
            int receivedBytes = m_easySocket.receive(buffer + seek + bytes, buffer_size);
            if (receivedBytes < 1)
            {
                bytes = receivedBytes;
                break;
            }
            bytes += receivedBytes;
        }

        if (bytes == -1)
        {
            if (m_easySocket.isDisconnected())
            {
                m_bConnected.store(false, std::memory_order_release);
                isListen = false;
                disconnected = true;
            }

            bytes = 0;
            seek = 0;

            continue;
        }

        if (bytes == 0)
        {
            seek = 0;
            isListen = false;
            break;
        }

        auto message = reinterpret_cast<const profiler::net::Message*>(buffer + seek);
        while (!message->isEasyNetMessage())
        {
            seek += sizeof(uint32_t);
            bytes -= sizeof(uint32_t);

            if (seek >= buffer_size || bytes <= 0)
            {
                seek = 0;
                bytes = 0;
                message = nullptr;
                break;
            }

            if (bytes < sizeof(profiler::net::Message))
            {
                message = nullptr;
                break;
            }

            message = reinterpret_cast<const profiler::net::Message*>(buffer + seek);
        }

        if (bytes < 1 || message == nullptr)
        {
            continue;
        }

        switch (message->type)
        {
            case profiler::net::MessageType::Connection_Accepted:
            {
                qInfo() << "Receive MessageType::Connection_Accepted";
                //m_easySocket.send(&request, sizeof(request));
                seek += sizeof(profiler::net::Message);
                bytes -= sizeof(profiler::net::Message);
                break;
            }

            case profiler::net::MessageType::Reply_Capturing_Started:
            {
                qInfo() << "Receive MessageType::Reply_Capturing_Started";
                seek += sizeof(profiler::net::Message);
                bytes -= sizeof(profiler::net::Message);
                break;
            }

            case profiler::net::MessageType::Reply_Blocks_End:
            {
                qInfo() << "Receive MessageType::Reply_Blocks_End";
                seek += sizeof(profiler::net::Message);
                bytes -= sizeof(profiler::net::Message);

                const auto dt = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - timeBegin);
                const auto bytesNumber = m_receivedData.str().size();
                qInfo() << "received " << bytesNumber << " bytes, " << dt.count() << " ms, average speed = "
                        << double(bytesNumber) * 1e3 / double(dt.count()) / 1024. << " kBytes/sec";

                isListen = false;

                break;
            }

            case profiler::net::MessageType::Reply_Blocks:
            {
                qInfo() << "Receive MessageType::Reply_Blocks";

                while (bytes < sizeof(profiler::net::DataMessage))
                {
                    int receivedBytes = m_easySocket.receive(buffer + seek + bytes, buffer_size);
                    if (receivedBytes < 1)
                    {
                        bytes = receivedBytes;
                        break;
                    }
                    bytes += receivedBytes;
                }

                if (bytes == -1)
                {
                    if (m_easySocket.isDisconnected())
                    {
                        m_bConnected.store(false, std::memory_order_release);
                        isListen = false;
                        disconnected = true;
                    }

                    bytes = 0;
                    seek = 0;

                    continue;
                }

                if (bytes == 0)
                {
                    seek = 0;
                    isListen = false;
                    continue;
                }

                seek += sizeof(profiler::net::DataMessage);
                bytes -= sizeof(profiler::net::DataMessage);
                auto dm = reinterpret_cast<const profiler::net::DataMessage*>(message);

                timeBegin = std::chrono::system_clock::now();

                int neededSize = dm->size;
                const int bytesNumber = std::min(neededSize, bytes);
                if (bytesNumber > 0)
                {
                    char* buf = buffer + seek;
                    m_receivedSize += bytesNumber;
                    m_receivedData.write(buf, bytesNumber);

                    neededSize -= bytesNumber;
                    bytes -= bytesNumber;
                    seek += bytesNumber;
                }

                if (bytes < 1)
                {
                    // this is possible only when (neededSize - bytesNumber) >= 0
                    seek = 0;
                }

                while (neededSize > 0)
                {
                    // We can get here only when neededSize > (bytes - seek). Therefore bytes == 0 and seek = 0 here.

                    bytes = m_easySocket.receive(buffer, buffer_size);
                    if (bytes == -1)
                    {
                        break;
                    }

                    const int toWrite = std::min(bytes, neededSize);
                    m_receivedSize += toWrite;
                    m_receivedData.write(buffer, toWrite);

                    neededSize -= toWrite;
                    bytes -= toWrite;

                    if (bytes < 1)
                    {
                        seek = 0;
                    }
                    else
                    {
                        seek = toWrite;
                    }
                }

                if (bytes == -1)
                {
                    if (m_easySocket.isDisconnected())
                    {
                        m_bConnected.store(false, std::memory_order_release);
                        isListen = false;
                        disconnected = true;
                    }

                    bytes = 0;
                    seek = 0;

                    continue;
                }

                if (m_bStopReceive.load(std::memory_order_acquire))
                {
                    profiler::net::Message request(profiler::net::MessageType::Request_Stop_Capture);
                    m_easySocket.send(&request, sizeof(request));
                    m_bStopReceive.store(false, std::memory_order_release);
                }

                break;
            }

            default:
            {
                //qInfo() << "Receive unknown " << message->type;
                break;
            }
        }
    }

    if (disconnected)
    {
        clearData();
    }

    delete [] buffer;

    m_bCaptureReady.store(true, std::memory_order_release);
}

void SocketListener::listenDescription()
{
    EASY_CONSTEXPR int buffer_size = 8 * 1024 * 1024;

    char* buffer = new char[buffer_size];
    int seek = 0, bytes = 0;

    bool isListen = true, disconnected = false;
    while (isListen && !m_bInterrupt.load(std::memory_order_acquire))
    {
        if (bytes < 1)
        {
            bytes = 0;
            seek = 0;
        }
        else if (seek > 0)
        {
            if (bytes < seek)
            {
                memcpy(buffer, buffer + seek, static_cast<size_t>(bytes));
            }
            else
            {
                memcpy(buffer, buffer + seek, static_cast<size_t>(seek));
                for (int i = seek; i < bytes; ++i)
                {
                    buffer[i] = buffer[seek + i];
                }
            }

            seek = 0;
        }

        while (bytes < sizeof(profiler::net::Message))
        {
            int receivedBytes = m_easySocket.receive(buffer + seek + bytes, buffer_size);
            if (receivedBytes < 1)
            {
                bytes = receivedBytes;
                break;
            }
            bytes += receivedBytes;
        }

        if (bytes == -1)
        {
            if (m_easySocket.isDisconnected())
            {
                m_bConnected.store(false, std::memory_order_release);
                isListen = false;
                disconnected = true;
            }

            bytes = 0;
            seek = 0;

            continue;
        }

        if (bytes == 0)
        {
            seek = 0;
            isListen = false;
            break;
        }

        auto message = reinterpret_cast<const profiler::net::Message*>(buffer + seek);
        while (!message->isEasyNetMessage())
        {
            seek += sizeof(uint32_t);
            bytes -= sizeof(uint32_t);

            if (seek >= buffer_size || bytes <= 0)
            {
                seek = 0;
                bytes = 0;
                message = nullptr;
                break;
            }

            if (bytes < sizeof(profiler::net::Message))
            {
                message = nullptr;
                break;
            }

            message = reinterpret_cast<const profiler::net::Message*>(buffer + seek);
        }

        if (bytes < 1 || message == nullptr)
        {
            continue;
        }

        switch (message->type)
        {
            case profiler::net::MessageType::Connection_Accepted:
            {
                qInfo() << "Receive MessageType::Connection_Accepted";
                seek += sizeof(profiler::net::Message);
                bytes -= sizeof(profiler::net::Message);
                break;
            }

            case profiler::net::MessageType::Reply_Blocks_Description_End:
            {
                qInfo() << "Receive MessageType::Reply_Blocks_Description_End";
                seek += sizeof(profiler::net::Message);
                bytes -= sizeof(profiler::net::Message);

                isListen = false;

                break;
            }

            case profiler::net::MessageType::Reply_Blocks_Description:
            {
                qInfo() << "Receive MessageType::Reply_Blocks_Description";

                while (bytes < sizeof(profiler::net::DataMessage))
                {
                    int receivedBytes = m_easySocket.receive(buffer + seek + bytes, buffer_size);
                    if (receivedBytes < 1)
                    {
                        bytes = receivedBytes;
                        break;
                    }
                    bytes += receivedBytes;
                }

                if (bytes == -1)
                {
                    if (m_easySocket.isDisconnected())
                    {
                        m_bConnected.store(false, std::memory_order_release);
                        isListen = false;
                        disconnected = true;
                    }

                    bytes = 0;
                    seek = 0;

                    continue;
                }

                if (bytes == 0)
                {
                    seek = 0;
                    isListen = false;
                    continue;
                }

                seek += sizeof(profiler::net::DataMessage);
                bytes -= sizeof(profiler::net::DataMessage);
                auto dm = reinterpret_cast<const profiler::net::DataMessage*>(message);

                int neededSize = dm->size;
                const int bytesNumber = std::min(neededSize, bytes);
                if (bytesNumber > 0)
                {
                    char* buf = buffer + seek;
                    m_receivedSize += bytesNumber;
                    m_receivedData.write(buf, bytesNumber);

                    neededSize -= bytesNumber;
                    bytes -= bytesNumber;
                    seek += bytesNumber;
                }

                if (bytes < 1)
                {
                    // this is possible only when (neededSize - bytesNumber) >= 0
                    seek = 0;
                }

                while (neededSize > 0)
                {
                    // We can get here only when neededSize > (bytes - seek). Therefore bytes == 0 and seek = 0 here.

                    bytes = m_easySocket.receive(buffer, buffer_size);
                    if (bytes == -1)
                    {
                        break;
                    }

                    const int toWrite = std::min(bytes, neededSize);
                    m_receivedSize += toWrite;
                    m_receivedData.write(buffer, toWrite);

                    neededSize -= toWrite;
                    bytes -= toWrite;

                    if (bytes < 1)
                    {
                        seek = 0;
                    }
                    else
                    {
                        seek = toWrite;
                    }
                }

                if (bytes == -1)
                {
                    if (m_easySocket.isDisconnected())
                    {
                        m_bConnected.store(false, std::memory_order_release);
                        isListen = false;
                        disconnected = true;
                    }

                    bytes = 0;
                    seek = 0;

                    continue;
                }

                break;
            }

            default:
            {
                break;
            }
        }
    }

    if (disconnected)
    {
        clearData();
    }

    delete[] buffer;
}

void SocketListener::listenFrameTime()
{
    EASY_CONSTEXPR size_t buffer_size = sizeof(profiler::net::TimestampMessage) << 3;

    char buffer[buffer_size] = {};
    int seek = 0, bytes = 0;

    bool isListen = true;
    while (isListen && !m_bInterrupt.load(std::memory_order_acquire))
    {
        if (bytes < 1)
        {
            bytes = 0;
            seek = 0;
        }
        else if (seek > 0)
        {
            if (bytes < seek)
            {
                memcpy(buffer, buffer + seek, static_cast<size_t>(bytes));
            }
            else
            {
                memcpy(buffer, buffer + seek, static_cast<size_t>(seek));
                for (int i = seek; i < bytes; ++i)
                {
                    buffer[i] = buffer[seek + i];
                }
            }

            seek = 0;
        }

        while (bytes < sizeof(profiler::net::Message))
        {
            int receivedBytes = m_easySocket.receive(buffer + seek + bytes, buffer_size);
            if (receivedBytes < 1)
            {
                bytes = receivedBytes;
                break;
            }
            bytes += receivedBytes;
        }

        if (bytes == -1)
        {
            if (m_easySocket.isDisconnected())
            {
                m_bConnected.store(false, std::memory_order_release);
                isListen = false;
            }

            bytes = 0;
            seek = 0;

            continue;
        }

        if (bytes == 0)
        {
            seek = 0;
            isListen = false;
            break;
        }

        auto message = reinterpret_cast<const profiler::net::Message*>(buffer + seek);
        while (!message->isEasyNetMessage())
        {
            seek += sizeof(uint32_t);
            bytes -= sizeof(uint32_t);

            if (seek >= buffer_size || bytes <= 0)
            {
                seek = 0;
                bytes = 0;
                message = nullptr;
                break;
            }

            if (bytes < sizeof(profiler::net::Message))
            {
                message = nullptr;
                break;
            }

            message = reinterpret_cast<const profiler::net::Message*>(buffer + seek);
        }

        if (bytes < 1 || message == nullptr)
        {
            continue;
        }

        switch (message->type)
        {
            case profiler::net::MessageType::Connection_Accepted:
            case profiler::net::MessageType::Reply_Capturing_Started:
            {
                seek += sizeof(profiler::net::Message);
                bytes -= sizeof(profiler::net::Message);
                break;
            }

            case profiler::net::MessageType::Reply_MainThread_FPS:
            {
                //qInfo() << "Receive MessageType::Reply_MainThread_FPS";

                seek += sizeof(profiler::net::TimestampMessage);
                bytes -= sizeof(profiler::net::TimestampMessage);

                if (seek <= buffer_size)
                {
                    auto timestampMessage = reinterpret_cast<const profiler::net::TimestampMessage*>(message);
                    m_frameMax.store(timestampMessage->maxValue, std::memory_order_release);
                    m_frameAvg.store(timestampMessage->avgValue, std::memory_order_release);
                    m_bFrameTimeReady.store(true, std::memory_order_release);
                }

                isListen = false;
                break;
            }

            default:
            {
                break;
            }
        }
    }
}
