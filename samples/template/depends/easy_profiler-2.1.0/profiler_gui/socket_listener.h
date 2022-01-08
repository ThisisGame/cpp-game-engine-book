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

#ifndef EASY_PROFILER_SOCKET_LISTENER_H
#define EASY_PROFILER_SOCKET_LISTENER_H

#include <atomic>
#include <sstream>
#include <string>
#include <thread>

#include <QObject>

#include <easy/easy_socket.h>

namespace profiler { namespace net { struct EasyProfilerStatus; } }

//////////////////////////////////////////////////////////////////////////

enum class ListenerRegime : uint8_t
{
    Idle = 0,
    Capture,
    Capture_Receive,
    Descriptors
};

class SocketListener Q_DECL_FINAL
{
    EasySocket            m_easySocket; ///<
    std::string              m_address; ///<
    std::stringstream   m_receivedData; ///<
    std::thread               m_thread; ///<
    uint64_t            m_receivedSize; ///<
    uint16_t                    m_port; ///<
    std::atomic<uint32_t>   m_frameMax; ///<
    std::atomic<uint32_t>   m_frameAvg; ///<
    std::atomic_bool      m_bInterrupt; ///<
    std::atomic_bool      m_bConnected; ///<
    std::atomic_bool    m_bStopReceive; ///<
    std::atomic_bool   m_bCaptureReady; ///<
    std::atomic_bool m_bFrameTimeReady; ///<
    ListenerRegime            m_regime; ///<

public:

    SocketListener();
    ~SocketListener();

    bool connected() const;
    bool captured() const;
    ListenerRegime regime() const;
    uint64_t size() const;
    const std::string& address() const;
    uint16_t port() const;

    std::stringstream& data();
    void clearData();

    void disconnect();
    void closeSocket();
    bool connect(const char* _ipaddress, uint16_t _port, profiler::net::EasyProfilerStatus& _reply, bool _disconnectFirst = false);
    bool reconnect(const char* _ipaddress, uint16_t _port, profiler::net::EasyProfilerStatus& _reply);

    bool startCapture();
    void stopCapture();
    void finalizeCapture();
    void requestBlocksDescription();

    bool frameTime(uint32_t& _maxTime, uint32_t& _avgTime);
    bool requestFrameTime();

    template <class T>
    void send(const T& _message) {
        m_easySocket.send(&_message, sizeof(T));
    }

private:

    void listenCapture();
    void listenDescription();
    void listenFrameTime();

}; // END of class SocketListener.

#endif //EASY_PROFILER_SOCKET_LISTENER_H
