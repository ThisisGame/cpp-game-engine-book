/************************************************************************
* file name         : event_trace_win.h
* ----------------- :
* creation time     : 2016/09/04
* author            : Victor Zarubkin
* email             : v.s.zarubkin@gmail.com
* ----------------- :
* description       : The file contains declaration of EasyEventTracer class used for tracing
*                   : Windows system events to get context switches.
* ----------------- :
* change log        : * 2016/09/04 Victor Zarubkin: initial commit.
*                   :
*                   : *
* ----------------- :
* license           : Lightweight profiler library for c++
*                   : Copyright(C) 2016-2019  Sergey Yagovtsev, Victor Zarubkin
*                   :
*                   : Licensed under either of
*                   :     * MIT license (LICENSE.MIT or http://opensource.org/licenses/MIT)
*                   :     * Apache License, Version 2.0, (LICENSE.APACHE or http://www.apache.org/licenses/LICENSE-2.0)
*                   : at your option.
*                   :
*                   : The MIT License
*                   :
*                   : Permission is hereby granted, free of charge, to any person obtaining a copy
*                   : of this software and associated documentation files (the "Software"), to deal
*                   : in the Software without restriction, including without limitation the rights 
*                   : to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies 
*                   : of the Software, and to permit persons to whom the Software is furnished 
*                   : to do so, subject to the following conditions:
*                   : 
*                   : The above copyright notice and this permission notice shall be included in all 
*                   : copies or substantial portions of the Software.
*                   : 
*                   : THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
*                   : INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR 
*                   : PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE 
*                   : LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, 
*                   : TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE 
*                   : USE OR OTHER DEALINGS IN THE SOFTWARE.
*                   : 
*                   : The Apache License, Version 2.0 (the "License")
*                   :
*                   : You may not use this file except in compliance with the License.
*                   : You may obtain a copy of the License at
*                   :
*                   : http://www.apache.org/licenses/LICENSE-2.0
*                   :
*                   : Unless required by applicable law or agreed to in writing, software
*                   : distributed under the License is distributed on an "AS IS" BASIS,
*                   : WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*                   : See the License for the specific language governing permissions and
*                   : limitations under the License.
************************************************************************/

#ifndef EASY_PROFILER_EVENT_TRACE_WINDOWS_H
#define EASY_PROFILER_EVENT_TRACE_WINDOWS_H
#ifdef _WIN32

#define INITGUID  // This is to enable using SystemTraceControlGuid in evntrace.h.
#include <Windows.h>
#include <Strsafe.h>
#include <wmistr.h>
#include <evntrace.h>
#include <evntcons.h>
#include <thread>
#include <atomic>
#include "event_trace_status.h"
#include "spin_lock.h"

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

class EasyEventTracer EASY_FINAL
{
#ifndef EASY_MAGIC_STATIC_AVAILABLE
    friend class EasyEventTracerInstance;
#endif

#pragma pack(push, 1)
    struct Properties {
        Properties();

        EVENT_TRACE_PROPERTIES base;
        char sessionName[sizeof(KERNEL_LOGGER_NAME)];
    };
#pragma pack(pop)

    std::thread       m_processThread;
    Properties             m_properties;
    EVENT_TRACE_LOGFILE         m_trace;
    profiler::spin_lock        m_spin;
    std::atomic_bool    m_lowPriority;
    TRACEHANDLE         m_sessionHandle = INVALID_PROCESSTRACE_HANDLE;
    TRACEHANDLE          m_openedHandle = INVALID_PROCESSTRACE_HANDLE;
    bool                     m_bEnabled = false;

public:

    static EasyEventTracer& instance();
    ~EasyEventTracer();

    bool isLowPriority() const;

    EventTracingEnableStatus enable(bool _force = false);
    void disable();
    void setLowPriority(bool _value);
    static void setProcessPrivileges();

private:

    EasyEventTracer();

    inline EVENT_TRACE_PROPERTIES* props()
    {
        return reinterpret_cast<EVENT_TRACE_PROPERTIES*>(&m_properties);
    }

    EventTracingEnableStatus startTrace(bool _force, int _step = 0);

}; // END of class EasyEventTracer.

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

#endif // _WIN32
#endif // EASY_PROFILER_EVENT_TRACE_WINDOWS_H
