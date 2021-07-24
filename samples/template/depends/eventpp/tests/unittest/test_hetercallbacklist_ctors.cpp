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
#include "eventpp/hetercallbacklist.h"

#include <vector>

TEST_CASE("HeterCallbackList, copy constructor from empty HeterCallbackList")
{
	using CL = eventpp::HeterCallbackList<eventpp::HeterTuple<void (), void (int)> >;
	CL callbackList;
	REQUIRE(callbackList.empty());

	CL copiedList(callbackList);
	REQUIRE(copiedList.empty());

	callbackList.append([](int){});
	REQUIRE(! callbackList.empty());
	REQUIRE(copiedList.empty());
}

TEST_CASE("HeterCallbackList, copy constructor from non-empty HeterCallbackList")
{
	using CL = eventpp::HeterCallbackList<eventpp::HeterTuple<void (), void (int)> >;
	
	std::vector<int> dataList(3);
	
	CL callbackList;
	callbackList.append([&dataList](){ ++dataList[0]; });
	callbackList.append([&dataList](int){ ++dataList[1]; });
	REQUIRE(! callbackList.empty());

	SECTION("without new callback") {
		CL copiedList(callbackList);
		REQUIRE(! copiedList.empty());
		
		copiedList();
		copiedList(5);
		REQUIRE(dataList == std::vector<int>{ 1, 1, 0 });
		
		callbackList();
		callbackList(5);
		REQUIRE(dataList == std::vector<int>{ 2, 2, 0 });
	}

	SECTION("with new callback") {
		CL copiedList(callbackList);
		REQUIRE(! copiedList.empty());

		callbackList.append([&dataList](int){ ++dataList[2]; });
		
		copiedList();
		copiedList(5);
		REQUIRE(dataList == std::vector<int>{ 1, 1, 0 });
		
		callbackList();
		callbackList(5);
		REQUIRE(dataList == std::vector<int>{ 2, 2, 1 });
	}
}

TEST_CASE("HeterCallbackList, assign to self")
{
	using CL = eventpp::HeterCallbackList<eventpp::HeterTuple<void (), void (int)> >;
	
	std::vector<int> dataList(3);
	
	CL assignedList;
	assignedList.append([&dataList](){ ++dataList[0]; });
	assignedList.append([&dataList](int){ ++dataList[1]; });
	REQUIRE(! assignedList.empty());

	SECTION("without new callback") {
		assignedList = assignedList;
		REQUIRE(! assignedList.empty());
		
		assignedList();
		assignedList(5);
		REQUIRE(dataList == std::vector<int>{ 1, 1, 0 });
	}

	SECTION("with new callback") {
		assignedList = assignedList;
		REQUIRE(! assignedList.empty());
		assignedList.append([&dataList](int){ ++dataList[2]; });
		
		assignedList();
		assignedList(5);
		REQUIRE(dataList == std::vector<int>{ 1, 1, 1 });
	}
}

TEST_CASE("HeterCallbackList, assign from empty HeterCallbackList")
{
	using CL = eventpp::HeterCallbackList<eventpp::HeterTuple<void (), void (int)> >;
	CL callbackList;
	REQUIRE(callbackList.empty());

	std::vector<int> dataList(2);

	CL assignedList;
	assignedList.append([&dataList](){ ++dataList[0]; });
	assignedList.append([&dataList](int){ ++dataList[1]; });
	REQUIRE(! assignedList.empty());

	SECTION("without assignment") {
		assignedList();
		assignedList(5);
		REQUIRE(dataList == std::vector<int>{ 1, 1 });
	}

	SECTION("with assignment") {
		assignedList = callbackList;
		REQUIRE(assignedList.empty());
		assignedList();
		assignedList(5);
		REQUIRE(dataList == std::vector<int>{ 0, 0 });
	}
}

TEST_CASE("HeterCallbackList, assign from non-empty HeterCallbackList")
{
	using CL = eventpp::HeterCallbackList<eventpp::HeterTuple<void (), void (int)> >;

	std::vector<int> dataList(3);

	CL callbackList;
	callbackList.append([&dataList](){ ++dataList[0]; });
	callbackList.append([&dataList](int){ ++dataList[1]; });
	REQUIRE(! callbackList.empty());

	CL assignedList;
	assignedList.append([&dataList](){ ++dataList[1]; });
	assignedList.append([&dataList](int){ ++dataList[2]; });
	REQUIRE(! assignedList.empty());

	SECTION("without assignment") {
		callbackList();
		callbackList(5);
		REQUIRE(dataList == std::vector<int>{ 1, 1, 0 });
		assignedList();
		assignedList(5);
		REQUIRE(dataList == std::vector<int>{ 1, 2, 1 });
	}

	SECTION("with assignment") {
		assignedList = callbackList;
		REQUIRE(! assignedList.empty());
		assignedList();
		assignedList(5);
		REQUIRE(dataList == std::vector<int>{ 1, 1, 0 });
		callbackList();
		callbackList(5);
		REQUIRE(dataList == std::vector<int>{ 2, 2, 0 });
	}
}

TEST_CASE("HeterCallbackList, move constructor from empty HeterCallbackList")
{
	using CL = eventpp::HeterCallbackList<eventpp::HeterTuple<void (), void (int)> >;
	CL callbackList;
	REQUIRE(callbackList.empty());

	CL copiedList(std::move(callbackList));
	REQUIRE(copiedList.empty());

	callbackList.append([](int){});
	REQUIRE(! callbackList.empty());
	REQUIRE(copiedList.empty());
}

TEST_CASE("HeterCallbackList, move constructor from non-empty HeterCallbackList")
{
	using CL = eventpp::HeterCallbackList<eventpp::HeterTuple<void (), void (int)> >;
	
	std::vector<int> dataList(3);
	
	CL callbackList;
	callbackList.append([&dataList](){ ++dataList[0]; });
	callbackList.append([&dataList](int){ ++dataList[1]; });
	REQUIRE(! callbackList.empty());

	SECTION("without new callback") {
		CL copiedList(std::move(callbackList));
		REQUIRE(! copiedList.empty());
		REQUIRE(callbackList.empty());
		
		copiedList();
		copiedList(5);
		REQUIRE(dataList == std::vector<int>{ 1, 1, 0 });
		
		callbackList();
		callbackList(5);
		REQUIRE(dataList == std::vector<int>{ 1, 1, 0 });
	}

	SECTION("with new callback") {
		CL copiedList(std::move(callbackList));
		REQUIRE(! copiedList.empty());
		REQUIRE(callbackList.empty());

		callbackList.append([&dataList](int){ ++dataList[2]; });
		
		copiedList();
		copiedList(5);
		REQUIRE(dataList == std::vector<int>{ 1, 1, 0 });
		
		callbackList();
		callbackList(5);
		REQUIRE(dataList == std::vector<int>{ 1, 1, 1 });
	}
}

TEST_CASE("HeterCallbackList, move assign to self")
{
	using CL = eventpp::HeterCallbackList<eventpp::HeterTuple<void (), void (int)> >;
	
	std::vector<int> dataList(3);
	
	CL assignedList;
	assignedList.append([&dataList](){ ++dataList[0]; });
	assignedList.append([&dataList](int){ ++dataList[1]; });
	REQUIRE(! assignedList.empty());

	SECTION("without new callback") {
		assignedList = std::move(assignedList);
		REQUIRE(! assignedList.empty());
		
		assignedList();
		assignedList(5);
		REQUIRE(dataList == std::vector<int>{ 1, 1, 0 });
	}

	SECTION("with new callback") {
		assignedList = std::move(assignedList);
		REQUIRE(! assignedList.empty());
		assignedList.append([&dataList](int){ ++dataList[2]; });
		
		assignedList();
		assignedList(5);
		REQUIRE(dataList == std::vector<int>{ 1, 1, 1 });
	}
}

TEST_CASE("HeterCallbackList, move assign from empty HeterCallbackList")
{
	using CL = eventpp::HeterCallbackList<eventpp::HeterTuple<void (), void (int)> >;
	CL callbackList;
	REQUIRE(callbackList.empty());

	std::vector<int> dataList(2);

	CL assignedList;
	assignedList.append([&dataList](){ ++dataList[0]; });
	assignedList.append([&dataList](int){ ++dataList[1]; });
	REQUIRE(! assignedList.empty());

	SECTION("without assignment") {
		assignedList();
		assignedList(5);
		REQUIRE(dataList == std::vector<int>{ 1, 1 });
	}

	SECTION("with assignment") {
		assignedList = std::move(callbackList);
		REQUIRE(assignedList.empty());
		assignedList();
		assignedList(5);
		REQUIRE(dataList == std::vector<int>{ 0, 0 });
	}
}

TEST_CASE("HeterCallbackList, move assign from non-empty HeterCallbackList")
{
	using CL = eventpp::HeterCallbackList<eventpp::HeterTuple<void (), void (int)> >;

	std::vector<int> dataList(3);

	CL callbackList;
	callbackList.append([&dataList](){ ++dataList[0]; });
	callbackList.append([&dataList](int){ ++dataList[1]; });
	REQUIRE(! callbackList.empty());

	CL assignedList;
	assignedList.append([&dataList](){ ++dataList[1]; });
	assignedList.append([&dataList](int){ ++dataList[2]; });
	REQUIRE(! assignedList.empty());

	SECTION("without assignment") {
		callbackList();
		callbackList(5);
		REQUIRE(dataList == std::vector<int>{ 1, 1, 0 });
		assignedList();
		assignedList(5);
		REQUIRE(dataList == std::vector<int>{ 1, 2, 1 });
	}

	SECTION("with assignment") {
		assignedList = std::move(callbackList);
		REQUIRE(! assignedList.empty());
		REQUIRE(callbackList.empty());
		assignedList();
		assignedList(5);
		REQUIRE(dataList == std::vector<int>{ 1, 1, 0 });
		callbackList();
		callbackList(5);
		REQUIRE(dataList == std::vector<int>{ 1, 1, 0 });
	}
}

TEST_CASE("HeterCallbackList, swap with self")
{
	using CL = eventpp::HeterCallbackList<eventpp::HeterTuple<void (), void (int)> >;
	
	std::vector<int> dataList(2);
	
	CL swappedList;
	REQUIRE(swappedList.empty());
	
	using std::swap;

	SECTION("without callback") {
		swap(swappedList, swappedList);
		REQUIRE(swappedList.empty());
	}

	SECTION("with callback") {
		swappedList.append([&dataList](){ ++dataList[0]; });
		swappedList.append([&dataList](int){ ++dataList[1]; });
		REQUIRE(! swappedList.empty());
		
		swappedList();
		swappedList(5);
		REQUIRE(dataList == std::vector<int>{ 1, 1 });

		swap(swappedList, swappedList);
		REQUIRE(! swappedList.empty());
		swappedList();
		swappedList(5);
		REQUIRE(dataList == std::vector<int>{ 2, 2 });
	}
}

TEST_CASE("HeterCallbackList, swap with empty HeterCallbackList")
{
	using CL = eventpp::HeterCallbackList<eventpp::HeterTuple<void (), void (int)> >;
	CL callbackList;
	REQUIRE(callbackList.empty());

	std::vector<int> dataList(2);

	CL swappedList;
	swappedList.append([&dataList](){ ++dataList[0]; });
	swappedList.append([&dataList](int){ ++dataList[1]; });
	REQUIRE(! swappedList.empty());

	using std::swap;

	SECTION("without swap") {
		swappedList();
		swappedList(5);
		REQUIRE(dataList == std::vector<int>{ 1, 1 });
	}

	SECTION("with swap") {
		swap(swappedList, callbackList);
		REQUIRE(swappedList.empty());
		swappedList();
		swappedList(5);
		REQUIRE(dataList == std::vector<int>{ 0, 0 });
		callbackList();
		callbackList(5);
		REQUIRE(dataList == std::vector<int>{ 1, 1 });
	}
}

TEST_CASE("HeterCallbackList, swap with non-empty HeterCallbackList")
{
	using CL = eventpp::HeterCallbackList<eventpp::HeterTuple<void (), void (int)> >;

	std::vector<int> dataList(3);

	CL callbackList;
	callbackList.append([&dataList](){ ++dataList[0]; });
	callbackList.append([&dataList](int){ ++dataList[1]; });
	REQUIRE(! callbackList.empty());

	CL swappedList;
	swappedList.append([&dataList](){ ++dataList[1]; });
	swappedList.append([&dataList](int){ ++dataList[2]; });
	REQUIRE(! swappedList.empty());

	using std::swap;

	SECTION("without swap") {
		swappedList();
		swappedList(5);
		REQUIRE(dataList == std::vector<int>{ 0, 1, 1 });
		callbackList();
		callbackList(5);
		REQUIRE(dataList == std::vector<int>{ 1, 2, 1 });
	}

	SECTION("with swap") {
		swap(swappedList, callbackList);
		REQUIRE(! swappedList.empty());
		swappedList();
		swappedList(5);
		REQUIRE(dataList == std::vector<int>{ 1, 1, 0 });
		callbackList();
		callbackList(5);
		REQUIRE(dataList == std::vector<int>{ 1, 2, 1 });
	}
}
