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

#ifndef EASY_PROFILER_H
#define EASY_PROFILER_H

#include <easy/details/profiler_public_types.h>

#define MAX_DYNAMIC_BLOCK_NAME_SIZE_ESTIMATED MAX_BLOCK_DATA_SIZE

#if defined ( __clang__ )
# pragma clang diagnostic push
# pragma clang diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"
#endif

//
// USING_EASY_PROFILER is defined in details/profiler_in_use.h
//                     if defined BUILD_WITH_EASY_PROFILER and not defined DISABLE_EASY_PROFILER
//

//
// BUILD_WITH_EASY_PROFILER is defined in CMakeLists.txt if your project is linked to easy_profiler.
//

//
// DISABLE_EASY_PROFILER may be defined manually in source-file before #include <easy/profiler.h>
//                       to disable profiler for certain source-file or project.
//

#ifdef USING_EASY_PROFILER

// EasyProfiler core API:

/** Macro for beginning of a scoped block with custom name and color.

\code
    #include <easy/profiler.h>
    void foo()
    {
        // some code ...

        EASY_BLOCK("Check something", profiler::OFF); // Disabled block (There is possibility to enable this block later via GUI)
        if(something){
            EASY_BLOCK("Calling bar()"); // Block with default color
            bar();
        }
        else{
            EASY_BLOCK("Calling baz()", profiler::colors::Red); // Red block
            baz();
        }
        EASY_END_BLOCK; // End of "Check something" block (Even if "Check something" is disabled, this EASY_END_BLOCK will not end any other block).

        EASY_BLOCK("Some another block", profiler::colors::Blue, profiler::ON_WITHOUT_CHILDREN); // Block with Blue color without
        // some another code...
        EASY_BLOCK("Calculate sum"); // This block will not be profiled because it's parent is ON_WITHOUT_CHILDREN
        int sum = 0;
        for (int i = 0; i < 10; ++i)
            sum += i;
        EASY_END_BLOCK; // End of "Calculate sum" block
    }
\endcode

Block will be automatically completed by destructor.

\ingroup profiler
*/
# define EASY_BLOCK(name, ...)\
    EASY_LOCAL_STATIC_PTR(const ::profiler::BaseBlockDescriptor*, EASY_UNIQUE_DESC(__LINE__), ::profiler::registerDescription(::profiler::extract_enable_flag(__VA_ARGS__),\
        EASY_UNIQUE_LINE_ID, EASY_COMPILETIME_NAME(name), __FILE__, __LINE__, ::profiler::BlockType::Block, ::profiler::extract_color(__VA_ARGS__),\
        ::std::is_base_of<::profiler::ForceConstStr, decltype(name)>::value));\
    ::profiler::Block EASY_UNIQUE_BLOCK(__LINE__)(EASY_UNIQUE_DESC(__LINE__), EASY_RUNTIME_NAME(name));\
    ::profiler::beginBlock(EASY_UNIQUE_BLOCK(__LINE__));

/** Macro for beginning of a non-scoped block with custom name and color.

You must end such block manually with EASY_END_BLOCK.

\code
    #include <easy/profiler.h>
    void foo() {
        EASY_NONSCOPED_BLOCK("Callback"); // Begin block which would not be finished when function returns.

        // some code ...
    }

    void bar() {
        // some another code...

        EASY_END_BLOCK; // This, as always, ends last opened block. You have to take care about blocks order by yourself.
    }

    void baz() {
        foo(); // non-scoped block begins here

        // some code...

        bar(); // non-scoped block ends here
    }
\endcode

Block will be automatically completed by destructor.

\ingroup profiler
*/
#define EASY_NONSCOPED_BLOCK(name, ...)\
    EASY_LOCAL_STATIC_PTR(const ::profiler::BaseBlockDescriptor*, EASY_UNIQUE_DESC(__LINE__), ::profiler::registerDescription(::profiler::extract_enable_flag(__VA_ARGS__),\
        EASY_UNIQUE_LINE_ID, EASY_COMPILETIME_NAME(name), __FILE__, __LINE__, ::profiler::BlockType::Block, ::profiler::extract_color(__VA_ARGS__),\
        ::std::is_base_of<::profiler::ForceConstStr, decltype(name)>::value));\
    ::profiler::beginNonScopedBlock(EASY_UNIQUE_DESC(__LINE__), EASY_RUNTIME_NAME(name));

/** Macro for beginning of a block with function name and custom color.

\code
    #include <easy/profiler.h>
    void foo(){
        EASY_FUNCTION(); // Block with name="foo" and default color
        //some code...
    }

    void bar(){
        EASY_FUNCTION(profiler::colors::Green); // Green block with name="bar"
        //some code...
    }

    void baz(){
        EASY_FUNCTION(profiler::FORCE_ON); // Force enabled block with name="baz" and default color (This block will be profiled even if it's parent is OFF_RECURSIVE)
        // som code...
    }
\endcode

Name of the block automatically created with function name.

\ingroup profiler
*/
# define EASY_FUNCTION(...) EASY_BLOCK(EASY_FUNC_NAME, ## __VA_ARGS__)

/** Macro for completion of last opened block explicitly.

\code
#include <easy/profiler.h>
int foo()
{
    // some code ...

    int sum = 0;
    EASY_BLOCK("Calculating sum");
    for (int i = 0; i < 10; ++i){
        sum += i;
    }
    EASY_END_BLOCK;

    // some antoher code here ...

    return sum;
}
\endcode

\ingroup profiler
*/
# define EASY_END_BLOCK ::profiler::endBlock();

/** Macro for creating event marker with custom name and color.

Event marker is a block with zero duration and special type.

\warning Event marker ends immediately and calling EASY_END_BLOCK after EASY_EVENT
will end previously opened EASY_BLOCK or EASY_FUNCTION.

\ingroup profiler
*/
# define EASY_EVENT(name, ...)\
    EASY_LOCAL_STATIC_PTR(const ::profiler::BaseBlockDescriptor*, EASY_UNIQUE_DESC(__LINE__), ::profiler::registerDescription(\
        ::profiler::extract_enable_flag(__VA_ARGS__), EASY_UNIQUE_LINE_ID, EASY_COMPILETIME_NAME(name),\
            __FILE__, __LINE__, ::profiler::BlockType::Event, ::profiler::extract_color(__VA_ARGS__),\
            ::std::is_base_of<::profiler::ForceConstStr, decltype(name)>::value));\
    ::profiler::storeEvent(EASY_UNIQUE_DESC(__LINE__), EASY_RUNTIME_NAME(name));

/** Macro for enabling profiler.

\ingroup profiler
*/
# define EASY_PROFILER_ENABLE ::profiler::setEnabled(true);

/** Macro for disabling profiler.

\ingroup profiler
*/
# define EASY_PROFILER_DISABLE ::profiler::setEnabled(false);

/** Macro for current thread registration.

\note If this thread has been already registered then nothing happens.

\ingroup profiler
*/
# define EASY_THREAD(name)\
    EASY_THREAD_LOCAL static const char* EASY_TOKEN_CONCATENATE(unique_profiler_thread_name, __LINE__) = nullptr;\
    if (!EASY_TOKEN_CONCATENATE(unique_profiler_thread_name, __LINE__))\
        EASY_TOKEN_CONCATENATE(unique_profiler_thread_name, __LINE__) = ::profiler::registerThread(name);

/** Macro for current thread registration and creating a thread guard object.

\note If this thread has been already registered then nothing happens.

\note Also creates thread guard which marks thread as "expired" on it's destructor
and creates "ThreadFinished" profiler event.

\ingroup profiler
*/
# define EASY_THREAD_SCOPE(name)\
    static EASY_THREAD_LOCAL const char* EASY_TOKEN_CONCATENATE(unique_profiler_thread_name, __LINE__) = nullptr;\
    ::profiler::ThreadGuard EASY_TOKEN_CONCATENATE(unique_profiler_thread_guard, __LINE__);\
    if (!EASY_TOKEN_CONCATENATE(unique_profiler_thread_name, __LINE__))\
        EASY_TOKEN_CONCATENATE(unique_profiler_thread_name, __LINE__) = ::profiler::registerThreadScoped(name,\
        EASY_TOKEN_CONCATENATE(unique_profiler_thread_guard, __LINE__));

/** Macro for main thread registration.

This is just for user's comfort. There is no difference for EasyProfiler GUI between different threads.

\ingroup profiler
*/
# define EASY_MAIN_THREAD EASY_THREAD("Main")

/** Enable or disable event tracing (context switch events).

\note Default value is controlled by EASY_OPTION_EVENT_TRACING_ENABLED macro.

\note Change will take effect on the next call to EASY_PROFILER_ENABLE.

\sa EASY_PROFILER_ENABLE, EASY_OPTION_EVENT_TRACING_ENABLED

\ingroup profiler
*/
# define EASY_SET_EVENT_TRACING_ENABLED(isEnabled) ::profiler::setEventTracingEnabled(isEnabled);

/** Set event tracing thread priority (low or normal).

Event tracing with low priority will affect your application performance much more less, but
it can be late to gather information about thread/process (thread could be finished to the moment
when event tracing thread will be awaken) and you will not see process name and process id
information in GUI for such threads. You will still be able to see all context switch events.

Event tracing with normal priority could gather more information about processes but potentially
it could affect performance as it has more work to do. Usually you will not notice any performance
breakdown, but if you care about that then you change set event tracing priority level to low.

\sa EASY_OPTION_LOW_PRIORITY_EVENT_TRACING

\ingroup profiler
*/
# define EASY_SET_LOW_PRIORITY_EVENT_TRACING(isLowPriority) ::profiler::setLowPriorityEventTracing(isLowPriority);

/** Macro for setting temporary log-file path for Unix event tracing system.

\note Default value is "/tmp/cs_profiling_info.log".

\ingroup profiler
*/
# define EASY_EVENT_TRACING_SET_LOG(filename) ::profiler::setContextSwitchLogFilename(filename);

/** Macro returning current path to the temporary log-file for Unix event tracing system.

\ingroup profiler
*/
# define EASY_EVENT_TRACING_LOG ::profiler::getContextSwitchLogFilename();

// EasyProfiler settings:

/** If != 0 then EasyProfiler will measure time for blocks storage expansion.
If 0 then EasyProfiler will be compiled without blocks of code responsible
for measuring these events.

These are "EasyProfiler.ExpandStorage" blocks on a diagram.

\ingroup profiler
*/
# ifndef EASY_OPTION_MEASURE_STORAGE_EXPAND
#  define EASY_OPTION_MEASURE_STORAGE_EXPAND 0
# endif

# if EASY_OPTION_MEASURE_STORAGE_EXPAND != 0
/** If true then "EasyProfiler.ExpandStorage" blocks are enabled by default and will be
writed to output file or translated over the net.
If false then you need to enable these blocks via GUI if you want to see them.

\ingroup profiler
*/
#  ifndef EASY_OPTION_STORAGE_EXPAND_BLOCKS_ON
#   define EASY_OPTION_STORAGE_EXPAND_BLOCKS_ON true
#  endif

# endif // EASY_OPTION_MEASURE_STORAGE_EXPAND != 0

/** If true then EasyProfiler event tracing is enabled by default
and will be turned on and off when you call profiler::setEnabled().
Otherwise, it have to be turned on via GUI and then it will be
turned on/off with next calls of profiler::setEnabled().

\ingroup profiler
*/
# ifndef EASY_OPTION_EVENT_TRACING_ENABLED
#  define EASY_OPTION_EVENT_TRACING_ENABLED true
# endif

/** If true then EasyProfiler.ETW thread (Event tracing for Windows) will have low priority by default.

\sa EASY_SET_LOW_PRIORITY_EVENT_TRACING

\note You can always change priority level via GUI or API while profiling session is not launched.
You don't need to rebuild or restart your application for that.

\ingroup profiler
*/
# ifndef EASY_OPTION_LOW_PRIORITY_EVENT_TRACING
#  define EASY_OPTION_LOW_PRIORITY_EVENT_TRACING true
# endif


/** If != 0 then EasyProfiler will print error messages into stderr.
Otherwise, no log messages will be printed.

\ingroup profiler
*/
# ifndef EASY_OPTION_LOG_ENABLED
#  define EASY_OPTION_LOG_ENABLED 0
# endif

/** If != 0 then EasyProfiler will start listening thread immediately on ProfileManager initialization.

\sa startListen

\ingroup profiler
*/
# ifndef EASY_OPTION_START_LISTEN_ON_STARTUP
#  define EASY_OPTION_START_LISTEN_ON_STARTUP 0
# endif

#else // #ifdef BUILD_WITH_EASY_PROFILER

# define EASY_BLOCK(...)
# define EASY_NONSCOPED_BLOCK(...)
# define EASY_FUNCTION(...)
# define EASY_END_BLOCK 
# define EASY_PROFILER_ENABLE 
# define EASY_PROFILER_DISABLE 
# define EASY_EVENT(...)
# define EASY_THREAD(...)
# define EASY_THREAD_SCOPE(...)
# define EASY_MAIN_THREAD 
# define EASY_SET_EVENT_TRACING_ENABLED(isEnabled) 
# define EASY_SET_LOW_PRIORITY_EVENT_TRACING(isLowPriority) 

# ifndef _WIN32
#  define EASY_EVENT_TRACING_SET_LOG(filename) 
#  define EASY_EVENT_TRACING_LOG ""
# endif

# ifndef EASY_OPTION_MEASURE_STORAGE_EXPAND
#  define EASY_OPTION_MEASURE_STORAGE_EXPAND 0
# endif

# ifndef EASY_OPTION_EVENT_TRACING_ENABLED
#  define EASY_OPTION_EVENT_TRACING_ENABLED false
# endif

# ifndef EASY_OPTION_LOW_PRIORITY_EVENT_TRACING
#  define EASY_OPTION_LOW_PRIORITY_EVENT_TRACING true
# endif

# ifndef EASY_OPTION_LOG_ENABLED
#  define EASY_OPTION_LOG_ENABLED 0
# endif

# ifndef EASY_OPTION_START_LISTEN_ON_STARTUP
#  define EASY_OPTION_START_LISTEN_ON_STARTUP 0
# endif

#endif // #ifndef BUILD_WITH_EASY_PROFILER

# ifndef EASY_DEFAULT_PORT
#  define EASY_DEFAULT_PORT 28077
# endif

/** Alias for EASY_PROFILER_ENABLE.

Added for clarification.

\sa EASY_PROFILER_ENABLE

\ingroup profiler
*/
#define EASY_START_CAPTURE EASY_PROFILER_ENABLE

/** Alias for EASY_PROFILER_DISABLE.

Added for clarification.

\sa EASY_PROFILER_DISABLE

\ingroup profiler
*/
#define EASY_STOP_CAPTURE EASY_PROFILER_DISABLE

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

namespace profiler {

    EASY_CONSTEXPR uint16_t DEFAULT_PORT = EASY_DEFAULT_PORT;

    //////////////////////////////////////////////////////////////////////
    // Core API
    // Note: It is better to use macros defined above than a direct calls to API.
    //       But some API functions does not have macro wrappers...

#ifdef USING_EASY_PROFILER
    extern "C" {

        /** Returns current time in ticks.

        You can use it if you want to store block explicitly.

        \retval Current CPU time in ticks.

        \ingroup profiler
        */
        PROFILER_API timestamp_t now();

        /** Convert ticks to nanoseconds.

        \retval _ticks converted to nanoseconds.

        \ingroup profiler
        */
        PROFILER_API timestamp_t toNanoseconds(timestamp_t _ticks);

        /** Convert ticks to microseconds.

        \retval _ticks converted to microseconds.

        \ingroup profiler
        */
        PROFILER_API timestamp_t toMicroseconds(timestamp_t _ticks);

        /** Registers static description of a block.

        It is general information which is common for all such blocks.
        Includes color, block type (see BlockType), file-name, line-number, compile-time name of a block and enable-flag.

        \note This API function is used by EASY_EVENT, EASY_BLOCK, EASY_FUNCTION macros.
        There is no need to invoke this function explicitly.

        \retval Pointer to registered block description.

        \ingroup profiler
        */
        PROFILER_API const BaseBlockDescriptor* registerDescription(EasyBlockStatus _status, const char* _autogenUniqueId, const char* _compiletimeName, const char* _filename, int _line, block_type_t _block_type, color_t _color, bool _copyName = false);

        /** Stores event in the blocks list.

        An event ends instantly and has zero duration.

        \note There is no need to invoke this function explicitly - use EASY_EVENT macro instead.

        \param _desc Reference to the previously registered description.
        \param _runtimeName Standard zero-terminated string which will be copied to the events buffer.

        \note _runtimeName must be an empty string ("") if you do not want to set name to the event at run-time.
        
        \ingroup profiler
        */
        PROFILER_API void storeEvent(const BaseBlockDescriptor* _desc, const char* _runtimeName = "");

        /** Stores block explicitly in the blocks list.

        Use this function for additional flexibility if you want to set block duration manually.

        \param _desc Reference to the previously registered description.
        \param _runtimeName Standard zero-terminated string which will be copied to the events buffer.
        \param _beginTime begin time of the block
        \param _endTime end time of the block

        \note _runtimeName must be an empty string ("") if you do not want to set name to the block at run-time.

        \ingroup profiler
        */
        PROFILER_API void storeBlock(const BaseBlockDescriptor* _desc, const char* _runtimeName, timestamp_t _beginTime, timestamp_t _endTime);

        /** Begins scoped block.

        \ingroup profiler
        */
        PROFILER_API void beginBlock(Block& _block);

        /** Begins non-scoped block.

        \param _desc Reference to the previously registered description (see registerDescription).
        \param _runtimeName Standard zero-terminated string which will be copied to the block buffer when block will end.

        \note There is no need to invoke this function explicitly - use EASY_NONSCOPED_BLOCK macro instead.
        EASY_NONSCOPED_BLOCK macro could be used for higher flexibility if you have to begin block in one
        function and end it in another one.

        \note _runtimeName must be an empty string ("") if you do not want to set name to the block at run-time.
        \note _runtimeName is copied only when block ends so you must ensure it's validity until block end.

        \warning You have to end this block explicitly.

        \ingroup profiler
        */
        PROFILER_API void beginNonScopedBlock(const BaseBlockDescriptor* _desc, const char* _runtimeName = "");

        /** Ends last started block.

        Use this only if you want to finish block explicitly.

        \ingroup profiler
        */
        PROFILER_API void endBlock();

        /** Enable or disable profiler.

        AKA start or stop profiling (capturing blocks).

        \ingroup profiler
        */
        PROFILER_API void setEnabled(bool _isEnable);
        PROFILER_API bool isEnabled();

        /** Save all gathered blocks into file.

        \note This also disables profiler.

        \retval Number of saved blocks. If 0 then nothing was profiled or an error occurred.

        \ingroup profiler
        */
        PROFILER_API uint32_t dumpBlocksToFile(const char* _filename);

        /** Register current thread and give it a name.

        Also creates a scoped ThreadGuard which would unregister thread on it's destructor.
        This helps for memory management while using an old compiler whitout thread_local support.

        \note Only first call of registerThread() for the current thread will have an effect.

        \note Use this function if you want to build your source code with an old compiler (MSVC < 2013, GCC < 4.8, Clang < 3.3).
        Otherwise there is no need in this function because a thread_local ThreadGuard created inside.

        \retval Registered name of the thread. It may differ from _name if the thread was registered before.

        \sa registerThread, ThreadGuard

        \ingroup profiler
        */
        PROFILER_API const char* registerThreadScoped(const char* _name, ThreadGuard&);

        /** Register current thread and give it a name.

        \note Only first call of registerThread() for the current thread will have an effect.

        \retval Registered name of the thread. It may differ from _name if the thread was registered before.

        \ingroup profiler
        */
        PROFILER_API const char* registerThread(const char* _name);

        /** Enable or disable event tracing.

        \note This change will take an effect on the next call of setEnabled(true);

        \sa setEnabled, EASY_SET_EVENT_TRACING_ENABLED

        \ingroup profiler
        */
        PROFILER_API void setEventTracingEnabled(bool _isEnable);
        PROFILER_API bool isEventTracingEnabled();

        /** Set event tracing thread priority (low or normal).

        \note This change will take effect on the next call of setEnabled(true);

        \sa setEnabled, EASY_SET_LOW_PRIORITY_EVENT_TRACING

        \ingroup profiler
        */
        PROFILER_API void setLowPriorityEventTracing(bool _isLowPriority);
        PROFILER_API bool isLowPriorityEventTracing();

        /** Set temporary log-file path for Unix event tracing system.

        \note Default value is "/tmp/cs_profiling_info.log".

        \ingroup profiler
        */
        PROFILER_API void setContextSwitchLogFilename(const char* _name);

        /** Returns current path to the temporary log-file for Unix event tracing system.

        \ingroup profiler
        */
        PROFILER_API const char* getContextSwitchLogFilename();

        /** Start listening for network commands.

        Launches a separate listening thread which would listen to the network commands (start, stop, etc.).
        The listening thread sends all profiled blocks via network after receiving network command 'stop'.

        \ingroup profiler
        */
        PROFILER_API void startListen(uint16_t _port = ::profiler::DEFAULT_PORT);

        /** Stops listening thread.

        \note This would be invoked automatically on application exit.

        \note Does not send any messages to the network, just stops thread.

        \ingroup profiler
        */
        PROFILER_API void stopListen();

        /** Check if listening thread launched.

        \ingroup profiler
        */
        PROFILER_API bool isListening();

        /** Returns current major version.
        
        \ingroup profiler
        */
        PROFILER_API uint8_t versionMajor();

        /** Returns current minor version.

        \ingroup profiler
        */
        PROFILER_API uint8_t versionMinor();

        /** Returns current version patch.

        \ingroup profiler
        */
        PROFILER_API uint16_t versionPatch();

        /** Returns current version in 32-bit integer format.

        \note Format is: 0x MAJ-MAJ MIN-MIN PATCH-PATCH-PATCH-PATCH
        For example v1.3.0 is: 0x01030000

        \ingroup profiler
        */
        PROFILER_API uint32_t version();

        /** Returns current version string.

        Example: "v1.3.0"

        \ingroup profiler
        */
        PROFILER_API const char* versionName();

        /** Returns true if current thread has been marked as Main.
        Otherwise, returns false.

        \ingroup profiler
        */
        PROFILER_API bool isMainThread();

        /** Returns last frame duration for current thread.

        \param _durationCast desired duration units (could be cpu-ticks or microseconds)

        \ingroup profiler
        */
        PROFILER_API timestamp_t this_thread_frameTime(Duration _durationCast = ::profiler::MICROSECONDS);

        /** Returns local max of frame duration for current thread.

        Local max is maximum frame duration since last frameTimeLocalMax() call.

        \param _durationCast desired duration units (could be cpu-ticks or microseconds)

        \ingroup profiler
        */
        PROFILER_API timestamp_t this_thread_frameTimeLocalMax(Duration _durationCast = ::profiler::MICROSECONDS);

        /** Returns local average of frame duration for current thread.

        Local average is average frame duration since last frameTimeLocalAvg() call.

        \param _durationCast desired duration units (could be cpu-ticks or microseconds)

        \ingroup profiler
        */
        PROFILER_API timestamp_t this_thread_frameTimeLocalAvg(Duration _durationCast = ::profiler::MICROSECONDS);

        /** Returns last frame duration for main thread.

        \param _durationCast desired duration units (could be cpu-ticks or microseconds)

        \ingroup profiler
        */
        PROFILER_API timestamp_t main_thread_frameTime(Duration _durationCast = ::profiler::MICROSECONDS);

        /** Returns local max of frame duration for main thread.

        Local max is maximum frame duration since last frameTimeLocalMax() call.

        \param _durationCast desired duration units (could be cpu-ticks or microseconds)

        \ingroup profiler
        */
        PROFILER_API timestamp_t main_thread_frameTimeLocalMax(Duration _durationCast = ::profiler::MICROSECONDS);

        /** Returns local average of frame duration for main thread.

        Local average is average frame duration since last frameTimeLocalAvg() call.

        \param _durationCast desired duration units (could be cpu-ticks or microseconds)

        \ingroup profiler
        */
        PROFILER_API timestamp_t main_thread_frameTimeLocalAvg(Duration _durationCast = ::profiler::MICROSECONDS);

    }
#else
    inline EASY_CONSTEXPR_FCN timestamp_t now() { return 0; }
    inline EASY_CONSTEXPR_FCN timestamp_t toNanoseconds(timestamp_t) { return 0; }
    inline EASY_CONSTEXPR_FCN timestamp_t toMicroseconds(timestamp_t) { return 0; }
    inline const BaseBlockDescriptor* registerDescription(EasyBlockStatus, const char*, const char*, const char*, int, block_type_t, color_t, bool = false)
    { return reinterpret_cast<const BaseBlockDescriptor*>(0xbad); }
    inline void endBlock() { }
    inline void setEnabled(bool) { }
    inline EASY_CONSTEXPR_FCN bool isEnabled() { return false; }
    inline void storeEvent(const BaseBlockDescriptor*, const char* = "") { }
    inline void storeBlock(const BaseBlockDescriptor*, const char*, timestamp_t, timestamp_t) { }
    inline void beginBlock(Block&) { }
    inline void beginNonScopedBlock(const BaseBlockDescriptor*, const char* = "") { }
    inline uint32_t dumpBlocksToFile(const char*) { return 0; }
    inline const char* registerThreadScoped(const char*, ThreadGuard&) { return ""; }
    inline const char* registerThread(const char*) { return ""; }
    inline void setEventTracingEnabled(bool) { }
    inline EASY_CONSTEXPR_FCN bool isEventTracingEnabled() { return false; }
    inline void setLowPriorityEventTracing(bool) { }
    inline EASY_CONSTEXPR_FCN bool isLowPriorityEventTracing() { return false; }
    inline void setContextSwitchLogFilename(const char*) { }
    inline EASY_CONSTEXPR_FCN const char* getContextSwitchLogFilename() { return ""; }
    inline void startListen(uint16_t = ::profiler::DEFAULT_PORT) { }
    inline void stopListen() { }
    inline EASY_CONSTEXPR_FCN bool isListening() { return false; }
    inline EASY_CONSTEXPR_FCN uint8_t versionMajor() { return 0; }
    inline EASY_CONSTEXPR_FCN uint8_t versionMinor() { return 0; }
    inline EASY_CONSTEXPR_FCN uint16_t versionPatch() { return 0; }
    inline EASY_CONSTEXPR_FCN uint32_t version() { return 0; }
    inline EASY_CONSTEXPR_FCN const char* versionName() { return "v0.0.0_disabled"; }
    inline EASY_CONSTEXPR_FCN bool isMainThread() { return false; }
    inline EASY_CONSTEXPR_FCN timestamp_t this_thread_frameTime(Duration = ::profiler::MICROSECONDS) { return 0; }
    inline EASY_CONSTEXPR_FCN timestamp_t this_thread_frameTimeLocalMax(Duration = ::profiler::MICROSECONDS) { return 0; }
    inline EASY_CONSTEXPR_FCN timestamp_t this_thread_frameTimeLocalAvg(Duration = ::profiler::MICROSECONDS) { return 0; }
    inline EASY_CONSTEXPR_FCN timestamp_t main_thread_frameTime(Duration = ::profiler::MICROSECONDS) { return 0; }
    inline EASY_CONSTEXPR_FCN timestamp_t main_thread_frameTimeLocalMax(Duration = ::profiler::MICROSECONDS) { return 0; }
    inline EASY_CONSTEXPR_FCN timestamp_t main_thread_frameTimeLocalAvg(Duration = ::profiler::MICROSECONDS) { return 0; }
#endif

    /** API functions binded to current thread.

    \ingroup profiler
    */
    namespace this_thread {

        inline const char* registrate(const char* _name) {
            return ::profiler::registerThread(_name);
        }

        inline const char* registrate(const char* _name, ThreadGuard& _threadGuard) {
            return ::profiler::registerThreadScoped(_name, _threadGuard);
        }

        inline timestamp_t frameTime(Duration _durationCast = ::profiler::MICROSECONDS) {
            return ::profiler::this_thread_frameTime(_durationCast);
        }

        inline timestamp_t frameTimeLocalMax(Duration _durationCast = ::profiler::MICROSECONDS) {
            return ::profiler::this_thread_frameTimeLocalMax(_durationCast);
        }

        inline timestamp_t frameTimeLocalAvg(Duration _durationCast = ::profiler::MICROSECONDS) {
            return ::profiler::this_thread_frameTimeLocalAvg(_durationCast);
        }

        inline bool isMain() {
            return ::profiler::isMainThread();
        }

    } // END of namespace this_thread.

    /** API functions binded to main thread.

    Could be called from any thread.

    \ingroup profiler
    */
    namespace main_thread {

        inline timestamp_t frameTime(Duration _durationCast = ::profiler::MICROSECONDS) {
            return ::profiler::main_thread_frameTime(_durationCast);
        }

        inline timestamp_t frameTimeLocalMax(Duration _durationCast = ::profiler::MICROSECONDS) {
            return ::profiler::main_thread_frameTimeLocalMax(_durationCast);
        }

        inline timestamp_t frameTimeLocalAvg(Duration _durationCast = ::profiler::MICROSECONDS) {
            return ::profiler::main_thread_frameTimeLocalAvg(_durationCast);
        }

        /** Always returns true.
        */
        inline EASY_CONSTEXPR_FCN bool isMain() {
            return true;
        }

    } // END of namespace main_thread.

    /** Alias for isEnabled().

    Added for clarification.

    \sa isEnabled

    \ingroup profiler
    */
    EASY_FORCE_INLINE bool isCapturing() { return isEnabled(); }

    /** Alias for EASY_PROFILER_ENABLE.

    Added for clarification.

    \sa EASY_PROFILER_ENABLE

    \ingroup profiler
    */
    EASY_FORCE_INLINE void startCapture() { EASY_PROFILER_ENABLE; }

    /** Alias for EASY_PROFILER_DISABLE.

    Added for clarification.

    \sa EASY_PROFILER_DISABLE

    \ingroup profiler
    */
    EASY_FORCE_INLINE void stopCapture() { EASY_PROFILER_DISABLE; }

    /** Alias for now().

    \ingroup profiler
    */
    EASY_FORCE_INLINE timestamp_t currentTime() { return now(); }

    //////////////////////////////////////////////////////////////////////

} // END of namespace profiler.

#if defined ( __clang__ )
# pragma clang diagnostic pop
#endif

#endif // EASY_PROFILER_H
