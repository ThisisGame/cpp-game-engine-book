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
#include "eventpp/eventdispatcher.h"

#include <thread>
#include <numeric>
#include <random>
#include <algorithm>

TEST_CASE("EventDispatcher, multi threading, int, void (int)")
{
	using ED = eventpp::EventDispatcher<int, void (int)>;
	ED dispatcher;

	constexpr int threadCount = 256;
	constexpr int eventCountPerThread = 1024 * 4;
	constexpr int itemCount = threadCount * eventCountPerThread;

	std::vector<int> eventList(itemCount);
	std::iota(eventList.begin(), eventList.end(), 0);
	std::shuffle(eventList.begin(), eventList.end(), std::mt19937(std::random_device()()));

	std::vector<int> dataList(itemCount);
	std::vector<ED::Handle> handleList(itemCount);

	std::vector<std::thread> threadList;

	for(int i = 0; i < threadCount; ++i) {
		threadList.emplace_back([i, eventCountPerThread, &dispatcher, &eventList, &handleList, &dataList]() {
			for(int k = i * eventCountPerThread; k < (i + 1) * eventCountPerThread; ++k) {
				handleList[k] = dispatcher.appendListener(eventList[k], [&dispatcher, k, &dataList, &eventList, &handleList](const int e) {
					dataList[k] += e;
					dispatcher.removeListener(eventList[k], handleList[k]);
				});
			}
		});
	}
	for(int i = 0; i < threadCount; ++i) {
		threadList[i].join();
	}

	threadList.clear();
	for(int i = 0; i < threadCount; ++i) {
		threadList.emplace_back([i, eventCountPerThread, &dispatcher, &eventList]() {
			for(int k = i * eventCountPerThread; k < (i + 1) * eventCountPerThread; ++k) {
				dispatcher.dispatch(eventList[k]);
			}
		});
	}
	for(int i = 0; i < threadCount; ++i) {
		threadList[i].join();
	}

	std::sort(eventList.begin(), eventList.end());
	std::sort(dataList.begin(), dataList.end());

	REQUIRE(eventList == dataList);
}

