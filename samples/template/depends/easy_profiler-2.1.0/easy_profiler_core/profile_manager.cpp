/************************************************************************
* file name         : profile_manager.cpp
* ----------------- :
* creation time     : 2016/02/16
* authors           : Sergey Yagovtsev, Victor Zarubkin
* emails            : yse.sey@gmail.com, v.s.zarubkin@gmail.com
* ----------------- :
* description       : The file contains implementation of Profile manager and implement access c-function
*                   :
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

#include <algorithm>
#include <future>
#include <fstream>
#include <ostream>
#include <sstream>
#include "profile_manager.h"

#include <easy/profiler.h>
#include <easy/arbitrary_value.h>
#include <easy/easy_net.h>

#ifndef _WIN32
# include <easy/easy_socket.h>
#else
# include "event_trace_win.h"
#endif

#include "block_descriptor.h"
#include "current_time.h"
#include "current_thread.h"

#ifdef __APPLE__
# include <mach/clock.h>
# include <mach/mach.h>
#endif

#if EASY_OPTION_LOG_ENABLED != 0
# include <iostream>

# ifndef EASY_ERRORLOG
#  define EASY_ERRORLOG std::cerr
# endif

# ifndef EASY_LOG
#  define EASY_LOG std::cerr
# endif

# ifndef EASY_ERROR
#  define EASY_ERROR(LOG_MSG) EASY_ERRORLOG << "EasyProfiler ERROR: " << LOG_MSG
# endif

# ifndef EASY_WARNING
#  define EASY_WARNING(LOG_MSG) EASY_ERRORLOG << "EasyProfiler WARNING: " << LOG_MSG
# endif

# ifndef EASY_LOGMSG
#  define EASY_LOGMSG(LOG_MSG) EASY_LOG << "EasyProfiler INFO: " << LOG_MSG
# endif

# ifndef EASY_LOG_ONLY
#  define EASY_LOG_ONLY(CODE) CODE
# endif

#else // EASY_OPTION_LOG_ENABLED == 0

# ifndef EASY_ERROR
#  define EASY_ERROR(LOG_MSG)
# endif

# ifndef EASY_WARNING
#  define EASY_WARNING(LOG_MSG)
# endif

# ifndef EASY_LOGMSG
#  define EASY_LOGMSG(LOG_MSG)
# endif

# ifndef EASY_LOG_ONLY
#  define EASY_LOG_ONLY(CODE)
# endif

#endif // EASY_OPTION_LOG_ENABLED

#ifdef min
# undef min
#endif

#ifndef EASY_ENABLE_BLOCK_STATUS
# define EASY_ENABLE_BLOCK_STATUS 1
#endif

#if !defined(_WIN32) && !defined(EASY_OPTION_REMOVE_EMPTY_UNGUARDED_THREADS)
# define EASY_OPTION_REMOVE_EMPTY_UNGUARDED_THREADS 0
#endif

#ifndef EASY_OPTION_IMPLICIT_THREAD_REGISTRATION
# define EASY_OPTION_IMPLICIT_THREAD_REGISTRATION 0
#endif

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

extern const uint32_t EASY_PROFILER_SIGNATURE;
extern const uint32_t EASY_PROFILER_VERSION;

//////////////////////////////////////////////////////////////////////////

extern const profiler::color_t EASY_COLOR_INTERNAL_EVENT = 0xffffffff; // profiler::colors::White
EASY_CONSTEXPR profiler::color_t EASY_COLOR_THREAD_END = 0xff212121; // profiler::colors::Dark
EASY_CONSTEXPR profiler::color_t EASY_COLOR_START = 0xff4caf50; // profiler::colors::Green
EASY_CONSTEXPR profiler::color_t EASY_COLOR_END = 0xfff44336; // profiler::colors::Red

//////////////////////////////////////////////////////////////////////////

EASY_CONSTEXPR uint8_t FORCE_ON_FLAG = profiler::FORCE_ON & ~profiler::ON;

//////////////////////////////////////////////////////////////////////////

static EASY_THREAD_LOCAL ::ThreadStorage* THIS_THREAD = nullptr;
static EASY_THREAD_LOCAL bool THIS_THREAD_IS_MAIN = false;

static EASY_THREAD_LOCAL profiler::timestamp_t THIS_THREAD_FRAME_T_MAX = 0ULL;
static EASY_THREAD_LOCAL profiler::timestamp_t THIS_THREAD_FRAME_T_CUR = 0ULL;
static EASY_THREAD_LOCAL profiler::timestamp_t THIS_THREAD_FRAME_T_ACC = 0ULL;
static EASY_THREAD_LOCAL uint32_t THIS_THREAD_N_FRAMES = 0;
static EASY_THREAD_LOCAL bool THIS_THREAD_FRAME_T_RESET_MAX = false;
static EASY_THREAD_LOCAL bool THIS_THREAD_FRAME_T_RESET_AVG = false;

#ifdef EASY_CXX11_TLS_AVAILABLE
thread_local static profiler::ThreadGuard THIS_THREAD_GUARD; // thread guard for monitoring thread life time
#endif

//////////////////////////////////////////////////////////////////////////

#ifdef BUILD_WITH_EASY_PROFILER
# define EASY_EVENT_RES(res, name, ...)\
    EASY_LOCAL_STATIC_PTR(const profiler::BaseBlockDescriptor*, EASY_UNIQUE_DESC(__LINE__),\
        ProfileManager::instance().addBlockDescriptor(profiler::extract_enable_flag(__VA_ARGS__),\
            EASY_UNIQUE_LINE_ID, EASY_COMPILETIME_NAME(name),\
            __FILE__, __LINE__, profiler::BlockType::Event, profiler::extract_color(__VA_ARGS__)));\
    res = ProfileManager::instance().storeBlock(EASY_UNIQUE_DESC(__LINE__), EASY_RUNTIME_NAME(name))

# define EASY_FORCE_EVENT(timestamp, name, ...)\
    EASY_LOCAL_STATIC_PTR(const profiler::BaseBlockDescriptor*, EASY_UNIQUE_DESC(__LINE__), addBlockDescriptor(\
        profiler::extract_enable_flag(__VA_ARGS__), EASY_UNIQUE_LINE_ID, EASY_COMPILETIME_NAME(name),\
            __FILE__, __LINE__, profiler::BlockType::Event, profiler::extract_color(__VA_ARGS__)));\
    storeBlockForce(EASY_UNIQUE_DESC(__LINE__), EASY_RUNTIME_NAME(name), timestamp)

# define EASY_FORCE_EVENT2(timestamp, name, ...)\
    EASY_LOCAL_STATIC_PTR(const profiler::BaseBlockDescriptor*, EASY_UNIQUE_DESC(__LINE__), addBlockDescriptor(\
        profiler::extract_enable_flag(__VA_ARGS__), EASY_UNIQUE_LINE_ID, EASY_COMPILETIME_NAME(name),\
            __FILE__, __LINE__, profiler::BlockType::Event, profiler::extract_color(__VA_ARGS__)));\
    storeBlockForce2(EASY_UNIQUE_DESC(__LINE__), EASY_RUNTIME_NAME(name), timestamp)

# define EASY_FORCE_EVENT3(ts, timestamp, name, ...)\
    EASY_LOCAL_STATIC_PTR(const profiler::BaseBlockDescriptor*, EASY_UNIQUE_DESC(__LINE__), addBlockDescriptor(\
        profiler::extract_enable_flag(__VA_ARGS__), EASY_UNIQUE_LINE_ID, EASY_COMPILETIME_NAME(name),\
            __FILE__, __LINE__, profiler::BlockType::Event, profiler::extract_color(__VA_ARGS__)));\
    ts.storeBlockForce(profiler::Block(timestamp, timestamp, EASY_UNIQUE_DESC(__LINE__)->id(), EASY_RUNTIME_NAME(name)))
#else
# ifndef EASY_PROFILER_API_DISABLED
#  define EASY_PROFILER_API_DISABLED
# endif
# define EASY_EVENT_RES(res, name, ...)
# define EASY_FORCE_EVENT(timestamp, name, ...)
# define EASY_FORCE_EVENT2(timestamp, name, ...)
# define EASY_FORCE_EVENT3(ts, timestamp, name, ...)
#endif

//////////////////////////////////////////////////////////////////////////

template <typename T>
static void write(std::ostream& _outstream, const char* _data, T _size)
{
    _outstream.write(_data, _size);
}

template <class T>
static void write(std::ostream& _outstream, const T& _data)
{
    _outstream.write((const char*)&_data, sizeof(T));
}

static void clear_sstream(std::stringstream& _outstream)
{
#if defined(__GNUC__) && __GNUC__ < 5
    // gcc 4 has a known bug which has been solved in gcc 5:
    // std::stringstream has no swap() method :(
    _outstream.str(std::string());
#else
    std::stringstream().swap(_outstream);
#endif
}

//////////////////////////////////////////////////////////////////////////

profiler::ThreadGuard::~ThreadGuard()
{
#ifndef EASY_PROFILER_API_DISABLED
    if (m_id != 0 && THIS_THREAD != nullptr && THIS_THREAD->id == m_id)
    {
        bool isMarked = false;
        EASY_EVENT_RES(isMarked, "ThreadFinished", EASY_COLOR_THREAD_END, profiler::FORCE_ON);
        //THIS_THREAD->markProfilingFrameEnded();
        THIS_THREAD->putMark();
        THIS_THREAD->expired.store(isMarked ? 2 : 1, std::memory_order_release);
        THIS_THREAD = nullptr;
    }
#endif
}

//////////////////////////////////////////////////////////////////////////

#if defined(EASY_CHRONO_CLOCK)
static EASY_CONSTEXPR_FCN int64_t calculate_cpu_frequency()
{
    return EASY_CHRONO_CLOCK::period::den / EASY_CHRONO_CLOCK::period::num;
}
#elif defined(_WIN32)
static int64_t calculate_cpu_frequency()
{
    LARGE_INTEGER freq;
    QueryPerformanceFrequency(&freq);
    return static_cast<int64_t>(freq.QuadPart);
}
#else
# ifndef __APPLE__
#  include <time.h>
# endif
static int64_t calculate_cpu_frequency()
{
    EASY_CONSTEXPR int NANO_SECONDS_IN_SEC = 1000000000;

    uint64_t begin = 0;
    uint64_t end = 0;

# ifdef __APPLE__
    clock_serv_t cclock;
    mach_timespec_t begints, endts;
    host_get_clock_service(mach_host_self(), SYSTEM_CLOCK, &cclock);
    clock_get_time(cclock, &begints);
# else
    struct timespec begints, endts;
    clock_gettime(CLOCK_MONOTONIC, &begints);
# endif

    begin = profiler::clock::now();
    volatile uint64_t i;
    for (i = 0; i < 100000000; i++); /* must be CPU intensive */
    end = profiler::clock::now();

# ifdef __APPLE__
    clock_get_time(cclock, &endts);
    mach_port_deallocate(mach_task_self(), cclock);
# else
    clock_gettime(CLOCK_MONOTONIC, &endts);
# endif

    struct timespec tmpts;
    tmpts.tv_sec = endts.tv_sec - begints.tv_sec;
    tmpts.tv_nsec = endts.tv_nsec - begints.tv_nsec;

    if (tmpts.tv_nsec < 0)
    {
        tmpts.tv_sec--;
        tmpts.tv_nsec += NANO_SECONDS_IN_SEC;
    }

    const uint64_t nsecElapsed = tmpts.tv_sec * 1000000000LL + tmpts.tv_nsec;
    const double ticksPerNanoSec = static_cast<double>(end - begin) / static_cast<double>(nsecElapsed);

    return int64_t(ticksPerNanoSec * 1000000);
}
#endif

//////////////////////////////////////////////////////////////////////////

ProfileManager::ProfileManager() :

#ifdef _WIN32
    m_processId(GetProcessId(GetCurrentProcess()))
#else
    m_processId((processid_t)getpid())
#endif

#if defined(EASY_CHRONO_CLOCK) || defined(_WIN32)
    , m_cpuFrequency(calculate_cpu_frequency())
#endif

    , m_descriptorsMemorySize(0)
    , m_beginTime(0)
    , m_endTime(0)
{
    m_profilerStatus = false;
    m_isEventTracingEnabled = EASY_OPTION_EVENT_TRACING_ENABLED;
    m_isAlreadyListening = false;
    m_stopDumping = false;
    m_stopListen = false;

    m_mainThreadId = 0;
    m_frameMax = 0;
    m_frameAvg = 0;
    m_frameCur = 0;
    m_frameMaxReset = false;
    m_frameAvgReset = false;

#if !defined(EASY_CHRONO_CLOCK) && !defined(_WIN32)
    m_cpuFrequency = 1;

# if !defined(EASY_PROFILER_API_DISABLED)
    const auto cpu_frequency = calculate_cpu_frequency();
    m_cpuFrequency.store(cpu_frequency, std::memory_order_release);
# endif

#endif

#if !defined(EASY_PROFILER_API_DISABLED) && EASY_OPTION_START_LISTEN_ON_STARTUP != 0
    startListen(profiler::DEFAULT_PORT);
#endif
}

ProfileManager::~ProfileManager()
{
#ifndef EASY_PROFILER_API_DISABLED
    stopListen();
#endif

    for (auto desc : m_descriptors)
        BlockDescriptor::destroy(desc);
}

#ifndef EASY_MAGIC_STATIC_AVAILABLE
class ProfileManagerInstance {
    friend ProfileManager;
    ProfileManager instance;
} PROFILE_MANAGER;
#endif

//////////////////////////////////////////////////////////////////////////

ProfileManager& ProfileManager::instance()
{
#ifndef EASY_MAGIC_STATIC_AVAILABLE
    return PROFILE_MANAGER.instance;
#else
    ///C++11 makes possible to create Singleton without any warry about thread-safeness
    ///http://preshing.com/20130930/double-checked-locking-is-fixed-in-cpp11/
    static ProfileManager profileManager;
    return profileManager;
#endif
}

//////////////////////////////////////////////////////////////////////////

ThreadStorage& ProfileManager::_threadStorage(profiler::thread_id_t _thread_id)
{
    return m_threads[_thread_id];
}

ThreadStorage* ProfileManager::_findThreadStorage(profiler::thread_id_t _thread_id)
{
    auto it = m_threads.find(_thread_id);
    return it != m_threads.end() ? &it->second : nullptr;
}

//////////////////////////////////////////////////////////////////////////

const profiler::BaseBlockDescriptor* ProfileManager::addBlockDescriptor(profiler::EasyBlockStatus _defaultStatus
    , const char* _autogenUniqueId, const char* _name, const char* _filename, int _line
    , profiler::block_type_t _block_type, profiler::color_t _color, bool _copyName)
{
    guard_lock_t lock(m_storedSpin);

    const descriptors_map_t::key_type key(_autogenUniqueId);
    auto it = m_descriptorsMap.find(key);
    if (it != m_descriptorsMap.end())
        return m_descriptors[it->second];

    const auto nameLen = strlen(_name);
    m_descriptorsMemorySize += sizeof(profiler::SerializedBlockDescriptor) + nameLen + strlen(_filename) + 2;

#if EASY_BLOCK_DESC_FULL_COPY == 0
    BlockDescriptor* desc = nullptr;

    if (_copyName)
    {
        void* data = malloc(sizeof(BlockDescriptor) + nameLen + 1);
        char* name = reinterpret_cast<char*>(data) + sizeof(BlockDescriptor);
        strncpy(name, _name, nameLen);
        desc = ::new (data)BlockDescriptor(static_cast<profiler::block_id_t>(m_descriptors.size()),
                                           _defaultStatus, name, _filename, _line, _block_type, _color);
    }
    else
    {
        void* data = malloc(sizeof(BlockDescriptor));
        desc = ::new (data)BlockDescriptor(static_cast<profiler::block_id_t>(m_descriptors.size()),
                                           _defaultStatus, _name, _filename, _line, _block_type, _color);
    }
#else
    auto desc = new BlockDescriptor(static_cast<profiler::block_id_t>(m_descriptors.size()),
                                    _defaultStatus, _name, _filename, _line, _block_type, _color);
    (void)_copyName; // unused
#endif

    m_descriptors.emplace_back(desc);
    m_descriptorsMap.emplace(key, desc->id());

    return desc;
}

//////////////////////////////////////////////////////////////////////////

void ProfileManager::storeValue(const profiler::BaseBlockDescriptor* _desc, profiler::DataType _type, const void* _data,
                                uint16_t _size, bool _isArray, profiler::ValueId _vin)
{
    if (!isEnabled() || (_desc->m_status & profiler::ON) == 0)
        return;

    if (THIS_THREAD == nullptr)
        registerThread();

#if EASY_ENABLE_BLOCK_STATUS != 0
    if (THIS_THREAD->stackSize > 0 || (!THIS_THREAD->allowChildren && (_desc->m_status & FORCE_ON_FLAG) == 0))
        return;
#else
    if (THIS_THREAD->stackSize > 0)
        // Prevent from store values until frame, which has been opened when profiler was disabled, finish
        return;
#endif

    THIS_THREAD->storeValue(profiler::clock::now(), _desc->id(), _type, _data, _size, _isArray, _vin);
}

//////////////////////////////////////////////////////////////////////////

bool ProfileManager::storeBlock(const profiler::BaseBlockDescriptor* _desc, const char* _runtimeName)
{
    if (!isEnabled() || (_desc->m_status & profiler::ON) == 0)
        return false;

    if (THIS_THREAD == nullptr)
        registerThread();

#if EASY_ENABLE_BLOCK_STATUS != 0
    if (THIS_THREAD->stackSize > 0 || (!THIS_THREAD->allowChildren && (_desc->m_status & FORCE_ON_FLAG) == 0))
        return false;
#else
    if (THIS_THREAD->stackSize > 0)
        // Prevent from store block until frame, which has been opened when profiler was disabled, finish
        return false;
#endif

    const auto time = profiler::clock::now();
    THIS_THREAD->storeBlock(profiler::Block(time, time, _desc->id(), _runtimeName));
    THIS_THREAD->putMarkIfEmpty();

    return true;
}

bool ProfileManager::storeBlock(const profiler::BaseBlockDescriptor* _desc, const char* _runtimeName,
                                profiler::timestamp_t _beginTime, profiler::timestamp_t _endTime)
{
    if (!isEnabled() || (_desc->m_status & profiler::ON) == 0)
        return false;

    if (THIS_THREAD == nullptr)
        registerThread();

#if EASY_ENABLE_BLOCK_STATUS != 0
    if (THIS_THREAD->stackSize > 0 || (!THIS_THREAD->allowChildren && (_desc->m_status & FORCE_ON_FLAG) == 0))
        return false;
#else
    if (THIS_THREAD->stackSize > 0)
        // Prevent from store block until frame, which has been opened when profiler was disabled, finish
        return false;
#endif

    profiler::Block b(_beginTime, _endTime, _desc->id(), _runtimeName);
    THIS_THREAD->storeBlock(b);
    b.m_end = b.m_begin;

    THIS_THREAD->putMarkIfEmpty();

    return true;
}

//////////////////////////////////////////////////////////////////////////

void ProfileManager::storeBlockForce(const profiler::BaseBlockDescriptor* _desc, const char* _runtimeName,
                                     profiler::timestamp_t& _timestamp)
{
    if ((_desc->m_status & profiler::ON) == 0)
        return;

    if (THIS_THREAD == nullptr)
        registerThread();

#if EASY_ENABLE_BLOCK_STATUS != 0
    if (!THIS_THREAD->allowChildren && (_desc->m_status & FORCE_ON_FLAG) == 0)
        return;
#endif

    _timestamp = profiler::clock::now();
    THIS_THREAD->storeBlock(profiler::Block(_timestamp, _timestamp, _desc->id(), _runtimeName));
    THIS_THREAD->putMark();
}

void ProfileManager::storeBlockForce2(const profiler::BaseBlockDescriptor* _desc, const char* _runtimeName,
                                      profiler::timestamp_t _timestamp)
{
    if ((_desc->m_status & profiler::ON) == 0)
        return;

    if (THIS_THREAD == nullptr)
        registerThread();

#if EASY_ENABLE_BLOCK_STATUS != 0
    if (!THIS_THREAD->allowChildren && (_desc->m_status & FORCE_ON_FLAG) == 0)
        return;
#endif

    THIS_THREAD->storeBlock(profiler::Block(_timestamp, _timestamp, _desc->id(), _runtimeName));
    THIS_THREAD->putMark();
}

//////////////////////////////////////////////////////////////////////////

void ProfileManager::beginBlock(profiler::Block& _block)
{
    if (THIS_THREAD == nullptr)
        registerThread();

    if (++THIS_THREAD->stackSize > 1)
    {
        // _block is a sibling of current opened frame and this frame has been opened
        // before profiler was enabled. This _block should be ignored.
        _block.m_status = profiler::OFF;
        THIS_THREAD->blocks.openedList.emplace_back(_block);
        return;
    }

    if (!isEnabled())
    {
        // _block is a top-level block (a.k.a. frame).
        // It should be ignored because profiler is disabled.
        _block.m_status = profiler::OFF;
        THIS_THREAD->blocks.openedList.emplace_back(_block);
        beginFrame(); // FPS counter
        return;
    }

    // Profiler is enabled. Begin block.

    THIS_THREAD->stackSize = 0;

    const auto blockStatus = _block.m_status;
#if EASY_ENABLE_BLOCK_STATUS != 0
    if (THIS_THREAD->allowChildren)
    {
#endif
        if (blockStatus & profiler::ON)
            _block.start();
#if EASY_ENABLE_BLOCK_STATUS != 0
        THIS_THREAD->allowChildren = ((blockStatus & profiler::OFF_RECURSIVE) == 0);
    }
    else if (blockStatus & FORCE_ON_FLAG)
    {
        _block.start();
        _block.m_status = profiler::FORCE_ON_WITHOUT_CHILDREN;
    }
    else
    {
        _block.m_status = profiler::OFF_RECURSIVE;
    }
#endif

    if (THIS_THREAD->blocks.openedList.empty())
        beginFrame(); // FPS counter

    THIS_THREAD->blocks.openedList.emplace_back(_block);
}

void ProfileManager::beginNonScopedBlock(const profiler::BaseBlockDescriptor* _desc, const char* _runtimeName)
{
    if (THIS_THREAD == nullptr)
        registerThread();

    NonscopedBlock& b = THIS_THREAD->nonscopedBlocks.push(_desc, _runtimeName, false);
    beginBlock(b);
    b.copyname();
}

void ProfileManager::beginContextSwitch(profiler::thread_id_t _thread_id, profiler::timestamp_t _time,
                                        profiler::thread_id_t _target_thread_id, const char* _target_process,
                                        bool _lockSpin)
{
    auto ts = _lockSpin ? findThreadStorage(_thread_id) : _findThreadStorage(_thread_id);
    if (ts != nullptr)
        // Dirty hack: _target_thread_id will be written to the field "block_id_t m_id"
        // and will be available calling method id().
        ts->sync.openedList.emplace_back(_time, _target_thread_id, _target_process);
}

//////////////////////////////////////////////////////////////////////////

void ProfileManager::endBlock()
{
    if (--THIS_THREAD->stackSize > 0)
    {
        // Just pop child blocks from stack until frame, which
        // has been opened before profiler was enabled, finish.
        THIS_THREAD->popSilent();
        return;
    }

    THIS_THREAD->stackSize = 0;
    if (!isEnabled())
    {
        THIS_THREAD->popSilent();
        endFrame(); // FPS counter
        return;
    }

    auto& currentThreadStack = THIS_THREAD->blocks.openedList;
    if (currentThreadStack.empty())
        return;

    profiler::Block& top = currentThreadStack.back();
    if (top.m_status & profiler::ON)
    {
        if (!top.finished())
            top.finish();
        THIS_THREAD->storeBlock(top);
    }
    else
    {
        // This is to restrict endBlock() call inside ~Block()
        top.m_end = top.m_begin;
    }

    if (!top.m_isScoped)
        THIS_THREAD->nonscopedBlocks.pop();

    currentThreadStack.pop_back();
    if (currentThreadStack.empty())
    {
        THIS_THREAD->putMark();
        endFrame(); // FPS counter
#if EASY_ENABLE_BLOCK_STATUS != 0
        THIS_THREAD->allowChildren = true;
    }
    else
    {
        THIS_THREAD->allowChildren = ((currentThreadStack.back().get().m_status & profiler::OFF_RECURSIVE) == 0);
#endif
    }
}

void ProfileManager::endContextSwitch(profiler::thread_id_t _thread_id, processid_t _process_id,
                                      profiler::timestamp_t _endtime, bool _lockSpin)
{
    ThreadStorage* ts = nullptr;
    if (_process_id == m_processId)
    {
        // Implicit thread registration.
        // If thread owned by current process then create new ThreadStorage if there is no one
#if EASY_OPTION_IMPLICIT_THREAD_REGISTRATION != 0
        ts = _lockSpin ? &threadStorage(_thread_id) : &_threadStorage(_thread_id);
# if !defined(_WIN32) && !defined(EASY_CXX11_TLS_AVAILABLE)
#  if EASY_OPTION_REMOVE_EMPTY_UNGUARDED_THREADS != 0
#   pragma message "Warning: Implicit thread registration together with removing empty unguarded threads may cause application crash because there is no possibility to check thread state (dead or alive) for pthreads and removed ThreadStorage may be reused if thread is still alive."
#  else
#   pragma message "Warning: Implicit thread registration without removing empty unguarded threads may lead to memory leak because there is no possibility to check thread state (dead or alive) for pthreads."
#  endif
# endif
#endif
    }
    else
    {
        // If thread owned by another process OR _process_id IS UNKNOWN then do not create ThreadStorage for this
        ts = _lockSpin ? findThreadStorage(_thread_id) : _findThreadStorage(_thread_id);
    }

    if (ts == nullptr || ts->sync.openedList.empty())
        return;

    CSwitchBlock& lastBlock = ts->sync.openedList.back();
    lastBlock.m_end = _endtime;

    ts->storeCSwitch(lastBlock);
    ts->sync.openedList.pop_back();
}

//////////////////////////////////////////////////////////////////////////

void ProfileManager::beginFrame()
{
    THIS_THREAD->beginFrame();
}

void ProfileManager::endFrame()
{
    if (!THIS_THREAD->frameOpened)
        return;

    const profiler::timestamp_t duration = THIS_THREAD->endFrame();

    if (THIS_THREAD_FRAME_T_RESET_MAX)
        THIS_THREAD_FRAME_T_MAX = 0;
    THIS_THREAD_FRAME_T_RESET_MAX = false;

    THIS_THREAD_FRAME_T_CUR = duration;
    if (duration > THIS_THREAD_FRAME_T_MAX)
        THIS_THREAD_FRAME_T_MAX = duration;

    THIS_THREAD_FRAME_T_RESET_AVG = THIS_THREAD_FRAME_T_RESET_AVG || THIS_THREAD_N_FRAMES > 10000;

    if (THIS_THREAD_IS_MAIN)
    {
        if (m_frameAvgReset.exchange(false, std::memory_order_acq_rel) || THIS_THREAD_FRAME_T_RESET_AVG)
        {
            if (THIS_THREAD_N_FRAMES > 0)
                m_frameAvg.store(THIS_THREAD_FRAME_T_ACC / THIS_THREAD_N_FRAMES, std::memory_order_release);
            THIS_THREAD_FRAME_T_RESET_AVG = false;
            THIS_THREAD_FRAME_T_ACC = duration;
            THIS_THREAD_N_FRAMES = 1;
        }
        else
        {
            THIS_THREAD_FRAME_T_ACC += duration;
            ++THIS_THREAD_N_FRAMES;
            m_frameAvg.store(THIS_THREAD_FRAME_T_ACC / THIS_THREAD_N_FRAMES, std::memory_order_release);
        }

        const auto maxDuration = m_frameMax.load(std::memory_order_acquire);
        if (m_frameMaxReset.exchange(false, std::memory_order_acq_rel) || duration > maxDuration)
            m_frameMax.store(duration, std::memory_order_release);

        m_frameCur.store(duration, std::memory_order_release);

        return;
    }

    const auto reset = (uint32_t)!THIS_THREAD_FRAME_T_RESET_AVG;
    THIS_THREAD_FRAME_T_RESET_AVG = false;
    THIS_THREAD_N_FRAMES = 1 + reset * THIS_THREAD_N_FRAMES;
    THIS_THREAD_FRAME_T_ACC = duration + reset * THIS_THREAD_FRAME_T_ACC;
}

profiler::timestamp_t ProfileManager::maxFrameDuration()
{
    auto duration = m_frameMax.load(std::memory_order_acquire);
    m_frameMaxReset.store(true, std::memory_order_release);
    return duration;
}

profiler::timestamp_t ProfileManager::avgFrameDuration()
{
    auto duration = m_frameAvg.load(std::memory_order_acquire);
    m_frameAvgReset.store(true, std::memory_order_release);
    return duration;
}

profiler::timestamp_t ProfileManager::curFrameDuration() const
{
    return m_frameCur.load(std::memory_order_acquire);
}

//////////////////////////////////////////////////////////////////////////

void ProfileManager::enableEventTracer()
{
#ifdef _WIN32
    if (m_isEventTracingEnabled.load(std::memory_order_acquire))
        EasyEventTracer::instance().enable(true);
#endif
}

void ProfileManager::disableEventTracer()
{
#ifdef _WIN32
    EasyEventTracer::instance().disable();
#endif
}

void ProfileManager::setEnabled(bool isEnable)
{
    guard_lock_t lock(m_dumpSpin);

    auto time = profiler::clock::now();
    if (m_profilerStatus.exchange(isEnable, std::memory_order_acq_rel) == isEnable)
        return;

    if (isEnable)
    {
        EASY_LOGMSG("Enabled profiling\n");
        enableEventTracer();
        m_beginTime = time;
    }
    else
    {
        EASY_LOGMSG("Disabled profiling\n");
        disableEventTracer();
        m_endTime = time;
    }
}

void ProfileManager::setEventTracingEnabled(bool _isEnable)
{
    m_isEventTracingEnabled.store(_isEnable, std::memory_order_release);
}

bool ProfileManager::isEventTracingEnabled() const
{
    return m_isEventTracingEnabled.load(std::memory_order_acquire);
}

//////////////////////////////////////////////////////////////////////////

char ProfileManager::checkThreadExpired(ThreadStorage& _registeredThread)
{
    const char val = _registeredThread.expired.load(std::memory_order_acquire);
    if (val != 0)
        return val;

    if (_registeredThread.guarded)
        return 0;

#ifdef _WIN32
    // Check thread state for Windows

    DWORD exitCode = 0;
    auto hThread = OpenThread(THREAD_QUERY_LIMITED_INFORMATION, FALSE, (DWORD)_registeredThread.id);
    if (hThread == nullptr || GetExitCodeThread(hThread, &exitCode) == FALSE || exitCode != STILL_ACTIVE)
    {
        // Thread has been expired
        _registeredThread.expired.store(1, std::memory_order_release);
        if (hThread != nullptr)
            CloseHandle(hThread);
        return 1;
    }

    if (hThread != nullptr)
        CloseHandle(hThread);

    return 0;
#else
    // Check thread state for Linux and MacOS/iOS

    // This would drop the application if pthread already died
    //return pthread_kill(_registeredThread.pthread_id, 0) != 0 ? 1 : 0;

    // There is no function to check external pthread state in Linux! :((

#ifndef EASY_CXX11_TLS_AVAILABLE
# pragma message "Warning: Your compiler does not support thread_local C++11 feature. Please use EASY_THREAD_SCOPE as much as possible. Otherwise, there is a possibility of memory leak if there are a lot of rapidly created and destroyed threads."
#endif

    return 0;
#endif
}

//////////////////////////////////////////////////////////////////////////

uint32_t ProfileManager::dumpBlocksToStream(std::ostream& _outputStream, bool _lockSpin, bool _async)
{
    EASY_LOGMSG("dumpBlocksToStream(_lockSpin = " << _lockSpin << ")...\n");

    if (_lockSpin)
        m_dumpSpin.lock();

#ifndef _WIN32
    const bool eventTracingEnabled = m_isEventTracingEnabled.load(std::memory_order_acquire);
#endif

    if (isEnabled())
    {
        m_profilerStatus.store(false, std::memory_order_release);
        disableEventTracer();
        m_endTime = profiler::clock::now();
    }

    if (_async && m_stopDumping.load(std::memory_order_acquire))
    {
        if (_lockSpin)
            m_dumpSpin.unlock();
        return 0;
    }

    // Wait for some time to be sure that all operations which began before setEnabled(false) will be finished.
    // This is much better than inserting spin-lock or atomic variable store/load into each storeBlock operation.
    //
    // Note: this means - wait for all ThreadStorage::storeBlock() to finish.
    std::this_thread::sleep_for(std::chrono::milliseconds(20));

    // This is to make sure that no new descriptors or new threads will be
    // added until we finish sending data.
    m_spin.lock();
    m_storedSpin.lock();
    // This is the only place using both spins, so no dead-lock will occur
    // TODO: think about better solution because this one is not 100% safe...

    const auto time = profiler::clock::now();
    const auto endtime = m_endTime == 0 ? time : std::min(time, m_endTime);

#ifndef _WIN32
    if (eventTracingEnabled)
    {
        // Read thread context switch events from temporary file

        if (_async && m_stopDumping.load(std::memory_order_acquire))
        {
            m_spin.unlock();
            m_storedSpin.unlock();
            if (_lockSpin)
                m_dumpSpin.unlock();
            return 0;
        }

        EASY_LOGMSG("Writing context switch events...\n");

        uint64_t timestamp = 0;
        profiler::thread_id_t thread_from = 0, thread_to = 0;

        std::ifstream infile(m_csInfoFilename.c_str());
        if(infile.is_open())
        {
            EASY_LOG_ONLY(uint32_t num = 0);
            std::string next_task_name;
            pid_t process_to = 0;
            while (infile >> timestamp >> thread_from >> thread_to >> next_task_name >> process_to)
            {
                if (_async && m_stopDumping.load(std::memory_order_acquire))
                {
                    m_spin.unlock();
                    m_storedSpin.unlock();
                    if (_lockSpin)
                        m_dumpSpin.unlock();
                    return 0;
                }

                beginContextSwitch(thread_from, timestamp, thread_to, next_task_name.c_str(), false);
                endContextSwitch(thread_to, (processid_t)process_to, timestamp, false);
                EASY_LOG_ONLY(++num);
            }

            EASY_LOGMSG("Done, " << num << " context switch events wrote\n");
        }
        EASY_LOG_ONLY(
            else {
                EASY_ERROR("Can not open context switch log-file \"" << m_csInfoFilename << "\"\n");
            }
        )
    }
#endif

    bool mainThreadExpired = false;

    // Calculate used memory total size and total blocks number
    uint64_t usedMemorySize = 0;
    uint32_t blocks_number = 0;
    for (auto thread_it = m_threads.begin(), end = m_threads.end(); thread_it != end;)
    {
        if (_async && m_stopDumping.load(std::memory_order_acquire))
        {
            m_spin.unlock();
            m_storedSpin.unlock();
            if (_lockSpin)
                m_dumpSpin.unlock();
            return 0;
        }

        auto& thread = thread_it->second;
        uint32_t num = thread.blocks.closedList.markedSize() + thread.sync.closedList.size();
        const char expired = ProfileManager::checkThreadExpired(thread);

#ifdef _WIN32
        if (num == 0 && expired != 0)
#elif defined(EASY_CXX11_TLS_AVAILABLE)
        // Removing !guarded thread when thread_local feature is supported is safe.
        if (num == 0 && (expired != 0 || !thread.guarded))
#elif EASY_OPTION_REMOVE_EMPTY_UNGUARDED_THREADS != 0
# pragma message "Warning: Removing !guarded thread without thread_local support may cause an application crash, but fixes potential memory leak when using pthreads."
        // Removing !guarded thread may cause an application crash if a thread would start to write blocks after ThreadStorage remove.
        // TODO: Find solution to check thread state for pthread or to nullify THIS_THREAD pointer for removed ThreadStorage
        if (num == 0 && (expired != 0 || !t.guarded))
#else
# pragma message "Warning: Can not check pthread state (dead or alive). This may cause memory leak because ThreadStorage-s would not be removed ever during an application launched."
        if (num == 0 && expired != 0)
#endif
        {
            // Remove thread if it contains no profiled information and has been finished (or is not guarded --deprecated).
            profiler::thread_id_t id = thread_it->first;
            if (!mainThreadExpired && m_mainThreadId.compare_exchange_weak(id, 0, std::memory_order_release, std::memory_order_acquire))
                mainThreadExpired = true;
            m_threads.erase(thread_it++);
            continue;
        }

        if (expired == 1)
        {
            EASY_FORCE_EVENT3(thread, endtime, "ThreadExpired", EASY_COLOR_THREAD_END);
            ++num;
        }

        usedMemorySize += thread.blocks.usedMemorySize + thread.sync.usedMemorySize;
        blocks_number += num;
        ++thread_it;
    }

    // Write profiler signature and version
    write(_outputStream, EASY_PROFILER_SIGNATURE);
    write(_outputStream, EASY_PROFILER_VERSION);
    write(_outputStream, m_processId);

    // Write CPU frequency to let GUI calculate real time value from CPU clocks
#if defined(EASY_CHRONO_CLOCK) || defined(_WIN32)
    write(_outputStream, m_cpuFrequency);
#else
    EASY_LOGMSG("Calculating CPU frequency\n");
    const int64_t cpu_frequency = calculate_cpu_frequency();
    write(_outputStream, cpu_frequency * 1000LL);
    EASY_LOGMSG("Done calculating CPU frequency\n");

    m_cpuFrequency.store(cpu_frequency, std::memory_order_release);
#endif

    // Write begin and end time
    write(_outputStream, m_beginTime);
    write(_outputStream, m_endTime);

    // Write blocks number and used memory size
    write(_outputStream, usedMemorySize);
    write(_outputStream, m_descriptorsMemorySize);
    write(_outputStream, blocks_number);
    write(_outputStream, static_cast<uint32_t>(m_descriptors.size()));
    write(_outputStream, static_cast<uint32_t>(m_threads.size()));
    write(_outputStream, static_cast<uint16_t>(0)); // Bookmarks count (they can be created by user in the UI)
    write(_outputStream, static_cast<uint16_t>(0)); // padding

    // Write block descriptors
    for (const auto descriptor : m_descriptors)
    {
        const auto name_size = descriptor->nameSize();
        const auto filename_size = descriptor->filenameSize();
        const auto size = static_cast<uint16_t>(sizeof(profiler::SerializedBlockDescriptor) + name_size + filename_size);

        write(_outputStream, size);
        write<profiler::BaseBlockDescriptor>(_outputStream, *descriptor);
        write(_outputStream, name_size);
        write(_outputStream, descriptor->name(), name_size);
        write(_outputStream, descriptor->filename(), filename_size);
    }

    // Write blocks and context switch events for each thread
    for (auto thread_it = m_threads.begin(), end = m_threads.end(); thread_it != end;)
    {
        if (_async && m_stopDumping.load(std::memory_order_acquire))
        {
            m_spin.unlock();
            m_storedSpin.unlock();
            if (_lockSpin)
                m_dumpSpin.unlock();
            return 0;
        }

        auto& thread = thread_it->second;

        write(_outputStream, thread_it->first);

        const auto name_size = static_cast<uint16_t>(thread.name.size() + 1);
        write(_outputStream, name_size);
        write(_outputStream, name_size > 1 ? thread.name.c_str() : "", name_size);

        write(_outputStream, thread.sync.closedList.size());
        if (!thread.sync.closedList.empty())
            thread.sync.closedList.serialize(_outputStream);

        write(_outputStream, thread.blocks.closedList.markedSize());
        if (!thread.blocks.closedList.markedEmpty())
            thread.blocks.closedList.serialize(_outputStream);

        thread.clearClosed();
        //t.blocks.openedList.clear();
        thread.sync.openedList.clear();

        if (thread.expired.load(std::memory_order_acquire) != 0)
        {
            // Remove expired thread after writing all profiled information
            profiler::thread_id_t id = thread_it->first;
            if (!mainThreadExpired && m_mainThreadId.compare_exchange_weak(id, 0, std::memory_order_release, std::memory_order_acquire))
                mainThreadExpired = true;
            m_threads.erase(thread_it++);
        }
        else
        {
            ++thread_it;
        }
    }

    // End of threads section
    write(_outputStream, EASY_PROFILER_SIGNATURE);

    m_storedSpin.unlock();
    m_spin.unlock();

    if (_lockSpin)
        m_dumpSpin.unlock();

    EASY_LOGMSG("Done dumpBlocksToStream(). Dumped " << blocks_number << " blocks\n");

    return blocks_number;
}

uint32_t ProfileManager::dumpBlocksToFile(const char* _filename)
{
    EASY_LOGMSG("dumpBlocksToFile(\"" << _filename << "\")...\n");

    std::ofstream outputFile(_filename, std::fstream::binary);
    if (!outputFile.is_open())
    {
        EASY_ERROR("Can not open \"" << _filename << "\" for writing\n");
        return 0;
    }

    // Write data directly to file
    const auto blocksNumber = dumpBlocksToStream(outputFile, true, false);

    EASY_LOGMSG("Done dumpBlocksToFile()\n");

    return blocksNumber;
}

void ProfileManager::registerThread()
{
    THIS_THREAD = &threadStorage(getCurrentThreadId());

#ifdef EASY_CXX11_TLS_AVAILABLE
    THIS_THREAD->guarded = true;
    THIS_THREAD_GUARD.m_id = THIS_THREAD->id;
#endif
}

const char* ProfileManager::registerThread(const char* name, profiler::ThreadGuard& threadGuard)
{
    if (THIS_THREAD == nullptr)
        THIS_THREAD = &threadStorage(getCurrentThreadId());

    THIS_THREAD->guarded = true;
    if (!THIS_THREAD->named)
    {
        THIS_THREAD->named = true;
        THIS_THREAD->name = name;

        if (THIS_THREAD->name == "Main")
        {
            profiler::thread_id_t id = 0;
            THIS_THREAD_IS_MAIN = m_mainThreadId.compare_exchange_weak(id, THIS_THREAD->id, std::memory_order_release, std::memory_order_acquire);
        }

#ifdef EASY_CXX11_TLS_AVAILABLE
        THIS_THREAD_GUARD.m_id = THIS_THREAD->id;
    }

    (void)threadGuard; // this is just to prevent from warning about unused variable
#else
    }

    threadGuard.m_id = THIS_THREAD->id;
#endif

    return THIS_THREAD->name.c_str();
}

const char* ProfileManager::registerThread(const char* name)
{
    if (THIS_THREAD == nullptr)
        THIS_THREAD = &threadStorage(getCurrentThreadId());

    if (!THIS_THREAD->named)
    {
        THIS_THREAD->named = true;
        THIS_THREAD->name = name;

        if (THIS_THREAD->name == "Main")
        {
            profiler::thread_id_t id = 0;
            THIS_THREAD_IS_MAIN = m_mainThreadId.compare_exchange_weak(id, THIS_THREAD->id, std::memory_order_release, std::memory_order_acquire);
        }

#ifdef EASY_CXX11_TLS_AVAILABLE
        THIS_THREAD->guarded = true;
        THIS_THREAD_GUARD.m_id = THIS_THREAD->id;
#endif
    }

    return THIS_THREAD->name.c_str();
}

void ProfileManager::setBlockStatus(profiler::block_id_t _id, profiler::EasyBlockStatus _status)
{
    if (isEnabled())
        return; // Changing blocks statuses is restricted while profile session is active

    guard_lock_t lock(m_storedSpin);
    if (_id < m_descriptors.size())
    {
        auto desc = m_descriptors[_id];
        lock.unlock();
        desc->m_status = _status;
    }
}

void ProfileManager::startListen(uint16_t _port)
{
    if (!m_isAlreadyListening.exchange(true, std::memory_order_acq_rel))
    {
        m_stopListen.store(false, std::memory_order_release);
        m_listenThread = std::thread(&ProfileManager::listen, this, _port);
    }
}

void ProfileManager::stopListen()
{
    m_stopListen.store(true, std::memory_order_release);
    if (m_listenThread.joinable())
        m_listenThread.join();
    m_isAlreadyListening.store(false, std::memory_order_release);
}

bool ProfileManager::isListening() const
{
    return m_isAlreadyListening.load(std::memory_order_acquire);
}

//////////////////////////////////////////////////////////////////////////

void ProfileManager::setContextSwitchLogFilename(const char* name)
{
    m_csInfoFilename = name;
}

const char* ProfileManager::getContextSwitchLogFilename() const
{
    return m_csInfoFilename.c_str();
}

//////////////////////////////////////////////////////////////////////////

#if defined(EASY_CHRONO_CLOCK) || defined(_WIN32)
profiler::timestamp_t ProfileManager::ticks2ns(profiler::timestamp_t ticks) const
{
    return static_cast<profiler::timestamp_t>(ticks * 1000000000LL / m_cpuFrequency);
}

profiler::timestamp_t ProfileManager::ticks2us(profiler::timestamp_t ticks) const
{
    return static_cast<profiler::timestamp_t>(ticks * 1000000LL / m_cpuFrequency);
}
#else
profiler::timestamp_t ProfileManager::ticks2ns(profiler::timestamp_t ticks) const
{
    return static_cast<profiler::timestamp_t>(ticks / m_cpuFrequency.load(std::memory_order_acquire));
}

profiler::timestamp_t ProfileManager::ticks2us(profiler::timestamp_t ticks) const
{
    return static_cast<profiler::timestamp_t>(ticks * 1000 / m_cpuFrequency.load(std::memory_order_acquire));
}
#endif

//////////////////////////////////////////////////////////////////////////

bool ProfileManager::isMainThread()
{
    return THIS_THREAD_IS_MAIN;
}

profiler::timestamp_t ProfileManager::this_thread_frameTime(profiler::Duration _durationCast)
{
    if (_durationCast == profiler::TICKS)
        return THIS_THREAD_FRAME_T_CUR;
    return ProfileManager::instance().ticks2us(THIS_THREAD_FRAME_T_CUR);
}

profiler::timestamp_t ProfileManager::this_thread_frameTimeLocalMax(profiler::Duration _durationCast)
{
    THIS_THREAD_FRAME_T_RESET_MAX = true;
    if (_durationCast == profiler::TICKS)
        return THIS_THREAD_FRAME_T_MAX;
    return ProfileManager::instance().ticks2us(THIS_THREAD_FRAME_T_MAX);
}

profiler::timestamp_t ProfileManager::this_thread_frameTimeLocalAvg(profiler::Duration _durationCast)
{
    THIS_THREAD_FRAME_T_RESET_AVG = true;
    auto avgDuration = THIS_THREAD_N_FRAMES > 0 ? THIS_THREAD_FRAME_T_ACC / THIS_THREAD_N_FRAMES : 0;
    if (_durationCast == profiler::TICKS)
        return avgDuration;
    return ProfileManager::instance().ticks2us(avgDuration);
}

profiler::timestamp_t ProfileManager::main_thread_frameTime(profiler::Duration _durationCast)
{
    const auto ticks = THIS_THREAD_IS_MAIN ? THIS_THREAD_FRAME_T_CUR : ProfileManager::instance().curFrameDuration();
    if (_durationCast == profiler::TICKS)
        return ticks;
    return ProfileManager::instance().ticks2us(ticks);
}

profiler::timestamp_t ProfileManager::main_thread_frameTimeLocalMax(profiler::Duration _durationCast)
{
    if (THIS_THREAD_IS_MAIN)
    {
        THIS_THREAD_FRAME_T_RESET_MAX = true;
        if (_durationCast == profiler::TICKS)
            return THIS_THREAD_FRAME_T_MAX;
        return ProfileManager::instance().ticks2us(THIS_THREAD_FRAME_T_MAX);
    }

    if (_durationCast == profiler::TICKS)
        return ProfileManager::instance().maxFrameDuration();
    return ProfileManager::instance().ticks2us(ProfileManager::instance().maxFrameDuration());
}

profiler::timestamp_t ProfileManager::main_thread_frameTimeLocalAvg(profiler::Duration _durationCast)
{
    if (THIS_THREAD_IS_MAIN)
    {
        THIS_THREAD_FRAME_T_RESET_AVG = true;
        auto avgDuration = THIS_THREAD_N_FRAMES > 0 ? THIS_THREAD_FRAME_T_ACC / THIS_THREAD_N_FRAMES : 0;
        if (_durationCast == profiler::TICKS)
            return avgDuration;
        return ProfileManager::instance().ticks2us(avgDuration);
    }

    if (_durationCast == profiler::TICKS)
        return ProfileManager::instance().avgFrameDuration();
    return ProfileManager::instance().ticks2us(ProfileManager::instance().avgFrameDuration());
}

//////////////////////////////////////////////////////////////////////////

template <class T>
static void join(std::future<T>& futureResult)
{
    if (futureResult.valid())
        futureResult.get();
}

void ProfileManager::listen(uint16_t _port)
{
    EASY_THREAD_SCOPE("EasyProfiler.Listen");

    EASY_LOGMSG("Listening started\n");

    std::stringstream os(std::ios_base::out | std::ios_base::binary);
    std::future<uint32_t> dumpingResult;
    bool dumping = false;

    const auto stopDumping = [&] {
        dumping = false;
        m_stopDumping.store(true, std::memory_order_release);
        join(dumpingResult);
        clear_sstream(os);
    };

    EasySocket socket;
    profiler::net::Message replyMessage(profiler::net::MessageType::Reply_Capturing_Started);

    socket.bind(_port);
    int bytes = 0;
    while (!m_stopListen.load(std::memory_order_acquire))
    {
        if (dumping)
            stopDumping();

        socket.listen();
        socket.accept();

        bool hasConnect = true;

        // Send reply
        {
            const bool wasLowPriorityET =
#ifdef _WIN32
                EasyEventTracer::instance().isLowPriority();
#else
                false;
#endif
            const profiler::net::EasyProfilerStatus connectionReply(isEnabled(), isEventTracingEnabled(), wasLowPriorityET);

            bytes = socket.send(&connectionReply, sizeof(profiler::net::EasyProfilerStatus));
            hasConnect = bytes > 0;
        }

        while (hasConnect && !m_stopListen.load(std::memory_order_acquire))
        {
            if (dumping)
            {
                if (!dumpingResult.valid())
                {
                    dumping = false;
                    socket.setReceiveTimeout(0);
                    clear_sstream(os);
                }
                else if (dumpingResult.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready)
                {
                    dumping = false;
                    dumpingResult.get();

                    const auto size = os.tellp();
                    static const decltype(size) badSize = -1;
                    if (size != badSize)
                    {
                        const profiler::net::DataMessage dm(static_cast<uint32_t>(size),
                                                            profiler::net::MessageType::Reply_Blocks);

                        const size_t packet_size = sizeof(dm) + dm.size;
                        std::string sendbuf;
                        sendbuf.reserve(packet_size + 1);

                        if (sendbuf.capacity() >= packet_size) // check if there is enough memory
                        {
                            sendbuf.append((const char*) &dm, sizeof(dm));
                            sendbuf += os.str(); // TODO: Avoid double-coping data from stringstream!
                            clear_sstream(os);

                            bytes = socket.send(sendbuf.c_str(), packet_size);
                            hasConnect = bytes > 0;
                            if (!hasConnect)
                                break;
                        }
                        else
                        {
                            EASY_ERROR("Can not send blocks. Not enough memory for allocating " << packet_size
                                                                                                << " bytes");
                            clear_sstream(os);
                        }
                    }
                    else
                    {
                        EASY_ERROR("Can not send blocks. Bad std::stringstream.tellp() == -1");
                        clear_sstream(os);
                    }

                    replyMessage.type = profiler::net::MessageType::Reply_Blocks_End;
                    bytes = socket.send(&replyMessage, sizeof(replyMessage));
                    hasConnect = bytes > 0;
                    if (!hasConnect)
                        break;

                    socket.setReceiveTimeout(0);
                }
            }

            char buffer[256] = {};
            bytes = socket.receive(buffer, 255);

            hasConnect = socket.isConnected();
            if (!hasConnect || bytes < static_cast<int>(sizeof(profiler::net::Message)))
                continue;

            auto message = (const profiler::net::Message*)buffer;
            if (!message->isEasyNetMessage())
                continue;

            switch (message->type)
            {
                case profiler::net::MessageType::Ping:
                {
                    EASY_LOGMSG("receive MessageType::Ping\n");
                    break;
                }

                case profiler::net::MessageType::Request_MainThread_FPS:
                {
                    profiler::timestamp_t maxDuration = maxFrameDuration(), avgDuration = avgFrameDuration();

                    maxDuration = ticks2us(maxDuration);
                    avgDuration = ticks2us(avgDuration);

                    const profiler::net::TimestampMessage reply(profiler::net::MessageType::Reply_MainThread_FPS,
                                                                (uint32_t)maxDuration, (uint32_t)avgDuration);

                    bytes = socket.send(&reply, sizeof(profiler::net::TimestampMessage));
                    hasConnect = bytes > 0;

                    break;
                }

                case profiler::net::MessageType::Request_Start_Capture:
                {
                    EASY_LOGMSG("receive MessageType::Request_Start_Capture\n");

                    profiler::timestamp_t t = 0;
                    EASY_FORCE_EVENT(t, "StartCapture", EASY_COLOR_START, profiler::OFF);

                    m_dumpSpin.lock();
                    if (!m_profilerStatus.exchange(true, std::memory_order_acq_rel))
                    {
                        enableEventTracer();
                        m_beginTime = t;
                    }
                    m_dumpSpin.unlock();

                    replyMessage.type = profiler::net::MessageType::Reply_Capturing_Started;
                    bytes = socket.send(&replyMessage, sizeof(replyMessage));
                    hasConnect = bytes > 0;

                    break;
                }

                case profiler::net::MessageType::Request_Stop_Capture:
                {
                    EASY_LOGMSG("receive MessageType::Request_Stop_Capture\n");

                    if (dumping)
                        break;

                    m_dumpSpin.lock();
                    auto time = profiler::clock::now();
                    if (m_profilerStatus.exchange(false, std::memory_order_acq_rel))
                    {
                        disableEventTracer();
                        m_endTime = time;
                    }
                    EASY_FORCE_EVENT2(m_endTime, "StopCapture", EASY_COLOR_END, profiler::OFF);

                    dumping = true;
                    socket.setReceiveTimeout(500); // We have to check if dumping ready or not

                    m_stopDumping.store(false, std::memory_order_release);
                    dumpingResult = std::async(std::launch::async, [this, &os]
                    {
                        auto result = dumpBlocksToStream(os, false, true);
                        m_dumpSpin.unlock();
                        return result;
                    });

                    break;
                }

                case profiler::net::MessageType::Request_Blocks_Description:
                {
                    EASY_LOGMSG("receive MessageType::Request_Blocks_Description\n");

                    if (dumping)
                        stopDumping();

                    // Write profiler signature and version
                    write(os, EASY_PROFILER_SIGNATURE);
                    write(os, EASY_PROFILER_VERSION);

                    // Write block descriptors
                    m_storedSpin.lock();
                    write(os, static_cast<uint32_t>(m_descriptors.size()));
                    write(os, m_descriptorsMemorySize);
                    for (const auto descriptor : m_descriptors)
                    {
                        const auto name_size = descriptor->nameSize();
                        const auto filename_size = descriptor->filenameSize();
                        const auto size = static_cast<uint16_t>(sizeof(profiler::SerializedBlockDescriptor)
                                                                + name_size + filename_size);

                        write(os, size);
                        write<profiler::BaseBlockDescriptor>(os, *descriptor);
                        write(os, name_size);
                        write(os, descriptor->name(), name_size);
                        write(os, descriptor->filename(), filename_size);
                    }
                    m_storedSpin.unlock();
                    // END of Write block descriptors.

                    const auto size = os.tellp();
                    static const decltype(size) badSize = -1;
                    if (size != badSize)
                    {
                        const profiler::net::DataMessage dm(static_cast<uint32_t>(size),
                                                            profiler::net::MessageType::Reply_Blocks_Description);

                        const size_t packet_size = sizeof(dm) + dm.size;
                        std::string sendbuf;
                        sendbuf.reserve(packet_size + 1);

                        if (sendbuf.capacity() >= packet_size) // check if there is enough memory
                        {
                            sendbuf.append((const char*)&dm, sizeof(dm));
                            sendbuf += os.str(); // TODO: Avoid double-coping data from stringstream!
                            clear_sstream(os);

                            bytes = socket.send(sendbuf.c_str(), packet_size);
                            //hasConnect = bytes > 0;
                        }
                        else
                        {
                            EASY_ERROR("Can not send block descriptions. Not enough memory for allocating " << packet_size << " bytes");
                        }
                    }
                    else
                    {
                        EASY_ERROR("Can not send block descriptions. Bad std::stringstream.tellp() == -1");
                    }

                    replyMessage.type = profiler::net::MessageType::Reply_Blocks_Description_End;
                    bytes = socket.send(&replyMessage, sizeof(replyMessage));
                    hasConnect = bytes > 0;

                    break;
                }

                case profiler::net::MessageType::Change_Block_Status:
                {
                    auto data = reinterpret_cast<const profiler::net::BlockStatusMessage*>(message);
                    EASY_LOGMSG("receive MessageType::ChangeBLock_Status id=" << data->id << " status=" << data->status << std::endl);
                    setBlockStatus(data->id, static_cast<profiler::EasyBlockStatus>(data->status));
                    break;
                }

                case profiler::net::MessageType::Change_Event_Tracing_Status:
                {
                    auto data = reinterpret_cast<const profiler::net::BoolMessage*>(message);
                    EASY_LOGMSG("receive MessageType::Change_Event_Tracing_Status on=" << data->flag << std::endl);
                    setEventTracingEnabled(data->flag);
                    break;
                }

                case profiler::net::MessageType::Change_Event_Tracing_Priority:
                {
#if defined(_WIN32) || EASY_OPTION_LOG_ENABLED != 0
                    auto data = reinterpret_cast<const profiler::net::BoolMessage*>(message);
#endif

                    EASY_LOGMSG("receive MessageType::Change_Event_Tracing_Priority low=" << data->flag << std::endl);

#if defined(_WIN32)
                    EasyEventTracer::instance().setLowPriority(data->flag);
#endif
                    break;
                }

                default:
                    break;
            }
        }
    }

    if (dumping)
    {
        m_stopDumping.store(true, std::memory_order_release);
        join(dumpingResult);
    }

    EASY_LOGMSG("Listening stopped\n");
}

//////////////////////////////////////////////////////////////////////////

