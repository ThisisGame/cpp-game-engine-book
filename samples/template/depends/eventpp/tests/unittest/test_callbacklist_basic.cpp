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

#include "test_callbacklist_util.h"

TEST_CASE("CallbackList, nested callbacks, new callbacks should not be triggered")
{
	using CL = eventpp::CallbackList<void()>;
	CL callbackList;
	int a = 0, b = 0;

	callbackList.append([&callbackList, &a, &b]() {
		a = 1;

		auto h1 = callbackList.append([&callbackList, &b] {
			++b;
			callbackList.append([&callbackList, &b] {
				++b;
			});
			auto h2 = callbackList.prepend([&callbackList, &b] {
				++b;
				callbackList.append([&callbackList, &b] {
					++b;
				});
			});
			callbackList.append([&callbackList, &b] {
				++b;
			});
			callbackList.insert([&callbackList, &b] {
				++b;
			}, h2);
			callbackList.prepend([&callbackList, &b] {
				++b;
			});
		});
		callbackList.prepend([&callbackList, &b] {
			++b;
		});
		callbackList.insert([&callbackList, &b] {
			++b;
		}, h1);
	});

	REQUIRE(a == 0);
	REQUIRE(b == 0);

	callbackList();

	REQUIRE(a == 1);
	REQUIRE(b == 0);

	callbackList();

	REQUIRE(a == 1);
	REQUIRE(b == 3); // there are 3 new top level callback

	b = 0;
	callbackList();

	REQUIRE(a == 1);
	REQUIRE(b > 3);
}

TEST_CASE("CallbackList, remove inside callback")
{
	RemovalTester(7, 3, { 0 }).test();
	RemovalTester(7, 3, { 1 }).test();
	RemovalTester(7, 3, { 2 }).test();
	RemovalTester(7, 3, { 3 }).test();
	RemovalTester(7, 3, { 4 }).test();
	RemovalTester(7, 3, { 5 }).test();
	RemovalTester(7, 3, { 6 }).test();

	RemovalTester(7, 3, { 0, 3 }).test();
	RemovalTester(7, 3, { 3, 0 }).test();
	RemovalTester(7, 3, { 1, 3 }).test();
	RemovalTester(7, 3, { 3, 1 }).test();
	RemovalTester(7, 3, { 2, 3 }).test();
	RemovalTester(7, 3, { 3, 2 }).test();
	RemovalTester(7, 3, { 3, 4 }).test();
	RemovalTester(7, 3, { 4, 3 }).test();
	RemovalTester(7, 3, { 3, 5 }).test();
	RemovalTester(7, 3, { 5, 3 }).test();
	RemovalTester(7, 3, { 3, 6 }).test();
	RemovalTester(7, 3, { 6, 3 }).test();

	RemovalTester(7, 3, { 2, 4 }).test();
	RemovalTester(7, 3, { 4, 2 }).test();
	RemovalTester(7, 3, { 0, 6 }).test();
	RemovalTester(7, 3, { 0, 0 }).test();

	RemovalTester(7, 3, { 4, 5 }).test();
	RemovalTester(7, 3, { 5, 4 }).test();

	RemovalTester(7, 3, { 3, 4, 5 }).test();
	RemovalTester(7, 3, { 3, 5, 4 }).test();

	RemovalTester(7, 3, { 0, 1, 2, 3, 4, 5, 6 }).test();
	RemovalTester(7, 3, { 6, 5, 4, 3, 2, 1, 0 }).test();
	RemovalTester(7, 3, { 0, 2, 1, 3, 5, 4, 6 }).test();
	RemovalTester(7, 3, { 6, 4, 5, 3, 1, 2, 0 }).test();
}

TEST_CASE("CallbackList, no memory leak after callback list is freed")
{
	using CL = eventpp::CallbackList<void(), FakeCallbackListPolicies>;

	std::vector<CL::Handle> nodeList;

	{
		CL callbackList;
		for(int i = 100; i < 200; ++i) {
			callbackList.append(i);
		}

		nodeList = extractCallbackListHandles(callbackList);
	}

	REQUIRE(checkAllWeakPtrAreFreed(nodeList));
}

TEST_CASE("CallbackList, no memory leak after all callbacks are removed")
{
	using CL = eventpp::CallbackList<void(), FakeCallbackListPolicies>;

	std::vector<CL::Handle> nodeList;
	std::vector<CL::Handle> handleList;

	CL callbackList;
	for(int i = 100; i < 200; ++i) {
		handleList.push_back(callbackList.append(i));
	}

	nodeList = extractCallbackListHandles(callbackList);

	for(auto & handle : handleList) {
		callbackList.remove(handle);
	}

	REQUIRE(checkAllWeakPtrAreFreed(nodeList));
}

TEST_CASE("CallbackList, no memory leak in move assignement")
{
	using CL = eventpp::CallbackList<void()>;
	CL callbackList;
	const auto h1 = callbackList.append([]() {});
	const auto h2 = callbackList.append([]() {});
	for(int i = 0; i < 100; ++i) {
		callbackList.append([]() {});
	}

	const std::vector<CL::Handle> nodeList = extractCallbackListHandles(callbackList);

	callbackList = {};

	// Make sure nodes were destroyed
	REQUIRE(h1.expired());
	REQUIRE(h2.expired());
	REQUIRE(checkAllWeakPtrAreFreed(nodeList));
}

TEST_CASE("CallbackList, forEach")
{
	using CL = eventpp::CallbackList<int()>;
	CL callbackList;

	callbackList.append([]() { return 1; });
	callbackList.append([]() { return 2; });
	callbackList.append([]() { return 3; });

	int i = 1;
	callbackList.forEach([&i](auto callback) {
		REQUIRE(callback() == i);
		++i;
	});

	i = 1;
	callbackList.forEach([&i, &callbackList](const CL::Handle & /*handle*/, auto callback) {
		REQUIRE(callback() == i);
		++i;
	});
}

TEST_CASE("CallbackList, forEachIf")
{
	using CL = eventpp::CallbackList<void ()>;
	CL callbackList;

	std::vector<int> dataList(3);

	callbackList.append([&dataList]() {
		dataList[0] += 1;
	});
	callbackList.append([&dataList]() {
		dataList[1] += 2;
	});
	callbackList.append([&dataList]() {
		dataList[2] += 3;
	});

	REQUIRE(dataList == std::vector<int>{ 0, 0, 0 });

	int i = 0;
	bool result = callbackList.forEachIf([&i](const std::function<void ()> & callback) -> bool {
		callback();
		++i;

		return i != 2;
	});

	REQUIRE(! result);
	REQUIRE(dataList == std::vector<int>{ 1, 2, 0 });
}

TEST_CASE("CallbackList, forEach and forEachIf")
{
	using CL = eventpp::CallbackList<void()>;
	CL callbackList;

	const int itemCount = 5;
	std::vector<int> dataList(itemCount);
	for(int i = 0; i < itemCount; ++i) {
		callbackList.append([&dataList, i]() {
			dataList[i] = i + 1;
		});
	}

	SECTION("forEach") {
		callbackList.forEach([](const CL::Callback & callback) -> void {
			callback();
		});

		REQUIRE(dataList == std::vector<int>{ 1, 2, 3, 4, 5 });
	}
	
	SECTION("forEachIf") {
		const bool result = callbackList.forEachIf([&dataList](const CL::Callback & callback) -> bool {
			constexpr int index = 2;
			const bool isZero = (dataList[index] == 0);
			callback();
			if(isZero && dataList[index] != 0) {
				return false;
			}
			return true;
		});

		REQUIRE(! result);
		REQUIRE(dataList == std::vector<int>{ 1, 2, 3, 0, 0 });
	}
}

TEST_CASE("CallbackList, non-lvaue-reference arguments should not be modified by callbacks")
{
	using CL = eventpp::CallbackList<void(std::string)>;
	CL callbackList;

	constexpr int itemCount = 2;
	std::vector<std::string> dataList(itemCount);

	// "std::string & s" can compile in MSVC 2017 but not GCC 7.2
	callbackList.append([&dataList](std::string && s) {
		dataList[0] = s;
		s = "modified";
	});
	callbackList.append([&dataList](const std::string & s) {
		dataList[1] = s;
	});

	SECTION("lvalue") {
		std::string s = "hello";
		callbackList(s);
		REQUIRE(dataList[0] == "hello");
		REQUIRE(dataList[1] == "hello");
		REQUIRE(s == "hello");
	}
	SECTION("rvalue") {
		callbackList("hello");
		REQUIRE(dataList[0] == "hello");
		REQUIRE(dataList[1] == "hello");
	}
}

TEST_CASE("CallbackList, lvaue-reference arguments cant be modified by callbacks")
{
	using CL = eventpp::CallbackList<void(std::string &)>;
	CL callbackList;

	constexpr int itemCount = 2;
	std::vector<std::string> dataList(itemCount);

	callbackList.append([&dataList](std::string & s) {
		s = "modified";
		dataList[0] = s;
	});
	callbackList.append([&dataList](std::string & s) {
		s += "2";
		dataList[1] = s;
	});

	SECTION("lvalue") {
		std::string s = "hello";
		callbackList(s);
		REQUIRE(dataList[0] == "modified");
		REQUIRE(dataList[1] == "modified2");
		REQUIRE(s == "modified2");
	}
}

TEST_CASE("CallbackList, append/remove/insert")
{
	using CL = eventpp::CallbackList<void(), FakeCallbackListPolicies>;

	CL callbackList;

	REQUIRE(! callbackList.head);
	REQUIRE(! callbackList.tail);

	CL::Handle h100, h101, h102, h103, h104, h105, h106, h107;

	{
		auto handle = callbackList.append(100);
		h100 = handle;
		verifyLinkedList(callbackList, std::vector<int>{ 100 });
	}

	{
		auto handle = callbackList.append(101);
		h101 = handle;
		verifyLinkedList(callbackList, std::vector<int>{ 100, 101 });
	}

	{
		auto handle = callbackList.append(102);
		h102 = handle;
		verifyLinkedList(callbackList, std::vector<int>{ 100, 101, 102 });
	}

	{
		auto handle = callbackList.append(103);
		h103 = handle;
		verifyLinkedList(callbackList, std::vector<int>{ 100, 101, 102, 103 });
	}

	{
		auto handle = callbackList.append(104);
		h104 = handle;
		verifyLinkedList(callbackList, std::vector<int>{ 100, 101, 102, 103, 104 });
	}

	{
		auto handle = callbackList.insert(105, h103); // before 103
		h105 = handle;
		verifyLinkedList(callbackList, std::vector<int>{ 100, 101, 102, 105, 103, 104 });
		
		h107 = callbackList.insert(107, h100); // before 100
		verifyLinkedList(callbackList, std::vector<int>{ 107, 100, 101, 102, 105, 103, 104 });

		h106 = callbackList.insert(106, handle); // before 105
		verifyLinkedList(callbackList, std::vector<int>{ 107, 100, 101, 102, 106, 105, 103, 104 });
	}

	callbackList.remove(h100);
	verifyLinkedList(callbackList, std::vector<int>{ 107, 101, 102, 106, 105, 103, 104 });

	callbackList.remove(h103);
	callbackList.remove(h102);
	verifyLinkedList(callbackList, std::vector<int>{ 107, 101, 106, 105, 104 });

	callbackList.remove(h105);
	callbackList.remove(h104);
	callbackList.remove(h106);
	callbackList.remove(h101);
	callbackList.remove(h107);
	verifyLinkedList(callbackList, std::vector<int>{});
}

TEST_CASE("CallbackList, insert")
{
	using CL = eventpp::CallbackList<void(), FakeCallbackListPolicies>;

	CL callbackList;
	
	auto h100 = callbackList.append(100);
	auto h101 = callbackList.append(101);
	auto h102 = callbackList.append(102);
	auto h103 = callbackList.append(103);
	auto h104 = callbackList.append(104);

	SECTION("before front") {
		callbackList.insert(105, h100);
		verifyLinkedList(callbackList, std::vector<int>{ 105, 100, 101, 102, 103, 104 });
	}

	SECTION("before second") {
		callbackList.insert(105, h101);
		verifyLinkedList(callbackList, std::vector<int>{ 100, 105, 101, 102, 103, 104 });
	}

	SECTION("before nonexist") {
		callbackList.insert(105, CL::Handle());
		verifyLinkedList(callbackList, std::vector<int>{ 100, 101, 102, 103, 104, 105 });
	}
}

TEST_CASE("CallbackList, remove")
{
	using CL = eventpp::CallbackList<void(), FakeCallbackListPolicies>;

	CL callbackList;

	auto h100 = callbackList.append(100);
	auto h101 = callbackList.append(101);
	auto h102 = callbackList.append(102);
	auto h103 = callbackList.append(103);
	auto h104 = callbackList.append(104);

	SECTION("remove front") {
		callbackList.remove(h100);
		verifyLinkedList(callbackList, std::vector<int>{ 101, 102, 103, 104 });

		callbackList.remove(h100);
		verifyLinkedList(callbackList, std::vector<int>{ 101, 102, 103, 104 });
	}

	SECTION("remove second") {
		callbackList.remove(h101);
		verifyLinkedList(callbackList, std::vector<int>{ 100, 102, 103, 104 });

		callbackList.remove(h101);
		verifyLinkedList(callbackList, std::vector<int>{ 100, 102, 103, 104 });
	}

	SECTION("remove end") {
		callbackList.remove(h104);
		verifyLinkedList(callbackList, std::vector<int>{ 100, 101, 102, 103 });

		callbackList.remove(h104);
		verifyLinkedList(callbackList, std::vector<int>{ 100, 101, 102, 103 });
	}

	SECTION("remove nonexist") {
		callbackList.remove(CL::Handle());
		verifyLinkedList(callbackList, std::vector<int>{ 100, 101, 102, 103, 104 });

		callbackList.remove(CL::Handle());
		verifyLinkedList(callbackList, std::vector<int>{ 100, 101, 102, 103, 104 });
	}

	SECTION("remove all") {
		callbackList.remove(h102);
		callbackList.remove(h104);
		callbackList.remove(h103);
		callbackList.remove(h101);
		callbackList.remove(h100);
		verifyLinkedList(callbackList, std::vector<int>{ });
	}
}

TEST_CASE("CallbackList, prototype convert")
{
	struct MyClass
	{
		MyClass(int) {}
	};

	using CL = eventpp::CallbackList<void (int)>;
	CL callbackList;

	std::vector<int> dataList(2);

	callbackList.append([&dataList](int) {
		++dataList[0];
	});
	callbackList.append([&dataList](const MyClass &) {
		++dataList[1];
	});

	REQUIRE(dataList == std::vector<int>{ 0, 0 });

	callbackList((int)5);
	REQUIRE(dataList == std::vector<int>{ 1, 1 });

	callbackList((char)5);
	REQUIRE(dataList == std::vector<int>{ 2, 2 });
}

TEST_CASE("CallbackList, internal counter overflow")
{
	eventpp::CallbackList<void()> callbackList;

	SECTION("no overflow") {
		std::vector<int> dataList(2);
		callbackList.append([&dataList, &callbackList]() {
			++dataList[0];
			callbackList.append([&dataList]() {
				++dataList[1];
			});
		});
		callbackList();
		REQUIRE(dataList == std::vector<int>{ 1, 0 });
	}

	SECTION("overflow") {
		std::vector<int> dataList(2);
		callbackList.append([&dataList, &callbackList]() {
			++dataList[0];
			callbackList.currentCounter = (unsigned int )-1;
			callbackList.append([&dataList]() {
				++dataList[1];
			});
		});
		callbackList();
		REQUIRE(dataList == std::vector<int>{ 1, 1 });
	}
}

TEST_CASE("CallbackList, no crash when doFreeNode(head)")
{
	eventpp::CallbackList<void()> callbackList;
	callbackList.append([](){});
	try {
		// This loop should not cause memory access crash
		while(callbackList.head) {
			callbackList.doFreeNode(callbackList.head);
		}
	}
	catch(...) {
		// The catch may not work on some platforms, but that doesn't matter,
		// if there is something wrong, we already notice the crash rather than asset failure.
		REQUIRE(false);
	}
}
