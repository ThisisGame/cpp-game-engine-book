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

#ifndef EASY_PROFILER_SPIN_LOCK_H
#define EASY_PROFILER_SPIN_LOCK_H

#define EASY_USE_CRITICAL_SECTION // Use CRITICAL_SECTION instead of std::atomic_flag

#if defined(_WIN32) && defined(EASY_USE_CRITICAL_SECTION)
#include <Windows.h>
#else
#include <atomic>
#endif

namespace profiler {

#if defined(_WIN32) && defined(EASY_USE_CRITICAL_SECTION)
    // std::atomic_flag on Windows works slower than critical section, so we will use it instead of std::atomic_flag...
    // By the way, Windows critical sections are slower than std::atomic_flag on Unix.
    class spin_lock { CRITICAL_SECTION m_lock; public:

        void lock() {
            EnterCriticalSection(&m_lock);
        }

        void unlock() {
            LeaveCriticalSection(&m_lock);
        }

        spin_lock() {
            InitializeCriticalSection(&m_lock);
        }

        ~spin_lock() {
            DeleteCriticalSection(&m_lock);
        }
    };
#else
    // std::atomic_flag on Unix works fine and very fast (almost instant!)
    class spin_lock { ::std::atomic_flag m_lock; public:

        void lock() {
            while (m_lock.test_and_set(::std::memory_order_acquire));
        }

        void unlock() {
            m_lock.clear(::std::memory_order_release);
        }

        spin_lock() {
            m_lock.clear();
        }
    };
#endif

    template <class T>
    class guard_lock
    {
        T& m_lock;
        bool m_isLocked = false;

    public:

        explicit guard_lock(T& m) : m_lock(m) {
            m_lock.lock();
            m_isLocked = true;
        }

        ~guard_lock() {
            unlock();
        }

        inline void unlock() {
            if (m_isLocked) {
                m_lock.unlock();
                m_isLocked = false;
            }
        }
    };

} // END of namespace profiler.

#ifdef EASY_USE_CRITICAL_SECTION
# undef EASY_USE_CRITICAL_SECTION
#endif

#endif // EASY_PROFILER_SPIN_LOCK_H
