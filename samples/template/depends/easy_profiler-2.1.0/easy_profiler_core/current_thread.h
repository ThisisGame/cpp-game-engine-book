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

#ifndef EASY_PROFILER_CURRENT_THREAD_H
#define EASY_PROFILER_CURRENT_THREAD_H

#include <easy/details/profiler_public_types.h>

#ifdef _WIN32
# include <Windows.h>
#elif defined(__APPLE__)
# include <pthread.h>
# include <Availability.h>
#elif defined(__QNX__)
# include <sys/mman.h>
# include <process.h>
# define __NR_gettid SYS_gettid
#else
# include <sys/types.h>
# include <unistd.h>
# include <sys/syscall.h>
#endif

inline profiler::thread_id_t getCurrentThreadId()
{
#ifdef _WIN32
    return (profiler::thread_id_t)::GetCurrentThreadId();
#elif defined(__APPLE__)
#   if (defined(__MAC_OS_X_VERSION_MIN_REQUIRED) && __MAC_OS_X_VERSION_MIN_REQUIRED >= __MAC_10_6) || \
       (defined(__IPHONE_OS_VERSION_MIN_REQUIRED) && __IPHONE_OS_VERSION_MIN_REQUIRED >= __IPHONE_8_0)
    static EASY_THREAD_LOCAL uint64_t _id = 0;
    if (!_id)
        pthread_threadid_np(NULL, &_id);
    return (profiler::thread_id_t)_id;
#   else
    return (profiler::thread_id_t)pthread_self();
#   endif
#elif defined(__QNX__)
    EASY_THREAD_LOCAL static const profiler::thread_id_t _id = (profiler::thread_id_t)gettid();
    return _id;
#else
    EASY_THREAD_LOCAL static const profiler::thread_id_t _id = (profiler::thread_id_t)syscall(__NR_gettid);
    return _id;
#endif
}

#endif // EASY_PROFILER_CURRENT_THREAD_H
