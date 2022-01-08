/************************************************************************
* file name         : profiler.cpp
* ----------------- :
* creation time     : 2018/05/06
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

#include <easy/profiler.h>
#include <easy/arbitrary_value.h>
#include "profile_manager.h"
#include "event_trace_win.h"
#include "current_time.h"

//////////////////////////////////////////////////////////////////////////

#ifndef BUILD_WITH_EASY_PROFILER
# ifndef EASY_PROFILER_API_DISABLED
#  define EASY_PROFILER_API_DISABLED
# endif
#endif

#if !defined(EASY_PROFILER_VERSION_MAJOR) || !defined(EASY_PROFILER_VERSION_MINOR) || !defined(EASY_PROFILER_VERSION_PATCH)
# ifdef _WIN32
#  error EASY_PROFILER_VERSION_MAJOR and EASY_PROFILER_VERSION_MINOR and EASY_PROFILER_VERSION_PATCH macros must be defined
# else
#  error "EASY_PROFILER_VERSION_MAJOR and EASY_PROFILER_VERSION_MINOR and EASY_PROFILER_VERSION_PATCH macros must be defined"
# endif
#endif

#define EASY_PROFILER_PRODUCT_VERSION "v" EASY_STRINGIFICATION(EASY_PROFILER_VERSION_MAJOR) "." \
                                          EASY_STRINGIFICATION(EASY_PROFILER_VERSION_MINOR) "." \
                                          EASY_STRINGIFICATION(EASY_PROFILER_VERSION_PATCH)

extern const uint32_t EASY_PROFILER_SIGNATURE = ('E' << 24) | ('a' << 16) | ('s' << 8) | 'y';
extern const uint32_t EASY_PROFILER_VERSION = (static_cast<uint32_t>(EASY_PROFILER_VERSION_MAJOR) << 24) |
                                              (static_cast<uint32_t>(EASY_PROFILER_VERSION_MINOR) << 16) |
                                               static_cast<uint32_t>(EASY_PROFILER_VERSION_PATCH);

#undef EASY_VERSION_INT

//////////////////////////////////////////////////////////////////////////

extern "C" {

PROFILER_API uint8_t versionMajor()
{
    static_assert(0 <= EASY_PROFILER_VERSION_MAJOR && EASY_PROFILER_VERSION_MAJOR <= 255,
                  "EASY_PROFILER_VERSION_MAJOR must be defined in range [0, 255]");

    return EASY_PROFILER_VERSION_MAJOR;
}

PROFILER_API uint8_t versionMinor()
{
    static_assert(0 <= EASY_PROFILER_VERSION_MINOR && EASY_PROFILER_VERSION_MINOR <= 255,
                  "EASY_PROFILER_VERSION_MINOR must be defined in range [0, 255]");

    return EASY_PROFILER_VERSION_MINOR;
}

PROFILER_API uint16_t versionPatch()
{
    static_assert(0 <= EASY_PROFILER_VERSION_PATCH && EASY_PROFILER_VERSION_PATCH <= 65535,
                  "EASY_PROFILER_VERSION_PATCH must be defined in range [0, 65535]");

    return EASY_PROFILER_VERSION_PATCH;
}

PROFILER_API uint32_t version()
{
    return EASY_PROFILER_VERSION;
}

PROFILER_API const char* versionName()
{
#ifdef EASY_PROFILER_API_DISABLED
    return EASY_PROFILER_PRODUCT_VERSION "_disabled";
#else
    return EASY_PROFILER_PRODUCT_VERSION;
#endif
}

//////////////////////////////////////////////////////////////////////////

#if !defined(EASY_PROFILER_API_DISABLED)

PROFILER_API profiler::timestamp_t now()
{
    return profiler::clock::now();
}

PROFILER_API profiler::timestamp_t toNanoseconds(profiler::timestamp_t _ticks)
{
    return ProfileManager::instance().ticks2ns(_ticks);
}

PROFILER_API profiler::timestamp_t toMicroseconds(profiler::timestamp_t _ticks)
{
    return ProfileManager::instance().ticks2us(_ticks);
}

PROFILER_API const profiler::BaseBlockDescriptor*
registerDescription(profiler::EasyBlockStatus _status, const char* _autogenUniqueId, const char* _name,
                    const char* _filename, int _line, profiler::block_type_t _block_type, profiler::color_t _color,
                    bool _copyName)
{
    return ProfileManager::instance().addBlockDescriptor(_status, _autogenUniqueId, _name, _filename, _line,
                                                         _block_type, _color, _copyName);
}

PROFILER_API void endBlock()
{
    ProfileManager::instance().endBlock();
}

PROFILER_API void setEnabled(bool isEnable)
{
    ProfileManager::instance().setEnabled(isEnable);
}

PROFILER_API bool isEnabled()
{
    return ProfileManager::instance().isEnabled();
}

PROFILER_API void storeValue(const profiler::BaseBlockDescriptor* _desc, profiler::DataType _type, const void* _data,
                             uint16_t _size, bool _isArray, profiler::ValueId _vin)
{
    ProfileManager::instance().storeValue(_desc, _type, _data, _size, _isArray, _vin);
}

PROFILER_API void storeEvent(const profiler::BaseBlockDescriptor* _desc, const char* _runtimeName)
{
    ProfileManager::instance().storeBlock(_desc, _runtimeName);
}

PROFILER_API void storeBlock(const profiler::BaseBlockDescriptor* _desc, const char* _runtimeName,
                             profiler::timestamp_t _beginTime, profiler::timestamp_t _endTime)
{
    ProfileManager::instance().storeBlock(_desc, _runtimeName, _beginTime, _endTime);
}

PROFILER_API void beginBlock(profiler::Block& _block)
{
    ProfileManager::instance().beginBlock(_block);
}

PROFILER_API void beginNonScopedBlock(const profiler::BaseBlockDescriptor* _desc, const char* _runtimeName)
{
    ProfileManager::instance().beginNonScopedBlock(_desc, _runtimeName);
}

PROFILER_API uint32_t dumpBlocksToFile(const char* filename)
{
    return ProfileManager::instance().dumpBlocksToFile(filename);
}

PROFILER_API const char* registerThreadScoped(const char* name, profiler::ThreadGuard& threadGuard)
{
    return ProfileManager::instance().registerThread(name, threadGuard);
}

PROFILER_API const char* registerThread(const char* name)
{
    return ProfileManager::instance().registerThread(name);
}

PROFILER_API void setEventTracingEnabled(bool _isEnable)
{
    ProfileManager::instance().setEventTracingEnabled(_isEnable);
}

PROFILER_API bool isEventTracingEnabled()
{
    return ProfileManager::instance().isEventTracingEnabled();
}

# ifdef _WIN32
PROFILER_API void setLowPriorityEventTracing(bool _isLowPriority)
{
    EasyEventTracer::instance().setLowPriority(_isLowPriority);
}

PROFILER_API bool isLowPriorityEventTracing()
{
    return EasyEventTracer::instance().isLowPriority();
}
# else
PROFILER_API void setLowPriorityEventTracing(bool) { }
PROFILER_API bool isLowPriorityEventTracing() { return false; }
# endif

PROFILER_API void setContextSwitchLogFilename(const char* name)
{
    return ProfileManager::instance().setContextSwitchLogFilename(name);
}

PROFILER_API const char* getContextSwitchLogFilename()
{
    return ProfileManager::instance().getContextSwitchLogFilename();
}

PROFILER_API void startListen(uint16_t _port)
{
    return ProfileManager::instance().startListen(_port);
}

PROFILER_API void stopListen()
{
    return ProfileManager::instance().stopListen();
}

PROFILER_API bool isListening()
{
    return ProfileManager::instance().isListening();
}

PROFILER_API bool isMainThread()
{
    return ProfileManager::isMainThread();
}

PROFILER_API profiler::timestamp_t this_thread_frameTime(profiler::Duration _durationCast)
{
    return ProfileManager::this_thread_frameTime(_durationCast);
}

PROFILER_API profiler::timestamp_t this_thread_frameTimeLocalMax(profiler::Duration _durationCast)
{
    return ProfileManager::this_thread_frameTimeLocalMax(_durationCast);
}

PROFILER_API profiler::timestamp_t this_thread_frameTimeLocalAvg(profiler::Duration _durationCast)
{
    return ProfileManager::this_thread_frameTimeLocalAvg(_durationCast);
}

PROFILER_API profiler::timestamp_t main_thread_frameTime(profiler::Duration _durationCast)
{
    return ProfileManager::main_thread_frameTime(_durationCast);
}

PROFILER_API profiler::timestamp_t main_thread_frameTimeLocalMax(profiler::Duration _durationCast)
{
    return ProfileManager::main_thread_frameTimeLocalMax(_durationCast);
}

PROFILER_API profiler::timestamp_t main_thread_frameTimeLocalAvg(profiler::Duration _durationCast)
{
    return ProfileManager::main_thread_frameTimeLocalAvg(_durationCast);
}

#else // EASY_PROFILER_API_DISABLED

PROFILER_API profiler::timestamp_t now() { return 0; }
PROFILER_API profiler::timestamp_t toNanoseconds(profiler::timestamp_t) { return 0; }
PROFILER_API profiler::timestamp_t toMicroseconds(profiler::timestamp_t) { return 0; }

PROFILER_API const profiler::BaseBlockDescriptor* registerDescription(profiler::EasyBlockStatus, const char*,
                                                                      const char*, const char*, int,
                                                                      profiler::block_type_t, profiler::color_t, bool)
{
    return reinterpret_cast<const BaseBlockDescriptor*>(0xbad);
}

PROFILER_API void endBlock() { }
PROFILER_API void setEnabled(bool) { }
PROFILER_API bool isEnabled() { return false; }

PROFILER_API void storeValue(const profiler::BaseBlockDescriptor*, profiler::DataType, const void*, uint16_t, bool,
                             profiler::ValueId)
{
}

PROFILER_API void storeEvent(const profiler::BaseBlockDescriptor*, const char*) { }
PROFILER_API void storeBlock(const profiler::BaseBlockDescriptor*, const char*, profiler::timestamp_t,
                             profiler::timestamp_t)
{
}

PROFILER_API void beginBlock(profiler::Block&) { }
PROFILER_API void beginNonScopedBlock(const profiler::BaseBlockDescriptor*, const char*) { }
PROFILER_API uint32_t dumpBlocksToFile(const char*) { return 0; }
PROFILER_API const char* registerThreadScoped(const char*, profiler::ThreadGuard&) { return ""; }
PROFILER_API const char* registerThread(const char*) { return ""; }
PROFILER_API void setEventTracingEnabled(bool) { }
PROFILER_API bool isEventTracingEnabled() { return false; }
PROFILER_API void setLowPriorityEventTracing(bool) { }
PROFILER_API bool isLowPriorityEventTracing(bool) { return false; }
PROFILER_API void setContextSwitchLogFilename(const char*) { }
PROFILER_API const char* getContextSwitchLogFilename() { return ""; }
PROFILER_API void startListen(uint16_t) { }
PROFILER_API void stopListen() { }
PROFILER_API bool isListening() { return false; }

PROFILER_API bool isMainThread() { return false; }
PROFILER_API profiler::timestamp_t this_thread_frameTime(profiler::Duration) { return 0; }
PROFILER_API profiler::timestamp_t this_thread_frameTimeLocalMax(profiler::Duration) { return 0; }
PROFILER_API profiler::timestamp_t this_thread_frameTimeLocalAvg(profiler::Duration) { return 0; }
PROFILER_API profiler::timestamp_t main_thread_frameTime(profiler::Duration) { return 0; }
PROFILER_API profiler::timestamp_t main_thread_frameTimeLocalMax(profiler::Duration) { return 0; }
PROFILER_API profiler::timestamp_t main_thread_frameTimeLocalAvg(profiler::Duration) { return 0; }

#endif // EASY_PROFILER_API_DISABLED

} // end extern "C".
