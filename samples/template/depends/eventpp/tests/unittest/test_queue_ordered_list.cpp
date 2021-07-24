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
#include "eventpp/utilities/orderedqueuelist.h"

#include <vector>
#include <numeric>

TEST_CASE("detectDataListOrder")
{
	std::vector<int> a { 1, 2, 3, 4, 5 };
	REQUIRE(detectDataListOrder(a.begin(), a.end()) == 1);

	std::vector<int> b { 5, 4, 3, 2, 1 };
	REQUIRE(detectDataListOrder(b.begin(), b.end()) == -1);

	std::vector<int> c { 4, 5, 3, 2, 1 };
	REQUIRE(detectDataListOrder(c.begin(), c.end()) == 0);

}
TEST_CASE("EventQueue, non-ordered list")
{
	constexpr int count = 100;
	eventpp::EventQueue<int, void ()> queue;

	std::vector<int> eventList(count);
	std::iota(eventList.begin(), eventList.end(), 1);
	for(int i = 0; i < count; i += 2) {
		std::swap(eventList[i], eventList[i + 1]);
	}
	REQUIRE(detectDataListOrder(eventList.begin(), eventList.end()) == 0);

	std::vector<int> dataList(count);

	int processedCount = 0;
	for(const int e : eventList) {
		queue.appendListener(e, [&dataList, e, &processedCount]() {
			dataList[processedCount] = e;
			++processedCount;
		});
	}

	for(const int e : eventList) {
		queue.enqueue(e);
	}
	queue.process();

	REQUIRE(dataList == eventList);
}

using MyCompareAscend = eventpp::OrderedQueueListCompare;
struct MyPolicyAscend
{
	template <typename Item>
	using QueueList = eventpp::OrderedQueueList<Item, MyCompareAscend >;
};

TEST_CASE("EventQueue, ordered list, ascend")
{
	constexpr int count = 100;
	eventpp::EventQueue<int, void (), MyPolicyAscend> queue;

	std::vector<int> eventList(count);
	std::iota(eventList.begin(), eventList.end(), 1);
	for(int i = 0; i < count; i += 2) {
		std::swap(eventList[i], eventList[i + 1]);
	}
	REQUIRE(detectDataListOrder(eventList.begin(), eventList.end()) == 0);

	std::vector<int> dataList(count);

	int processedCount = 0;
	for(const int e : eventList) {
		queue.appendListener(e, [&dataList, e, &processedCount]() {
			dataList[processedCount] = e;
			++processedCount;
		});
	}

	for(const int e : eventList) {
		queue.enqueue(e);
	}
	queue.process();

	REQUIRE(dataList != eventList);
	REQUIRE(detectDataListOrder(dataList.begin(), dataList.end()) == 1);
}

struct MyCompareDescend
{
	template <typename T>
	bool operator() (const T & a, const T & b) const {
		return a.event > b.event;
	}
};
struct MyPolicyDescend
{
	template <typename Item>
	using QueueList = eventpp::OrderedQueueList<Item, MyCompareDescend >;
};

TEST_CASE("EventQueue, ordered list, descend")
{
	constexpr int count = 100;
	eventpp::EventQueue<int, void (), MyPolicyDescend> queue;

	std::vector<int> eventList(count);
	std::iota(eventList.begin(), eventList.end(), 1);
	for(int i = 0; i < count; i += 2) {
		std::swap(eventList[i], eventList[i + 1]);
	}
	REQUIRE(detectDataListOrder(eventList.begin(), eventList.end()) == 0);

	std::vector<int> dataList(count);

	int processedCount = 0;
	for(const int e : eventList) {
		queue.appendListener(e, [&dataList, e, &processedCount]() {
			dataList[processedCount] = e;
			++processedCount;
		});
	}

	for(const int e : eventList) {
		queue.enqueue(e);
	}
	queue.process();

	REQUIRE(dataList != eventList);
	REQUIRE(detectDataListOrder(dataList.begin(), dataList.end()) == -1);
}

