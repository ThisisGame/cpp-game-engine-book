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

#ifndef EASY_PROFILER_CURRENT_TIME_H
#define EASY_PROFILER_CURRENT_TIME_H

#include <easy/details/profiler_public_types.h>

#if defined(_MSC_VER) && _MSC_VER <= 1800
// std::chrono for MSVC2013 is broken - it has very low resolution of 16ms
// restrict usage of std::chrono
# if EASY_CHRONO_HIGHRES_CLOCK
#  undef EASY_CHRONO_HIGHRES_CLOCK
# endif
# if EASY_CHRONO_STEADY_CLOCK
#  undef EASY_CHRONO_STEADY_CLOCK
# endif
#endif

#if EASY_CHRONO_HIGHRES_CLOCK
# include <chrono>
# define EASY_CHRONO_CLOCK std::chrono::high_resolution_clock
#elif EASY_CHRONO_STEADY_CLOCK
# include <chrono>
# define EASY_CHRONO_CLOCK std::chrono::steady_clock
#elif defined(_WIN32)
# include <Windows.h>
#else
# include <chrono>
# include <time.h>
# ifdef __ARM_ARCH
#  include <sys/time.h>
# endif//__ARM_ARCH
#endif
# ifdef __mips__
#  include <sys/time.h>
# endif//__mips__

namespace profiler { namespace clock {

static inline profiler::timestamp_t now()
{
#if EASY_CHRONO_HIGHRES_CLOCK || EASY_CHRONO_STEADY_CLOCK
    return (profiler::timestamp_t)EASY_CHRONO_CLOCK::now().time_since_epoch().count();
#elif defined(_WIN32)
    //see https://msdn.microsoft.com/library/windows/desktop/dn553408(v=vs.85).aspx
    LARGE_INTEGER elapsedMicroseconds;
    if (!QueryPerformanceCounter(&elapsedMicroseconds))
        return 0;
    return (profiler::timestamp_t)elapsedMicroseconds.QuadPart;
#else// not _WIN32

#if (defined(__GNUC__) || defined(__ICC))

    // part of code from google/benchmark library (Licensed under the Apache License, Version 2.0)
    // see https://github.com/google/benchmark/blob/master/src/cycleclock.h#L111
    #if defined(__i386__)
      int64_t ret;
      __asm__ volatile("rdtsc" : "=A"(ret));
      return ret;
    #elif defined(__x86_64__) || defined(__amd64__)
      uint64_t low, high;
      __asm__ volatile("rdtsc" : "=a"(low), "=d"(high));
      return (high << 32) | low;
    #elif defined(__powerpc__) || defined(__ppc__)
      // This returns a time-base, which is not always precisely a cycle-count.
      int64_t tbl, tbu0, tbu1;
      asm("mftbu %0" : "=r"(tbu0));
      asm("mftb  %0" : "=r"(tbl));
      asm("mftbu %0" : "=r"(tbu1));
      tbl &= -static_cast<int64>(tbu0 == tbu1);
      // high 32 bits in tbu1; low 32 bits in tbl  (tbu0 is garbage)
      return (tbu1 << 32) | tbl;
    #elif defined(__sparc__)
      int64_t tick;
      asm(".byte 0x83, 0x41, 0x00, 0x00");
      asm("mov   %%g1, %0" : "=r"(tick));
      return tick;
    #elif defined(__ia64__)
      int64_t itc;
      asm("mov %0 = ar.itc" : "=r"(itc));
      return itc;
    #elif defined(_MSC_VER) && defined(_M_IX86)
      // Older MSVC compilers (like 7.x) don't seem to support the
      // __rdtsc intrinsic properly, so I prefer to use _asm instead
      // when I know it will work.  Otherwise, I'll use __rdtsc and hope
      // the code is being compiled with a non-ancient compiler.
      _asm rdtsc
    #elif defined(_MSC_VER)
      return __rdtsc();
    #elif defined(__aarch64__)
      // System timer of ARMv8 runs at a different frequency than the CPU's.
      // The frequency is fixed, typically in the range 1-50MHz.  It can be
      // read at CNTFRQ special register.  We assume the OS has set up
      // the virtual timer properly.
      int64_t virtual_timer_value;
      asm volatile("mrs %0, cntvct_el0" : "=r"(virtual_timer_value));
      return virtual_timer_value;
    #elif defined(__ARM_ARCH)
    #if (__ARM_ARCH >= 6)  // V6 is the earliest arch that has a standard cyclecount
      uint32_t pmccntr;
      uint32_t pmuseren;
      uint32_t pmcntenset;
      // Read the user mode perf monitor counter access permissions.
      asm volatile("mrc p15, 0, %0, c9, c14, 0" : "=r"(pmuseren));
      if (pmuseren & 1) {  // Allows reading perfmon counters for user mode code.
        asm volatile("mrc p15, 0, %0, c9, c12, 1" : "=r"(pmcntenset));
        if (pmcntenset & 0x80000000ul) {  // Is it counting?
          asm volatile("mrc p15, 0, %0, c9, c13, 0" : "=r"(pmccntr));
          // The counter is set up to count every 64th cycle
          return static_cast<int64_t>(pmccntr) * 64;  // Should optimize to << 6
        }
      }
    #endif
      struct timeval tv;
      gettimeofday(&tv, nullptr);
      return static_cast<int64_t>(tv.tv_sec) * 1000000 + tv.tv_usec;
    #elif defined(__mips__)
      // mips apparently only allows rdtsc for superusers, so we fall
      // back to gettimeofday.  It's possible clock_gettime would be better.
      struct timeval tv;
      gettimeofday(&tv, nullptr);
      return static_cast<int64_t>(tv.tv_sec) * 1000000 + tv.tv_usec;
    #else
      #warning You need to define fast now() for your OS and CPU
      return std::chrono::high_resolution_clock::now().time_since_epoch().count();
    #define EASY_CHRONO_CLOCK std::chrono::high_resolution_clock
    #endif

#else // not _WIN32, __GNUC__, __ICC
    #warning You need to define fast now() for your OS and CPU
    return std::chrono::high_resolution_clock::now().time_since_epoch().count();
    #define EASY_CHRONO_CLOCK std::chrono::high_resolution_clock
#endif

#endif
}

} } // end of namespace profiler::clock.

#endif // EASY_PROFILER_CURRENT_TIME_H
