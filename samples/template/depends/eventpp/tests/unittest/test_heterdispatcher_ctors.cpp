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
#include "eventpp/hetereventdispatcher.h"
#undef private

TEST_CASE("HeterEventDispatcher, copy constructor from empty HeterEventDispatcher")
{
	using ED = eventpp::HeterEventDispatcher<int, eventpp::HeterTuple<void (), void (int)> >;
	ED dispatcher;

	REQUIRE(dispatcher.eventCallbackListMap.empty());

	ED copiedDispatcher(dispatcher);
	REQUIRE(copiedDispatcher.eventCallbackListMap.empty());
	REQUIRE(dispatcher.eventCallbackListMap.empty());
}

TEST_CASE("HeterEventDispatcher, copy constructor from non-empty HeterEventDispatcher")
{
	using ED = eventpp::HeterEventDispatcher<int, eventpp::HeterTuple<void (), void (int)> >;
	ED dispatcher;
	constexpr int event = 3;

	std::vector<int> dataList(3);

	dispatcher.appendListener(event, [&dataList]() {
		++dataList[0];
	});
	dispatcher.appendListener(event, [&dataList](int) {
		++dataList[1];
	});

	REQUIRE(dataList == std::vector<int> { 0, 0, 0 });
	dispatcher.dispatch(event);
	REQUIRE(dataList == std::vector<int> { 1, 0, 0 });
	dispatcher.dispatch(event, 5);
	REQUIRE(dataList == std::vector<int> { 1, 1, 0 });
	
	ED copiedDispatcher(dispatcher);
	copiedDispatcher.dispatch(event);
	REQUIRE(dataList == std::vector<int> { 2, 1, 0 });
	copiedDispatcher.dispatch(event, 5);
	REQUIRE(dataList == std::vector<int> { 2, 2, 0 });
	copiedDispatcher.appendListener(event, [&dataList]() {
		++dataList[2];
	});
	copiedDispatcher.dispatch(event);
	copiedDispatcher.dispatch(event, 5);
	REQUIRE(dataList == std::vector<int> { 3, 3, 1 });
	dispatcher.dispatch(event);
	dispatcher.dispatch(event, 5);
	REQUIRE(dataList == std::vector<int> { 4, 4, 1 });
}

TEST_CASE("HeterEventDispatcher, assign from non-empty HeterEventDispatcher")
{
	using ED = eventpp::HeterEventDispatcher<int, eventpp::HeterTuple<void (), void (int)> >;
	ED dispatcher;
	constexpr int event = 3;

	std::vector<int> dataList(3);

	dispatcher.appendListener(event, [&dataList]() {
		++dataList[0];
	});
	dispatcher.appendListener(event, [&dataList](int) {
		++dataList[1];
	});

	REQUIRE(dataList == std::vector<int> { 0, 0, 0 });
	dispatcher.dispatch(event);
	dispatcher.dispatch(event, 5);
	REQUIRE(dataList == std::vector<int> { 1, 1, 0 });
	
	ED assignedDispatcher;
	assignedDispatcher = dispatcher;
	assignedDispatcher.dispatch(event);
	assignedDispatcher.dispatch(event, 5);
	REQUIRE(dataList == std::vector<int> { 2, 2, 0 });
	assignedDispatcher.appendListener(event, [&dataList]() {
		++dataList[2];
	});
	assignedDispatcher.dispatch(event);
	assignedDispatcher.dispatch(event, 5);
	REQUIRE(dataList == std::vector<int> { 3, 3, 1 });
	dispatcher.dispatch(event);
	dispatcher.dispatch(event, 5);
	REQUIRE(dataList == std::vector<int> { 4, 4, 1 });
}

TEST_CASE("HeterEventDispatcher, move assign from non-empty HeterEventDispatcher")
{
	using ED = eventpp::HeterEventDispatcher<int, eventpp::HeterTuple<void (), void (std::string)> >;
	ED dispatcher;
	constexpr int event = 3;

	std::vector<int> dataList(3);

	dispatcher.appendListener(event, [&dataList]() {
		++dataList[0];
	});
	dispatcher.appendListener(event, [&dataList](std::string) {
		++dataList[1];
	});

	REQUIRE(dataList == std::vector<int> { 0, 0, 0 });
	dispatcher.dispatch(event);
	dispatcher.dispatch(event, "a");
	REQUIRE(dataList == std::vector<int> { 1, 1, 0 });
	
	ED assignedDispatcher;
	assignedDispatcher = std::move(dispatcher);
	assignedDispatcher.dispatch(event);
	assignedDispatcher.dispatch(event, "a");
	REQUIRE(dataList == std::vector<int> { 2, 2, 0 });
	assignedDispatcher.appendListener(event, [&dataList]() {
		++dataList[2];
	});
	assignedDispatcher.dispatch(event);
	assignedDispatcher.dispatch(event, "a");
	REQUIRE(dataList == std::vector<int> { 3, 3, 1 });
	dispatcher.dispatch(event);
	dispatcher.dispatch(event, "a");
	REQUIRE(dataList == std::vector<int> { 3, 3, 1 });
}

