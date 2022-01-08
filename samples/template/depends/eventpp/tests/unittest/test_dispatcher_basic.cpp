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
#include "eventpp/mixins/mixinfilter.h"

#include <numeric>
#include <random>
#include <algorithm>

TEST_CASE("EventDispatcher, std::string, void (const std::string &)")
{
	eventpp::EventDispatcher<std::string, void (const std::string &)> dispatcher;

	int a = 1;
	int b = 5;

	dispatcher.appendListener("event1", [&a](const std::string &) {
		a = 2;
	});
	dispatcher.appendListener("event1", eraseArgs1([&b]() {
		b = 8;
	}));

	REQUIRE(a != 2);
	REQUIRE(b != 8);

	dispatcher.dispatch("event1");
	REQUIRE(a == 2);
	REQUIRE(b == 8);
}

TEST_CASE("EventDispatcher, int, void ()")
{
	eventpp::EventDispatcher<int, void ()> dispatcher;

	int a = 1;
	int b = 5;

	dispatcher.appendListener(3, [&a]() {
		a = 2;
	});
	dispatcher.appendListener(3, [&b]() {
		b = 8;
	});

	REQUIRE(a != 2);
	REQUIRE(b != 8);

	dispatcher.dispatch(3);
	REQUIRE(a == 2);
	REQUIRE(b == 8);
}

TEST_CASE("EventDispatcher, add/remove, int, void ()")
{
	eventpp::EventDispatcher<int, void ()> dispatcher;
	constexpr int event = 3;

	int a = 1;
	int b = 5;

	decltype(dispatcher)::Handle ha;
	decltype(dispatcher)::Handle hb;

	ha = dispatcher.appendListener(event, [event, &a, &dispatcher, &ha, &hb]() {
		a = 2;
		REQUIRE(dispatcher.removeListener(event, hb));
		REQUIRE(dispatcher.removeListener(event, ha));
	});
	hb = dispatcher.appendListener(event, [&b]() {
		b = 8;
	});

	REQUIRE(ha);
	REQUIRE(hb);

	REQUIRE(a != 2);
	REQUIRE(b != 8);

	dispatcher.dispatch(event);

	REQUIRE(! ha);
	REQUIRE(! hb);

	REQUIRE(a == 2);
	REQUIRE(b != 8);

	a = 1;
	REQUIRE(a != 2);
	REQUIRE(b != 8);

	dispatcher.dispatch(event);
	REQUIRE(a != 2);
	REQUIRE(b != 8);

	REQUIRE(! dispatcher.removeListener(event + 1, ha));
	REQUIRE(! dispatcher.removeListener(event + 1, hb));
}

TEST_CASE("EventDispatcher, add another listener inside a listener, int, void ()")
{
	eventpp::EventDispatcher<int, void ()> dispatcher;
	constexpr int event = 3;

	int a = 1;
	int b = 5;

	dispatcher.appendListener(event, [&event, &a, &dispatcher, &b]() {
		a = 2;
		dispatcher.appendListener(event, [&event, &b]() {
			b = 8;
		});
	});

	REQUIRE(a != 2);
	REQUIRE(b != 8);

	dispatcher.dispatch(event);
	REQUIRE(a == 2);
	REQUIRE(b != 8);
}

TEST_CASE("EventDispatcher, inside EventDispatcher, int, void ()")
{
	eventpp::EventDispatcher<int, void ()> dispatcher;
	constexpr int event1 = 3;
	constexpr int event2 = 5;

	int a = 1;
	int b = 5;

	decltype(dispatcher)::Handle ha;
	decltype(dispatcher)::Handle hb;

	ha = dispatcher.appendListener(event1, [&a, &dispatcher, event2]() {
		a = 2;
		dispatcher.dispatch(event2);
	});
	hb = dispatcher.appendListener(event2, [&b, &dispatcher, event1, event2, &ha, &hb]() {
		b = 8;
		REQUIRE(dispatcher.removeListener(event1, ha));
		REQUIRE(dispatcher.removeListener(event2, hb));
	});

	REQUIRE(ha);
	REQUIRE(hb);

	REQUIRE(a != 2);
	REQUIRE(b != 8);

	dispatcher.dispatch(event1);

	REQUIRE(! ha);
	REQUIRE(! hb);

	REQUIRE(a == 2);
	REQUIRE(b == 8);
}

TEST_CASE("EventDispatcher, int, void (const std::string &, int)")
{
	eventpp::EventDispatcher<int, void (const std::string &, int)> dispatcher;
	constexpr int event = 3;

	std::vector<std::string> sList(2);
	std::vector<int> iList(sList.size());

	dispatcher.appendListener(event, [event, &dispatcher, &sList, &iList](const std::string & s, const FromInt & i) {
		sList[0] = s;
		iList[0] = i.value;
	});
	dispatcher.appendListener(event, [event, &dispatcher, &sList, &iList](std::string s, const int i) {
		sList[1] = s + "2";
		iList[1] = i + 5;
	});

	REQUIRE(sList[0] != "first");
	REQUIRE(sList[1] != "first2");
	REQUIRE(iList[0] != 3);
	REQUIRE(iList[1] != 8);

	dispatcher.dispatch(event, "first", 3);

	REQUIRE(sList[0] == "first");
	REQUIRE(sList[1] == "first2");
	REQUIRE(iList[0] == 3);
	REQUIRE(iList[1] == 8);
}

TEST_CASE("EventDispatcher, forEach")
{
	eventpp::EventDispatcher<int, void ()> dispatcher;
	constexpr int event = 3;

	std::vector<int> dataList(3);

	dispatcher.appendListener(event, [event, &dataList]() {
		++dataList[0];
	});
	dispatcher.appendListener(event, [event, &dataList]() {
		++dataList[1];
	});
	dispatcher.appendListener(event, [event, &dataList]() {
		++dataList[2];
	});

	int i = 0;
	dispatcher.forEach(event, [&i, &dataList](auto callback) {
		REQUIRE(dataList[i] == 0);
		callback();
		REQUIRE(dataList[i] == 1);
		++i;
	});
}

TEST_CASE("EventDispatcher, forEachIf")
{
	eventpp::EventDispatcher<int, void ()> dispatcher;
	constexpr int event = 3;

	std::vector<int> dataList(3);

	dispatcher.appendListener(event, [event, &dataList]() {
		++dataList[0];
	});
	dispatcher.appendListener(event, [event, &dataList]() {
		++dataList[1];
	});
	dispatcher.appendListener(event, [event, &dataList]() {
		++dataList[2];
	});

	int i = 0;
	dispatcher.forEachIf(event, [&i, &dataList](auto callback) {
		if(i == 1) {
			return false;
		}
		callback();
		++i;
		return true;
	});
	REQUIRE(dataList == std::vector<int>{ 1, 0, 0 });
}

TEST_CASE("EventDispatcher, Event struct, void (const std::string &, int)")
{
	struct MyEvent {
		int type;
		std::string message;
		int param;
	};
	struct EventPolicies
	{
		static int getEvent(const MyEvent & e, const std::string &, int) {
			return e.type;
		}
	};


	eventpp::EventDispatcher<int, void (const MyEvent &, const std::string &, int), EventPolicies> dispatcher;
	constexpr int event = 3;

	std::vector<std::string> sList(2);
	std::vector<int> iList(sList.size());

	dispatcher.appendListener(event, [event, &dispatcher, &sList, &iList](const MyEvent & e, const std::string & s, int i) {
		sList[0] = e.message + " " + s;
		iList[0] = e.param + i;
	});
	dispatcher.appendListener(event, [event, &dispatcher, &sList, &iList](const MyEvent & e, std::string s, const int i) {
		sList[1] = s + " " + e.message;
		iList[1] = e.param * i;
	});

	REQUIRE(sList[0] != "Hello World");
	REQUIRE(sList[1] != "World Hello");
	REQUIRE(iList[0] != 8);
	REQUIRE(iList[1] != 15);

	dispatcher.dispatch(MyEvent{ event, "Hello", 5 }, "World", 3);

	REQUIRE(sList[0] == "Hello World");
	REQUIRE(sList[1] == "World Hello");
	REQUIRE(iList[0] == 8);
	REQUIRE(iList[1] == 15);
}

TEST_CASE("EventDispatcher, many, int, void (int)")
{
	eventpp::EventDispatcher<int, void (int)> dispatcher;

	constexpr int eventCount = 1024 * 64;
	std::vector<int> eventList(eventCount);

	std::iota(eventList.begin(), eventList.end(), 0);
	std::shuffle(eventList.begin(), eventList.end(), std::mt19937(std::random_device()()));

	std::vector<int> dataList(eventCount);

	for(int i = 1; i < eventCount; i += 2) {
		dispatcher.appendListener(eventList[i], [&dispatcher, i, &dataList](const int e) {
			dataList[i] = e;
		});
	}

	for(int i = 0; i < eventCount; i += 2) {
		dispatcher.appendListener(eventList[i], [&dispatcher, i, &dataList](const int e) {
			dataList[i] = e;
		});
	}

	for(int i = 0; i < eventCount; ++i) {
		dispatcher.dispatch(eventList[i]);
	}

	std::sort(eventList.begin(), eventList.end());
	std::sort(dataList.begin(), dataList.end());

	REQUIRE(eventList == dataList);
}

TEST_CASE("EventDispatcher, event filter")
{
	struct MyPolicies {
		using Mixins = eventpp::MixinList<eventpp::MixinFilter>;
	};
	using ED = eventpp::EventDispatcher<int, void (int, int), MyPolicies>;
	ED dispatcher;

	constexpr int itemCount = 5;
	std::vector<int> dataList(itemCount);

	for(int i = 0; i < itemCount; ++i) {
		dispatcher.appendListener(i, [&dataList, i](int e, int index) {
			dataList[e] = index;
		});
	}

	constexpr int filterCount = 2;
	std::vector<int> filterData(filterCount);

	SECTION("Filter invoked count") {
		dispatcher.appendFilter([&filterData](int /*e*/, int /*index*/) -> bool {
			++filterData[0];
			return true;
		});
		dispatcher.appendFilter([&filterData](int /*e*/, int /*index*/) -> bool {
			++filterData[1];
			return true;
		});

		for(int i = 0; i < itemCount; ++i) {
			dispatcher.dispatch(i, 58);
		}

		REQUIRE(filterData == std::vector<int>{ itemCount, itemCount });
		REQUIRE(dataList == std::vector<int>{ 58, 58, 58, 58, 58 });
	}

	SECTION("First filter blocks all other filters and listeners") {
		dispatcher.appendFilter([&filterData](int e, int /*index*/) -> bool {
			++filterData[0];
			if(e >= 2) {
				return false;
			}
			return true;
		});
		dispatcher.appendFilter([&filterData](int /*e*/, int /*index*/) -> bool {
			++filterData[1];
			return true;
		});

		for(int i = 0; i < itemCount; ++i) {
			dispatcher.dispatch(i, 58);
		}

		REQUIRE(filterData == std::vector<int>{ itemCount, 2 });
		REQUIRE(dataList == std::vector<int>{ 58, 58, 0, 0, 0 });
	}

	SECTION("Second filter doesn't block first filter but all listeners") {
		dispatcher.appendFilter([&filterData](int /*e*/, int /*index*/) -> bool {
			++filterData[0];
			return true;
		});
		dispatcher.appendFilter([&filterData](int e, int /*index*/) -> bool {
			++filterData[1];
			if(e >= 2) {
				return false;
			}
			return true;
		});

		for(int i = 0; i < itemCount; ++i) {
			dispatcher.dispatch(i, 58);
		}

		REQUIRE(filterData == std::vector<int>{ itemCount, itemCount });
		REQUIRE(dataList == std::vector<int>{ 58, 58, 0, 0, 0 });
	}

	SECTION("Filter manipulates the parameters") {
		dispatcher.appendFilter([&filterData](int e, int & index) -> bool {
			++filterData[0];
			if(e >= 2) {
				++index;
			}
			return true;
		});
		dispatcher.appendFilter([&filterData](int /*e*/, int /*index*/) -> bool {
			++filterData[1];
			return true;
		});

		for(int i = 0; i < itemCount; ++i) {
			dispatcher.dispatch(i, 58);
		}

		REQUIRE(filterData == std::vector<int>{ itemCount, itemCount });
		REQUIRE(dataList == std::vector<int>{ 58, 58, 59, 59, 59 });
	}
}

TEST_CASE("EventDispatcher, explicit single threading, int, void (int)")
{
	struct MyEventPolicies
	{
		using Threading = eventpp::SingleThreading;
		static int getEvent(const int e, const int) {
			return e;
		}

	};
	using ED = eventpp::EventDispatcher<int, void (int), MyEventPolicies>;
	ED dispatcher;

	int a = 1;
	int b = 5;

	dispatcher.appendListener(3, [&a](int n) {
		a += n;
	});
	dispatcher.appendListener(3, [&b](const int & n) {
		b += n;
	});

	REQUIRE(a != 6);
	REQUIRE(b != 10);

	dispatcher.dispatch(3, 5);
	REQUIRE(a == 6);
	REQUIRE(b == 10);
}

