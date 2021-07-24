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

#include <functional>

TEST_CASE("EventQueue, std::string, void (const std::string &)")
{
	eventpp::EventQueue<std::string, void (const std::string &)> queue;

	int a = 1;
	int b = 5;

	queue.appendListener("event1", [&a](const std::string &) {
		a = 2;
	});
	queue.appendListener("event1", eraseArgs1([&b]() {
		b = 8;
	}));

	REQUIRE(a != 2);
	REQUIRE(b != 8);

	queue.enqueue("event1");
	queue.process();
	REQUIRE(a == 2);
	REQUIRE(b == 8);
}

TEST_CASE("EventQueue, int, void ()")
{
	eventpp::EventQueue<int, void ()> queue;

	int a = 1;
	int b = 5;

	queue.appendListener(3, [&a]() {
		a += 1;
	});
	queue.appendListener(3, [&b]() {
		b += 3;
	});

	REQUIRE(a != 2);
	REQUIRE(b != 8);

	queue.enqueue(3);
	queue.process();

	REQUIRE(a == 2);
	REQUIRE(b == 8);
}

TEST_CASE("EventQueue, processOne, int, void ()")
{
	eventpp::EventQueue<int, void ()> queue;

	int a = 1;
	int b = 5;

	queue.appendListener(3, [&a]() {
		a += 1;
	});
	queue.appendListener(5, [&b]() {
		b += 3;
	});

	REQUIRE(a == 1);
	REQUIRE(b == 5);

	queue.enqueue(3);
	queue.enqueue(5);

	REQUIRE(queue.processOne());
	REQUIRE(a == 2);
	REQUIRE(b == 5);

	REQUIRE(queue.processOne());
	REQUIRE(a == 2);
	REQUIRE(b == 8);

	REQUIRE(! queue.processOne());
}

TEST_CASE("EventQueue, int, void (const std::string &, int)")
{
	struct NonDefaultConstructible
	{
		explicit NonDefaultConstructible(const int i) : i(i) {}
		int i;
	};

	eventpp::EventQueue<int, void (const std::string &, const NonDefaultConstructible &)> queue;

	const int event = 3;

	std::vector<std::string> sList(2);
	std::vector<int> iList(sList.size());

	queue.appendListener(event, [&sList, &iList](const std::string & s, const NonDefaultConstructible & n) {
		sList[0] = s;
		iList[0] = n.i;
	});
	queue.appendListener(event, [&sList, &iList](const std::string & s, NonDefaultConstructible n) {
		sList[1] = s + "2";
		iList[1] = n.i + 5;
	});

	REQUIRE(sList[0] != "first");
	REQUIRE(sList[1] != "first2");
	REQUIRE(iList[0] != 3);
	REQUIRE(iList[1] != 8);

	SECTION("Parameters") {
		queue.enqueue(event, "first", NonDefaultConstructible(3));
		queue.process();

		REQUIRE(sList[0] == "first");
		REQUIRE(sList[1] == "first2");
		REQUIRE(iList[0] == 3);
		REQUIRE(iList[1] == 8);
	}

	SECTION("Reference parameters should not be modified") {
		std::string s = "first";
		queue.enqueue(event, s, NonDefaultConstructible(3));
		s = "";
		queue.process();

		REQUIRE(sList[0] == "first");
		REQUIRE(sList[1] == "first2");
		REQUIRE(iList[0] == 3);
		REQUIRE(iList[1] == 8);
	}
}

TEST_CASE("EventQueue, customized event")
{
	struct MyEvent {
		int type;
		std::string message;
		int param;
	};

	struct MyEventPolicies
	{
		static int getEvent(const MyEvent & e, std::string) {
			return e.type;
		}
	};

	eventpp::EventQueue<int, void (const MyEvent &, std::string), MyEventPolicies> queue;

	std::string a = "Hello ";
	std::string b = "World ";

	queue.appendListener(3, [&a](const MyEvent & e, const std::string & s) {
		a += e.message + s + std::to_string(e.param);
	});
	queue.appendListener(3, [&b](const MyEvent & e, const std::string & s) {
		b += e.message + s + std::to_string(e.param);
	});

	REQUIRE(a == "Hello ");
	REQUIRE(b == "World ");

	queue.enqueue(MyEvent { 3, "very ", 38 }, "good");
	queue.process();

	REQUIRE(a == "Hello very good38");
	REQUIRE(b == "World very good38");
}

TEST_CASE("EventQueue, no memory leak in queued arguments")
{
	using SP = std::shared_ptr<int>;
	using WP = std::weak_ptr<int>;
	using EQ = eventpp::EventQueue<int, void (SP)>;

	std::unique_ptr<EQ> queue(new EQ());
	std::vector<WP> wpList;

	auto add = [&wpList, &queue](int n) {
		SP sp(std::make_shared<int>(n));
		queue->enqueue(n, sp);
		wpList.push_back(WP(sp));
	};

	add(1);
	add(2);
	add(3);

	REQUIRE(! checkAllWeakPtrAreFreed(wpList));

	SECTION("No memory leak after process()") {
		queue->process();
		REQUIRE(checkAllWeakPtrAreFreed(wpList));
	}

	SECTION("No memory leak after queue is deleted") {
		queue.reset();
		REQUIRE(checkAllWeakPtrAreFreed(wpList));
	}
}

TEST_CASE("EventQueue, no memory leak in queued arguments after queue is destroyed")
{
	using SP = std::shared_ptr<int>;
	using WP = std::weak_ptr<int>;
	using EQ = eventpp::EventQueue<int, void (SP)>;

	std::vector<WP> wpList;

	{
		std::unique_ptr<EQ> queue(new EQ());

		auto add = [&wpList, &queue](int n) {
			SP sp(std::make_shared<int>(n));
			queue->enqueue(n, sp);
			wpList.push_back(WP(sp));
		};

		add(1);
		add(2);
		add(3);

		REQUIRE(! checkAllWeakPtrAreFreed(wpList));
	}

	REQUIRE(checkAllWeakPtrAreFreed(wpList));
}

TEST_CASE("EventQueue, no memory leak or double free in queued arguments")
{
	struct Item {
		Item(const int index, std::vector<int> * counterList)
			: index(index), counterList(counterList)
		{
			++(*counterList)[index];
		}

		~Item()
		{
			--(*counterList)[index];
		}

		Item(Item && other)
			: index(other.index), counterList(other.counterList)
		{
			++(*counterList)[index];
		}

		Item(const Item & other)
			: index(other.index), counterList(other.counterList)
		{
			++(*counterList)[index];
		}

		Item & operator = (const Item & other)
		{
			index = other.index;
			counterList = other.counterList;
			++(*counterList)[index];
			return *this;
		}

		int index;
		std::vector<int> * counterList;
	};
	using EQ = eventpp::EventQueue<int, void (const Item &)>;

	std::vector<int> counterList(4);
	std::unique_ptr<EQ> queue(new EQ());

	auto add = [&counterList, &queue](int n) {
		queue->enqueue(n, Item(n, &counterList));
	};

	add(0);
	add(1);
	add(2);
	add(3);

	REQUIRE(counterList == std::vector<int>{ 1, 1, 1, 1 });

	SECTION("No memory leak or double free after process()") {
		queue->process();
		REQUIRE(counterList == std::vector<int>{ 0, 0, 0, 0 });
	}

	SECTION("No memory leak or double free after queue is deleted") {
		queue.reset();
		REQUIRE(counterList == std::vector<int>{ 0, 0, 0, 0 });
	}
}

TEST_CASE("EventQueue, non-copyable but movable unique_ptr")
{
	using PTR = std::unique_ptr<int>;
	using EQ = eventpp::EventQueue<int, void (const PTR &)>;
	EQ queue;

	constexpr int itemCount = 3;

	std::vector<int> dataList(itemCount);

	queue.appendListener(3, [&dataList](const PTR & ptr) {
		++dataList[*ptr];
	});

	queue.enqueue(3, PTR(new int(0)));
	queue.enqueue(3, PTR(new int(1)));
	queue.enqueue(3, PTR(new int(2)));

	SECTION("process") {
		queue.process();
		REQUIRE(dataList == std::vector<int>{ 1, 1, 1 });
	}

	// peekEvent doesn't compile, it requires the argument copyable.

	SECTION("takeEvent/dispatch") {
		EQ::QueuedEvent event;
		REQUIRE(queue.takeEvent(&event));
		queue.dispatch(event);
		REQUIRE(dataList == std::vector<int>{ 1, 0, 0 });
	}

	SECTION("takeEvent/process") {
		EQ::QueuedEvent event;
		REQUIRE(queue.takeEvent(&event));
		queue.process();
		REQUIRE(dataList == std::vector<int>{ 0, 1, 1 });
	}
}

TEST_CASE("EventQueue, peekEvent/takeEvent/dispatch")
{
	using SP = std::shared_ptr<int>;
	using WP = std::weak_ptr<int>;
	using EQ = eventpp::EventQueue<int, void (SP)>;

	std::unique_ptr<EQ> queue(new EQ());
	std::vector<WP> wpList;
	constexpr int itemCount = 3;

	std::vector<int> dataList(itemCount);

	queue->appendListener(3, [&dataList](const SP & sp) {
		++dataList[*sp];
	});

	auto add = [&wpList, &queue](int e, int n) {
		SP sp(std::make_shared<int>(n));
		queue->enqueue(e, sp);
		wpList.push_back(WP(sp));
	};

	add(3, 0);
	add(3, 1);
	add(3, 2);

	SECTION("peek") {
		EQ::QueuedEvent event;
		REQUIRE(queue->peekEvent(&event));
		REQUIRE(event.event == 3);
		REQUIRE(*event.getArgument<0>() == 0);
		REQUIRE(wpList[0].use_count() == 2);
	}

	SECTION("peek/peek") {
		EQ::QueuedEvent event;
		REQUIRE(queue->peekEvent(&event));
		REQUIRE(event.event == 3);
		REQUIRE(*event.getArgument<0>() == 0);
		REQUIRE(wpList[0].use_count() == 2);

		EQ::QueuedEvent event2;
		REQUIRE(queue->peekEvent(&event2));
		REQUIRE(event2.event == 3);
		REQUIRE(*event2.getArgument<0>() == 0);
		REQUIRE(wpList[0].use_count() == 3);
	}

	SECTION("peek/take") {
		EQ::QueuedEvent event;
		REQUIRE(queue->peekEvent(&event));
		REQUIRE(event.event == 3);
		REQUIRE(*event.getArgument<0>() == 0);
		REQUIRE(wpList[0].use_count() == 2);

		EQ::QueuedEvent event2;
		REQUIRE(queue->takeEvent(&event2));
		REQUIRE(event2.event == 3);
		REQUIRE(*event2.getArgument<0>() == 0);
		REQUIRE(wpList[0].use_count() == 2);
	}

	SECTION("peek/take/peek") {
		EQ::QueuedEvent event;
		REQUIRE(queue->peekEvent(&event));
		REQUIRE(event.event == 3);
		REQUIRE(*event.getArgument<0>() == 0);
		REQUIRE(wpList[0].use_count() == 2);

		EQ::QueuedEvent event2;
		REQUIRE(queue->takeEvent(&event2));
		REQUIRE(event2.event == 3);
		REQUIRE(*event2.getArgument<0>() == 0);
		REQUIRE(wpList[0].use_count() == 2);

		EQ::QueuedEvent event3;
		REQUIRE(queue->peekEvent(&event3));
		REQUIRE(event3.event == 3);
		REQUIRE(*event3.getArgument<0>() == 1);
		REQUIRE(wpList[0].use_count() == 2);
		REQUIRE(wpList[1].use_count() == 2);
	}

	SECTION("peek/dispatch/peek/dispatch again") {
		EQ::QueuedEvent event;
		REQUIRE(queue->peekEvent(&event));
		REQUIRE(event.event == 3);
		REQUIRE(*event.getArgument<0>() == 0);
		REQUIRE(wpList[0].use_count() == 2);

		queue->dispatch(event);

		EQ::QueuedEvent event2;
		REQUIRE(queue->peekEvent(&event2));
		REQUIRE(event2.event == 3);
		REQUIRE(*event2.getArgument<0>() == 0);
		REQUIRE(wpList[0].use_count() == 3);

		REQUIRE(dataList == std::vector<int>{ 1, 0, 0 });

		queue->dispatch(event);
		REQUIRE(dataList == std::vector<int>{ 2, 0, 0 });
	}

	SECTION("process") {
		// test the queue works with simple process(), ensure the process()
		// in the next "take all/process" works correctly.
		REQUIRE(dataList == std::vector<int>{ 0, 0, 0 });
		queue->process();
		REQUIRE(dataList == std::vector<int>{ 1, 1, 1 });
	}

	SECTION("take all/process") {
		for(int i = 0; i < itemCount; ++i) {
			EQ::QueuedEvent event;
			REQUIRE(queue->takeEvent(&event));
		}

		EQ::QueuedEvent event;
		REQUIRE(! queue->peekEvent(&event));
		REQUIRE(! queue->takeEvent(&event));

		REQUIRE(dataList == std::vector<int>{ 0, 0, 0 });
		queue->process();
		REQUIRE(dataList == std::vector<int>{ 0, 0, 0 });
	}
}

TEST_CASE("EventQueue, clearEvents")
{
	eventpp::EventQueue<int, void ()> queue;

	int a = 1;
	int b = 5;

	queue.appendListener(3, [&a]() {
		a += 1;
	});
	queue.appendListener(3, [&b]() {
		b += 3;
	});

	REQUIRE(a == 1);
	REQUIRE(b == 5);

	queue.enqueue(3);
	queue.process();

	REQUIRE(a == 2);
	REQUIRE(b == 8);

	queue.enqueue(3);
	queue.clearEvents();
	queue.process();

	REQUIRE(a == 2);
	REQUIRE(b == 8);
}

TEST_CASE("EventQueue, processIf")
{
	eventpp::EventQueue<int, void (int)> queue;

	std::vector<int> dataList(3);

	queue.appendListener(5, [&dataList](int) {
		++dataList[0];
	});
	queue.appendListener(6, [&dataList](int) {
		++dataList[1];
	});
	queue.appendListener(7, [&dataList](int) {
		++dataList[2];
	});

	REQUIRE(dataList == std::vector<int>{ 0, 0, 0 });

	queue.enqueue(5);
	queue.enqueue(6);
	queue.enqueue(7);
	queue.process();
	REQUIRE(dataList == std::vector<int>{ 1, 1, 1 });

	queue.enqueue(5);
	queue.enqueue(6);
	queue.enqueue(7);
	REQUIRE(queue.processIf([](const int event) -> bool { return event == 6; }));
	REQUIRE(dataList == std::vector<int>{ 1, 2, 1 });
	// Now the queue contains 5, 7

	queue.enqueue(5);
	queue.enqueue(6);
	queue.enqueue(7);
	REQUIRE(queue.processIf([](const int event) -> bool { return event == 5; }));
	REQUIRE(dataList == std::vector<int>{ 3, 2, 1 });
	// Now the queue contains 6, 7, 7

	// Ensure the callback in processIf is not called for unncessary times.
	// Veriy the internal loops in processIf is correct.
	std::vector<int> callbackCounters(10);

	queue.enqueue(5);
	queue.enqueue(6);
	queue.enqueue(7);
	REQUIRE(queue.processIf([&callbackCounters](const int event) -> bool {
		++callbackCounters[event];
		return event == 7;
	}));
	REQUIRE(dataList == std::vector<int>{ 3, 2, 4 });
	// Now the queue contains 5, 6, 6

	REQUIRE(callbackCounters[5] == 1);
	REQUIRE(callbackCounters[6] == 2);
	REQUIRE(callbackCounters[7] == 3);

	queue.process();
	REQUIRE(dataList == std::vector<int>{ 4, 4, 4 });

	REQUIRE(! queue.processIf([](const int /*event*/) -> bool { return true; }));
}

class MyInt
{
public:
	template <typename T>
	MyInt(const T value) : value(value) {
	}

	int getValue() const {
		return value;
	}

private:
	int value;
};

bool operator == (const MyInt & a, const MyInt & b)
{
	return a.getValue() == b.getValue();
}

namespace std
{
template<> struct hash<MyInt>
{
	std::size_t operator()(const MyInt & value) const noexcept
	{
		return std::hash<int>()(value.getValue());
	}
};
} //namespace std

// This test is to reproduce a compiling issue that existed before this testg was added.
// For MyInt in above, if its constructor is a plain `MyInt(const int value)`, the code
// compiles fine. But if its constructor is template like how it is now, the compile
// fails on the line `queue.enqueue(5);`, because MyInt is being constructed with a std::tuple<int>.
// The issue is fixed after the test was added.
TEST_CASE("EventQueue, implicit cast with template constructor")
{
	eventpp::EventQueue<MyInt, void(const MyInt &)> queue;

	std::vector<int> dataList(3);

	queue.appendListener(5, [&dataList](const MyInt & e) {
		REQUIRE(e.getValue() == 5);
		++dataList[0];
	});
	queue.appendListener(6, [&dataList](const MyInt & e) {
		REQUIRE(e.getValue() == 6);
		++dataList[1];
	});
	queue.appendListener(7, [&dataList](const MyInt & value) {
		REQUIRE(value.getValue() == 2);
		++dataList[2];
	});

	REQUIRE(dataList == std::vector<int>{ 0, 0, 0 });

	queue.enqueue(5);
	queue.enqueue(6);
	queue.enqueue(7, 2); // This triggers the issue in the overloaded enqueue
	queue.process();
	REQUIRE(dataList == std::vector<int>{ 1, 1, 1 });
}
