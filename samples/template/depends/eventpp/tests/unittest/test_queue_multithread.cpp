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
#include <numeric>
#include <random>
#include <algorithm>

TEST_CASE("EventQueue, multi threading, int, void (int)")
{
	using EQ = eventpp::EventQueue<int, void (int)>;
	EQ queue;

	constexpr int threadCount = 256;
	constexpr int dataCountPerThread = 1024 * 4;
	constexpr int itemCount = threadCount * dataCountPerThread;

	std::vector<int> eventList(itemCount);
	std::iota(eventList.begin(), eventList.end(), 0);
	std::shuffle(eventList.begin(), eventList.end(), std::mt19937(std::random_device()()));

	std::vector<int> dataList(itemCount);

	for(int i = 0; i < itemCount; ++i) {
		queue.appendListener(eventList[i], [&queue, i, &dataList](const int d) {
			dataList[i] += d;
		});
	}

	std::vector<std::thread> threadList;
	for(int i = 0; i < threadCount; ++i) {
		threadList.emplace_back([i, dataCountPerThread, &queue, itemCount]() {
			for(int k = i * dataCountPerThread; k < (i + 1) * dataCountPerThread; ++k) {
				queue.enqueue(k, 3);
			}
			for(int k = 0; k < 10; ++k) {
				queue.process();
			}
		});
	}
	for(int i = 0; i < threadCount; ++i) {
		threadList[i].join();
	}

	std::vector<int> compareList(itemCount);
	std::fill(compareList.begin(), compareList.end(), 3);
	REQUIRE(dataList == compareList);
}

TEST_CASE("EventQueue, multi threading, one thread waits")
{
	using EQ = eventpp::EventQueue<int, void (int)>;
	EQ queue;

	// note, all events will be process from the other thread instead of main thread
	constexpr int stopEvent = 1;
	constexpr int otherEvent = 2;

	constexpr int itemCount = 5;

	std::vector<int> dataList(itemCount);

	std::atomic<int> threadProcessCount(0);

	std::thread thread([stopEvent, otherEvent, &dataList, &queue, &threadProcessCount]() {
		volatile bool shouldStop = false;
		queue.appendListener(stopEvent, [&shouldStop](int) {
			shouldStop = true;
		});
		queue.appendListener(otherEvent, [&dataList](const int index) {
			dataList[index] += index + 1;
		});

		while(! shouldStop) {
			queue.wait();

			++threadProcessCount;

			queue.process();
		}
	});
	
	REQUIRE(threadProcessCount.load() == 0);

	auto waitUntilQueueEmpty = [&queue]() {
		while(queue.waitFor(std::chrono::nanoseconds(0))) ;
	};

	SECTION("Enqueue one by one") {
		queue.enqueue(otherEvent, 1);
		waitUntilQueueEmpty();
		REQUIRE(threadProcessCount.load() == 1);
		REQUIRE(queue.emptyQueue());
		REQUIRE(dataList == std::vector<int>{ 0, 2, 0, 0, 0 });

		queue.enqueue(otherEvent, 3);
		waitUntilQueueEmpty();
		REQUIRE(threadProcessCount.load() == 2);
		REQUIRE(queue.emptyQueue());
		REQUIRE(dataList == std::vector<int>{ 0, 2, 0, 4, 0 });
	}

	SECTION("Enqueue two") {
		queue.enqueue(otherEvent, 1);
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
		REQUIRE(threadProcessCount.load() == 1);
		REQUIRE(queue.emptyQueue());

		queue.enqueue(otherEvent, 3);
		waitUntilQueueEmpty();

		REQUIRE(threadProcessCount.load() == 2);
		REQUIRE(dataList == std::vector<int>{ 0, 2, 0, 4, 0 });
	}

	SECTION("Batching enqueue") {
		{
			EQ::DisableQueueNotify disableNotify(&queue);

			queue.enqueue(otherEvent, 2);
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
			REQUIRE(threadProcessCount.load() == 0);
			REQUIRE(! queue.emptyQueue());

			queue.enqueue(otherEvent, 4);
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
			REQUIRE(threadProcessCount.load() == 0);
			REQUIRE(! queue.emptyQueue());
		}

		waitUntilQueueEmpty();
		REQUIRE(threadProcessCount.load() == 1);
		REQUIRE(dataList == std::vector<int>{ 0, 0, 3, 0, 5 });
	}

	queue.enqueue(stopEvent, 1);
	thread.join();
}

TEST_CASE("EventQueue, multi threading, many threads wait")
{
	using EQ = eventpp::EventQueue<int, void (int)>;
	EQ queue;

	// note, all events will be process from the other thread instead of main thread
	constexpr int stopEvent = 1;
	constexpr int otherEvent = 2;

	constexpr int unit = 3;
	constexpr int itemCount = 30 * unit;

	std::vector<int> dataList(itemCount);

	std::vector<std::thread> threadList;

	std::atomic<bool> shouldStop(false);

	queue.appendListener(stopEvent, [&shouldStop](int) {
		shouldStop = true;
	});
	queue.appendListener(otherEvent, [&dataList](const int index) {
		++dataList[index];
	});

	for(int i = 0; i < itemCount; ++i) {
		threadList.emplace_back([stopEvent, otherEvent, i, &dataList, &queue, &shouldStop]() {
			for(;;) {
				// can't use queue.wait() because the thread can't be waken up by shouldStop = true
				while(! queue.waitFor(std::chrono::milliseconds(10)) && ! shouldStop.load()) ;

				if(shouldStop.load()) {
					break;
				}

				queue.process();
			}
		});
	}

	for(int i = 0; i < itemCount; ++i) {
		queue.enqueue(otherEvent, i);
		std::this_thread::sleep_for(std::chrono::milliseconds(0));
	}

	for(int i = 0; i < itemCount; i += unit) {
		EQ::DisableQueueNotify disableNotify(&queue);
		for(int k = 0; k < unit; ++k) {
			queue.enqueue(otherEvent, i);
			std::this_thread::sleep_for(std::chrono::milliseconds(0));
		}
	}

	queue.enqueue(stopEvent);

	for(auto & thread : threadList) {
		thread.join();
	}

	REQUIRE(std::accumulate(dataList.begin(), dataList.end(), 0) == itemCount * 2);
}

