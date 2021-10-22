#include <chrono>
#include <thread>
#include <vector>
#include <iostream>
#include <condition_variable>
#include <cstdlib>
#include <math.h>
#include <type_traits>

#include <unistd.h>
#include <sys/syscall.h>
#include <sys/time.h>


static inline uint64_t now()
{
#if defined(__i386__)
      int64_t ret;
      __asm__ volatile("rdtsc" : "=A"(ret));
      return ret;
#elif defined(__x86_64__) || defined(__amd64__)
      uint64_t low, high;
      __asm__ volatile("rdtsc" : "=a"(low), "=d"(high));
      return (high << 32) | low;
#endif
}

int OBJECTS = 500;
#define STR(x) #x

int64_t calculate_cpu_frequency()//per sec
{
    double g_TicksPerNanoSec;
    struct timespec begints, endts;
    uint64_t begin = 0, end = 0;
    clock_gettime(CLOCK_MONOTONIC, &begints);
    begin = now();
    volatile uint64_t i;
    for (i = 0; i < 100000000; i++); /* must be CPU intensive */
    end = now();
    clock_gettime(CLOCK_MONOTONIC, &endts);
    struct timespec tmpts;
    const int NANO_SECONDS_IN_SEC = 1000000000;
    tmpts.tv_sec = endts.tv_sec - begints.tv_sec;
    tmpts.tv_nsec = endts.tv_nsec - begints.tv_nsec;
    if (tmpts.tv_nsec < 0)
    {
        tmpts.tv_sec--;
        tmpts.tv_nsec += NANO_SECONDS_IN_SEC;
    }

    uint64_t nsecElapsed = tmpts.tv_sec * 1000000000LL + tmpts.tv_nsec;
    g_TicksPerNanoSec = (double)(end - begin) / (double)nsecElapsed;

    int64_t cpu_frequency = int(g_TicksPerNanoSec * 1000);

    return cpu_frequency;
}

const auto CPU_FREQUENCY = calculate_cpu_frequency();

# define TICKS_TO_US(ticks) ticks / CPU_FREQUENCY

void localSleep(int magic=200000)
{
    volatile int i = 0;
    for (; i < magic; ++i);
}

template<class Clock>
auto calcDelta(int magic=200000) -> decltype(Clock::now().time_since_epoch().count())
{
    auto start = Clock::now().time_since_epoch().count();
    localSleep(magic);
    auto end = Clock::now().time_since_epoch().count();
    return end - start;
}

template<class Clock>
double calcDuration(int objects)
{
    const auto frequency = Clock::period::den / Clock::period::num;

    auto start = Clock::now();

    decltype(Clock::now().time_since_epoch().count()) summ = 0;

    for (int i=0; i < objects; i++)
    {
        summ += calcDelta<Clock>();
    }

    summ = summ * 1000000LL / frequency;

    auto end = Clock::now();
    auto elapsed =
            std::chrono::duration_cast<std::chrono::microseconds>(end - start);


    return (elapsed.count()-summ)/double(objects)/2.0;
}

uint64_t calcDeltaRdtsc(int magic=200000)
{
    auto start = now();
    localSleep(magic);
    auto end = now();
    return end - start;
}

double calcDurationByRdtsc(int objects)
{
    auto start = now();

    uint64_t summ = 0;

    for (int i=0; i < objects; i++)
    {
        summ += calcDeltaRdtsc();
    }

    auto end = now();
    return TICKS_TO_US((end - start - summ))/double(objects)/2.0;
}

uint64_t calcDeltaSysCall(int magic, int type)
{
    timespec tp0,tp1;
    syscall(SYS_clock_gettime, type, &tp0);
    auto start = tp0.tv_sec*1000000000+tp0.tv_nsec;
    localSleep(magic);
    syscall(SYS_clock_gettime, type, &tp1);
    auto end = tp1.tv_sec*1000000000+tp1.tv_nsec;
    return end - start;
}

double calcDurationBySyscall(int objects, int type)
{
    timespec tp0,tp1;
    syscall(SYS_clock_gettime, type, &tp0);
    auto start = tp0.tv_sec*1000000000+tp0.tv_nsec;

    uint64_t summ = 0;

    for (int i=0; i < objects; i++)
    {
        summ += calcDeltaSysCall(200000,type);
    }

    syscall(SYS_clock_gettime, type, &tp1);
    auto end = tp1.tv_sec*1000000000+tp1.tv_nsec;
    return (end - start - summ)/double(objects)/2.0/1000.0;
}

uint64_t calcDeltaSysGetTime(int magic, int type)
{
    timespec tp0,tp1;
    clock_gettime(type, &tp0);
    auto start = tp0.tv_sec*1000000000+tp0.tv_nsec;
    localSleep(magic);
    clock_gettime(type, &tp1);
    auto end = tp1.tv_sec*1000000000+tp1.tv_nsec;
    return end - start;
}

double calcDurationByGetTime(int objects, int type)
{
    timespec tp0,tp1;
    clock_gettime(type, &tp0);
    auto start = tp0.tv_sec*1000000000+tp0.tv_nsec;

    uint64_t summ = 0;

    for (int i=0; i < objects; i++)
    {
        summ += calcDeltaSysGetTime(200000,type);
    }

    clock_gettime(type, &tp1);
    auto end = tp1.tv_sec*1000000000+tp1.tv_nsec;
    return (end - start - summ)/double(objects)/2.0/1000.0;
}

uint64_t calcDeltaSysGetTimeOfDay(int magic=200000)
{
    timeval tv0,tv1;
    gettimeofday(&tv0,0);
    auto start = tv0.tv_sec*1000000+tv0.tv_usec;
    localSleep(magic);
    gettimeofday(&tv1, 0);
    auto end = tv1.tv_sec*1000000+tv1.tv_usec;
    return end - start;
}

double calcDurationByGetTimeOfDay(int objects)
{
    timeval tv0,tv1;
    gettimeofday(&tv0,0);
    auto start = tv0.tv_sec*1000000+tv0.tv_usec;

    uint64_t summ = 0;

    for (int i=0; i < objects; i++)
    {
        summ += calcDeltaSysGetTimeOfDay();
    }

    gettimeofday(&tv1, 0);
    auto end = tv1.tv_sec*1000000+tv1.tv_usec;
    return (end - start - summ)/double(objects)/2.0;
}

int main(int argc, char* argv[])
{
    if (argc > 1 && argv[1]){
        OBJECTS = std::atoi(argv[1]);
    }


    std::cout << STR(std::chrono::steady_clock) << ": "<<calcDuration<std::chrono::steady_clock>(OBJECTS) << " usec\n";
    std::cout << STR(std::chrono::high_resolution_clock)<< ": " << calcDuration<std::chrono::high_resolution_clock>(OBJECTS) << " usec\n";
    std::cout << STR(std::chrono::system_clock)<< ": " << calcDuration<std::chrono::system_clock>(OBJECTS) << " usec\n";

    std::cout << "\n";

    std::cout << "rdtsc: " << calcDurationByRdtsc(OBJECTS) << " usec\n";

    std::cout << "\n";

    std::cout << "syscall(SYS_clock_gettime, CLOCK_MONOTONIC): " << calcDurationBySyscall(OBJECTS,CLOCK_MONOTONIC) << " usec\n";
    std::cout << "syscall(SYS_clock_gettime, CLOCK_REALTIME): " << calcDurationBySyscall(OBJECTS,CLOCK_REALTIME) << " usec\n";
    std::cout << "syscall(SYS_clock_gettime, CLOCK_MONOTONIC_RAW): " << calcDurationBySyscall(OBJECTS,CLOCK_MONOTONIC_RAW) << " usec\n";
    std::cout << "syscall(SYS_clock_gettime, CLOCK_MONOTONIC_COARSE): " << calcDurationBySyscall(OBJECTS,CLOCK_MONOTONIC_COARSE) << " usec\n";
    std::cout << "syscall(SYS_clock_gettime, CLOCK_REALTIME_COARSE): " << calcDurationBySyscall(OBJECTS,CLOCK_REALTIME_COARSE) << " usec\n";

    std::cout << "\n";

    std::cout << "clock_gettime(CLOCK_MONOTONIC): " << calcDurationByGetTime(OBJECTS,CLOCK_MONOTONIC) << " usec\n";
    std::cout << "clock_gettime(CLOCK_REALTIME): " << calcDurationByGetTime(OBJECTS,CLOCK_REALTIME) << " usec\n";
    std::cout << "clock_gettime(CLOCK_MONOTONIC_RAW): " << calcDurationByGetTime(OBJECTS,CLOCK_MONOTONIC_RAW) << " usec\n";
    std::cout << "clock_gettime(CLOCK_MONOTONIC_COARSE): " << calcDurationByGetTime(OBJECTS,CLOCK_MONOTONIC_COARSE) << " usec\n";
    std::cout << "clock_gettime(CLOCK_REALTIME_COARSE): " << calcDurationByGetTime(OBJECTS,CLOCK_REALTIME_COARSE) << " usec\n";

    std::cout << "\n";

    std::cout << "gettimeofday(): " << calcDurationByGetTimeOfDay(OBJECTS) << " usec\n";

    return 0;
}
