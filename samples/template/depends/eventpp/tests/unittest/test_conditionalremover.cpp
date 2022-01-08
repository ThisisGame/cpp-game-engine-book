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
#include "eventpp/utilities/conditionalremover.h"
#include "eventpp/eventqueue.h"
#include "eventpp/hetereventqueue.h"

TEST_CASE("ConditionalRemover, EventQueue")
{
	eventpp::EventQueue<int, void ()> dispatcher;
	constexpr int event = 3;
	
	std::vector<int> dataList(4);
	
	dispatcher.appendListener(event, [&dataList]() {
		++dataList[0];
	});

	int removeCounter = 0;
	eventpp::conditionalRemover(dispatcher).prependListener(event, [&dataList]() {
		++dataList[1];
	}, [&removeCounter]() -> bool {
		return removeCounter == 1;
	});
	auto handle = eventpp::conditionalRemover(dispatcher).appendListener(event, [&dataList]() {
		++dataList[2];
	}, [&removeCounter]() -> bool {
		return removeCounter == 2;
	});
	eventpp::conditionalRemover(dispatcher).insertListener(event, [&dataList]() {
		++dataList[3];
	}, handle, [&removeCounter]() -> bool {
		return removeCounter == 3;
	});
	
	REQUIRE(dataList == std::vector<int> { 0, 0, 0, 0 });

	dispatcher.dispatch(event);
	REQUIRE(dataList == std::vector<int> { 1, 1, 1, 1 });

	++removeCounter;
	dispatcher.dispatch(event);
	REQUIRE(dataList == std::vector<int> { 2, 2, 2, 2 });

	++removeCounter;
	dispatcher.dispatch(event);
	REQUIRE(dataList == std::vector<int> { 3, 2, 3, 3 });

	++removeCounter;
	dispatcher.dispatch(event);
	REQUIRE(dataList == std::vector<int> { 4, 2, 3, 4 });

	++removeCounter;
	dispatcher.dispatch(event);
	REQUIRE(dataList == std::vector<int> { 5, 2, 3, 4 });
}

TEST_CASE("ConditionalRemover, EventQueue, has parameters")
{
	eventpp::EventQueue<int, void (int, int)> dispatcher;
	constexpr int event = 3;
	
	std::vector<int> dataList(4);
	
	dispatcher.appendListener(event, [&dataList](int, int) {
		++dataList[0];
	});

	int removeCounter = 0;
	eventpp::conditionalRemover(dispatcher).prependListener(event, [&dataList](int, int) {
		++dataList[1];
	}, [&removeCounter]() -> bool {
		return removeCounter == 1;
	});
	auto handle = eventpp::conditionalRemover(dispatcher).appendListener(event, [&dataList](int, int) {
		++dataList[2];
	}, [&removeCounter]() -> bool {
		return removeCounter == 2;
	});
	eventpp::conditionalRemover(dispatcher).insertListener(event, [&dataList](int, int) {
		++dataList[3];
	}, handle, [&removeCounter]() -> bool {
		return removeCounter == 3;
	});
	
	REQUIRE(dataList == std::vector<int> { 0, 0, 0, 0 });

	dispatcher.dispatch(event, 3);
	REQUIRE(dataList == std::vector<int> { 1, 1, 1, 1 });

	++removeCounter;
	dispatcher.dispatch(event, 3, 5);
	REQUIRE(dataList == std::vector<int> { 2, 2, 2, 2 });

	++removeCounter;
	dispatcher.dispatch(event, 3);
	REQUIRE(dataList == std::vector<int> { 3, 2, 3, 3 });

	++removeCounter;
	dispatcher.dispatch(event, 3, 5);
	REQUIRE(dataList == std::vector<int> { 4, 2, 3, 4 });

	++removeCounter;
	dispatcher.dispatch(event, 5);
	REQUIRE(dataList == std::vector<int> { 5, 2, 3, 4 });
}

TEST_CASE("ConditionalRemover, EventQueue, condition checks arguments")
{
	eventpp::EventQueue<int, void (int, int)> dispatcher;
	constexpr int event = 3;
	
	std::vector<int> dataList(4);
	
	dispatcher.appendListener(event, [&dataList](int, int) {
		++dataList[0];
	});

	eventpp::conditionalRemover(dispatcher).prependListener(event, [&dataList](int, int) {
		++dataList[1];
	}, [](const int, const int b) -> bool {
		return b == 1;
	});
	auto handle = eventpp::conditionalRemover(dispatcher).appendListener(event, [&dataList](int, int) {
		++dataList[2];
	}, [](const int, const int b) -> bool {
		return b == 2;
	});
	eventpp::conditionalRemover(dispatcher).insertListener(event, [&dataList](int, int) {
		++dataList[3];
	}, handle, [](const int, const int b) -> bool {
		return b == 3;
	});
	
	REQUIRE(dataList == std::vector<int> { 0, 0, 0, 0 });

	dispatcher.dispatch(event, 5);
	REQUIRE(dataList == std::vector<int> { 1, 1, 1, 1 });

	dispatcher.dispatch(event, 3, 1);
	REQUIRE(dataList == std::vector<int> { 2, 2, 2, 2 });

	dispatcher.dispatch(event, 2);
	REQUIRE(dataList == std::vector<int> { 3, 2, 3, 3 });

	dispatcher.dispatch(event, 3);
	REQUIRE(dataList == std::vector<int> { 4, 2, 3, 4 });

	dispatcher.dispatch(event, 4);
	REQUIRE(dataList == std::vector<int> { 5, 2, 3, 4 });
}

TEST_CASE("ConditionalRemover, CallbackList")
{
	eventpp::CallbackList<void ()> callbackList;
	
	std::vector<int> dataList(4);
	
	callbackList.append([&dataList]() {
		++dataList[0];
	});

	int removeCounter = 0;
	eventpp::conditionalRemover(callbackList).prepend([&dataList]() {
		++dataList[1];
	}, [&removeCounter]() -> bool {
		return removeCounter == 1;
	});
	auto handle = eventpp::conditionalRemover(callbackList).append([&dataList]() {
		++dataList[2];
	}, [&removeCounter]() -> bool {
		return removeCounter == 2;
	});
	eventpp::conditionalRemover(callbackList).insert([&dataList]() {
		++dataList[3];
	}, handle, [&removeCounter]() -> bool {
		return removeCounter == 3;
	});
	
	REQUIRE(dataList == std::vector<int> { 0, 0, 0, 0 });

	callbackList();
	REQUIRE(dataList == std::vector<int> { 1, 1, 1, 1 });

	++removeCounter;
	callbackList();
	REQUIRE(dataList == std::vector<int> { 2, 2, 2, 2 });

	++removeCounter;
	callbackList();
	REQUIRE(dataList == std::vector<int> { 3, 2, 3, 3 });

	++removeCounter;
	callbackList();
	REQUIRE(dataList == std::vector<int> { 4, 2, 3, 4 });

	++removeCounter;
	callbackList();
	REQUIRE(dataList == std::vector<int> { 5, 2, 3, 4 });
}

TEST_CASE("ConditionalRemover, CallbackList, has parameters")
{
	eventpp::CallbackList<void (int, int)> callbackList;
	
	std::vector<int> dataList(4);
	
	callbackList.append([&dataList](int, int) {
		++dataList[0];
	});

	int removeCounter = 0;
	eventpp::conditionalRemover(callbackList).prepend([&dataList](int, int) {
		++dataList[1];
	}, [&removeCounter]() -> bool {
		return removeCounter == 1;
	});
	auto handle = eventpp::conditionalRemover(callbackList).append([&dataList](int, int) {
		++dataList[2];
	}, [&removeCounter]() -> bool {
		return removeCounter == 2;
	});
	eventpp::conditionalRemover(callbackList).insert([&dataList](int, int) {
		++dataList[3];
	}, handle, [&removeCounter]() -> bool {
		return removeCounter == 3;
	});
	
	REQUIRE(dataList == std::vector<int> { 0, 0, 0, 0 });

	callbackList(3, 5);
	REQUIRE(dataList == std::vector<int> { 1, 1, 1, 1 });

	++removeCounter;
	callbackList(3, 5);
	REQUIRE(dataList == std::vector<int> { 2, 2, 2, 2 });

	++removeCounter;
	callbackList(3, 5);
	REQUIRE(dataList == std::vector<int> { 3, 2, 3, 3 });

	++removeCounter;
	callbackList(3, 5);
	REQUIRE(dataList == std::vector<int> { 4, 2, 3, 4 });

	++removeCounter;
	callbackList(3, 5);
	REQUIRE(dataList == std::vector<int> { 5, 2, 3, 4 });
}

TEST_CASE("ConditionalRemover, CallbackList, condition checks arguments")
{
	eventpp::CallbackList<void (int, int)> callbackList;
	
	std::vector<int> dataList(4);
	
	callbackList.append([&dataList](int, int) {
		++dataList[0];
	});

	eventpp::conditionalRemover(callbackList).prepend([&dataList](int, int) {
		++dataList[1];
	}, [](const int a, const int) -> bool {
		return a == 1;
	});
	auto handle = eventpp::conditionalRemover(callbackList).append([&dataList](int, int) {
		++dataList[2];
	}, [](const int a, const int) -> bool {
		return a == 2;
	});
	eventpp::conditionalRemover(callbackList).insert([&dataList](int, int) {
		++dataList[3];
	}, handle, [](const int a, const int) -> bool {
		return a == 3;
	});
	
	REQUIRE(dataList == std::vector<int> { 0, 0, 0, 0 });

	callbackList(8, 1);
	REQUIRE(dataList == std::vector<int> { 1, 1, 1, 1 });

	callbackList(1, 5);
	REQUIRE(dataList == std::vector<int> { 2, 2, 2, 2 });

	callbackList(2, 5);
	REQUIRE(dataList == std::vector<int> { 3, 2, 3, 3 });

	callbackList(3, 5);
	REQUIRE(dataList == std::vector<int> { 4, 2, 3, 4 });

	callbackList(4, 5);
	REQUIRE(dataList == std::vector<int> { 5, 2, 3, 4 });
}

TEST_CASE("ConditionalRemover, HeterEventQueue")
{
	eventpp::HeterEventQueue<int, eventpp::HeterTuple<void ()> > dispatcher;
	constexpr int event = 3;

	std::vector<int> dataList(4);

	dispatcher.appendListener(event, [&dataList]() {
		++dataList[0];
	});

	int removeCounter = 0;
	eventpp::conditionalRemover(dispatcher).prependListener(event, [&dataList]() {
		++dataList[1];
	}, [&removeCounter]() -> bool {
		return removeCounter == 1;
	});
	auto handle = eventpp::conditionalRemover(dispatcher).appendListener(event, [&dataList]() {
		++dataList[2];
	}, [&removeCounter]() -> bool {
		return removeCounter == 2;
	});
	eventpp::conditionalRemover(dispatcher).insertListener(event, [&dataList]() {
		++dataList[3];
	}, handle, [&removeCounter]() -> bool {
		return removeCounter == 3;
	});

	REQUIRE(dataList == std::vector<int> { 0, 0, 0, 0 });

	dispatcher.dispatch(event);
	REQUIRE(dataList == std::vector<int> { 1, 1, 1, 1 });

	++removeCounter;
	dispatcher.dispatch(event);
	REQUIRE(dataList == std::vector<int> { 2, 2, 2, 2 });

	++removeCounter;
	dispatcher.dispatch(event);
	REQUIRE(dataList == std::vector<int> { 3, 2, 3, 3 });

	++removeCounter;
	dispatcher.dispatch(event);
	REQUIRE(dataList == std::vector<int> { 4, 2, 3, 4 });

	++removeCounter;
	dispatcher.dispatch(event);
	REQUIRE(dataList == std::vector<int> { 5, 2, 3, 4 });
}

