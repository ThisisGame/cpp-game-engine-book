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

#include <thread>
#include <chrono>
#include <random>

TEST_CASE("CallbackList, multi threading, append")
{
	using CL = eventpp::CallbackList<void(), FakeCallbackListPolicies>;

	CL callbackList;

	constexpr int threadCount = 256;
	constexpr int taskCountPerThread = 1024 * 4;
	constexpr int itemCount = threadCount * taskCountPerThread;

	std::vector<int> taskList(itemCount);
	std::iota(taskList.begin(), taskList.end(), 0);
	std::shuffle(taskList.begin(), taskList.end(), std::mt19937(std::random_device()()));

	std::vector<std::thread> threadList;
	for(int i = 0; i < threadCount; ++i) {
		threadList.emplace_back([i, taskCountPerThread, &callbackList, &taskList]() {
			for(int k = i * taskCountPerThread; k < (i + 1) * taskCountPerThread; ++k) {
				callbackList.append(taskList[k]);
			}
		});
	}

	for(auto & thread : threadList) {
		thread.join();
	}

	taskList.clear();

	std::vector<int> compareList(itemCount);
	std::iota(compareList.begin(), compareList.end(), 0);

	verifyDisorderedLinkedList(callbackList, compareList);
}

TEST_CASE("CallbackList, multi threading, remove")
{
	using CL = eventpp::CallbackList<void(), FakeCallbackListPolicies>;

	CL callbackList;

	constexpr int threadCount = 256;
	constexpr int taskCountPerThread = 1024 * 4;
	constexpr int itemCount = threadCount * taskCountPerThread;

	std::vector<int> taskList(itemCount);
	std::iota(taskList.begin(), taskList.end(), 0);
	std::shuffle(taskList.begin(), taskList.end(), std::mt19937(std::random_device()()));

	std::vector<CL::Handle> handleList;

	for(const auto & item : taskList) {
		handleList.push_back(callbackList.append(item));
	}

	std::vector<std::thread> threadList;
	for(int i = 0; i < threadCount; ++i) {
		threadList.emplace_back([i, taskCountPerThread, &callbackList, &handleList]() {
			for(int k = i * taskCountPerThread; k < (i + 1) * taskCountPerThread; ++k) {
				callbackList.remove(handleList[k]);
			}
		});
	}

	for(auto & thread : threadList) {
		thread.join();
	}

	taskList.clear();

	REQUIRE(! callbackList.head);
	REQUIRE(! callbackList.tail);
}

TEST_CASE("CallbackList, multi threading, double remove")
{
	using CL = eventpp::CallbackList<void(), FakeCallbackListPolicies>;

	CL callbackList;

	constexpr int threadCount = 256;
	constexpr int taskCountPerThread = 1024 * 4;
	constexpr int itemCount = threadCount * taskCountPerThread;

	std::vector<int> taskList(itemCount);
	std::iota(taskList.begin(), taskList.end(), 0);
	std::shuffle(taskList.begin(), taskList.end(), std::mt19937(std::random_device()()));

	std::vector<CL::Handle> handleList;

	for(const auto & item : taskList) {
		handleList.push_back(callbackList.append(item));
	}

	std::vector<std::thread> threadList;
	for(int i = 0; i < threadCount; ++i) {
		threadList.emplace_back([i, taskCountPerThread, &callbackList, &handleList, threadCount]() {
			int start = i;
			int end = i + 1;
			if(i > 0) {
				--start;
			}
			else if(i < threadCount - 1) {
				++end;
			}
			for(int k = start * taskCountPerThread; k < end * taskCountPerThread; ++k) {
				callbackList.remove(handleList[k]);
			}
		});
	}

	for(auto & thread : threadList) {
		thread.join();
	}

	taskList.clear();

	REQUIRE(! callbackList.head);
	REQUIRE(! callbackList.tail);
}

TEST_CASE("CallbackList, multi threading, append/double remove")
{
	using CL = eventpp::CallbackList<void(), FakeCallbackListPolicies>;

	CL callbackList;

	constexpr int threadCount = 256;
	constexpr int taskCountPerThread = 1024 * 4;
	constexpr int itemCount = threadCount * taskCountPerThread;

	std::vector<int> taskList(itemCount);
	std::iota(taskList.begin(), taskList.end(), 0);
	std::shuffle(taskList.begin(), taskList.end(), std::mt19937(std::random_device()()));

	std::vector<CL::Handle> handleList(taskList.size());

	std::vector<std::thread> threadList;
	for(int i = 0; i < threadCount; ++i) {
		threadList.emplace_back([i, taskCountPerThread, &callbackList, &handleList, threadCount, &taskList]() {
			for(int k = i * taskCountPerThread; k < (i + 1) * taskCountPerThread; ++k) {
				handleList[k] = callbackList.append(taskList[k]);
			}
			int start = i;
			int end = i + 1;
			if(i > 0) {
				--start;
			}
			else if(i < threadCount - 1) {
				++end;
			}
			for(int k = start * taskCountPerThread; k < end * taskCountPerThread; ++k) {
				callbackList.remove(handleList[k]);
			}
		});
	}

	for(auto & thread : threadList) {
		thread.join();
	}

	taskList.clear();

	REQUIRE(! callbackList.head);
	REQUIRE(! callbackList.tail);
}

TEST_CASE("CallbackList, multi threading, insert")
{
	using CL = eventpp::CallbackList<void(), FakeCallbackListPolicies>;

	CL callbackList;

	constexpr int threadCount = 256;
	constexpr int taskCountPerThread = 1024;
	constexpr int itemCount = threadCount * taskCountPerThread;

	std::vector<int> taskList(itemCount);
	std::iota(taskList.begin(), taskList.end(), 0);
	std::shuffle(taskList.begin(), taskList.end(), std::mt19937(std::random_device()()));

	std::vector<CL::Handle> handleList(taskList.size());

	std::vector<std::thread> threadList;
	for(int i = 0; i < threadCount; ++i) {
		threadList.emplace_back([i, taskCountPerThread, &callbackList, &taskList, &handleList]() {
			int k = i * taskCountPerThread;
			for(; k < i * taskCountPerThread + taskCountPerThread / 2; ++k) {
				handleList[k] = callbackList.append(taskList[k]);
			}
			int offset = 0;
			for(; k < i * taskCountPerThread + taskCountPerThread / 2 + taskCountPerThread / 4; ++k) {
				handleList[k] = callbackList.insert(taskList[k], handleList[offset++]);
			}
			for(; k < (i + 1) * taskCountPerThread; ++k) {
				handleList[k] = callbackList.insert(taskList[k], handleList[offset++]);
			}
		});
	}

	for(auto & thread : threadList) {
		thread.join();
	}

	taskList.clear();

	std::vector<int> compareList(itemCount);
	std::iota(compareList.begin(), compareList.end(), 0);

	verifyDisorderedLinkedList(callbackList, compareList);
}

