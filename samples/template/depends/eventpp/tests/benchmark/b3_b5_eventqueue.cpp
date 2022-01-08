// eventpp library
// Copyright (C) 2018 Wang Qi (wqking)
// Github: https://github.com/wqking/eventpp
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//   http://www.apache.org/licenses/LICENSE-2.0
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "test.h"
#include "eventpp/eventqueue.h"

#include <thread>
#include <vector>

namespace {

template <typename Policies>
void doExecuteEventQueue(
		const std::string & message,
		const size_t queueSize,
		const size_t iterateCount,
		const size_t eventCount,
		size_t listenerCount = 0
	)
{
	using EQ = eventpp::EventQueue<size_t, void (size_t), Policies>;
	EQ eventQueue;
	
	if(listenerCount == 0) {
		listenerCount = eventCount;
	}

	for(size_t i = 0; i < listenerCount; ++i) {
		eventQueue.appendListener(i % eventCount, [](size_t) {});
	}
	
	const uint64_t time = measureElapsedTime([
			queueSize,
			iterateCount,
			eventCount,
			listenerCount,
			&eventQueue
		]{
		for(size_t iterate = 0; iterate < iterateCount; ++iterate) {
			for(size_t i = 0; i < queueSize; ++i) {
				eventQueue.enqueue(i % eventCount);
			}
			eventQueue.process();
		}
	});
	
	std::cout
		<< message
		<< " queueSize: " << queueSize
		<< " iterateCount: " << iterateCount
		<< " eventCount: " << eventCount
		<< " listenerCount: " << listenerCount
		<< " Time: " << time
		<< std::endl;
	;
}

template <typename Policies>
void doMultiThreadingExecuteEventQueue(
		const std::string & message,
		const size_t enqueueThreadCount,
		const size_t processThreadCount,
		const size_t totalEventCount,
		const size_t eventCount,
		size_t listenerCount = 0
	)
{
	using EQ = eventpp::EventQueue<size_t, void (size_t), Policies>;
	EQ eventQueue;
	
	if(listenerCount == 0) {
		listenerCount = eventCount;
	}

	for(size_t i = 0; i < listenerCount; ++i) {
		eventQueue.appendListener(i % eventCount, [](size_t) { });
	}
	
	std::atomic<bool> start(false);
	std::atomic<bool> stop(false);
	std::vector<std::thread> enqueueThreadList;
	std::vector<std::thread> processThreadList;
	for(size_t i = 0; i < enqueueThreadCount; ++i) {
		const size_t begin = i * (totalEventCount / enqueueThreadCount);
		const size_t end = (i == enqueueThreadCount - 1 ? totalEventCount : start + totalEventCount / enqueueThreadCount);
		enqueueThreadList.emplace_back([&start, begin, end, &eventQueue, eventCount]() {
			while(! start.load()) {
			}

			for(size_t i = begin; i < end; ++i) {
				eventQueue.enqueue(i % eventCount);
			}
		});
	}

	for(size_t i = 0; i < processThreadCount; ++i) {
		processThreadList.emplace_back([&start, &stop, &eventQueue]() {
			while(! start.load()) {
			}

			while(! stop.load() || eventQueue.processOne()) {
			}

			while(eventQueue.processOne()) {
			}
		});
	}

	const uint64_t time = measureElapsedTime([
			&start,
			&stop,
			&enqueueThreadList,
			&processThreadList,
			totalEventCount,
			eventCount,
			listenerCount,
			&eventQueue
		]{
		start.store(true);

		for(auto & thread : enqueueThreadList) {
			thread.join();
		}

		stop.store(true);
		
		for(auto & thread : processThreadList) {
			thread.join();
		}
	});
	
	std::cout
		<< message
		<< " enqueueThreadCount: " << enqueueThreadCount
		<< " processThreadCount: " << processThreadCount
		<< " totalEventCount: " << totalEventCount
		<< " eventCount: " << eventCount
		<< " listenerCount: " << listenerCount
		<< " Time: " << time
		<< std::endl;
	;
}


} //unnamed namespace

// To avoid warning "typedef locally defined but not used" in GCC,
// let's move the policies outside of the test case functions

struct B3PoliciesMultiThreading {
	using Threading = eventpp::MultipleThreading;
};
struct B3PoliciesSingleThreading {
	using Threading = eventpp::SingleThreading;
};

TEST_CASE("b3, EventQueue, one thread")
{
	doExecuteEventQueue<B3PoliciesMultiThreading>("Multi threading", 100, 1000 * 100, 100);
	doExecuteEventQueue<B3PoliciesMultiThreading>("Multi threading", 1000, 1000 * 100, 100);
	doExecuteEventQueue<B3PoliciesMultiThreading>("Multi threading", 1000, 1000 * 100, 1000);

	doExecuteEventQueue<B3PoliciesSingleThreading>("Single threading", 100, 1000 * 100, 100);
	doExecuteEventQueue<B3PoliciesSingleThreading>("Single threading", 1000, 1000 * 100, 100);
	doExecuteEventQueue<B3PoliciesSingleThreading>("Single threading", 1000, 1000 * 100, 1000);
}

struct B4PoliciesMultiThreading {
	using Threading = eventpp::GeneralThreading<std::mutex>;
};

TEST_CASE("b4, EventQueue, multi threads, mutex")
{
	doMultiThreadingExecuteEventQueue<B4PoliciesMultiThreading>("Mutex", 1, 1, 1000 * 1000 * 10, 100);
	doMultiThreadingExecuteEventQueue<B4PoliciesMultiThreading>("Mutex", 1, 3, 1000 * 1000 * 10, 100);
	doMultiThreadingExecuteEventQueue<B4PoliciesMultiThreading>("Mutex", 2, 2, 1000 * 1000 * 10, 100);
	doMultiThreadingExecuteEventQueue<B4PoliciesMultiThreading>("Mutex", 4, 4, 1000 * 1000 * 10, 100);
	doMultiThreadingExecuteEventQueue<B4PoliciesMultiThreading>("Mutex", 16, 16, 1000 * 1000 * 10, 100);
}

struct B5PoliciesMultiThreading {
	using Threading = eventpp::GeneralThreading<eventpp::SpinLock>;
};

TEST_CASE("b5, EventQueue, multi threads, spinlock")
{
	doMultiThreadingExecuteEventQueue<B5PoliciesMultiThreading>("Spinlock", 1, 1, 1000 * 1000 * 10, 100);
	doMultiThreadingExecuteEventQueue<B5PoliciesMultiThreading>("Spinlock", 1, 3, 1000 * 1000 * 10, 100);
	doMultiThreadingExecuteEventQueue<B5PoliciesMultiThreading>("Spinlock", 2, 2, 1000 * 1000 * 10, 100);
	doMultiThreadingExecuteEventQueue<B5PoliciesMultiThreading>("Spinlock", 4, 4, 1000 * 1000 * 10, 100);
	doMultiThreadingExecuteEventQueue<B5PoliciesMultiThreading>("Spinlock", 16, 16, 1000 * 1000 * 10, 100);
}

