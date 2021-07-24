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
#include "eventpp/hetereventqueue.h"

TEST_CASE("HeterEventQueue, clearEvents")
{
	eventpp::HeterEventQueue<int, eventpp::HeterTuple<void (), void (int)> > queue;

	SECTION("void ()") {
		REQUIRE(queue.emptyQueue());

		queue.enqueue(1);

		REQUIRE(! queue.emptyQueue());

		queue.clearEvents();
		REQUIRE(queue.emptyQueue());
	}

	SECTION("void (int)") {
		REQUIRE(queue.emptyQueue());

		queue.enqueue(6, 3);

		REQUIRE(! queue.emptyQueue());

		queue.clearEvents();
		REQUIRE(queue.emptyQueue());
	}

	SECTION("void (), more events") {
		REQUIRE(queue.emptyQueue());

		queue.enqueue(6);

		REQUIRE(! queue.emptyQueue());

		queue.enqueue(10);
		queue.enqueue(99);

		queue.clearEvents();
		REQUIRE(queue.emptyQueue());
	}

	SECTION("void (int), more events") {
		REQUIRE(queue.emptyQueue());

		queue.enqueue(1, 3);

		REQUIRE(! queue.emptyQueue());

		queue.enqueue(2, 5);
		queue.enqueue(8, 6);

		queue.clearEvents();
		REQUIRE(queue.emptyQueue());
	}
}

TEST_CASE("HeterEventQueue, enqueue")
{
	eventpp::HeterEventQueue<int, eventpp::HeterTuple<void (), void (int)> > queue;
	constexpr int event = 3;

	SECTION("void ()") {
		REQUIRE(queue.emptyQueue());

		queue.enqueue(event);

		REQUIRE(! queue.emptyQueue());
	}

	SECTION("void (int)") {
		REQUIRE(queue.emptyQueue());

		queue.enqueue(event, 3);

		REQUIRE(! queue.emptyQueue());
	}
}

TEST_CASE("HeterEventQueue, process")
{
	eventpp::HeterEventQueue<int, eventpp::HeterTuple<void (), void (int)> > queue;
	constexpr int event = 3;

	std::vector<int> dataList(2);

	queue.appendListener(event, [&dataList]() {
		++dataList[0];
	});
	queue.appendListener(event, [&dataList](int n) {
		dataList[1] += n;
	});

	SECTION("0, 1") {
		REQUIRE(dataList == std::vector<int>{ 0, 0 });

		queue.enqueue(event); // 0 argument
		queue.enqueue(event, 2); // 1 argument
		queue.process();
		REQUIRE(dataList == std::vector<int>{ 1, 2 });
	}

	SECTION("0, 0, 1, 1") {
		REQUIRE(dataList == std::vector<int>{ 0, 0 });

		queue.enqueue(event);
		queue.enqueue(event);
		queue.enqueue(event, 2);
		queue.enqueue(event, 2);
		queue.process();
		REQUIRE(dataList == std::vector<int>{ 2, 4 });
	}

	SECTION("1, 0, 1, 0") {
		REQUIRE(dataList == std::vector<int>{ 0, 0 });

		queue.enqueue(event, 2);
		queue.enqueue(event);
		queue.enqueue(event, 2);
		queue.enqueue(event);
		queue.process();
		REQUIRE(dataList == std::vector<int>{ 2, 4 });
	}
}

TEST_CASE("HeterEventQueue, processOne")
{
	eventpp::HeterEventQueue<int, eventpp::HeterTuple<void (), void (int)> > queue;
	constexpr int event = 3;

	std::vector<int> dataList(2);

	queue.appendListener(event, [&dataList]() {
		++dataList[0];
	});
	queue.appendListener(event, [&dataList](int n) {
		dataList[1] += n;
	});

	REQUIRE(dataList == std::vector<int>{ 0, 0 });

	queue.enqueue(event);
	queue.enqueue(event, 2);
	queue.processOne();
	REQUIRE(dataList == std::vector<int>{ 1, 0 });

	queue.enqueue(event);
	queue.enqueue(event, 2);
	queue.processOne();
	REQUIRE(dataList == std::vector<int>{ 1, 2 });

	queue.processOne();
	REQUIRE(dataList == std::vector<int>{ 2, 2 });
	queue.processOne();
	REQUIRE(dataList == std::vector<int>{ 2, 4 });
}

TEST_CASE("HeterEventQueue, processIf")
{
	struct MyEventPolicies
	{
		using ArgumentPassingMode = eventpp::ArgumentPassingIncludeEvent;
	};

	eventpp::HeterEventQueue<int, eventpp::HeterTuple<void (int), void (int, int)>, MyEventPolicies> queue;

	std::vector<int> dataList(3);

	queue.appendListener(5, [&dataList](int) {
		++dataList[0];
	});
	queue.appendListener(6, [&dataList](int) {
		++dataList[1];
	});
	queue.appendListener(7, [&dataList](int, int) {
		++dataList[2];
	});

	REQUIRE(dataList == std::vector<int>{ 0, 0, 0 });

	queue.enqueue(5);
	queue.enqueue(6);
	queue.enqueue(7, 8);
	queue.process();
	REQUIRE(dataList == std::vector<int>{ 1, 1, 1 });

	queue.enqueue(5);
	queue.enqueue(6);
	queue.enqueue(7, 8);
	queue.processIf([](const int event) -> bool { return event == 6; });
	REQUIRE(dataList == std::vector<int>{ 1, 2, 1 });
	// Now the queue contains 5, 7

	queue.enqueue(5);
	queue.enqueue(6);
	queue.enqueue(7, 8);
	queue.processIf([](const int event) -> bool { return event == 5; });
	REQUIRE(dataList == std::vector<int>{ 3, 2, 1 });
	// Now the queue contains 6, 7, 7

	queue.enqueue(5);
	queue.enqueue(6);
	queue.enqueue(7, 8);
	queue.processIf([](const int event, int) -> bool { return event == 7; });
	REQUIRE(dataList == std::vector<int>{ 3, 2, 4 });
	// Now the queue contains 5, 6, 6

	queue.process();
	REQUIRE(dataList == std::vector<int>{ 4, 4, 4 });
}

