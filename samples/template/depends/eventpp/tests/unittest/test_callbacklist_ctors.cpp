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

TEST_CASE("CallbackList, copy constructor from empty CallbackList")
{
	using CL = eventpp::CallbackList<void(), FakeCallbackListPolicies>;
	CL callbackList;
	REQUIRE(! callbackList);
	REQUIRE(! callbackList.head);
	REQUIRE(! callbackList.tail);

	CL copiedList(callbackList);
	REQUIRE(! copiedList);
	REQUIRE(! copiedList.head);
	REQUIRE(! copiedList.tail);
}

TEST_CASE("CallbackList, copy constructor from non-empty CallbackList")
{
	using CL = eventpp::CallbackList<void(), FakeCallbackListPolicies>;
	CL callbackList;
	callbackList.append(100);
	callbackList.append(101);
	callbackList.append(102);
	verifyLinkedList(callbackList, std::vector<int>{ 100, 101, 102 });
	
	CL copiedList(callbackList);
	verifyLinkedList(copiedList, std::vector<int>{ 100, 101, 102 });
	REQUIRE(copiedList.head->callback == 100);
	REQUIRE(copiedList.tail->callback == 102);
	copiedList.prepend(99);
	copiedList.append(103);
	verifyLinkedList(copiedList, std::vector<int>{ 99, 100, 101, 102, 103 });
	// Verify it's deep copy so callbackList is not effected.
	verifyLinkedList(callbackList, std::vector<int>{ 100, 101, 102 });
}

TEST_CASE("CallbackList, invoke, copy constructor from non-empty CallbackList")
{
	using CL = eventpp::CallbackList<void()>;
	std::vector<int> dataList(3);
	REQUIRE(dataList == std::vector<int> { 0, 0, 0 });

	CL callbackList;
	callbackList.append([&dataList]() { ++dataList[0]; });
	callbackList.append([&dataList]() { ++dataList[1]; });
	callbackList();
	REQUIRE(dataList == std::vector<int> { 1, 1, 0 });
	
	CL copiedList(callbackList);
	copiedList();
	REQUIRE(dataList == std::vector<int> { 2, 2, 0 });
	copiedList.append([&dataList]() { ++dataList[2]; });
	copiedList();
	REQUIRE(dataList == std::vector<int> { 3, 3, 1 });
	callbackList();
	REQUIRE(dataList == std::vector<int> { 4, 4, 1 });
}

TEST_CASE("CallbackList, assign to self")
{
	using CL = eventpp::CallbackList<void(), FakeCallbackListPolicies>;
	CL assignedList;

	REQUIRE(! assignedList);
	REQUIRE(! assignedList.head);
	REQUIRE(! assignedList.tail);
	
	assignedList = assignedList;
	REQUIRE(! assignedList);
	REQUIRE(! assignedList.head);
	REQUIRE(! assignedList.tail);

	assignedList.append(100);
	assignedList.append(101);
	assignedList.append(102);
	verifyLinkedList(assignedList, std::vector<int>{ 100, 101, 102 });
	
	const auto oldHead = assignedList.head;
	const auto oldTail = assignedList.tail;
	
	assignedList = assignedList;
	verifyLinkedList(assignedList, std::vector<int>{ 100, 101, 102 });
	REQUIRE(oldHead == assignedList.head);
	REQUIRE(oldTail == assignedList.tail);
}

TEST_CASE("CallbackList, assign from empty CallbackList")
{
	using CL = eventpp::CallbackList<void(), FakeCallbackListPolicies>;
	CL callbackList;
	REQUIRE(! callbackList);
	REQUIRE(! callbackList.head);
	REQUIRE(! callbackList.tail);

	CL assignedList;
	assignedList.append(3);
	assignedList.append(8);
	REQUIRE(assignedList);
	REQUIRE(assignedList.head->callback == 3);
	REQUIRE(assignedList.tail->callback == 8);

	assignedList = callbackList;
	REQUIRE(! assignedList);
	REQUIRE(! assignedList.head);
	REQUIRE(! assignedList.tail);
}

TEST_CASE("CallbackList, assign from non-empty CallbackList")
{
	using CL = eventpp::CallbackList<void(), FakeCallbackListPolicies>;
	CL callbackList;
	callbackList.append(100);
	callbackList.append(101);
	callbackList.append(102);
	verifyLinkedList(callbackList, std::vector<int>{ 100, 101, 102 });

	CL assignedList;
	assignedList.append(3);
	assignedList.append(8);
	REQUIRE(assignedList);
	REQUIRE(assignedList.head->callback == 3);
	REQUIRE(assignedList.tail->callback == 8);

	assignedList = callbackList;
	verifyLinkedList(assignedList, std::vector<int>{ 100, 101, 102 });
	REQUIRE(assignedList.head->callback == 100);
	REQUIRE(assignedList.tail->callback == 102);
	assignedList.prepend(99);
	assignedList.append(103);
	verifyLinkedList(assignedList, std::vector<int>{ 99, 100, 101, 102, 103 });
	// Verify it's deep copy so callbackList is not effected.
	verifyLinkedList(callbackList, std::vector<int>{ 100, 101, 102 });
}

TEST_CASE("CallbackList, invoke, assign from non-empty CallbackList")
{
	using CL = eventpp::CallbackList<void()>;
	std::vector<int> dataList(3);
	REQUIRE(dataList == std::vector<int> { 0, 0, 0 });

	CL callbackList;
	callbackList.append([&dataList]() { ++dataList[0]; });
	callbackList.append([&dataList]() { ++dataList[1]; });
	callbackList();
	REQUIRE(dataList == std::vector<int> { 1, 1, 0 });
	
	CL assignedList;
	assignedList = callbackList;
	assignedList();
	REQUIRE(dataList == std::vector<int> { 2, 2, 0 });
	assignedList.append([&dataList]() { ++dataList[2]; });
	assignedList();
	REQUIRE(dataList == std::vector<int> { 3, 3, 1 });
	callbackList();
	REQUIRE(dataList == std::vector<int> { 4, 4, 1 });
}

TEST_CASE("CallbackList, move constructor from empty CallbackList")
{
	using CL = eventpp::CallbackList<void(), FakeCallbackListPolicies>;
	CL callbackList;
	REQUIRE(! callbackList);
	REQUIRE(! callbackList.head);
	REQUIRE(! callbackList.tail);

	CL movedList(std::move(callbackList));
	REQUIRE(! movedList);
	REQUIRE(! movedList.head);
	REQUIRE(! movedList.tail);
}

TEST_CASE("CallbackList, move constructor from non-empty CallbackList")
{
	using CL = eventpp::CallbackList<void(), FakeCallbackListPolicies>;
	CL callbackList;
	callbackList.append(100);
	callbackList.append(101);
	callbackList.append(102);
	verifyLinkedList(callbackList, std::vector<int>{ 100, 101, 102 });
	
	CL movedList(std::move(callbackList));
	verifyLinkedList(movedList, std::vector<int>{ 100, 101, 102 });
	REQUIRE(movedList.head->callback == 100);
	REQUIRE(movedList.tail->callback == 102);
	movedList.prepend(99);
	movedList.append(103);
	verifyLinkedList(movedList, std::vector<int>{ 99, 100, 101, 102, 103 });

	REQUIRE(! callbackList);
	REQUIRE(! callbackList.head);
	REQUIRE(! callbackList.tail);
}

TEST_CASE("CallbackList, invoke, move constructor from non-empty CallbackList")
{
	using CL = eventpp::CallbackList<void()>;
	std::vector<int> dataList(3);
	REQUIRE(dataList == std::vector<int> { 0, 0, 0 });

	CL callbackList;
	callbackList.append([&dataList]() { ++dataList[0]; });
	callbackList.append([&dataList]() { ++dataList[1]; });
	callbackList();
	REQUIRE(dataList == std::vector<int> { 1, 1, 0 });
	
	CL movedList(std::move(callbackList));
	movedList();
	REQUIRE(dataList == std::vector<int> { 2, 2, 0 });
	movedList.append([&dataList]() { ++dataList[2]; });
	movedList();
	REQUIRE(dataList == std::vector<int> { 3, 3, 1 });
	callbackList();
	REQUIRE(dataList == std::vector<int> { 3, 3, 1 });
}

TEST_CASE("CallbackList, move assign to self")
{
	using CL = eventpp::CallbackList<void(), FakeCallbackListPolicies>;
	CL assignedList;

	REQUIRE(! assignedList);
	REQUIRE(! assignedList.head);
	REQUIRE(! assignedList.tail);
	
	assignedList = std::move(assignedList);
	REQUIRE(! assignedList);
	REQUIRE(! assignedList.head);
	REQUIRE(! assignedList.tail);

	assignedList.append(100);
	assignedList.append(101);
	assignedList.append(102);
	verifyLinkedList(assignedList, std::vector<int>{ 100, 101, 102 });
	
	const auto oldHead = assignedList.head;
	const auto oldTail = assignedList.tail;
	
	assignedList = std::move(assignedList);
	verifyLinkedList(assignedList, std::vector<int>{ 100, 101, 102 });
	REQUIRE(oldHead == assignedList.head);
	REQUIRE(oldTail == assignedList.tail);
}

TEST_CASE("CallbackList, move assign from empty CallbackList")
{
	using CL = eventpp::CallbackList<void(), FakeCallbackListPolicies>;
	CL callbackList;
	REQUIRE(! callbackList);
	REQUIRE(! callbackList.head);
	REQUIRE(! callbackList.tail);

	CL assignedList;
	assignedList.append(3);
	assignedList.append(8);
	REQUIRE(assignedList);
	REQUIRE(assignedList.head->callback == 3);
	REQUIRE(assignedList.tail->callback == 8);

	assignedList = std::move(callbackList);
	REQUIRE(! assignedList);
	REQUIRE(! assignedList.head);
	REQUIRE(! assignedList.tail);

	REQUIRE(! callbackList);
	REQUIRE(! callbackList.head);
	REQUIRE(! callbackList.tail);
}

TEST_CASE("CallbackList, move assign from non-empty CallbackList")
{
	using CL = eventpp::CallbackList<void(), FakeCallbackListPolicies>;
	CL callbackList;
	callbackList.append(100);
	callbackList.append(101);
	callbackList.append(102);
	verifyLinkedList(callbackList, std::vector<int>{ 100, 101, 102 });

	CL assignedList;
	assignedList.append(3);
	assignedList.append(8);
	REQUIRE(assignedList);
	REQUIRE(assignedList.head->callback == 3);
	REQUIRE(assignedList.tail->callback == 8);

	assignedList = std::move(callbackList);
	verifyLinkedList(assignedList, std::vector<int>{ 100, 101, 102 });
	REQUIRE(assignedList.head->callback == 100);
	REQUIRE(assignedList.tail->callback == 102);
	assignedList.prepend(99);
	assignedList.append(103);
	verifyLinkedList(assignedList, std::vector<int>{ 99, 100, 101, 102, 103 });

	REQUIRE(! callbackList);
	REQUIRE(! callbackList.head);
	REQUIRE(! callbackList.tail);
}

TEST_CASE("CallbackList, invoke, move assign from non-empty CallbackList")
{
	using CL = eventpp::CallbackList<void()>;
	std::vector<int> dataList(3);
	REQUIRE(dataList == std::vector<int> { 0, 0, 0 });

	CL callbackList;
	callbackList.append([&dataList]() { ++dataList[0]; });
	callbackList.append([&dataList]() { ++dataList[1]; });
	callbackList();
	REQUIRE(dataList == std::vector<int> { 1, 1, 0 });
	
	CL assignedList;
	assignedList = std::move(callbackList);
	assignedList();
	REQUIRE(dataList == std::vector<int> { 2, 2, 0 });
	assignedList.append([&dataList]() { ++dataList[2]; });
	assignedList();
	REQUIRE(dataList == std::vector<int> { 3, 3, 1 });
	callbackList();
	REQUIRE(dataList == std::vector<int> { 3, 3, 1 });
}

TEST_CASE("CallbackList, swap with self")
{
	using CL = eventpp::CallbackList<void(), FakeCallbackListPolicies>;
	CL swappedList;

	REQUIRE(! swappedList);
	REQUIRE(! swappedList.head);
	REQUIRE(! swappedList.tail);
	
	using std::swap;
	swap(swappedList, swappedList);
	REQUIRE(! swappedList);
	REQUIRE(! swappedList.head);
	REQUIRE(! swappedList.tail);

	swappedList.append(100);
	swappedList.append(101);
	swappedList.append(102);
	verifyLinkedList(swappedList, std::vector<int>{ 100, 101, 102 });
	
	const auto oldHead = swappedList.head;
	const auto oldTail = swappedList.tail;
	
	swap(swappedList, swappedList);
	verifyLinkedList(swappedList, std::vector<int>{ 100, 101, 102 });
	REQUIRE(oldHead == swappedList.head);
	REQUIRE(oldTail == swappedList.tail);
}

TEST_CASE("CallbackList, swap with empty CallbackList")
{
	using CL = eventpp::CallbackList<void(), FakeCallbackListPolicies>;
	CL callbackList;
	REQUIRE(! callbackList);
	REQUIRE(! callbackList.head);
	REQUIRE(! callbackList.tail);

	CL swappedList;
	swappedList.append(3);
	swappedList.append(8);
	REQUIRE(swappedList);
	REQUIRE(swappedList.head->callback == 3);
	REQUIRE(swappedList.tail->callback == 8);

	swappedList.swap(callbackList);
	REQUIRE(! swappedList);
	REQUIRE(! swappedList.head);
	REQUIRE(! swappedList.tail);
	REQUIRE(callbackList);
	REQUIRE(callbackList.head->callback == 3);
	REQUIRE(callbackList.tail->callback == 8);

	using std::swap;
	swap(swappedList, callbackList);
	REQUIRE(! callbackList);
	REQUIRE(! callbackList.head);
	REQUIRE(! callbackList.tail);
	REQUIRE(swappedList);
	REQUIRE(swappedList.head->callback == 3);
	REQUIRE(swappedList.tail->callback == 8);
}

TEST_CASE("CallbackList, swap with non-empty CallbackList")
{
	using CL = eventpp::CallbackList<void(), FakeCallbackListPolicies>;
	CL callbackList;
	callbackList.append(100);
	callbackList.append(101);
	callbackList.append(102);
	verifyLinkedList(callbackList, std::vector<int>{ 100, 101, 102 });

	CL swappedList;
	swappedList.append(3);
	swappedList.append(8);
	REQUIRE(swappedList);
	REQUIRE(swappedList.head->callback == 3);
	REQUIRE(swappedList.tail->callback == 8);

	using std::swap;
	swap(swappedList, callbackList);
	verifyLinkedList(swappedList, std::vector<int>{ 100, 101, 102 });
	REQUIRE(swappedList.head->callback == 100);
	REQUIRE(swappedList.tail->callback == 102);
	swappedList.prepend(99);
	swappedList.append(103);
	verifyLinkedList(swappedList, std::vector<int>{ 99, 100, 101, 102, 103 });
	REQUIRE(callbackList);
	REQUIRE(callbackList.head->callback == 3);
	REQUIRE(callbackList.tail->callback == 8);
}

TEST_CASE("CallbackList, invoke, swap with non-empty CallbackList")
{
	using CL = eventpp::CallbackList<void()>;
	std::vector<int> dataList(3);
	REQUIRE(dataList == std::vector<int> { 0, 0, 0 });

	CL callbackList;
	callbackList.append([&dataList]() { ++dataList[0]; });
	callbackList.append([&dataList]() { ++dataList[1]; });
	callbackList();
	REQUIRE(dataList == std::vector<int> { 1, 1, 0 });
	
	CL swappedList;
	swappedList.append([&dataList]() { ++dataList[2]; });
	swappedList();
	REQUIRE(dataList == std::vector<int> { 1, 1, 1 });
	using std::swap;
	swap(swappedList, callbackList);
	swappedList();
	REQUIRE(dataList == std::vector<int> { 2, 2, 1 });
	callbackList();
	REQUIRE(dataList == std::vector<int> { 2, 2, 2 });
}

