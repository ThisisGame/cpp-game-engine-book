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
#include "eventpp/eventdispatcher.h"
#undef private

TEST_CASE("EventDispatcher, copy constructor from empty EventDispatcher")
{
	using ED = eventpp::EventDispatcher<int, void ()>;
	ED dispatcher;

	REQUIRE(dispatcher.eventCallbackListMap.empty());

	ED copiedDispatcher(dispatcher);
	REQUIRE(copiedDispatcher.eventCallbackListMap.empty());
	REQUIRE(dispatcher.eventCallbackListMap.empty());
}

TEST_CASE("EventDispatcher, copy constructor from non-empty EventDispatcher")
{
	using ED = eventpp::EventDispatcher<int, void ()>;
	ED dispatcher;
	constexpr int event = 3;

	std::vector<int> dataList(3);

	dispatcher.appendListener(event, [&dataList]() {
		++dataList[0];
	});
	dispatcher.appendListener(event, [&dataList]() {
		++dataList[1];
	});

	REQUIRE(dataList == std::vector<int> { 0, 0, 0 });
	dispatcher.dispatch(event);
	REQUIRE(dataList == std::vector<int> { 1, 1, 0 });
	
	ED copiedDispatcher(dispatcher);
	copiedDispatcher.dispatch(event);
	REQUIRE(dataList == std::vector<int> { 2, 2, 0 });
	copiedDispatcher.appendListener(event, [&dataList]() {
		++dataList[2];
	});
	copiedDispatcher.dispatch(event);
	REQUIRE(dataList == std::vector<int> { 3, 3, 1 });
	dispatcher.dispatch(event);
	REQUIRE(dataList == std::vector<int> { 4, 4, 1 });
}

TEST_CASE("EventDispatcher, assign from non-empty EventDispatcher")
{
	using ED = eventpp::EventDispatcher<int, void ()>;
	ED dispatcher;
	constexpr int event = 3;

	std::vector<int> dataList(3);

	dispatcher.appendListener(event, [&dataList]() {
		++dataList[0];
	});
	dispatcher.appendListener(event, [&dataList]() {
		++dataList[1];
	});

	REQUIRE(dataList == std::vector<int> { 0, 0, 0 });
	dispatcher.dispatch(event);
	REQUIRE(dataList == std::vector<int> { 1, 1, 0 });
	
	ED assignedDispatcher;
	assignedDispatcher = dispatcher;
	assignedDispatcher.dispatch(event);
	REQUIRE(dataList == std::vector<int> { 2, 2, 0 });
	assignedDispatcher.appendListener(event, [&dataList]() {
		++dataList[2];
	});
	assignedDispatcher.dispatch(event);
	REQUIRE(dataList == std::vector<int> { 3, 3, 1 });
	dispatcher.dispatch(event);
	REQUIRE(dataList == std::vector<int> { 4, 4, 1 });
}

TEST_CASE("EventDispatcher, move constructor from empty EventDispatcher")
{
	using ED = eventpp::EventDispatcher<int, void ()>;
	ED dispatcher;

	REQUIRE(dispatcher.eventCallbackListMap.empty());

	ED movedDispatcher(std::move(dispatcher));
	REQUIRE(movedDispatcher.eventCallbackListMap.empty());
	REQUIRE(dispatcher.eventCallbackListMap.empty());
}

TEST_CASE("EventDispatcher, move constructor from non-empty EventDispatcher")
{
	using ED = eventpp::EventDispatcher<int, void ()>;
	ED dispatcher;

	constexpr int event = 3;

	std::vector<int> dataList(3);

	dispatcher.appendListener(event, [&dataList]() {
		++dataList[0];
	});
	dispatcher.appendListener(event, [&dataList]() {
		++dataList[1];
	});
	dispatcher.appendListener(event, [&dataList]() {
		++dataList[2];
	});

	REQUIRE(! dispatcher.eventCallbackListMap.empty());
	REQUIRE(dataList == std::vector<int> { 0, 0, 0 });
	dispatcher.dispatch(event);
	REQUIRE(dataList == std::vector<int> { 1, 1, 1 });

	ED movedDispatcher(std::move(dispatcher));
	REQUIRE(! movedDispatcher.eventCallbackListMap.empty());
	REQUIRE(dispatcher.eventCallbackListMap.empty());
	movedDispatcher.dispatch(event);
	REQUIRE(dataList == std::vector<int> { 2, 2, 2 });
	dispatcher.dispatch(event);
	REQUIRE(dataList == std::vector<int> { 2, 2, 2 });
}

TEST_CASE("EventDispatcher, move assign from non-empty EventDispatcher")
{
	using ED = eventpp::EventDispatcher<int, void ()>;
	ED dispatcher;
	constexpr int event = 3;

	std::vector<int> dataList(3);

	dispatcher.appendListener(event, [&dataList]() {
		++dataList[0];
	});
	dispatcher.appendListener(event, [&dataList]() {
		++dataList[1];
	});

	REQUIRE(dataList == std::vector<int> { 0, 0, 0 });
	dispatcher.dispatch(event);
	REQUIRE(dataList == std::vector<int> { 1, 1, 0 });
	
	ED assignedDispatcher;
	assignedDispatcher = std::move(dispatcher);
	assignedDispatcher.dispatch(event);
	REQUIRE(dataList == std::vector<int> { 2, 2, 0 });
	assignedDispatcher.appendListener(event, [&dataList]() {
		++dataList[2];
	});
	assignedDispatcher.dispatch(event);
	REQUIRE(dataList == std::vector<int> { 3, 3, 1 });
	dispatcher.dispatch(event);
	REQUIRE(dataList == std::vector<int> { 3, 3, 1 });
}

TEST_CASE("EventDispatcher, swap with self")
{
	using ED = eventpp::EventDispatcher<int, void ()>;
	ED swappedDispatcher;
	REQUIRE(swappedDispatcher.eventCallbackListMap.empty());

	using std::swap;
	swap(swappedDispatcher, swappedDispatcher);
	REQUIRE(swappedDispatcher.eventCallbackListMap.empty());
}

TEST_CASE("EventDispatcher, swap with empty EventDispatcher")
{
	using ED = eventpp::EventDispatcher<int, void ()>;
	ED dispatcher;
	REQUIRE(dispatcher.eventCallbackListMap.empty());

	ED swappedDispatcher;

	using std::swap;
	swap(swappedDispatcher, dispatcher);
	REQUIRE(swappedDispatcher.eventCallbackListMap.empty());

	dispatcher.appendListener(1, []() {});
	REQUIRE(! dispatcher.eventCallbackListMap.empty());
	REQUIRE(swappedDispatcher.eventCallbackListMap.empty());
}

TEST_CASE("EventDispatcher, swap with non-empty EventDispatcher")
{
	using ED = eventpp::EventDispatcher<int, void ()>;
	ED dispatcher;
	constexpr int event = 3;

	std::vector<int> dataList(3);

	dispatcher.appendListener(event, [&dataList]() {
		++dataList[0];
	});
	dispatcher.appendListener(event, [&dataList]() {
		++dataList[1];
	});

	ED swappedDispatcher;

	swappedDispatcher.appendListener(event, [&dataList]() {
		++dataList[1];
	});
	swappedDispatcher.appendListener(event, [&dataList]() {
		++dataList[2];
	});
	
	dispatcher.dispatch(event);
	REQUIRE(dataList == std::vector<int> { 1, 1, 0 });

	using std::swap;
	swap(swappedDispatcher, dispatcher);
	dispatcher.dispatch(event);
	REQUIRE(dataList == std::vector<int> { 1, 2, 1 });
	swappedDispatcher.dispatch(event);
	REQUIRE(dataList == std::vector<int> { 2, 3, 1 });
}
