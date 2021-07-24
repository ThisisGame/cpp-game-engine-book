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
#include "eventpp/callbacklist.h"
#include "eventpp/eventdispatcher.h"

#include <memory>

class CopyMoveCounter
{
public:
	struct Counter
	{
		int copied;
		int moved;
		int called;
	};

public:
	CopyMoveCounter()
		: counter(std::make_shared<Counter>()) {
	}

	CopyMoveCounter(const CopyMoveCounter & other)
		: counter(other.counter) {
		++counter->copied;
	}

	CopyMoveCounter(CopyMoveCounter && other)
		: counter(std::move(other.counter)) {
		++counter->moved;
	}

	const Counter & getCounter() const {
		return *counter;
	}

	void called() const {
		// This is to test the callback is added to callbacklist properly and be invoked.
		++counter->called;
	}

	int getCalledAndReset() const {
		const int result = counter->called;
		counter->called = 0;
		return result;
	}

private:
	std::shared_ptr<Counter> counter;
};

TEST_CASE("CopyMoveCounter")
{
	CopyMoveCounter obj1;
	REQUIRE(obj1.getCounter().copied == 0);
	REQUIRE(obj1.getCounter().moved == 0);

	CopyMoveCounter copy1(obj1);
	REQUIRE(obj1.getCounter().copied == 1);
	REQUIRE(obj1.getCounter().moved == 0);

	CopyMoveCounter copy2(copy1);
	REQUIRE(obj1.getCounter().copied == 2);
	REQUIRE(obj1.getCounter().moved == 0);

	CopyMoveCounter move1(std::move(obj1));
	REQUIRE(move1.getCounter().copied == 2); // can't use obj1 any more
	REQUIRE(move1.getCounter().moved == 1);

	CopyMoveCounter move2(std::move(move1));
	REQUIRE(move2.getCounter().copied == 2);
	REQUIRE(move2.getCounter().moved == 2);
}

TEST_CASE("copymove, CallbackList<void(const &)>, callback(const &)")
{
	using CL = eventpp::CallbackList<void(const CopyMoveCounter &, int)>;
	CL callbackList;

	auto func = [](const CopyMoveCounter & obj, const int expected) {
		REQUIRE(obj.getCounter().copied == expected);
		obj.called();
	};

	SECTION("Raw function: temporary object") {
		func(CopyMoveCounter(), 0);
	}
	SECTION("Raw function: object variable") {
		CopyMoveCounter obj1;
		REQUIRE(obj1.getCalledAndReset() == 0);
		func(obj1, 0);
		REQUIRE(obj1.getCalledAndReset() > 0);
	}
	
	SECTION("CallbackList: temporary object") {
		callbackList.append(func);
		callbackList(CopyMoveCounter(), 0);
	}
	SECTION("CallbackList: object variable") {
		callbackList.append(func);
		CopyMoveCounter obj1;
		REQUIRE(obj1.getCalledAndReset() == 0);
		callbackList(obj1, 0);
		REQUIRE(obj1.getCalledAndReset() > 0);
	}
}

TEST_CASE("copymove, CallbackList<void(value)>, callback(value)")
{
	using CL = eventpp::CallbackList<void(CopyMoveCounter, int)>;
	CL callbackList;

	auto func = [](CopyMoveCounter obj, const int expected) {
		REQUIRE(obj.getCounter().copied == expected);
		obj.called();
	};
	
	SECTION("Raw function: temporary object") {
		func(CopyMoveCounter(), 0);
	}
	SECTION("Raw function: object variable") {
		CopyMoveCounter obj1;
		REQUIRE(obj1.getCalledAndReset() == 0);
		func(obj1, 1);
		REQUIRE(obj1.getCalledAndReset() > 0);
	}
	
	// For the reason in the comment in CallbackList::operator(), i.e, can't use std::forward,
	// the copied count in the CallbackList version is larger by 1 than the raw function version.
	SECTION("CallbackList: temporary object") {
		callbackList.append(func);
		callbackList(CopyMoveCounter(), 1);
	}
	SECTION("CallbackList: object variable") {
		callbackList.append(func);
		CopyMoveCounter obj1;
		REQUIRE(obj1.getCalledAndReset() == 0);
		callbackList(obj1, 2);
		REQUIRE(obj1.getCalledAndReset() > 0);
	}
}

TEST_CASE("copymove, CallbackList<void(value)>, callback(const &)")
{
	using CL = eventpp::CallbackList<void(CopyMoveCounter, int)>;
	CL callbackList;

	auto func = [](const CopyMoveCounter & obj, const int expected) {
		REQUIRE(obj.getCounter().copied == expected);
		obj.called();
	};
	
	SECTION("Raw function: temporary object") {
		func(CopyMoveCounter(), 0);
	}
	SECTION("Raw function: object variable") {
		CopyMoveCounter obj1;
		REQUIRE(obj1.getCalledAndReset() == 0);
		func(obj1, 0);
		REQUIRE(obj1.getCalledAndReset() > 0);
	}
	
	SECTION("CallbackList: temporary object") {
		callbackList.append(func);
		callbackList(CopyMoveCounter(), 1);
	}
	SECTION("CallbackList: object variable") {
		callbackList.append(func);
		CopyMoveCounter obj1;
		REQUIRE(obj1.getCalledAndReset() == 0);
		callbackList(obj1, 2);
		REQUIRE(obj1.getCalledAndReset() > 0);
	}
}

TEST_CASE("copymove, CallbackList<void(const &)>, callback(value)")
{
	using CL = eventpp::CallbackList<void(const CopyMoveCounter &, int)>;
	CL callbackList;

	auto func = [](CopyMoveCounter obj, const int expected) {
		REQUIRE(obj.getCounter().copied == expected);
		obj.called();
	};

	SECTION("Raw function: temporary object") {
		func(CopyMoveCounter(), 0);
	}
	SECTION("Raw function: object variable") {
		CopyMoveCounter obj1;
		const CopyMoveCounter & ref1 = obj1;
		REQUIRE(ref1.getCalledAndReset() == 0);
		func(ref1, 1);
		REQUIRE(ref1.getCalledAndReset() > 0);
	}

	SECTION("CallbackList: temporary object") {
		callbackList.append(func);
		callbackList(CopyMoveCounter(), 1);
	}
	SECTION("CallbackList: object variable") {
		callbackList.append(func);
		CopyMoveCounter obj1;
		const CopyMoveCounter & ref1 = obj1;
		REQUIRE(ref1.getCalledAndReset() == 0);
		callbackList(ref1, 1);
		REQUIRE(ref1.getCalledAndReset() > 0);
	}
}

TEST_CASE("copymove, EventDispatcher<void(const &)>, callback(const &)")
{
	using ED = eventpp::EventDispatcher<int, void(const CopyMoveCounter &, int)>;
	ED eventDispatcher;

	auto func = [](const CopyMoveCounter & obj, const int expected) {
		REQUIRE(obj.getCounter().copied == expected);
		obj.called();
	};

	SECTION("Raw function: temporary object") {
		func(CopyMoveCounter(), 0);
	}
	SECTION("Raw function: object variable") {
		CopyMoveCounter obj1;
		REQUIRE(obj1.getCalledAndReset() == 0);
		func(obj1, 0);
		REQUIRE(obj1.getCalledAndReset() > 0);
	}

	SECTION("EventDispatcher: temporary object") {
		eventDispatcher.appendListener(1, func);
		eventDispatcher.dispatch(1, CopyMoveCounter(), 0);
	}
	SECTION("eventDispatcher: object variable") {
		eventDispatcher.appendListener(1, func);
		CopyMoveCounter obj1;
		REQUIRE(obj1.getCalledAndReset() == 0);
		eventDispatcher.dispatch(1, obj1, 0);
		REQUIRE(obj1.getCalledAndReset() > 0);
	}
}

TEST_CASE("copymove, EventDispatcher<void(value)>, callback(value)")
{
	using ED = eventpp::EventDispatcher<int, void(CopyMoveCounter, int)>;
	ED eventDispatcher;

	auto func = [](CopyMoveCounter obj, const int expected) {
		REQUIRE(obj.getCounter().copied == expected);
		obj.called();
	};

	SECTION("Raw function: temporary object") {
		func(CopyMoveCounter(), 0);
	}
	SECTION("Raw function: object variable") {
		CopyMoveCounter obj1;
		REQUIRE(obj1.getCalledAndReset() == 0);
		func(obj1, 1);
		REQUIRE(obj1.getCalledAndReset() > 0);
	}

	SECTION("EventDispatcher: temporary object") {
		eventDispatcher.appendListener(1, func);
		eventDispatcher.dispatch(1, CopyMoveCounter(), 1);
	}
	SECTION("eventDispatcher: object variable") {
		eventDispatcher.appendListener(1, func);
		CopyMoveCounter obj1;
		REQUIRE(obj1.getCalledAndReset() == 0);
		eventDispatcher.dispatch(1, obj1, 2);
		REQUIRE(obj1.getCalledAndReset() > 0);
	}
}

TEST_CASE("copymove, EventDispatcher<void(value)>, callback(const &)")
{
	using ED = eventpp::EventDispatcher<int, void(CopyMoveCounter, int)>;
	ED eventDispatcher;

	auto func = [](CopyMoveCounter obj, const int expected) {
		REQUIRE(obj.getCounter().copied == expected);
		obj.called();
	};

	SECTION("Raw function: temporary object") {
		func(CopyMoveCounter(), 0);
	}
	SECTION("Raw function: object variable") {
		CopyMoveCounter obj1;
		REQUIRE(obj1.getCalledAndReset() == 0);
		func(obj1, 1);
		REQUIRE(obj1.getCalledAndReset() > 0);
	}

	SECTION("EventDispatcher: temporary object") {
		eventDispatcher.appendListener(1, func);
		eventDispatcher.dispatch(1, CopyMoveCounter(), 1);
	}
	SECTION("eventDispatcher: object variable") {
		eventDispatcher.appendListener(1, func);
		CopyMoveCounter obj1;
		REQUIRE(obj1.getCalledAndReset() == 0);
		eventDispatcher.dispatch(1, obj1, 2);
		REQUIRE(obj1.getCalledAndReset() > 0);
	}
}

TEST_CASE("copymove, EventDispatcher<void(const &)>, callback(value)")
{
	using ED = eventpp::EventDispatcher<int, void(const CopyMoveCounter &, int)>;
	ED eventDispatcher;

	auto func = [](CopyMoveCounter obj, const int expected) {
		REQUIRE(obj.getCounter().copied == expected);
		obj.called();
	};

	SECTION("Raw function: temporary object") {
		func(CopyMoveCounter(), 0);
	}
	SECTION("Raw function: object variable") {
		CopyMoveCounter obj1;
		REQUIRE(obj1.getCalledAndReset() == 0);
		func(obj1, 1);
		REQUIRE(obj1.getCalledAndReset() > 0);
	}

	SECTION("EventDispatcher: temporary object") {
		eventDispatcher.appendListener(1, func);
		eventDispatcher.dispatch(1, CopyMoveCounter(), 1);
	}
	SECTION("eventDispatcher: object variable") {
		eventDispatcher.appendListener(1, func);
		CopyMoveCounter obj1;
		REQUIRE(obj1.getCalledAndReset() == 0);
		eventDispatcher.dispatch(1, obj1, 1);
		REQUIRE(obj1.getCalledAndReset() > 0);
	}
}

