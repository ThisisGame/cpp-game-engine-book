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
#define private public
#include "eventpp/eventqueue.h"
#undef private

TEST_CASE("EventQueue, copy constructor from empty EventQueue")
{
	using EQ = eventpp::EventQueue<int, void ()>;
	EQ queue;

	REQUIRE(queue.eventCallbackListMap.empty());

	EQ copiedQueue(queue);
	REQUIRE(copiedQueue.eventCallbackListMap.empty());
	REQUIRE(copiedQueue.eventCallbackListMap.empty());
}

TEST_CASE("EventQueue, copy constructor from non-empty EventQueue")
{
	using EQ = eventpp::EventQueue<int, void ()>;
	EQ queue;
	constexpr int event = 3;

	std::vector<int> dataList(3);

	queue.appendListener(event, [&dataList]() {
		++dataList[0];
	});
	queue.appendListener(event, [&dataList]() {
		++dataList[1];
	});

	REQUIRE(dataList == std::vector<int> { 0, 0, 0 });
	queue.dispatch(event);
	REQUIRE(dataList == std::vector<int> { 1, 1, 0 });
	
	EQ copiedQueue(queue);
	copiedQueue.dispatch(event);
	REQUIRE(dataList == std::vector<int> { 2, 2, 0 });
	copiedQueue.appendListener(event, [&dataList]() {
		++dataList[2];
	});
	copiedQueue.dispatch(event);
	REQUIRE(dataList == std::vector<int> { 3, 3, 1 });
	queue.dispatch(event);
	REQUIRE(dataList == std::vector<int> { 4, 4, 1 });
}

TEST_CASE("EventQueue, assign from non-empty EventQueue")
{
	using EQ = eventpp::EventQueue<int, void ()>;
	EQ queue;
	constexpr int event = 3;

	std::vector<int> dataList(3);

	queue.appendListener(event, [&dataList]() {
		++dataList[0];
	});
	queue.appendListener(event, [&dataList]() {
		++dataList[1];
	});

	REQUIRE(dataList == std::vector<int> { 0, 0, 0 });
	queue.dispatch(event);
	REQUIRE(dataList == std::vector<int> { 1, 1, 0 });
	
	EQ assignedQueue;
	assignedQueue = queue;
	assignedQueue.dispatch(event);
	REQUIRE(dataList == std::vector<int> { 2, 2, 0 });
	assignedQueue.appendListener(event, [&dataList]() {
		++dataList[2];
	});
	assignedQueue.dispatch(event);
	REQUIRE(dataList == std::vector<int> { 3, 3, 1 });
	queue.dispatch(event);
	REQUIRE(dataList == std::vector<int> { 4, 4, 1 });
}

TEST_CASE("EventQueue, move constructor from empty EventQueue")
{
	using EQ = eventpp::EventQueue<int, void ()>;
	EQ queue;

	REQUIRE(queue.eventCallbackListMap.empty());

	EQ movedQueue(std::move(queue));
	REQUIRE(movedQueue.eventCallbackListMap.empty());
	REQUIRE(queue.eventCallbackListMap.empty());
}

TEST_CASE("EventQueue, move constructor from non-empty EventQueue")
{
	using EQ = eventpp::EventQueue<int, void ()>;
	EQ queue;

	constexpr int event = 3;

	std::vector<int> dataList(3);

	queue.appendListener(event, [&dataList]() {
		++dataList[0];
	});
	queue.appendListener(event, [&dataList]() {
		++dataList[1];
	});
	queue.appendListener(event, [&dataList]() {
		++dataList[2];
	});

	REQUIRE(! queue.eventCallbackListMap.empty());
	REQUIRE(dataList == std::vector<int> { 0, 0, 0 });
	queue.dispatch(event);
	REQUIRE(dataList == std::vector<int> { 1, 1, 1 });

	EQ movedQueue(std::move(queue));
	REQUIRE(! movedQueue.eventCallbackListMap.empty());
	REQUIRE(queue.eventCallbackListMap.empty());
	movedQueue.dispatch(event);
	REQUIRE(dataList == std::vector<int> { 2, 2, 2 });
	queue.dispatch(event);
	REQUIRE(dataList == std::vector<int> { 2, 2, 2 });
}

TEST_CASE("EventQueue, move assign from non-empty EventQueue")
{
	using EQ = eventpp::EventQueue<int, void ()>;
	EQ queue;
	constexpr int event = 3;

	std::vector<int> dataList(3);

	queue.appendListener(event, [&dataList]() {
		++dataList[0];
	});
	queue.appendListener(event, [&dataList]() {
		++dataList[1];
	});

	REQUIRE(dataList == std::vector<int> { 0, 0, 0 });
	queue.dispatch(event);
	REQUIRE(dataList == std::vector<int> { 1, 1, 0 });
	
	EQ assignedQueue;
	assignedQueue = std::move(queue);
	assignedQueue.dispatch(event);
	REQUIRE(dataList == std::vector<int> { 2, 2, 0 });
	assignedQueue.appendListener(event, [&dataList]() {
		++dataList[2];
	});
	assignedQueue.dispatch(event);
	REQUIRE(dataList == std::vector<int> { 3, 3, 1 });
	queue.dispatch(event);
	REQUIRE(dataList == std::vector<int> { 3, 3, 1 });
}

TEST_CASE("EventQueue, swap with self")
{
	using EQ = eventpp::EventQueue<int, void ()>;
	EQ swappedQueue;
	REQUIRE(swappedQueue.eventCallbackListMap.empty());

	using std::swap;
	swap(swappedQueue, swappedQueue);
	REQUIRE(swappedQueue.eventCallbackListMap.empty());
}

TEST_CASE("EventQueue, swap with empty EventQueue")
{
	using EQ = eventpp::EventQueue<int, void ()>;
	EQ queue;
	REQUIRE(queue.eventCallbackListMap.empty());

	EQ swappedQueue;

	using std::swap;
	swap(swappedQueue, queue);
	REQUIRE(swappedQueue.eventCallbackListMap.empty());

	queue.appendListener(1, []() {});
	REQUIRE(! queue.eventCallbackListMap.empty());
	REQUIRE(swappedQueue.eventCallbackListMap.empty());
}

TEST_CASE("EventQueue, swap with non-empty EventQueue")
{
	using EQ = eventpp::EventQueue<int, void ()>;
	EQ queue;
	constexpr int event = 3;

	std::vector<int> dataList(3);

	queue.appendListener(event, [&dataList]() {
		++dataList[0];
	});
	queue.appendListener(event, [&dataList]() {
		++dataList[1];
	});

	EQ swappedQueue;

	swappedQueue.appendListener(event, [&dataList]() {
		++dataList[1];
	});
	swappedQueue.appendListener(event, [&dataList]() {
		++dataList[2];
	});
	
	queue.dispatch(event);
	REQUIRE(dataList == std::vector<int> { 1, 1, 0 });

	using std::swap;
	swap(swappedQueue, queue);
	queue.dispatch(event);
	REQUIRE(dataList == std::vector<int> { 1, 2, 1 });
	swappedQueue.dispatch(event);
	REQUIRE(dataList == std::vector<int> { 2, 3, 1 });
}
