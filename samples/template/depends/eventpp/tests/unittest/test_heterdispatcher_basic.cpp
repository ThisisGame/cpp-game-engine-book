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
#include "eventpp/hetereventdispatcher.h"
#include "eventpp/mixins/mixinfilter.h"
#include "eventpp/mixins/mixinheterfilter.h"

TEST_CASE("HeterEventDispatcher, appendListener")
{
	eventpp::HeterEventDispatcher<int, eventpp::HeterTuple<void (), void (int)> > dispatcher;
	constexpr int event = 3;

	std::vector<int> orderList(5);

	int order;

	dispatcher.appendListener(event, [&orderList, &order]() {
		orderList[order++] = 1;
	});
	dispatcher.appendListener(event, [&orderList, &order]() {
		orderList[order++] = 2;
	});
	dispatcher.appendListener(event, [&orderList, &order](int) {
		orderList[order++] = 3;
	});
	dispatcher.appendListener(event, [&orderList, &order](int) {
		orderList[order++] = 4;
	});
	dispatcher.appendListener(event, [&orderList, &order](int) {
		orderList[order++] = 5;
	});

	REQUIRE(orderList == std::vector<int>{ 0, 0, 0, 0, 0 });

	order = 0;
	dispatcher.dispatch(event, 3);
	REQUIRE(orderList == std::vector<int>{ 3, 4, 5, 0, 0 });
	dispatcher.dispatch(event);
	REQUIRE(orderList == std::vector<int>{ 3, 4, 5, 1, 2 });
}

TEST_CASE("HeterEventDispatcher, prependListener")
{
	eventpp::HeterEventDispatcher<int, eventpp::HeterTuple<void (), void (int)> > dispatcher;
	constexpr int event = 3;

	std::vector<int> orderList(5);

	int order;

	dispatcher.prependListener(event, [&orderList, &order]() {
		orderList[order++] = 1;
	});
	dispatcher.prependListener(event, [&orderList, &order]() {
		orderList[order++] = 2;
	});
	dispatcher.prependListener(event, [&orderList, &order](int) {
		orderList[order++] = 3;
	});
	dispatcher.prependListener(event, [&orderList, &order](int) {
		orderList[order++] = 4;
	});
	dispatcher.prependListener(event, [&orderList, &order](int) {
		orderList[order++] = 5;
	});

	REQUIRE(orderList == std::vector<int>{ 0, 0, 0, 0, 0 });

	order = 0;
	dispatcher.dispatch(event, 3);
	REQUIRE(orderList == std::vector<int>{ 5, 4, 3, 0, 0 });
	dispatcher.dispatch(event);
	REQUIRE(orderList == std::vector<int>{ 5, 4, 3, 2, 1 });
}

TEST_CASE("HeterEventDispatcher, insertListener")
{
	eventpp::HeterEventDispatcher<int, eventpp::HeterTuple<void (), void (int)> > dispatcher;
	constexpr int event = 3;

	std::vector<int> orderList(5);

	int order;

	auto h1 = dispatcher.appendListener(event, [&orderList, &order]() {
		orderList[order++] = 1;
	});
	dispatcher.insertListener(event, [&orderList, &order]() {
		orderList[order++] = 2;
	}, h1);

	auto h2 = dispatcher.appendListener(event, [&orderList, &order](int) {
		orderList[order++] = 3;
	});
	dispatcher.insertListener(event, [&orderList, &order](int) {
		orderList[order++] = 4;
	}, h2);
	dispatcher.insertListener(event, [&orderList, &order](int) {
		orderList[order++] = 5;
	}, h2);

	REQUIRE(orderList == std::vector<int>{ 0, 0, 0, 0, 0 });

	order = 0;
	dispatcher.dispatch(event, 3);
	REQUIRE(orderList == std::vector<int>{ 4, 5, 3, 0, 0 });
	dispatcher.dispatch(event);
	REQUIRE(orderList == std::vector<int>{ 4, 5, 3, 2, 1 });
}

TEST_CASE("HeterEventDispatcher, removeListener")
{
	eventpp::HeterEventDispatcher<int, eventpp::HeterTuple<void (), void (int)> > dispatcher;
	constexpr int event = 3;

	std::vector<int> dataList(5);

	auto h1 = dispatcher.appendListener(event, [&dataList]() {
		++dataList[0];
	});
	auto h2 = dispatcher.appendListener(event, [&dataList]() {
		++dataList[1];
	});
	auto h3 = dispatcher.appendListener(event, [&dataList](int) {
		++dataList[2];
	});
	auto h4 = dispatcher.appendListener(event, [&dataList](int) {
		++dataList[3];
	});
	auto h5 = dispatcher.appendListener(event, [&dataList](int) {
		++dataList[4];
	});

	REQUIRE(dataList == std::vector<int>{ 0, 0, 0, 0, 0 });

	dispatcher.dispatch(event, 3);
	REQUIRE(dataList == std::vector<int>{ 0, 0, 1, 1, 1 });
	dispatcher.dispatch(event);
	REQUIRE(dataList == std::vector<int>{ 1, 1, 1, 1, 1 });

	dispatcher.removeListener(event, h2);
	dispatcher.dispatch(event, 3);
	dispatcher.dispatch(event);
	REQUIRE(dataList == std::vector<int>{ 2, 1, 2, 2, 2 });

	// double remove, no effect
	dispatcher.removeListener(event, h2);
	dispatcher.dispatch(event, 3);
	dispatcher.dispatch(event);
	REQUIRE(dataList == std::vector<int>{ 3, 1, 3, 3, 3 });

	dispatcher.removeListener(event, h3);
	dispatcher.dispatch(event, 3);
	dispatcher.dispatch(event);
	REQUIRE(dataList == std::vector<int>{ 4, 1, 3, 4, 4 });

	dispatcher.removeListener(event, h5);
	dispatcher.dispatch(event, 3);
	dispatcher.dispatch(event);
	REQUIRE(dataList == std::vector<int>{ 5, 1, 3, 5, 4 });

	dispatcher.removeListener(event, h1);
	dispatcher.dispatch(event, 3);
	dispatcher.dispatch(event);
	REQUIRE(dataList == std::vector<int>{ 5, 1, 3, 6, 4 });

	dispatcher.removeListener(event, h4);
	dispatcher.dispatch(event, 3);
	dispatcher.dispatch(event);
	REQUIRE(dataList == std::vector<int>{ 5, 1, 3, 6, 4 });
}

TEST_CASE("HeterEventDispatcher, forEach")
{
	eventpp::HeterEventDispatcher<int, eventpp::HeterTuple<int (), int (int)> > dispatcher;
	constexpr int event = 3;

	dispatcher.appendListener(event, []() { return 1; });
	dispatcher.appendListener(event, []() { return 2; });
	dispatcher.appendListener(event, []() { return 3; });

	dispatcher.appendListener(event, [](int n) { return n + 5 + 0; });
	dispatcher.appendListener(event, [](int n) { return n + 5 + 1; });
	dispatcher.appendListener(event, [](int n) { return n + 5 + 2; });

	int i = 1;
	dispatcher.forEach<int ()>(event, [&i](const std::function<int ()> & callback) {
		REQUIRE(callback() == i);
		++i;
	});

	i = 0;
	dispatcher.forEach<int (int)>(event, [&i](const std::function<int (int)> & callback) {
		REQUIRE(callback(3) == 8 + i);
		++i;
	});
}

TEST_CASE("HeterEventDispatcher, forEachIf")
{
	eventpp::HeterEventDispatcher<int, eventpp::HeterTuple<void (), void (int)> > dispatcher;
	constexpr int event = 3;

	std::vector<int> dataListNoArg(3);
	std::vector<int> dataListWithArg(3);

	dispatcher.appendListener(event, [&dataListNoArg]() {
		dataListNoArg[0] += 1;
	});
	dispatcher.appendListener(event, [&dataListNoArg]() {
		dataListNoArg[1] += 2;
	});
	dispatcher.appendListener(event, [&dataListNoArg]() {
		dataListNoArg[2] += 3;
	});

	dispatcher.appendListener(event, [&dataListWithArg](int n) {
		dataListWithArg[0] += n;
	});
	dispatcher.appendListener(event, [&dataListWithArg](int n) {
		dataListWithArg[1] += n * 2;
	});
	dispatcher.appendListener(event, [&dataListWithArg](int n) {
		dataListWithArg[2] += n * 3;
	});

	REQUIRE(dataListNoArg == std::vector<int>{ 0, 0, 0 });
	REQUIRE(dataListWithArg == std::vector<int>{ 0, 0, 0 });

	int i = 0;
	bool result = dispatcher.forEachIf<void ()>(event, [&i](const std::function<void ()> & callback) -> bool {
		callback();
		++i;

		return i != 2;
	});

	REQUIRE(! result);
	REQUIRE(dataListNoArg == std::vector<int>{ 1, 2, 0 });
	REQUIRE(dataListWithArg == std::vector<int>{ 0, 0, 0 });

	i = 0;
	result = dispatcher.forEachIf<void (int)>(event, [&i](const std::function<void (int)> & callback) -> bool {
		callback(3);
		++i;

		return i != 1;
	});

	REQUIRE(! result);
	REQUIRE(dataListNoArg == std::vector<int>{ 1, 2, 0 });
	REQUIRE(dataListWithArg == std::vector<int>{ 3, 0, 0 });
}

TEST_CASE("HeterEventDispatcher, invoke")
{
	eventpp::HeterEventDispatcher<int, eventpp::HeterTuple<void (), void (int)> > dispatcher;
	constexpr int event = 3;

	std::vector<int> dataListNoArg(3);
	std::vector<int> dataListWithArg(3);

	dispatcher.appendListener(event, [&dataListNoArg]() {
		dataListNoArg[0] += 1;
	});
	dispatcher.appendListener(event, [&dataListNoArg]() {
		dataListNoArg[1] += 2;
	});
	dispatcher.appendListener(event, [&dataListNoArg]() {
		dataListNoArg[2] += 3;
	});

	dispatcher.appendListener(event, [&dataListWithArg](int n) {
		dataListWithArg[0] += n;
	});
	dispatcher.appendListener(event, [&dataListWithArg](int n) {
		dataListWithArg[1] += n * 2;
	});
	dispatcher.appendListener(event, [&dataListWithArg](int n) {
		dataListWithArg[2] += n * 3;
	});

	REQUIRE(dataListNoArg == std::vector<int>{ 0, 0, 0 });
	REQUIRE(dataListWithArg == std::vector<int>{ 0, 0, 0 });

	dispatcher.dispatch(event);
	REQUIRE(dataListNoArg == std::vector<int>{ 1, 2, 3 });
	REQUIRE(dataListWithArg == std::vector<int>{ 0, 0, 0 });

	dispatcher.dispatch(event);
	REQUIRE(dataListNoArg == std::vector<int>{ 2, 4, 6 });
	REQUIRE(dataListWithArg == std::vector<int>{ 0, 0, 0 });

	dispatcher.dispatch(event, 1);
	REQUIRE(dataListNoArg == std::vector<int>{ 2, 4, 6 });
	REQUIRE(dataListWithArg == std::vector<int>{ 1, 2, 3 });

	dispatcher.dispatch(event, 2);
	REQUIRE(dataListNoArg == std::vector<int>{ 2, 4, 6 });
	REQUIRE(dataListWithArg == std::vector<int>{ 3, 6, 9 });
}

TEST_CASE("HeterEventDispatcher, invoke, ArgumentPassingIncludeEvent")
{
	struct EventPolicies
	{
		using ArgumentPassingMode = eventpp::ArgumentPassingIncludeEvent;
	};

	eventpp::HeterEventDispatcher<int, eventpp::HeterTuple<void (int), void (int, int)>, EventPolicies> dispatcher;
	constexpr int event = 3;

	std::vector<int> dataListNoArg(3);
	std::vector<int> dataListWithArg(3);

	dispatcher.appendListener(event, [&dataListNoArg](int) {
		dataListNoArg[0] += 1;
	});
	dispatcher.appendListener(event, [&dataListNoArg](int) {
		dataListNoArg[1] += 2;
	});
	dispatcher.appendListener(event, [&dataListNoArg](int) {
		dataListNoArg[2] += 3;
	});

	dispatcher.appendListener(event, [&dataListWithArg](int, int n) {
		dataListWithArg[0] += n;
	});
	dispatcher.appendListener(event, [&dataListWithArg](int, int n) {
		dataListWithArg[1] += n * 2;
	});
	dispatcher.appendListener(event, [&dataListWithArg](int, int n) {
		dataListWithArg[2] += n * 3;
	});

	REQUIRE(dataListNoArg == std::vector<int>{ 0, 0, 0 });
	REQUIRE(dataListWithArg == std::vector<int>{ 0, 0, 0 });

	dispatcher.dispatch(event);
	REQUIRE(dataListNoArg == std::vector<int>{ 1, 2, 3 });
	REQUIRE(dataListWithArg == std::vector<int>{ 0, 0, 0 });

	dispatcher.dispatch(event);
	REQUIRE(dataListNoArg == std::vector<int>{ 2, 4, 6 });
	REQUIRE(dataListWithArg == std::vector<int>{ 0, 0, 0 });

	dispatcher.dispatch(event, 1);
	REQUIRE(dataListNoArg == std::vector<int>{ 2, 4, 6 });
	REQUIRE(dataListWithArg == std::vector<int>{ 1, 2, 3 });

	dispatcher.dispatch(event, 2);
	REQUIRE(dataListNoArg == std::vector<int>{ 2, 4, 6 });
	REQUIRE(dataListWithArg == std::vector<int>{ 3, 6, 9 });
}

TEST_CASE("HeterEventDispatcher, getEvent")
{
	struct MyEvent {
		int type;
	};
	struct EventPolicies
	{
		static int getEvent(const MyEvent & e) {
			return e.type;
		}

		static int getEvent(const MyEvent & e, int) {
			return e.type;
		}
	};


	eventpp::HeterEventDispatcher<int, eventpp::HeterTuple<void (), void (int)>, EventPolicies> dispatcher;
	constexpr int event = 3;

	std::vector<int> dataList(2);

	dispatcher.appendListener(event, [&dataList]() {
		++dataList[0];
	});
	dispatcher.appendListener(event, [&dataList](int i) {
		dataList[1] = i;
	});

	REQUIRE(dataList == std::vector<int>{ 0, 0 });

	dispatcher.dispatch(MyEvent{ event });
	REQUIRE(dataList == std::vector<int>{ 1, 0 });
	dispatcher.dispatch(MyEvent{ event }, 3);
	REQUIRE(dataList == std::vector<int>{ 1, 3 });
}

TEST_CASE("HeterEventDispatcher, getEvent, ArgumentPassingIncludeEvent")
{
	struct MyEvent {
		int type;
		int param;
	};
	struct EventPolicies
	{
		using ArgumentPassingMode = eventpp::ArgumentPassingIncludeEvent;

		static int getEvent(const MyEvent & e) {
			return e.type;
		}

		static int getEvent(const MyEvent & e, int) {
			return e.type;
		}
	};


	eventpp::HeterEventDispatcher<int, eventpp::HeterTuple<void (const MyEvent &), void (const MyEvent &, int)>, EventPolicies> dispatcher;
	constexpr int event = 3;

	std::vector<int> dataList(2);

	dispatcher.appendListener(event, [&dataList](const MyEvent & e) {
		dataList[0] = e.param;
	});
	dispatcher.appendListener(event, [&dataList](const MyEvent & e, int i) {
		dataList[1] = e.param + i;
	});

	REQUIRE(dataList == std::vector<int>{ 0, 0 });

	dispatcher.dispatch(MyEvent{ event, 5 });
	REQUIRE(dataList == std::vector<int>{ 5, 0 });
	dispatcher.dispatch(MyEvent{ event, 5 }, 3);
	REQUIRE(dataList == std::vector<int>{ 5, 8 });
}

TEST_CASE("HeterEventDispatcher, event filter")
{
	struct MyPolicies {
		using Mixins = eventpp::MixinList<eventpp::MixinHeterFilter>;
		using ArgumentPassingMode = eventpp::ArgumentPassingIncludeEvent;
	};
	using ED = eventpp::HeterEventDispatcher<int, eventpp::HeterTuple<void (int, int), void (int)>, MyPolicies>;
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
		auto handle1 = dispatcher.appendFilter([&filterData](int, int) -> bool {
			++filterData[0];
			return true;
		});
		auto handle2 = dispatcher.appendFilter([&filterData](int) -> bool {
			++filterData[1];
			return true;
		});

		for(int i = 0; i < itemCount; ++i) {
			dispatcher.dispatch(i, 58);
			dispatcher.dispatch(i);
		}

		REQUIRE(filterData == std::vector<int>{ itemCount, itemCount });
		REQUIRE(dataList == std::vector<int>{ 58, 58, 58, 58, 58 });

		dispatcher.removeFilter(handle1);

		for(int i = 0; i < itemCount; ++i) {
			dispatcher.dispatch(i, 38);
			dispatcher.dispatch(i);
		}

		REQUIRE(filterData == std::vector<int>{ itemCount, itemCount * 2 });

		dispatcher.removeFilter(handle2);
		dispatcher.removeFilter(handle2);

		for(int i = 0; i < itemCount; ++i) {
			dispatcher.dispatch(i, 38);
			dispatcher.dispatch(i);
		}

		REQUIRE(filterData == std::vector<int>{ itemCount, itemCount * 2 });
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

