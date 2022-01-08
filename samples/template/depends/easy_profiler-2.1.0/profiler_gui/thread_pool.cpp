/************************************************************************
* file name         : thread_pool.cpp
* ----------------- :
* creation time     : 2018/01/28
* author            : Victor Zarubkin
* email             : v.s.zarubkin@gmail.com
* ----------------- :
* description       : The file contains implementation of ThreadPool.
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

#include "thread_pool.h"
#include <algorithm>

#ifdef _MSC_VER
// std::back_inserter is defined in <iterator> for Visual C++ ...
#include <iterator>
#endif

#ifdef _WIN32
// For including SetThreadPriority()
# include <Windows.h>
# ifdef __MINGW32__
#  include <processthreadsapi.h>
# endif
#elif !defined(__APPLE__)
// For including pthread_setschedprio()
# include <pthread.h>
#else
# pragma message "TODO: include proper headers to be able to use pthread_setschedprio() on OSX and iOS (pthread.h is not enough...)"
#endif

static void setLowestThreadPriority()
{
#ifdef _WIN32
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_LOWEST);
#elif !defined(__APPLE__)
    pthread_attr_t attr;
    if (pthread_attr_init(&attr) == 0)
    {
        int policy = 0;
        if (pthread_attr_getschedpolicy(&attr, &policy) == 0)
            pthread_setschedprio(pthread_self(), sched_get_priority_min(policy));
        pthread_attr_destroy(&attr);
    }
#else
    /// TODO: include proper headers to be able to use pthread_setschedprio() on OSX and iOS (pthread.h is not enough...)
#endif
}

ThreadPool& ThreadPool::instance()
{
    static ThreadPool pool;
    return pool;
}

ThreadPool::ThreadPool()
{
    auto count = std::thread::hardware_concurrency();
#ifdef EASY_THREADPOOL_SEPARATE_QT_THREAD
    if (count > 1)
        count -= 1;
    m_threads.reserve(count + 2);
#else
    m_threads.reserve(count + 1);
#endif

    // N threads for main tasks
    std::generate_n(std::back_inserter(m_threads), count, [this] {
        return std::thread(&ThreadPool::tasksWorker, this, std::ref(m_tasks));
    });

#ifdef EASY_THREADPOOL_SEPARATE_QT_THREAD
    // FIXME: Workaround for Qt on Linux
    //
    // There is a memory leak in Qt when creating Qt objects in several separate threads
    // and removing them in one another (different) thread.
    //
    // But there is no memory leak when there is only one separate thread (apart from main thread)
    // for creating Qt objects and one separate (different!) thread for removing them.
    //
    m_threads.emplace_back(&ThreadPool::tasksWorker, this, std::ref(m_qtasks));
#endif

    // One thread for background jobs
    m_threads.emplace_back(&ThreadPool::jobsWorker, this);
}

ThreadPool::~ThreadPool()
{
    m_interrupt.store(true, std::memory_order_release);
    m_tasks.cv.notify_all();
#ifdef EASY_THREADPOOL_SEPARATE_QT_THREAD
    m_qtasks.cv.notify_all();
#endif
    m_backgroundJobs.cv.notify_all();
    for (auto& thread : m_threads)
        thread.join();
}

void ThreadPool::backgroundJob(std::function<void()>&& func)
{
    m_backgroundJobs.mutex.lock();
    m_backgroundJobs.queue.push_back(std::move(func));
    m_backgroundJobs.mutex.unlock();
    m_backgroundJobs.cv.notify_one();
}

void ThreadPool::enqueue(ThreadPoolTask& task)
{
#ifdef EASY_THREADPOOL_SEPARATE_QT_THREAD
    if (task.creatingQtObjects())
        enqueue(task, m_qtasks);
    else
#endif
        enqueue(task, m_tasks);
}

void ThreadPool::dequeue(ThreadPoolTask& task)
{
#ifdef EASY_THREADPOOL_SEPARATE_QT_THREAD
    if (task.creatingQtObjects())
        dequeue(task, m_qtasks);
    else
#endif
        dequeue(task, m_tasks);
}

void ThreadPool::enqueue(ThreadPoolTask& task, TaskJobs& tasks)
{
    tasks.mutex.lock();
    tasks.queue.emplace_back(task);
    tasks.mutex.unlock();
    tasks.cv.notify_one();
}

void ThreadPool::dequeue(ThreadPoolTask& task, TaskJobs& tasks)
{
    const std::lock_guard<std::mutex> lock(tasks.mutex);

    if (task.status() != TaskStatus::Enqueued)
        return;

    for (auto it = tasks.queue.begin(); it != tasks.queue.end(); ++it)
    {
        if (&it->get() == &task)
        {
            tasks.queue.erase(it);
            break;
        }
    }
}

void ThreadPool::tasksWorker(Jobs<std::reference_wrapper<ThreadPoolTask> >& tasks)
{
    while (!m_interrupt.load(std::memory_order_acquire))
    {
        std::unique_lock<std::mutex> lock(tasks.mutex);
        tasks.cv.wait(lock, [this, &tasks] {
            return !tasks.queue.empty() || m_interrupt.load(std::memory_order_acquire);
        });

        while (!tasks.queue.empty() && !m_interrupt.load(std::memory_order_acquire)) // execute all available tasks
        {
            auto& task = tasks.queue.front().get();
            task.setStatus(TaskStatus::Processing);
            tasks.queue.pop_front();

            // unlock to permit tasks execution for other worker threads and for adding new tasks
            lock.unlock();

            // execute task
            task();

            // lock again to check if there are new tasks in the queue
            lock.lock();
        }
    }
}

void ThreadPool::jobsWorker()
{
    setLowestThreadPriority(); // Background thread has lowest priority

    while (!m_interrupt.load(std::memory_order_acquire))
    {
        std::unique_lock<std::mutex> lock(m_backgroundJobs.mutex);
        m_backgroundJobs.cv.wait(lock, [this] {
            return !m_backgroundJobs.queue.empty() || m_interrupt.load(std::memory_order_acquire);
        });

        while (!m_backgroundJobs.queue.empty()) // execute all available jobs
        {
            auto job = std::move(m_backgroundJobs.queue.front());
            m_backgroundJobs.queue.pop_front();

            // unlock to permit adding new jobs while executing current job
            lock.unlock();

            // execute job
            job();

            // lock again to check if there are new jobs in the queue
            lock.lock();
        }
    }
}
